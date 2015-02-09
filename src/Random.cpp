/*
  Copyright (C) 2009 Rob Bateman / Jon Macey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITH ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this progra_m.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Random.h"
#include <cstdlib> // for time
#include <ctime>
//----------------------------------------------------------------------------------------------------------------------
/// @file Random.cpp
/// @brief implementation files for Random class
//----------------------------------------------------------------------------------------------------------------------
namespace ngl
{

//----------------------------------------------------------------------------------------------------------------------
void Random::setSeed() noexcept
{
  m_generator.seed(static_cast<unsigned int>(std::time(0)));
}

//----------------------------------------------------------------------------------------------------------------------
void Random::setSeed(int _value) noexcept
{
  m_generator.seed(_value);
}


//----------------------------------------------------------------------------------------------------------------------
Random::Random() noexcept
{
  // we have two default generators built in

  // first create a simple uniform real distrib
  boost::uniform_real<Real> MinusPlusOneFloatDistrib(-1.0, 1.0);
  // create the generator
  boost::variate_generator<ENGINE &, boost::uniform_real<Real> >RandFloat(m_generator,MinusPlusOneFloatDistrib);
  // now assign the resultant function to the map for later use
  m_floatGenerators["RandomFloat"] =RandFloat;
  // same for below but using 0-1 for distrib
  boost::uniform_real<Real> ZeroOneFloatDistrib(0.0, 1.0);
  boost::variate_generator<ENGINE &, boost::uniform_real<Real> >RandPosFloat(m_generator,ZeroOneFloatDistrib);
  m_floatGenerators["RandomPositiveFloat"] =RandPosFloat;


}


//----------------------------------------------------------------------------------------------------------------------
Real Random::getFloatFromGeneratorName(const std::string &_name) noexcept
{
  // grab a function pointer based on the _name from the map
  boost::function <Real (void)> func=m_floatGenerators[_name];
  // see if we got anything we can use
  if(func!=0)
  {
    // if it exists execute the function and return the value
    return func();
  }
  else
  {
    // otherwise we return the safest possible value 0
    return 0;
  }
}


//----------------------------------------------------------------------------------------------------------------------
Colour Random::getRandomColour() noexcept
{
  // get our positive gen function and assign valus to a colour (alpha =1)
  boost::function <Real (void)> gen=m_floatGenerators["RandomPositiveFloat"];
  return Colour(gen(),gen(),gen());
}

//----------------------------------------------------------------------------------------------------------------------
Colour Random::getRandomColourAndAlpha() noexcept
{
	// get our positive gen function and assign valus to a colour with rand alpha

	boost::function <Real (void)> gen=m_floatGenerators["RandomPositiveFloat"];
	return Colour(gen(),gen(),gen(),gen());
}

//----------------------------------------------------------------------------------------------------------------------
Vec4 Random::getRandomVec4() noexcept
{
	boost::function <Real (void)> gen=m_floatGenerators["RandomFloat"];
	return Vec4(gen(),gen(),gen(),0.0f);
}

//----------------------------------------------------------------------------------------------------------------------
Vec4 Random::getRandomNormalizedVec4() noexcept
{
	boost::function <Real (void)> gen=m_floatGenerators["RandomFloat"];
	Vec4 v(gen(),gen(),gen(),0.0f);
	v.normalize();
	return v;
}


//----------------------------------------------------------------------------------------------------------------------
Vec3 Random::getRandomVec3() noexcept
{
  boost::function <Real (void)> gen=m_floatGenerators["RandomFloat"];
  return Vec3(gen(),gen(),gen());
}

//----------------------------------------------------------------------------------------------------------------------
Vec3 Random::getRandomNormalizedVec3() noexcept
{
  boost::function <Real (void)> gen=m_floatGenerators["RandomFloat"];
  Vec3 v(gen(),gen(),gen());
  v.normalize();
  return v;
}

//----------------------------------------------------------------------------------------------------------------------
Vec2 Random::getRandomVec2() noexcept
{
  boost::function <Real (void)> gen=m_floatGenerators["RandomFloat"];
  return Vec2(gen(),gen());
}

//----------------------------------------------------------------------------------------------------------------------
Vec2 Random::getRandomNormalizedVec2() noexcept
{
  boost::function <Real (void)> gen=m_floatGenerators["RandomFloat"];
  Vec2 v(gen(),gen());
  v.normalize();
  return v;
}




Vec3 Random::getRandomPoint( Real _xRange, Real _yRange,  Real _zRange) noexcept
{
  boost::function <Real (void)> gen=m_floatGenerators["RandomFloat"];
  return Vec3(gen()*_xRange,gen()*_yRange,gen()*_zRange);

}


//----------------------------------------------------------------------------------------------------------------------
Real Random::randomNumber(Real _mult) noexcept
{
	boost::function <Real (void)> gen=m_floatGenerators["RandomFloat"];
	return gen()*_mult;
}

//----------------------------------------------------------------------------------------------------------------------
Real Random::randomPositiveNumber(Real _mult) noexcept
{
	boost::function <Real (void)> gen=m_floatGenerators["RandomPositiveFloat"];
	return gen()*_mult;
}

//----------------------------------------------------------------------------------------------------------------------
void Random::addGenerator( const std::string &_name,RANDDIST _distribution, Real _min, Real _max, Real _prob) noexcept
{
  /// this is rather tediously wrapping all the generators and attaching them to a
  /// generator and creating a map for it by name.
  if(_distribution==RANDDIST::uniform_smallint)
  {
    boost::uniform_smallint<int> distrib(_min, _max);
    boost::variate_generator<ENGINE &, boost::uniform_smallint<int> >gen(m_generator,distrib);
    m_floatGenerators[_name] =gen;
  }

  else if(_distribution==RANDDIST::uniform_int)
  {
    boost::uniform_int<int> distrib(_min, _max);
    boost::variate_generator<ENGINE &, boost::uniform_int<int> >gen(m_generator,distrib);
    m_floatGenerators[_name] =gen;
  }
  else if(_distribution==RANDDIST::uniform_real)
  {
    boost::uniform_real<Real> distrib(_min, _max);
    boost::variate_generator<ENGINE &, boost::uniform_real<Real> >gen(m_generator,distrib);
    m_floatGenerators[_name] =gen;
  }
  else if(_distribution==RANDDIST::bernoulli_distribution)
  {
    boost::bernoulli_distribution<Real> distrib(_min);

    boost::variate_generator<ENGINE &, boost::bernoulli_distribution<Real> >gen(m_generator,distrib);
    m_floatGenerators[_name] =gen;
  }

  else if(_distribution==RANDDIST::binomial_distribution)
  {
    boost::binomial_distribution<> distrib(int(_min),_max);

    boost::variate_generator<ENGINE &, boost::binomial_distribution<> >gen(m_generator,distrib);
    m_floatGenerators[_name] =gen;
  }
  else if(_distribution==RANDDIST::cauchy_distribution)
  {
    boost::cauchy_distribution<Real> distrib(_min,_max);

    boost::variate_generator<ENGINE &, boost::cauchy_distribution<Real> >gen(m_generator,distrib);
    m_floatGenerators[_name] =gen;
  }
  else if(_distribution==RANDDIST::gamma_distribution)
  {
    boost::gamma_distribution<Real> distrib(_min);

    boost::variate_generator<ENGINE &, boost::gamma_distribution<Real> >gen(m_generator,distrib);
    m_floatGenerators[_name] =gen;
  }
  else if(_distribution==RANDDIST::poisson_distribution)
  {
    boost::poisson_distribution<int> distrib(_min);

    boost::variate_generator<ENGINE &, boost::poisson_distribution<int> >gen(m_generator,distrib);
    m_floatGenerators[_name] =gen;
  }

  else if(_distribution==RANDDIST::geometric_distribution)
  {
    boost::geometric_distribution<int> distrib(_min);

    boost::variate_generator<ENGINE &, boost::geometric_distribution<int> >gen(m_generator,distrib);
    m_floatGenerators[_name] =gen;
  }

  else if(_distribution==RANDDIST::triangle_distribution)
  {
    boost::triangle_distribution<Real> distrib(_min,_prob,_max);

    boost::variate_generator<ENGINE &, boost::triangle_distribution<Real> >gen(m_generator,distrib);
    m_floatGenerators[_name] =gen;
  }

  else if(_distribution==RANDDIST::exponential_distribution)
  {
    boost::exponential_distribution<Real> distrib(_min);

    boost::variate_generator<ENGINE &, boost::exponential_distribution<Real> >gen(m_generator,distrib);
    m_floatGenerators[_name] =gen;
  }

  else if(_distribution==RANDDIST::normal_distribution)
  {
    boost::normal_distribution<Real> distrib(_min,_max);

    boost::variate_generator<ENGINE &, boost::normal_distribution<Real> >gen(m_generator,distrib);
    m_floatGenerators[_name] =gen;
  }

  else if(_distribution==RANDDIST::lognormal_distribution)
  {
    boost::lognormal_distribution<Real> distrib(_min,_max);

    boost::variate_generator<ENGINE &, boost::lognormal_distribution<Real> >gen(m_generator,distrib);
    m_floatGenerators[_name] =gen;
  }

}
//----------------------------------------------------------------------------------------------------------------------

} // end of namespace

//----------------------------------------------------------------------------------------------------------------------


