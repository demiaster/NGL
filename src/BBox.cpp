/*
  Copyright (C) 2009 Jon Macey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
//----------------------------------------------------------------------------------------------------------------------
/// @file BBox.cpp
/// @brief a simple bounding box class
//----------------------------------------------------------------------------------------------------------------------
#include "BBox.h"
#include <iostream>

namespace ngl
{
const static GLubyte indices[]=  {
                                    0,1,5,0,4,5, // back
                                    3,2,6,7,6,3, // front
                                    0,1,2,3,2,0, // top
                                      4,5,6,7,6,4, // bottom
                                      0,3,4,4,7,3,
                                      1,5,2,2,6,5

                                 };
// indices for line loop
const static GLubyte lindices[]=  {
                                    0,3,7,4,0, /// Left
                                    0,1,5,4,0, /// back quad
                                    1,2,6,5,1, // right
                                    0,1,2,3,0,
                                    4,5,6,7,4
                                 };

//----------------------------------------------------------------------------------------------------------------------
BBox::BBox( const glm::vec3& _center,  Real _width, Real _height, Real _depth  ) noexcept
{
	// Calculate the Vertices based on the w,h,d params passed in the box is asumed
	// to be centered on the _center with equal w / h / d
	// -x +y -z

    m_vert[0].x=_center.x-(_width/2.0f); m_vert[0].y=_center.y+(_height/2.0f); m_vert[0].z=_center.z-(_depth/2.0f);
	// + x -y -z
    m_vert[1].x=_center.x+(_width/2.0f); m_vert[1].y=_center.y+(_height/2.0f); m_vert[1].z=_center.z-(_depth/2.0f);
    m_vert[2].x=_center.x+(_width/2.0f); m_vert[2].y=_center.y+(_height/2.0f); m_vert[2].z=_center.z+(_depth/2.0f);
    m_vert[3].x=_center.x-(_width/2.0f); m_vert[3].y=_center.y+(_height/2.0f); m_vert[3].z=_center.z+(_depth/2.0f);
    m_vert[4].x=_center.x-(_width/2.0f); m_vert[4].y=_center.y-(_height/2.0f); m_vert[4].z=_center.z-(_depth/2.0f);
    m_vert[5].x=_center.x+(_width/2.0f); m_vert[5].y=_center.y-(_height/2.0f); m_vert[5].z=_center.z-(_depth/2.0f);
    m_vert[6].x=_center.x+(_width/2.0f); m_vert[6].y=_center.y-(_height/2.0f); m_vert[6].z=_center.z+(_depth/2.0f);
    m_vert[7].x=_center.x-(_width/2.0f); m_vert[7].y=_center.y-(_height/2.0f); m_vert[7].z=_center.z+(_depth/2.0f);

	//Store the _center
	m_center=_center;
	// Setup the Plane Normals for Collision Detection
    m_norm[0]=glm::vec3(0.0f,1.0f,0.0f);
    m_norm[1]=glm::vec3(0.0f,-1.0f,0.0f);
    m_norm[2]=glm::vec3(1.0f,0.0f,0.0f);
    m_norm[3]=glm::vec3(-1.0f,0.0f,0.0f);
    m_norm[4]=glm::vec3(0.0f,0.0f,1.0f);
    m_norm[5]=glm::vec3(0.0f,0.0f,-1.0f);
	// store width height and depth
	m_width=_width;
	m_height=_height;
	m_depth=_depth;
	m_drawMode=GL_LINE;
	m_vao=0;
	setVAO();
	for(int i=0; i<8; ++i)
	{
		std::cout<<&m_vert[i]<<"\n";
	}
}


//----------------------------------------------------------------------------------------------------------------------
BBox::BBox() noexcept
{
  //default constructor creates a unit BBox
  m_center.x=m_center.y=m_center.z=0.0f;
  m_drawMode=GL_LINE;
  m_width=2.0;
  m_height=2.0;
  m_depth=2.0;
  m_vao=0;
  setVAO();
}

BBox::BBox(const BBox &_b) noexcept
{
  m_center=_b.m_center;
  m_width=_b.m_width;
  m_height=_b.m_height;
  m_depth=_b.m_depth;
  m_drawMode=_b.m_drawMode;
  m_minX=_b.m_minX;
  m_maxX=_b.m_maxX;
  m_minY=_b.m_minY;
  m_maxY=_b.m_maxY;
  m_minZ=_b.m_minZ;
  m_maxZ=_b.m_maxZ;
  m_vao=0;
  recalculate();
}


//----------------------------------------------------------------------------------------------------------------------
BBox::BBox( Real _minX, Real _maxX,  Real _minY, Real _maxY, Real _minZ, Real _maxZ  ) noexcept
{

	m_minX=_minX;
	m_maxX=_maxX;
	m_minY=_minY;
	m_maxY=_maxY;
	m_minZ=_minZ;
	m_maxZ=_maxZ;


    m_center=glm::vec3(0,0,0);

    m_vert[0].x=_minX; m_vert[0].y=_maxY; m_vert[0].z=_minZ;
    m_vert[1].x=_maxX; m_vert[1].y=_maxY; m_vert[1].z=_minZ;
    m_vert[2].x=_maxX; m_vert[2].y=_maxY; m_vert[2].z=_maxZ;
    m_vert[3].x=_minX; m_vert[3].y=_maxY; m_vert[3].z=_maxZ;

    m_vert[4].x=_minX; m_vert[4].y=_minY; m_vert[4].z=_minZ;
    m_vert[5].x=_maxX; m_vert[5].y=_minY; m_vert[5].z=_minZ;
    m_vert[6].x=_maxX; m_vert[6].y=_minY; m_vert[6].z=_maxZ;
    m_vert[7].x=_minX; m_vert[7].y=_minY; m_vert[7].z=_maxZ;
	m_drawMode=GL_LINE;
	m_width=m_maxX-m_minX;
	m_height=m_maxY-m_minY;
	m_depth=m_maxZ-m_minZ;
	m_vao=0;
	setVAO();

}

//----------------------------------------------------------------------------------------------------------------------
void BBox::setDrawMode( GLenum _mode) noexcept
{
  m_drawMode=_mode;
  setVAO();
}

void BBox::setVAO()
{
	// if we change the size we re-do the VAO
	if( m_vao != 0 )
	{
		//std::cout<<"VAO already exists so delete and re-create\n";
		GLuint id=m_vao->getID();
		glDeleteVertexArrays(1,&id);
	}
	// if were not doing line drawing then use tris
	if(m_drawMode !=GL_LINE)
	{
		m_vao=VertexArrayObject::createVOA(GL_TRIANGLES);

    // now we have our data add it to the VAO, we need to tell the VAO the following
    // how much (in bytes) data we are copying
    // a pointer to the first element of data (in this case the address of the first element of the
    // std::vector
    m_vao->bind();
    m_vao->setIndexedData(8*sizeof(glm::vec3),m_vert[0].x,sizeof(indices),&indices[0],GL_UNSIGNED_BYTE,GL_STATIC_DRAW);

    m_vao->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(glm::vec3),0);

    m_vao->setNumIndices(sizeof(indices));
    // finally we have finished for now so time to unbind the VAO
    m_vao->unbind();
  }
  else // we just need to draw the lines
  {
    m_vao=VertexArrayObject::createVOA(GL_LINE_LOOP);

    // now we have our data add it to the VAO, we need to tell the VAO the following
    // how much (in bytes) data we are copying
    // a pointer to the first element of data (in this case the address of the first element of the


    m_vao->bind();
    m_vao->setIndexedData(8*sizeof(glm::vec3),m_vert[0].x,sizeof(lindices),&lindices[0],GL_UNSIGNED_BYTE,GL_STATIC_DRAW);

    m_vao->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(glm::vec3),0);

    m_vao->setNumIndices(sizeof(lindices));
    // finally we have finished for now so time to unbind the VAO
    m_vao->unbind();


  }


}


//----------------------------------------------------------------------------------------------------------------------
void BBox::draw() const noexcept
{
  glPolygonMode(GL_FRONT_AND_BACK,m_drawMode);
  m_vao->bind();
  m_vao->draw();
  m_vao->unbind();
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}
//----------------------------------------------------------------------------------------------------------------------

void BBox::setCenter(const glm::vec3 &_center, bool _recalc) noexcept
{
	// Calculate the Vertices based on the w,h,d params passed in the box is asumed
	// to be centered on the _center with equal w / h / d
	// -x +y -z
	m_center=_center;
    m_vert[0].x=_center.x-(m_width/2.0f); m_vert[0].y=_center.y+(m_height/2.0f); m_vert[0].z=_center.z-(m_depth/2.0f);
	// + x -y -z
    m_vert[1].x=_center.x+(m_width/2.0f); m_vert[1].y=_center.y+(m_height/2.0f); m_vert[1].z=_center.z-(m_depth/2.0f);
    m_vert[2].x=_center.x+(m_width/2.0f); m_vert[2].y=_center.y+(m_height/2.0f); m_vert[2].z=_center.z+(m_depth/2.0f);
    m_vert[3].x=_center.x-(m_width/2.0f); m_vert[3].y=_center.y+(m_height/2.0f); m_vert[3].z=_center.z+(m_depth/2.0f);
    m_vert[4].x=_center.x-(m_width/2.0f); m_vert[4].y=_center.y-(m_height/2.0f); m_vert[4].z=_center.z-(m_depth/2.0f);
    m_vert[5].x=_center.x+(m_width/2.0f); m_vert[5].y=_center.y-(m_height/2.0f); m_vert[5].z=_center.z-(m_depth/2.0f);
    m_vert[6].x=_center.x+(m_width/2.0f); m_vert[6].y=_center.y-(m_height/2.0f); m_vert[6].z=_center.z+(m_depth/2.0f);
    m_vert[7].x=_center.x-(m_width/2.0f); m_vert[7].y=_center.y-(m_height/2.0f); m_vert[7].z=_center.z+(m_depth/2.0f);
	if(_recalc)
		recalculate();
}

void BBox::width(Real _w, bool _recalc) noexcept
{
  m_width=_w;
  if(_recalc)
  {
    recalculate();
  }
}

void BBox::height(Real _h, bool _recalc) noexcept
{
  m_height=_h;
  if(_recalc)
  {
    recalculate();
  }
}

void BBox::depth(Real _d, bool _recalc) noexcept
{
  m_depth=_d;
  if(_recalc)
  {
    recalculate();
  }
}

void BBox::recalculate() noexcept
{
  // Calculate the Vertices based on the w,h,d params passed in the box is asumed
  // to be centered on the _center with equal w / h / d
  // -x +y -z
  m_vert[0].x=m_center.x-(m_width/2.0f); m_vert[0].y=m_center.y+(m_height/2.0f); m_vert[0].z=m_center.z-(m_depth/2.0f);
  // + x -y -z
  m_vert[1].x=m_center.x+(m_width/2.0f); m_vert[1].y=m_center.y+(m_height/2.0f); m_vert[1].z=m_center.z-(m_depth/2.0f);
  m_vert[2].x=m_center.x+(m_width/2.0f); m_vert[2].y=m_center.y+(m_height/2.0f); m_vert[2].z=m_center.z+(m_depth/2.0f);
  m_vert[3].x=m_center.x-(m_width/2.0f); m_vert[3].y=m_center.y+(m_height/2.0f); m_vert[3].z=m_center.z+(m_depth/2.0f);
  m_vert[4].x=m_center.x-(m_width/2.0f); m_vert[4].y=m_center.y-(m_height/2.0f); m_vert[4].z=m_center.z-(m_depth/2.0f);
  m_vert[5].x=m_center.x+(m_width/2.0f); m_vert[5].y=m_center.y-(m_height/2.0f); m_vert[5].z=m_center.z-(m_depth/2.0f);
  m_vert[6].x=m_center.x+(m_width/2.0f); m_vert[6].y=m_center.y-(m_height/2.0f); m_vert[6].z=m_center.z+(m_depth/2.0f);
  m_vert[7].x=m_center.x-(m_width/2.0f); m_vert[7].y=m_center.y-(m_height/2.0f); m_vert[7].z=m_center.z+(m_depth/2.0f);
  setVAO();
}

//----------------------------------------------------------------------------------------------------------------------

BBox::~BBox() noexcept
{
  m_vao->removeVOA();
  delete m_vao;
}
//----------------------------------------------------------------------------------------------------------------------

} // end namespace ngl



