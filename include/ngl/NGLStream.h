/*
  Copyright (C) 2013 Jon Macey

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
/// @file NGLStream.h
/// @brief stream helpers for ngl data types

#include <iostream>
#include "Types.h"
namespace ngl
{
  class Light;
  class Plane;
  class Colour;
  class TransformStack;
  class Quaternion;
  class AABB;
  class BBox;
  class Camera;
  class BezierCurve;
  class Material;
  class PathCamera;
  class SpotLight;
  class Transformation;
 //----------------------------------------------------------------------------------------------------------------------
 /// @brief << extraction operator to prints out the colour in the format [r,g,b,a]
 /// @param[in] _output the outstream class pointer
 /// @param[in] _c the camera to print out
 //----------------------------------------------------------------------------------------------------------------------
 NGL_DLLEXPORT std::ostream& operator<<(std::ostream& _output, const Camera& _c) noexcept;
 //----------------------------------------------------------------------------------------------------------------------
 /// @brief << extraction operator to prints out the colour in the format [r,g,b,a]
 /// @param[in] _output the outstream class pointer
 /// @param[in] _s the colour to print out
 //----------------------------------------------------------------------------------------------------------------------
 NGL_DLLEXPORT  std::ostream&  operator<<( std::ostream& _output, const Colour& _s ) noexcept;
 //----------------------------------------------------------------------------------------------------------------------
 /// @brief >> insertion operator to read in the colour data r -> g -> b -> a
 /// @param[in] _input the instream class pointer
 /// @param[in] _s the colour to read in to
 //----------------------------------------------------------------------------------------------------------------------
 NGL_DLLEXPORT  std::istream& operator>>( std::istream& _input, Colour& _s ) noexcept;

 //----------------------------------------------------------------------------------------------------------------------
 /// @brief  insertion operator to write matrix to stream
 /// @param[in] _output the stream to write to
 /// @param[in] _m the matrix-stack to write
 //----------------------------------------------------------------------------------------------------------------------
 NGL_DLLEXPORT std::ostream& operator<<(std::ostream& _output,  TransformStack& _m) noexcept;



} // end of namespace ngl
