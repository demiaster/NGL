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
#include "Types.h"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>
#include <string>
//----------------------------------------------------------------------------------------------------------------------
/// @file Util.cpp
/// @brief implementation files for Util class
//----------------------------------------------------------------------------------------------------------------------
namespace ngl
{

//----------------------------------------------------------------------------------------------------------------------
NGL_DLLEXPORT glm::vec3 calcNormal(const glm::vec4 &_p1, const glm::vec4 &_p2, const glm::vec4 &_p3 ) noexcept
{

	Real coa, cob, coc ;
	Real px1, py1, pz1 ;
	Real px2, py2, pz2 ;
	Real px3, py3, pz3 ;
	Real absvec;
	glm::vec3 norm;
    px1 = _p1.x; py1 = _p1.y; pz1 = _p1.z;
    px2 = _p2.x; py2 = _p2.y; pz2 = _p2.z;
    px3 = _p3.x; py3 = _p3.y; pz3 = _p3.z;

	// generate cross product

	coa = -(py1 * (pz2-pz3) + py2*(pz3-pz1) + py3*(pz1-pz2)) ;
	cob = -(pz1 * (px2-px3) + pz2*(px3-px1) + pz3*(px1-px2)) ;
	coc = -(px1 * (py2-py3) + px2*(py3-py1) + px3*(py1-py2)) ;

  // normalise the results to get unit length normal
  absvec = sqrtf ( ((coa*coa) + (cob*cob) + (coc*coc))) ;
  norm.x = coa/absvec ;
  norm.y = cob/absvec ;
  norm.z = coc/absvec ;

  return norm;
}


//----------------------------------------------------------------------------------------------------------------------
NGL_DLLEXPORT glm::vec3 calcNormal (const glm::vec3 &_p1,  const glm::vec3 &_p2, const glm::vec3 &_p3  ) noexcept
{

  Real coa, cob, coc ;
  Real px1, py1, pz1 ;
  Real px2, py2, pz2 ;
  Real px3, py3, pz3 ;
  Real absvec;
  glm::vec3 norm;
  px1 = _p1.x; py1 = _p1.y; pz1 = _p1.z;
  px2 = _p2.x; py2 = _p2.y; pz2 = _p2.z;
  px3 = _p3.x; py3 = _p3.y; pz3 = _p3.z;

  // generate cross product

  coa = -(py1 * (pz2-pz3) + py2*(pz3-pz1) + py3*(pz1-pz2)) ;
  cob = -(pz1 * (px2-px3) + pz2*(px3-px1) + pz3*(px1-px2)) ;
  coc = -(px1 * (py2-py3) + px2*(py3-py1) + px3*(py1-py2)) ;

  // normalise the results to get unit length normal
  absvec = sqrtf ( ((coa*coa) + (cob*cob) + (coc*coc))) ;
  norm.x = coa/absvec ;
  norm.y = cob/absvec ;
  norm.z = coc/absvec ;

  return norm;
}

NGL_DLLEXPORT Real radians(const Real _deg ) noexcept
{
  return (_deg/180.0f) * M_PI;
}

//----------------------------------------------------------------------------------------------------------------------
NGL_DLLEXPORT Real degrees( const Real _rad   ) noexcept
{
  return (_rad / M_PI) * 180.0f;
}


NGL_DLLEXPORT  void NGLCheckGLError( const std::string  &_file, const int _line ) noexcept
{
 int errNum = glGetError();

  while (errNum != GL_NO_ERROR)
  {
    std::string str;
    errNum = glGetError();
    switch(errNum)
    {
      case GL_INVALID_ENUM : str="GL_INVALID_ENUM error"; break;
      case GL_INVALID_VALUE : str="GL_INVALID_VALUE error"; break;
      case GL_INVALID_OPERATION : str="GL_INVALID_OPERATION error"; break;
      case GL_OUT_OF_MEMORY : str="GL_OUT_OF_MEMORY error"; break;
      // this should be in the spec but it's not on the mac! will check linux
      default : break;
    }
    if(errNum !=GL_NO_ERROR)
    {
      std::cerr<<"GL error"<< str<<" line : "<<_line<<" file : "<<_file<<"\n";
    }
   }
}


NGL_DLLEXPORT glm::mat4 perspective(Real _fovy,Real _aspect, Real _zNear, Real _zFar) noexcept
{
    Real range = tan(radians(_fovy / 2.0)) * _zNear;
    Real left = -range * _aspect;
    Real right = range * _aspect;
    Real bottom = -range;
    Real top = range;
    glm::mat4 result;
    result[0].x = (2.0f * _zNear) / (right - left);
    result[1].y = (2.0f * _zNear) / (top - bottom);
    result[2].z = - (_zFar + _zNear) / (_zFar - _zNear);
    result[2].w = - 1.0f;
    result[3].z = - (2.0f* _zFar * _zNear) / (_zFar - _zNear);
    return result;
}


NGL_DLLEXPORT glm::mat4 perspectiveFov(Real const & _fov, Real const & _width, Real const & _height, Real const & _zNear, Real const & _zFar) noexcept
{
    Real rad = radians(_fov);
    Real h = cos(0.5f * rad) / sin(0.5f * rad);
    Real w = h * _height / _width;
    glm::mat4 result;
    result[0].x = w;
    result[1].y = h;
    result[2].z = - (_zFar + _zNear) / (_zFar - _zNear);
    result[2].w = - 1.0f;
    result[3].z = - (2.0f* _zFar * _zNear) / (_zFar - _zNear);
    return result;
}


NGL_DLLEXPORT glm::mat4 infinitePerspective(Real _fovy, Real _aspect, Real _zNear) noexcept
{
  Real const range = tan(radians(_fovy / 2.0f)) * _zNear;
  Real left = -range * _aspect;
  Real right = range * _aspect;
  Real bottom = -range;
  Real top = range;

  glm::mat4 result;
  result[0].x = (2.0f * _zNear) / (right - left);
  result[1].y = (2.0f * _zNear) / (top - bottom);
  result[2].z = - 1.0f;
  result[2].w = - 1.0f;
  result[3].z = - 2.0f * _zNear;
  return result;
}


NGL_DLLEXPORT glm::mat4 lookAt(const glm::vec3  & _eye,const glm::vec3  & _center,const glm::vec3  & _up) noexcept
{

    glm::vec3 n =   _center-_eye;
    glm::vec3 u = _up;
    glm::vec3 v = glm::cross( n,u);
    u = glm::cross(v,n); //v.cross(n);
    glm::normalize(n);
    glm::normalize(v);
    glm::normalize(u);

    glm::mat4 result(1.0f);
    result[0].x= v.x;
    result[1].x= v.y;
    result[2].x= v.z;
    result[0].y= u.x;
    result[1].y= u.y;
    result[2].y= u.z;
    result[0].z=-n.x;
    result[1].z=-n.y;
    result[2].z=-n.z;
    result[3].x=-glm::dot(_eye,v);
    result[3].y=-glm::dot(_eye,u);
    result[3].z=-glm::dot(_eye,n);
    return result;
}

NGL_DLLEXPORT glm::mat4 ortho(Real _left, Real _right, Real _bottom, Real _top, Real _zNear, Real _zFar) noexcept
{
  glm::mat4 result(1);
  result[0].x= 2.0f / (_right - _left);
  result[1].y= 2.0f / (_top - _bottom);
  result[2].z= - 2.0f / (_zFar - _zNear);
  result[3].x= - (_right + _left) / (_right - _left);
  result[3].y= - (_top + _bottom) / (_top - _bottom);
  result[3].z= - (_zFar + _zNear) / (_zFar - _zNear);
  return result;
}

NGL_DLLEXPORT glm::mat4 ortho(Real _left, Real _right, Real _bottom, Real _top) noexcept
{
  glm::mat4 result(1);
  result[0].x= Real(2) / (_right - _left);
  result[1].y= Real(2) / (_top - _bottom);
  result[2].z= - Real(1);
  result[3].x= - (_right + _left) / (_right - _left);
  result[3].y= - (_top + _bottom) / (_top - _bottom);
  return result;
}

NGL_DLLEXPORT glm::mat4 frustum(Real _left, Real _right, Real _bottom, Real _top, Real _nearVal, Real _farVal) noexcept
{
  glm::mat4 result(0.0f);
  result[0].x = (2.0f * _nearVal) / (_right - _left);
  result[1].y = (2.0f * _nearVal) / (_top - _bottom);
  result[2].x = (_right + _left) / (_right - _left);
  result[2].y = (_top + _bottom) / (_top - _bottom);
  result[2].z = -(_farVal + _nearVal) / (_farVal - _nearVal);
  result[2].w = -1.0f;
  result[3].z = -(2.0f * _farVal * _nearVal) / (_farVal - _nearVal);
  return result;
}

// for more details see this site some greate stuff here (this code was modified from it)
// http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
NGL_DLLEXPORT bool isPowerOfTwo (unsigned int _x) noexcept
{
 while (((_x % 2) == 0) && _x > 1) /* While x is even and > 1 */
 {
     _x /= 2;
 }
  return (_x == 1);
}

// from http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2

NGL_DLLEXPORT unsigned int nextPow2(unsigned int _x) noexcept
{
	_x -= 1;
	_x |= _x >> 1;
	_x |= _x >> 2;
	_x |= _x >> 4;
	_x |= _x >> 8;
	_x |= _x >> 16;
	return _x + 1;
}


} // end of namespace

//----------------------------------------------------------------------------------------------------------------------



