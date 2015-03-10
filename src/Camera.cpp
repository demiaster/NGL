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
#include "Camera.h"
#include "Util.h"
#include "NGLassert.h"
#include <VertexArrayObject.h>
#include <iostream>
#include <vector>
#include <boost/format.hpp>
#include <glm/gtc/matrix_transform.hpp>
//----------------------------------------------------------------------------------------------------------------------
/// @file Camera.cpp
/// @brief implementation files for Camera class
//----------------------------------------------------------------------------------------------------------------------
namespace ngl
{
// a lot of this stuff is from the HILL book Computer Graphics with OpenGL 2nd Ed Prentice Hall
// a very good book
const static Real CAMERANEARLIMIT=0.00001f;

//----------------------------------------------------------------------------------------------------------------------

Camera::Camera() noexcept
{

  m_zNear=0.0001f;
  m_zFar=350.0f;
  m_aspect=720.0f/576.0f;
  m_fov=45.0f;
  m_width=720;
  m_height=576;
  m_eye=glm::vec3(1,1,1);
}


//----------------------------------------------------------------------------------------------------------------------
void Camera :: setDefaultCamera() noexcept
{
  // make default camera
  m_eye=glm::vec3(1.0f);
  m_look=glm::vec3(0.0f);
  m_up=glm::vec3(0.0f,1.0f,0.0f);
  m_fov=45.0;
  m_zNear=0.0001f;
  m_zFar=350.0f;
  m_aspect=720.0f/576.0f;
  m_fov=45.0f;

  setShape(m_fov, m_aspect, m_zNear, m_zFar); // good default values here
  set(glm::vec3(5.0, 5.0, 5.0),glm::vec3( 0.0, 0.0, 0.0),glm::vec3(0, 1, 0));
}

//----------------------------------------------------------------------------------------------------------------------
void Camera :: set(const glm::vec3 &_eye, const glm::vec3 &_look,  const glm::vec3 &_up  ) noexcept
{
	// make U, V, N vectors
    m_eye=glm::vec3(_eye);
    m_look=glm::vec3(_look);
    m_up=_up;
    m_n=glm::normalize(m_eye-m_look);
    m_u=glm::normalize(glm::cross(_up,m_n));
    m_v=glm::normalize(glm::cross(m_n,m_u));

    setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
Camera::Camera(const glm::vec3 &_eye, const glm::vec3 &_look, const glm::vec3 &_up ) noexcept
{
	setDefaultCamera();
	set(_eye,_look,_up);
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::setViewMatrix() noexcept
{
    // grab a pointer to the matrix so we can index is quickly
    Real *M=(Real *)&m_viewMatrix;
    M[0] =  m_u.x;         M[1] =  m_v.x;        M[2] =  m_n.x;        M[3] = 0.0;
    M[4] =  m_u.y;         M[5] =  m_v.y;        M[6] =  m_n.y;        M[7] = 0.0;
    M[8]  = m_u.z;         M[9] =  m_v.z;        M[10]=  m_n.z;        M[11] =0.0;
    M[12] = -glm::dot(m_eye,m_u); M[13]= -glm::dot(m_eye,m_v); M[14]= -glm::dot(m_eye,m_n); M[15] =1.0;

    calculateFrustum();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::setPerspProjection() noexcept
{
  // note 1/tan == cotangent
  Real f= 1.0/tan(radians(m_fov)/2.0);
  m_projectionMatrix=glm::mat4(1.0f);

  m_projectionMatrix[0][0]=f/m_aspect;
  m_projectionMatrix[1][1]=f;

  m_projectionMatrix[2][2]=(m_zFar+m_zNear)/(m_zNear-m_zFar);
  m_projectionMatrix[3][2]=(2*m_zFar*m_zNear)/(m_zNear-m_zFar);

  m_projectionMatrix[2][3]=-1;
  m_projectionMatrix[3][3]=1.0;

}

//----------------------------------------------------------------------------------------------------------------------
void Camera::setProjectionMatrix() noexcept
{
    m_projectionMatrix=glm::mat4(0.0f);
	setPerspProjection();

}
//----------------------------------------------------------------------------------------------------------------------

void Camera::setShape(Real _viewAngle, Real _aspect, Real _near, Real _far  ) noexcept

{ // load projection matrix and camera values
	if(_viewAngle >180.0)
	{
		_viewAngle=180.0;
	}
	NGL_ASSERT(_far>_near);
	NGL_ASSERT(_near>CAMERANEARLIMIT);
	if(_near<CAMERANEARLIMIT)
	{
		_near=CAMERANEARLIMIT;
	}
	m_fov = _viewAngle; // viewangle in degrees - must be < 180
	m_aspect = _aspect;
	m_zNear = _near;
	m_zFar = _far;
	setProjectionMatrix();
	//calculateFrustum();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::setAspect( Real _asp  ) noexcept
{
	m_aspect = _asp;
	setShape(m_fov,m_aspect,m_zNear,m_zFar);
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::setViewAngle( Real _angle	 ) noexcept
{
	m_fov=_angle;
	setShape(_angle,m_aspect,m_zNear,m_zFar);
}


//----------------------------------------------------------------------------------------------------------------------
void Camera::slide( Real _du,  Real _dv,   Real _dn ) noexcept
{
	// slide eye by amount du * u + dv * v + dn * n;
    m_eye.x += _du * m_u.x + _dv * m_v.x + _dn * m_n.x;
    m_eye.y += _du * m_u.y + _dv * m_v.y + _dn * m_n.y;
    m_eye.z += _du * m_u.z + _dv * m_v.z + _dn * m_n.z;
	setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::move( Real _dx, Real _dy,  Real _dz ) noexcept
{
// simply add the translation to the current eye point
    m_eye.x += _dx;
    m_eye.y += _dy;
    m_eye.z += _dz;
	setViewMatrix();
}
//----------------------------------------------------------------------------------------------------------------------
void Camera::moveBoth( Real _dx, Real _dy, Real _dz ) noexcept
{
    m_eye.x +=_dx;
    m_eye.y +=_dy;
    m_eye.z +=_dz;
    m_look.x+=_dx;
    m_look.y+=_dy;
    m_look.z+=_dz;
    m_n=glm::normalize(m_eye-m_look);
    m_u=glm::normalize(glm::cross(m_up,m_n));
    m_v=glm::normalize(glm::cross(m_n,m_u));
	// normalize vectors
    // pass to OpenGL
	setViewMatrix();
}
//----------------------------------------------------------------------------------------------------------------------
void Camera::rotAxes( glm::vec3& io_a, glm::vec3& io_b,  const Real _angle  ) noexcept
{
// rotate orthogonal vectors a (like x axis) and b(like y axis) through angle degrees
	// convert to radians
	Real ang = radians(_angle);
	// pre-calc cos and sine
	Real c = cos(ang);
	Real s = sin(ang);
	// tmp for io_a vector
    glm::vec3 t( c * io_a.x + s * io_b.x,  c * io_a.y + s * io_b.y,  c * io_a.z + s * io_b.z);
	// now set to new rot value
    io_b=glm::vec3(-s * io_a.x + c * io_b.x, -s * io_a.y + c * io_b.y, -s * io_a.z + c * io_b.z);
	// put tmp into _a'
    io_a=glm::vec3(t.x, t.y, t.z);
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::roll(Real _angle  ) noexcept
{
  rotAxes(m_u, m_v, -_angle);
  setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::pitch(Real _angle ) noexcept
{
  rotAxes(m_n, m_v, _angle);
  setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::yaw(Real _angle ) noexcept
{
  rotAxes(m_u, m_n, _angle);
  setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::moveEye( Real _dx, Real _dy,   Real _dz  ) noexcept
{
    m_eye.x+=_dx;
    m_eye.y+=_dy;
    m_eye.z+=_dz;
    m_n=glm::normalize(m_eye-m_look);
    m_u=glm::normalize(glm::cross(m_up,m_n));
    m_v=glm::normalize(glm::cross(m_n,m_u));
	// pass to OpenGL
	setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::moveLook(Real _dx,  Real _dy,  Real _dz   ) noexcept
{
    m_look.x+=_dx;
    m_look.y+=_dy;
    m_look.z+=_dz;
    m_n=glm::normalize(m_eye-m_look);
    m_u=glm::normalize(glm::cross(m_up,m_n));
    m_v=glm::normalize(glm::cross(m_n,m_u));
	// pass to OpenGL
	setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::update() noexcept
{
	setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::normalisedYaw( Real _angle ) noexcept
{
  // build a rotation matrix around the y axis
  glm::mat3 mat(1.0f);

  mat=glm::rotate(glm::mat3(1.0),_angle,glm::vec3(0.0,1.0,0.0));
  //multiply all three local coord vectors by the matrix
  m_u = m_u * mat;
  m_v = m_v * mat;
  m_n = m_n * mat;
  // reset the modelview matrix
  setViewMatrix();
}
//----------------------------------------------------------------------------------------------------------------------
void Camera::normalisedRoll(Real _angle  ) noexcept
{
  // build a rotation matrix around the y axis
  Mat4 mat;
  mat.identity();
  mat.rotateZ(_angle);
  //multiply all three local coord vectors by the matrix
  m_u = m_u * mat;
  m_v = m_v * mat;
  m_n = m_n * mat;
  // reset the modelview matrix
  setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::normalisedPitch(Real _angle  ) noexcept
{
  // build a rotation matrix around the y axis
  Mat4 mat;
  mat.identity();
  mat.rotateX(_angle);
  //multiply all three local coord vectors by the matrix
  m_u = m_u * mat;
  m_v = m_v * mat;
  m_n = m_n * mat;
  // reset the modelview matrix
  setViewMatrix();
}



//----------------------------------------------------------------------------------------------------------------------
void Camera::writeRib( RibExport &_rib ) const noexcept
{
	if(_rib.isOpen()!=0)
	{
		Real *M=(Real *)&m_viewMatrix.m_m;
		_rib.writeTabs();
		_rib.getStream() <<"# Camera transform from GraphicsLib Camera\n"  ;
		_rib.getStream() <<"# now we need to flip the Z axis\n";
		_rib.getStream() <<"Scale 1 1 -1 \n";

		_rib.getStream() <<"ConcatTransform [ ";
		for (int i=0; i<16; i++)
		{
			_rib.getStream() <<M[i]<<" ";
		}
		_rib.getStream() <<"]\n";
		_rib.getStream() <<"# now we Set the clipping \n";
		_rib.getStream() <<"Clipping "<<m_zNear<<" "<<m_zFar<<"\n";
		_rib.getStream() <<"Projection \"perspective\" \"fov\" ["<<m_fov<<"]\n";
		_rib.getStream() <<"#End of Camera from GraphicsLib\n";
	}
}
//----------------------------------------------------------------------------------------------------------------------
/// Code modified from http://www.lighthouse3d.com/opengl/viewfrustum/index.php?intro
///
void Camera::calculateFrustum() noexcept
{

    Real tang = (Real)tan(radians(m_fov) * 0.5) ;
    Real nh = m_zNear * tang;
    Real nw = nh * m_aspect;
    Real fh = m_zFar  * tang;
    Real fw = fh * m_aspect;

    glm::vec3 nc = (m_eye - m_n * m_zNear).toVec3();
    glm::vec3 fc = (m_eye - m_n * m_zFar).toVec3();

    m_ntl = nc + m_v.toVec3() * nh - m_u.toVec3() * nw;
    m_ntr = nc + m_v.toVec3() * nh + m_u.toVec3() * nw;
    m_nbl = nc - m_v.toVec3() * nh - m_u.toVec3() * nw;
    m_nbr = nc - m_v.toVec3() * nh + m_u.toVec3() * nw;

    m_ftl = fc + m_v.toVec3() * fh - m_u.toVec3() * fw;
    m_ftr = fc + m_v.toVec3() * fh + m_u.toVec3() * fw;
    m_fbl = fc - m_v.toVec3() * fh - m_u.toVec3() * fw;
    m_fbr = fc - m_v.toVec3() * fh + m_u.toVec3() * fw;
    m_planes[TOP].setPoints(m_ntr,m_ntl,m_ftl);
    m_planes[BOTTOM].setPoints(m_nbl,m_nbr,m_fbr);
    m_planes[LEFT].setPoints(m_ntl,m_nbl,m_fbl);
    m_planes[RIGHT].setPoints(m_nbr,m_ntr,m_fbr);
    m_planes[NEARP].setPoints(m_ntl,m_ntr,m_nbr);
    m_planes[FARP].setPoints(m_ftr,m_ftl,m_fbl);
}

void Camera::drawFrustum() noexcept
{
  std::vector<glm::vec3>points;

  // draw the sides as lines
  points.push_back(m_ntl);
  points.push_back(m_ftl);

  points.push_back(m_ntr);
  points.push_back(m_ftr);

  points.push_back(m_nbl);
  points.push_back(m_fbl);

  points.push_back(m_nbr);
  points.push_back(m_fbr);
  // near plane lines
  points.push_back(m_ntr);
  points.push_back(m_ntl);
  points.push_back(m_nbr);
  points.push_back(m_nbl);
  points.push_back(m_ntr);
  points.push_back(m_nbr);
  points.push_back(m_ntl);
  points.push_back(m_nbl);
  // far plane lines
  points.push_back(m_ftr);
  points.push_back(m_ftl);
  points.push_back(m_fbr);
  points.push_back(m_fbl);
  points.push_back(m_ftr);
  points.push_back(m_fbr);
  points.push_back(m_ftl);
  points.push_back(m_fbl);


  // now we create a VAO to store the data
 VertexArrayObject *vao=VertexArrayObject::createVOA(GL_LINES);
  // bind it so we can set values
  vao->bind();
  // set the vertex data (4 for x,y,z)
  vao->setData(points.size()*sizeof(glm::vec3),points[0].x);
  // now we set the attribute pointer to be 0 (as this matches vertIn in our shader)
  vao->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(glm::vec3),0);
  // say how many indecis to be rendered
  vao->setNumIndices(points.size());
  vao->draw();
  // now unbind
  vao->unbind();
  vao->removeVOA();

}


CAMERAINTERCEPT Camera::isPointInFrustum( const glm::vec3 &_p ) const noexcept
{
	CAMERAINTERCEPT result = INSIDE;
	for(int i=0; i < 6; ++i)
	{

		if (m_planes[i].distance(_p) < 0)
		{
			return OUTSIDE;
		}
	}

	return result;
}


CAMERAINTERCEPT Camera::isSphereInFrustum(const glm::vec3 &_p,  Real _radius ) const noexcept
{

	CAMERAINTERCEPT result = INSIDE;
	Real distance;

	for(int i=0; i< 6; ++i)
	{
		distance = m_planes[i].distance(_p);
		if (distance < -_radius)
		{
			return OUTSIDE;
		}
		else if (distance < _radius)
		{
			result =  INTERSECT;
		}
	}
	return result;
}

/*
int Camera::boxInFrustum(AABox &b) {

	int result = INSIDE;
	for(int i=0; i < 6; i++) {

		if (pl[i].distance(b.getVertexP(pl[i].normal)) < 0)
			return OUTSIDE;
		else if (pl[i].distance(b.getVertexN(pl[i].normal)) < 0)
			result =  INTERSECT;
	}
	return(result);

 }

*/


/// end citation http://www.lighthouse3d.com/opengl/viewfrustum/index.php?intro


} // end namespace ngl

