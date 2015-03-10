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

#include "AbstractMesh.h"
#include "Util.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <list>
#include "NGLStream.h"
//----------------------------------------------------------------------------------------------------------------------
/// @file AbstractMesh.cpp
/// @brief a series of classes used to define an abstract 3D mesh of Faces, Vertex Normals and TexCords
//----------------------------------------------------------------------------------------------------------------------

namespace ngl
{
  //----------------------------------------------------------------------------------------------------------------------
  /// pre-define the boost tokenizer so we don't have to use the full dec
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
void AbstractMesh::drawBBox() const noexcept
{
  m_ext->draw();
}

void AbstractMesh::scale(Real _sx, Real _sy, Real _sz ) noexcept
{
  m_center=glm::vec3(0.0f);
  // in c++ 11 we can use
  for (auto &v : m_verts)
  {
    v.x*=_sx;
    v.y*=_sy;
    v.z*=_sz;
    m_center+=v;
  }
// calculate the center
  m_center/=m_nVerts;
  calcDimensions();
}



//----------------------------------------------------------------------------------------------------------------------
AbstractMesh::~AbstractMesh() noexcept
{
  if(m_loaded == true)
  {
    m_verts.erase(m_verts.begin(),m_verts.end());
    m_norm.erase(m_norm.begin(),m_norm.end());
    m_tex.erase(m_tex.begin(),m_tex.end());
    m_face.erase(m_face.begin(),m_face.end());
    m_indices.erase(m_indices.begin(),m_indices.end());
    m_outIndices.erase(m_outIndices.begin(),m_outIndices.end());

    if(m_vbo)
    {
      glDeleteBuffers(1,&m_vboBuffers);
      if(m_vaoMesh!=0)
      {
        delete m_vaoMesh;
      }
    }
    if(m_ext !=0)
    {
      delete m_ext;
    }
  }
}


//----------------------------------------------------------------------------------------------------------------------
void AbstractMesh::loadTexture( const std::string& _fName  ) noexcept
{
	// load in the texture
	Texture  *t=new Texture(_fName);
	m_textureID=t->setTextureGL();
	delete t;
	m_texture=true;
}

/// @todo correct the rib exporter and  add Normal information to  the export
/// @verbatim
/// The following code is adapted from an MSc project by
/// Authors: Elaine Kieran, Gavin Harrison, Luke Openshaw
/// Write the obj as a SubdivisionMesh package to the rib file
/// Renderman specification: SubdivisionMesh scheme nverts vertids tags nargs intargs floatargs parameterlist
/// @endverbatim

//----------------------------------------------------------------------------------------------------------------------
void AbstractMesh::writeToRibSubdiv(RibExport& _ribFile )const noexcept
{
	// Declare the variables
	std::list< int > lVertLink;
	std::list< int >::iterator vertLinkItr;
	std::vector< float > vVerts;
	std::vector< float >::iterator vertsItr;

  // Check if the rib exists
  if( _ribFile.isOpen() != 0 )
  {
    _ribFile.comment( "OBJ AbstractMeshect" );
  // Start printing the SubdivisionPolygons tag to the rib
    _ribFile.getStream() << "SubdivisionMesh \"catmull-clark\" [ ";

		// Loop through all the Polygons
		for (unsigned long  int I=0; I<m_nVerts; ++I)
		{
		// Print the count of vertices for the current polygon to the rib
		_ribFile.getStream() << m_face[I].m_numVerts << " ";
		// Start building the vertids and parameterlist
		for (unsigned long int i = 0; i < m_face[I].m_numVerts; ++i)
		{
			// Set the verts vector size and testing variables
			int iVecSize = vVerts.size();
			bool bTest = false;
			int counter = 0;
			// Loop through the expanding vector checking whether
			// the current vertice exists
			for (int j = 0; j < iVecSize; j = j + 3)
			{
				// If the vertice if found in the vector, set the test
				// flag and exit the loop. Else keep going.
				if( ( FCompare(m_verts[i].x ,vVerts[j]) ) &&
                    ( FCompare(m_verts[i].y,vVerts[j + 1]) ) &&
                    ( FCompare(m_verts[i].y,vVerts[j + 2]) )
					 )
				{
					bTest = true;
					break;
				}
				else
				{
					counter++;
				}
			} //end for

			// Add the vertice to the vector if it is not found
			if( bTest == false )
			{
				vVerts.push_back( m_verts[m_face[I].m_vert[i]].x );
                vVerts.push_back( m_verts[m_face[I].m_vert[i]].y );
                vVerts.push_back( m_verts[m_face[I].m_vert[i]].z );
				lVertLink.push_back( counter );
			}
			else
			{
				lVertLink.push_back( counter );
			}

		}// end outer for

	}// end if

	_ribFile.getStream() << "] [ ";

	// Print the vertids to the rib
	std::list< int >::iterator vertLinkEnd=lVertLink.end();
	for (vertLinkItr = lVertLink.begin(); vertLinkItr != vertLinkEnd; ++vertLinkItr)
	{
		_ribFile.getStream() << *vertLinkItr << " ";
	}

	_ribFile.getStream() << "] [\"interpolateboundary\"] [0 0] [] [] \"P\" [ ";
	// Print the parameterlist to the rib
	std::vector< float >::iterator vVertEnd=vVerts.end();
	for (vertsItr = vVerts.begin(); vertsItr != vVertEnd; ++vertsItr)
	{
		_ribFile.getStream() << *vertsItr << " ";
	}

	// Print new lines to the rib
	_ribFile.getStream() << "]\n\n";
	}

}

//----------------------------------------------------------------------------------------------------------------------
bool AbstractMesh::isTriangular() noexcept
{
  for(unsigned int i=0; i<m_nFaces; ++i)
  {
    if (m_face[i].m_numVerts >3)
    {
      return false;
    }
  }
  return true;
/* c++ 11 version
	for(auto f : m_face)
	{
		if (f.m_numVerts >3)
		{
			return false;
		}
	}
	return true;
*/
}


// Code from Rob Bateman (www.robthebloke.org)
// checks if v/n/t exist as a combination in the indices array. If it does, re-use that
// index and insert into the out_indices array. If the v/n/t combo has not been used before,
// generate a new vertex index....
//
//----------------------------------------------------------------------------------------------------------------------
bool AbstractMesh::addIndex(const unsigned _v, const unsigned _n,  const unsigned _t, std::vector<IndexRef>& io_indices, std::vector<GLuint>& io_outIndices  ) noexcept
{
	size_t size=io_indices.size();

	for(size_t i=0;i<size;++i)
	{
		// if v/n/t already exist, re-use...
		if(io_indices[i].m_v == _v && io_indices[i].m_n == _n && io_indices[i].m_t == _t)
		{
			io_outIndices.push_back((GLuint)i);
			return true;
		}
	}

	io_outIndices.push_back( (GLuint) io_indices.size() );
	io_indices.push_back(IndexRef(_v,_n,_t));
	return false;
}



// a simple structure to hold our vertex data
// had to move this outside the method as g++ complains about it
// clang doesn't have a problem tho
struct VertData
{
  GLfloat u; // tex cords
  GLfloat v; // tex cords
  GLfloat nx; // normal from obj mesh
  GLfloat ny;
  GLfloat nz;
  GLfloat x; // position from obj
  GLfloat y;
  GLfloat z;
};


void AbstractMesh::createVAO() noexcept
{
	// if we have already created a VBO just return.
	if(m_vao == true)
	{
		std::cout<<"VAO exist so returning\n";
		return;
	}
// else allocate space as build our VAO
	m_dataPackType=0;
	if(isTriangular())
	{
		m_dataPackType=GL_TRIANGLES;
		std::cout <<"Doing Tri Data"<<std::endl;
	}
	// data is mixed of > quad so exit error
	if(m_dataPackType == 0)
	{
		std::cerr<<"Can only create VBO from all Triangle or ALL Quad data at present"<<std::endl;
		exit(EXIT_FAILURE);
	}

  // now we are going to process and pack the mesh into an ngl::VertexArrayObject
  std::vector <VertData> vboMesh;
  VertData d;
  int loopFaceCount=3;


	// loop for each of the faces
	for(unsigned int i=0;i<m_nFaces;++i)
	{
		// now for each triangle in the face (remember we ensured tri above)
		for(int j=0;j<loopFaceCount;++j)
		{

			// pack in the vertex data first
			d.x=m_verts[m_face[i].m_vert[j]].x;
            d.y=m_verts[m_face[i].m_vert[j]].y;
            d.z=m_verts[m_face[i].m_vert[j]].z;
			// now if we have norms of tex (possibly could not) pack them as well
			if(m_nNorm >0 && m_nTex > 0)
			{

        d.nx=m_norm[m_face[i].m_norm[j]].x;
        d.ny=m_norm[m_face[i].m_norm[j]].y;
        d.nz=m_norm[m_face[i].m_norm[j]].z;

				d.u=m_tex[m_face[i].m_tex[j]].x;
                d.v=m_tex[m_face[i].m_tex[j]].y;

      }
      // now if neither are present (only verts like Zbrush models)
      else if(m_nNorm ==0 && m_nTex==0)
      {
        d.nx=0;
        d.ny=0;
        d.nz=0;
        d.u=0;
        d.v=0;
      }
      // here we've got norms but not tex
      else if(m_nNorm >0 && m_nTex==0)
      {
        d.nx=m_norm[m_face[i].m_norm[j]].x;
        d.ny=m_norm[m_face[i].m_norm[j]].y;
        d.nz=m_norm[m_face[i].m_norm[j]].z;
        d.u=0;
        d.v=0;
      }
      // here we've got tex but not norm least common
      else if(m_nNorm ==0 && m_nTex>0)
      {
        d.nx=0;
        d.ny=0;
        d.nz=0;
        d.u=m_tex[m_face[i].m_tex[j]].x;
        d.v=m_tex[m_face[i].m_tex[j]].y;
      }
    vboMesh.push_back(d);
    }
  }

  // first we grab an instance of our VOA
  m_vaoMesh = VertexArrayObject::createVOA(m_dataPackType);
  // next we bind it so it's active for setting data
  m_vaoMesh->bind();
  m_meshSize=vboMesh.size();

	// now we have our data add it to the VAO, we need to tell the VAO the following
	// how much (in bytes) data we are copying
	// a pointer to the first element of data (in this case the address of the first element of the
	// std::vector
	m_vaoMesh->setData(m_meshSize*sizeof(VertData),vboMesh[0].u);
	// in this case we have packed our data in interleaved format as follows
	// u,v,nx,ny,nz,x,y,z
	// If you look at the shader we have the following attributes being used
	// attribute glm::vec3 inVert; attribute 0
	// attribute vec2 inUV; attribute 1
	// attribute glm::vec3 inNormal; attribure 2
	// so we need to set the vertexAttributePointer so the correct size and type as follows
	// vertex is attribute 0 with x,y,z(3) parts of type GL_FLOAT, our complete packed data is
	// sizeof(vertData) and the offset into the data structure for the first x component is 5 (u,v,nx,ny,nz)..x
	m_vaoMesh->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(VertData),5);
	// uv same as above but starts at 0 and is attrib 1 and only u,v so 2
	m_vaoMesh->setVertexAttributePointer(1,2,GL_FLOAT,sizeof(VertData),0);
	// normal same as vertex only starts at position 2 (u,v)-> nx
	m_vaoMesh->setVertexAttributePointer(2,3,GL_FLOAT,sizeof(VertData),2);


	// now we have set the vertex attributes we tell the VAO class how many indices to draw when
	// glDrawArrays is called, in this case we use buffSize (but if we wished less of the sphere to be drawn we could
	// specify less (in steps of 3))
	m_vaoMesh->setNumIndices(m_meshSize);
	// finally we have finished for now so time to unbind the VAO
	m_vaoMesh->unbind();

	// indicate we have a vao now
	m_vao=true;

}



//----------------------------------------------------------------------------------------------------------------------
void AbstractMesh::draw() const noexcept
{
  if(m_vao == true)
  {
    if(m_texture == true)
    {
      glBindTexture(GL_TEXTURE_2D,m_textureID);
    }
    m_vaoMesh->bind();
    m_vaoMesh->draw();
    m_vaoMesh->unbind();
  }

}


//----------------------------------------------------------------------------------------------------------------------
Real * AbstractMesh::mapVAOVerts() noexcept
{

	Real* ptr=0;

	// bind our VBO data
//
  m_vaoMesh->bind();
  //NGLCheckGLError("Abstract Mesh",__LINE__);
  glBindBuffer(GL_ARRAY_BUFFER, m_vaoMesh->getVBOid(0));
  //NGLCheckGLError("Abstract Mesh",__LINE__);
  ptr = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
  //NGLCheckGLError("Abstract Mesh",__LINE__);
  m_vboMapped=true;
  return ptr;
}

//----------------------------------------------------------------------------------------------------------------------
void AbstractMesh::unMapVAO() noexcept
{

	if(m_vboMapped==true)
	{
		glUnmapBuffer(GL_ARRAY_BUFFER); // unmap it after use
		m_vboMapped=false;
		m_vaoMesh->unbind();
	}

}
//----------------------------------------------------------------------------------------------------------------------
void AbstractMesh::calcDimensions() noexcept
{
  // Calculate the center of the object.
  m_center=glm::vec3(0.0f);
  BOOST_FOREACH(glm::vec3 v,m_verts)
  {
    m_center+=v;
  }
  m_center/=m_nVerts;
  // calculate the extents
  m_maxX=m_minX=m_center.x;
  m_maxY=m_minY=m_center.y;
  m_maxZ=m_minZ=m_center.z;


  // Calculate the center of the object.
  m_center=glm::vec3(0.0f);
  for( auto  v :m_verts)
  {
    m_center+=v;
  }
  m_center/=m_nVerts;
  // calculate the extents
  m_maxX=m_minX=m_center.x;
  m_maxY=m_minY=m_center.y;
  m_maxZ=m_minZ=m_center.z;

  for(auto v : m_verts)
  {
    if     (v.x >m_maxX) { m_maxX=v.x; }
    else if(v.x <m_minX) { m_minX=v.x; }
    if     (v.y >m_maxY) { m_maxY=v.y; }
    else if(v.y <m_minY) { m_minY=v.y; }
    if     (v.z >m_maxZ) { m_maxZ=v.z; }
    else if(v.z <m_minZ) { m_minZ=v.z; }
  } // end BOOST_FOREACH


  // destroy the previous bounding box
  if(m_ext !=0)
  {
    delete m_ext;

  }
  // create a new bbox based on the new object size
  m_ext=new BBox(m_minX,m_maxX,m_minY,m_maxY,m_minZ,m_maxZ);

}

void AbstractMesh::saveNCCABinaryMesh( const std::string &_fname  ) noexcept
{
// so basically we need to save all the state data from the abstract mesh
// then map the vbo on the gpu and dump that in one go, this means we have to
// call CreateVBO first the Save
  std::fstream file;
  file.open(_fname.c_str(),std::ios::out | std::ios::binary);
  if (!file.is_open())
  {
    std::cerr<<"problems Opening File "<<_fname<<std::endl;
    return;
  }
  // lets write out our own Magic Number file ID
  const std::string header("ngl::bin");
  file.write(header.c_str(),header.length());
  /// The number of vertices in the object
  file.write(reinterpret_cast <char *>(&m_nVerts),sizeof(unsigned long int));
  /// The number of normals in the object
  file.write(reinterpret_cast <char *>(&m_nNorm),sizeof(unsigned long int));

  /// the number of texture co-ordinates in the object
  file.write(reinterpret_cast <char *>(&m_nTex),sizeof(unsigned long int));

  /// the number of faces in the object
  file.write(reinterpret_cast <char *>(&m_nFaces),sizeof(unsigned long int));
  file.write(reinterpret_cast <char *>(&m_center.x),sizeof(Real));
  file.write(reinterpret_cast <char *>(&m_center.y),sizeof(Real));
  file.write(reinterpret_cast <char *>(&m_center.z),sizeof(Real));

  file.write(reinterpret_cast <char *>(&m_texture),sizeof(bool));

  file.write(reinterpret_cast <char *>(&m_maxX),sizeof(Real));
  file.write(reinterpret_cast <char *>(&m_maxY),sizeof(Real));
  file.write(reinterpret_cast <char *>(&m_maxZ),sizeof(Real));
  file.write(reinterpret_cast <char *>(&m_minX),sizeof(Real));
  file.write(reinterpret_cast <char *>(&m_minY),sizeof(Real));
  file.write(reinterpret_cast <char *>(&m_minZ),sizeof(Real));

  file.write(reinterpret_cast <char *>(&  m_dataPackType),sizeof(GLuint));
  file.write(reinterpret_cast <char *>(&  m_indexSize),sizeof(unsigned int));
  file.write(reinterpret_cast <char *>(&  m_bufferPackSize),sizeof(unsigned int));
  /// now we can dump the data from the vbo
  unsigned int size=m_indexSize*m_bufferPackSize*sizeof(GLfloat);
  std::cout<<"size is"<<size<<std::endl;
  file.write(reinterpret_cast <char *>(&size),sizeof(unsigned int));


  Real *vboMem=this->mapVAOVerts();
  file.write(reinterpret_cast<char *>(vboMem),size);

  // now write the indices
  // first the size
  size=m_outIndices.size();
  std::cout<<"Size of out indices ="<<size<<std::endl;
  file.write(reinterpret_cast <char *>(&size),sizeof(unsigned int));


  for (auto d : m_outIndices)
  {
    file.write(reinterpret_cast <char *>(d),sizeof(unsigned int));
  }



  file.close();
  this->unMapVAO();

}

/// modified from example in Rick Parent book
/// Computer Animation Algorithms and Techniques
/// Morgan Korfman Appendix B
void AbstractMesh::calcBoundingSphere() noexcept
{
unsigned int size=m_verts.size();
if( size <=0 )
{
	std::cerr<<"now vertices loaded \n";
    m_sphereCenter=glm::vec3(0.0f);
    m_sphereRadius=0.0f;
	return;

}
// find minimal and maximal extents and indexs into
// into vert array
int minXI=0; int minYI=0; int minZI=0;
int maxXI=0; int maxYI=0; int maxZI=0;
Real minX=m_verts[0].x; Real maxX=m_verts[0].x;
Real minY=m_verts[0].y; Real maxY=m_verts[0].y;
Real minZ=m_verts[0].z; Real maxZ=m_verts[0].z;

for(unsigned int i=0; i<size; ++i)
{
  if(m_verts[i].x < minX) { minXI=i; minX=m_verts[i].x; }
  if(m_verts[i].x > maxX) { maxXI=i; maxX=m_verts[i].x; }
  if(m_verts[i].y < minY) { minYI=i; minY=m_verts[i].y; }
  if(m_verts[i].y > maxY) { maxYI=i; maxY=m_verts[i].y; }
  if(m_verts[i].z < minZ) { minZI=i; minZ=m_verts[i].z; }
  if(m_verts[i].z > maxZ) { maxZI=i; maxZ=m_verts[i].z; }
}
// now we find maximally seperated points from the 3 pairs
// we will use this to initialise the spheres
Real dx=m_verts[minXI].x-m_verts[maxXI].x;
Real dy=m_verts[minXI].y-m_verts[maxXI].y;
Real dz=m_verts[minXI].z-m_verts[maxXI].z;
Real diam2x=dx*dx+dy*dy+dz*dz;

dx=m_verts[minYI].x-m_verts[maxYI].x;
dy=m_verts[minYI].y-m_verts[maxYI].y;
dz=m_verts[minYI].z-m_verts[maxYI].z;
Real diam2y=dx*dx+dy*dy+dz*dz;

dx=m_verts[minZI].x-m_verts[maxZI].x;
dy=m_verts[minZI].y-m_verts[maxZI].y;
dz=m_verts[minZI].z-m_verts[maxZI].z;
Real diam2z=dx*dx+dy*dy+dz*dz;

Real diamTwo=diam2x;
int p1i=minXI;
int p2i=maxXI;
if(diam2y>diamTwo){ diamTwo=diam2y; p1i=minYI; p2i=maxYI;}
if(diam2z>diamTwo){ diamTwo=diam2z; p1i=minZI; p2i=maxZI;}
// now we can get the center of the sphere as the average
// of the two points
m_sphereCenter=glm::vec3(m_verts[p1i]+m_verts[p2i])/2.0f;
// now calculate radius and radius^2 of the initial sphere
Real radTwo=diamTwo/4.0;
Real rad=sqrt(radTwo);
// now check and adjust for outlying points
glm::vec3 newCenter;
Real newRad2;
Real newRad;
Real dist2;
Real dist;
Real delta;

for (auto v : m_verts)
{
  dx=v.x-m_sphereCenter.x;
  dy=v.y-m_sphereCenter.y;
  dz=v.z-m_sphereCenter.z;
  // distance squared of old center to current point
  dist2=dx*dx+dy*dy+dz*dz;
  // need to update the sphere if this point is outside the radius
  if(dist2 > radTwo)
  {
    dist=sqrt(dist2);
    newRad=(rad+dist)/2.0;
    newRad2=newRad*newRad;
    delta=dist-newRad;
    // now compute new center using the weights above
    newCenter.x=(newRad*m_sphereCenter.x+delta*v.x)/dist;
    newCenter.y=(newRad*m_sphereCenter.y+delta*v.y)/dist;
    newCenter.z=(newRad*m_sphereCenter.z+delta*v.z)/dist;
    // now test to see if we have a fit
    dx=v.x-newCenter.x;
    dy=v.y-newCenter.y;
    dz=v.z-newCenter.z;
    dist2=dx*dx+dy*dy+dz*dz;
    if(dist2 > newRad2)
    {
      std::cerr<<"something wrong here\n";
      std::cerr<<"error margin "<<dist2-newRad2<<"\n";
    }
    m_sphereCenter=newCenter;
    rad=newRad;
    radTwo=rad*rad;
  } // end if dist2>rad2
  m_sphereRadius=rad;

}

m_sphereRadius=rad;

}
/// end of citation


} //end ngl namespace


