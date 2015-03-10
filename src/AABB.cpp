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
#include "AABB.h"
//-----------------------------------------------------------------------------
/// @file AABB.cpp
/// @brief implementation files for AABB class
//-----------------------------------------------------------------------------
namespace ngl
{

//-----------------------------------------------------------------------------
AABB::AABB() noexcept
{
    m_corner=glm::vec4(0,0,0,1);

	m_x = 1.0f;
	m_y = 1.0f;
	m_z = 1.0f;
}
//-----------------------------------------------------------------------------
AABB::AABB(const glm::vec4 &_corner,  Real _x,  Real _y,  Real _z ) noexcept
{
 set(_corner,_x,_y,_z);
}

//-----------------------------------------------------------------------------
AABB::~AABB() noexcept
{

}
//-----------------------------------------------------------------------------
void AABB::set(const glm::vec4 &_corner,Real _x,Real _y,	Real _z	) noexcept
{
	m_corner=_corner;

	if (_x < 0.0)
	{
		_x = -_x;
        m_corner.x -= _x;
	}
	if (_y < 0.0)
	{
		_y = -_y;
        m_corner.y -= _y;
	}
	if (_z < 0.0)
	{
		_z = -_z;
        m_corner.z -= _z;
	}
	m_x = _x;
	m_y = _y;
	m_z = _z;
}

glm::vec4 AABB::getVertexP(const glm::vec4 &_normal) noexcept
{
    glm::vec4 res = m_corner;

    if (_normal.x > 0)
	{
        res.x += m_x;
	}
    if (_normal.y > 0)
	{
        res.y += m_y;
	}

    if (_normal.z > 0)
	{
        res.z += m_z;
	}
	return res;
}

glm::vec4 AABB::getVertexN(const glm::vec4 &_normal) noexcept
{
    glm::vec4 res = m_corner;

    if (_normal.x < 0)
	{
        res.x += m_x;
	}
    if (_normal.y < 0)
	{
        res.y += m_y;
	}

    if (_normal.z < 0)
	{
        res.z += m_z;
	}
	return res;
}


} // end of ngl namespace
