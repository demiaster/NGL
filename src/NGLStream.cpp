#include "Camera.h"
#include "Colour.h"

#include <iostream>
#include <limits>
#include <iomanip>
namespace ngl
{



//----------------------------------------------------------------------------------------------------------------------
NGL_DLLEXPORT std::ostream& operator<<(std::ostream& _output, const Camera &_c) noexcept
{
}

//----------------------------------------------------------------------------------------------------------------------
NGL_DLLEXPORT  std::ostream&  operator<<( std::ostream &_output, const Colour& _s) noexcept
{
 return _output<<"["<<_s.m_r<<","<<_s.m_g<<","<<_s.m_b<<","<<_s.m_a<<"]";
}

//----------------------------------------------------------------------------------------------------------------------

NGL_DLLEXPORT  std::istream& operator>>( std::istream &_input, Colour &_s) noexcept
{
 _input >> _s.m_r >> _s.m_g >> _s.m_b >> _s.m_a;
 return _input;
}



}
