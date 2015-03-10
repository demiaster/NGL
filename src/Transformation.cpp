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
#include "ShaderLib.h"
#include "Transformation.h"
#include <glm/gtc/matrix_transform.hpp>
//----------------------------------------------------------------------------------------------------------------------
/// @file Transformation.cpp
/// @brief implementation files for Transformation class
//----------------------------------------------------------------------------------------------------------------------
namespace ngl
{

// Constructor -------------------------------------------------------------------------------------------------------------------
Transformation::Transformation() noexcept
{
  m_position = glm::vec3(0.0f,0.0f,0.0f);
  m_scale = glm::vec3(1.0f,1.0f,1.0f);
  m_rotation = glm::vec3(0.0f,0.0f,0.0f);
  m_isMatrixComputed = false;
  m_matrix=glm::mat4(1.0f);
  computeMatrices();

}

Transformation::Transformation(const Transformation &_t) noexcept
{
  //m_isMatrixComputed=false;

  this->m_position=_t.m_position;
  this->m_scale = _t.m_scale;
  this->m_rotation = _t.m_rotation;
  this->m_isMatrixComputed = true;
  this->m_matrix=_t.m_matrix;

}

Transformation & Transformation::operator =(const Transformation &_t) noexcept
{
  //m_isMatrixComputed=false;

  this->m_position=_t.m_position;
  this->m_scale = _t.m_scale;
  this->m_rotation = _t.m_rotation;
  this->m_isMatrixComputed = true;
  this->m_matrix=_t.m_matrix;
  return *this;
}




void Transformation::setMatrix(const glm::mat4 &_m) noexcept
{
  m_matrix=_m;
  m_isMatrixComputed = true;
}

// Set scale ---------------------------------------------------------------------------------------------------------------------
void Transformation::setScale( const glm::vec3& _scale) noexcept
{
  m_scale = _scale;
  m_isMatrixComputed = false;
}

void Transformation::setScale(const glm::vec4 &_scale) noexcept
{
  m_scale = glm::vec3(_scale.x,_scale.y,_scale.z);
  m_isMatrixComputed = false;
}

void Transformation::setScale(Real _x, Real _y, Real _z) noexcept
{
  m_scale=glm::vec3(_x,_y,_z);
  m_isMatrixComputed = false;
}

// add scale ---------------------------------------------------------------------------------------------------------------------
void Transformation::addScale( const glm::vec3& _scale ) noexcept
{
  m_scale += _scale;
  m_isMatrixComputed = false;
}


void Transformation::addScale(Real _x,  Real _y,  Real _z ) noexcept
{
  m_scale.x+=_x;
  m_scale.y+=_y;
  m_scale.z+=_z;

  m_isMatrixComputed = false;
}

// Set position --------------------------------------------------------------------------------------------------------------------
void Transformation::setPosition(const glm::vec4 &_position) noexcept
{
  m_position = glm::vec3(_position.x,_position.y,_position.z);
  m_isMatrixComputed = false;
}
void Transformation::setPosition(const glm::vec3 &_position) noexcept
{
  m_position = _position;
  m_isMatrixComputed = false;
}
void Transformation::setPosition(Real _x, Real _y, Real _z) noexcept
{
  m_position=glm::vec3(_x,_y,_z);
  m_isMatrixComputed = false;
}

// Set position --------------------------------------------------------------------------------------------------------------------
void Transformation::addPosition( const glm::vec3& _position) noexcept
{
  m_position+= _position;
  m_isMatrixComputed = false;
}
void Transformation::addPosition( Real _x, Real _y, Real _z) noexcept
{
  m_position.x+=_x;
  m_position.y+=_y;
  m_position.z+=_z;

  m_isMatrixComputed = false;
}


// set rotation -------------------------------------------------------------------------------------------------------------------
void Transformation::setRotation( const glm::vec3 &_rotation) noexcept
{
  m_rotation = _rotation;
  m_isMatrixComputed = false;
}
void Transformation::setRotation( const glm::vec4 &_rotation) noexcept
{
  m_rotation = glm::vec3(_rotation.x,_rotation.y,_rotation.z);
  m_isMatrixComputed = false;
}


void Transformation::setRotation(Real _x, Real _y,  Real _z) noexcept
{
  m_rotation=glm::vec3(_x,_y,_z);

  m_isMatrixComputed = false;
}


// set rotation -------------------------------------------------------------------------------------------------------------------
void Transformation::addRotation(const glm::vec3 &_rotation) noexcept
{
  m_rotation+= _rotation;
  m_isMatrixComputed = false;
}
void Transformation::addRotation(Real _x, Real _y, Real _z) noexcept
{
  m_rotation.x+=_x;
  m_rotation.y+=_y;
  m_rotation.z+=_z;
  m_isMatrixComputed = false;
}


// reset matrix ---------------------------------------------------------------------------------------------------------------------
void Transformation::reset() noexcept
{
  m_position = glm::vec3(0.0f,0.0f,0.0f);
  m_scale = glm::vec3(1.0f,1.0f,1.0f);
  m_rotation = glm::vec3(0.0f,0.0f,0.0f);
  m_isMatrixComputed = false;
  computeMatrices();
}

// comptue matrix ---------------------------------------------------------------------------------------------------------------------
void Transformation::computeMatrices() noexcept
{
  if (!m_isMatrixComputed)       // need to recalculate
  {
    glm::mat4 scale;
    glm::mat4 rX;
    glm::mat4 rY;
    glm::mat4 rZ;
    glm::mat4 trans;

    // rotation/scale matrix
    glm::mat4 rotationScale;

    scale=glm::scale(glm::mat4(1.0f),glm::vec3(m_scale.x, m_scale.y, m_scale.z));
    rX=glm::rotate(rX,m_rotation.x,glm::vec3(1.0f,0.0f,0.0f));
    rY=glm::rotate(rY,m_rotation.y,glm::vec3(0,1,0));
    rZ=glm::rotate(rZ,m_rotation.z,glm::vec3(0,0,1));


    rotationScale = scale * rX * rY * rZ;

    // transform matrix
    m_matrix = rotationScale;
    m_matrix[3][0] = m_position.x;
    m_matrix[3][1] = m_position.y;
    m_matrix[3][2] = m_position.z;
    m_matrix[3][3] = 1;



    m_isMatrixComputed = true;
  }
}

void Transformation::operator*= ( const Transformation &_m ) noexcept
{
  m_isMatrixComputed=false;

  computeMatrices();
  m_matrix*=_m.m_matrix;

}

Transformation Transformation::operator*(const Transformation &_m  ) noexcept
{
  m_isMatrixComputed=false;
  computeMatrices();
  Transformation t;
  t.m_matrix=m_matrix*_m.m_matrix;

  return t;
}
void Transformation::loadMatrixToShader(const std::string &_param, const ACTIVEMATRIX &_which ) noexcept
{
  computeMatrices();
  ShaderLib *shader=ShaderLib::instance();
  shader->setShaderParamFromMat4(_param,m_matrix);

}


void Transformation::loadGlobalAndCurrentMatrixToShader(const std::string &_param, Transformation &_globalL) noexcept
{
  computeMatrices();
  ShaderLib *shader=ShaderLib::instance();
  glm::mat4 tx=_globalL.getMatrix()*this->getMatrix();

}


} // end ngl
