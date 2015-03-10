/*
  Copyright (C) 2011 Jon Macey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received m_a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Plane.h"
#include <boost/format.hpp>
#include <glm/gtc/matrix_transform.hpp>
//----------------------------------------------------------------------------------------------------------------------
/// @file Plane.cpp
/// @brief implementation files for Plane class
//----------------------------------------------------------------------------------------------------------------------
namespace ngl
{

//----------------------------------------------------------------------------------------------------------------------
Plane::Plane() noexcept
{

}
//----------------------------------------------------------------------------------------------------------------------
Plane::Plane( const glm::vec3 &_v1, const glm::vec3 &_v2,  const glm::vec3 &_v3) noexcept
{
  setPoints(_v1,_v2,_v3);
}

//----------------------------------------------------------------------------------------------------------------------
Plane::~Plane() noexcept
{

}
//----------------------------------------------------------------------------------------------------------------------
void Plane::setPoints(const glm::vec3 &_v1, const glm::vec3 &_v2, const glm::vec3 &_v3) noexcept
{
	glm::vec3 aux1, aux2;

	aux1 = _v1 - _v2;
	aux2 = _v3 - _v2;
    m_normal =  glm::normalize(glm::cross(aux2,aux1));

	m_point=_v2;
    m_d = -glm::dot(m_normal,m_point);
}
//----------------------------------------------------------------------------------------------------------------------
void Plane::setNormalPoint( const glm::vec3 &_normal, const glm::vec3 &_point) noexcept
{
	m_point=_point;
    m_normal=glm::normalize(_normal);

    m_d = -( glm::dot(m_normal,m_point) );
}
//----------------------------------------------------------------------------------------------------------------------
void Plane::setFloats(Real _a,Real _b,	Real _c,	Real _d) noexcept
{
	// set the normal vector
    m_normal=glm::vec3(_a,_b,_c);
	//compute the lenght of the vector
    Real l = glm::length(m_normal);
	// normalize the vector
    m_normal=glm::normalize(m_normal);
	// and divide d by th length as well
	m_d = _d/l;
}

//----------------------------------------------------------------------------------------------------------------------
Real Plane::distance( const glm::vec3 &_p) const noexcept
{
    return (m_d +  glm::dot(m_normal,_p));
}

} // end of ngl namespace
