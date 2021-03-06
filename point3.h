﻿/**  \file
 \brief  point in 3-dimensional space


*/

#pragma once

#ifndef __GBMATH_H__
    #error  DO NOT INCLUDE THIS FILE. USE:   #include <gbmath/_gbmath.h>
#endif


 
namespace gbmath
{




	/** \brief describes a point/coordinate/position in 3-dimensional space. Level higher than the vector */
	class point3 {
	public:
		float _x;
		float _y;
		float _z;

		//! \brief   set all to zero
		inline point3() 
		{ 
			_x=0.0f; 
			_y=0.0f; 
			_z=0.0f;  
		}
		
		inline point3(const point3& p) { _x=p._x; _y=p._y; _z=p._z; }
		inline point3(float x, float y, float z) {_x=x; _y=y; _z=z; }

		inline operator  const float*() const  { return &_x; };
		inline operator        float*()        { return &_x; };

		inline void operator = (const  vec3& v) { _x=v.x; _y=v.y; _z=v.z; }

		inline operator       vec3 () const { return  vec3 (_x,_y,_z);  }
		inline operator const vec3*() const { return (vec3*)&_x; }

#ifdef _D3D9_H_
		inline operator D3DVECTOR*() { return (D3DVECTOR*)&_x; }
		inline operator const D3DVECTOR*() const { return (D3DVECTOR*)&_x; }
		inline operator D3DVECTOR() const  { D3DVECTOR r; r.x=_x; r.y=_y; r.z=_z; return r;  }
		inline void operator = (const D3DVECTOR& v) {	_x=v.x; _y=v.y; _z=v.z; }
#endif


		//! \brief  set middle point
		inline point3& setMiddle(const  vec3& p1, const  vec3& p2)
		{
			_x = (p1.x + p2.x) / 2.0f;
			_y = (p1.y + p2.y) / 2.0f;
			_z = (p1.z + p2.z) / 2.0f;
			return *this;
		}


		//! \brief   set middle point
		inline point3& setMiddle(const point3& p)
		{
			_x = (_x + p._x) / 2.0f;
			_y = (_y + p._y) / 2.0f;
			_z = (_z + p._z) / 2.0f;
			return *this;
		}

		//! \brief  Going to the point posTo a distance-distance
		point3& moveTo(const point3& posTo, float distance)
		{
			 vec3 vn = ( vec3)posTo - ( vec3)*this;
			vn.normalize();
			vn *= distance;
			 _x += vn.x; _y=vn.y; _z=vn.z;
			return *this;
		}

		//! \brief Moving toward the point normal at a distance distance
		point3& moveAlongNormal(const normal3& normal, float distance)
		{
			 vec3 vn = normal;
			vn *= distance;
			_x += vn.x; _y=vn.y; _z=vn.z;
			return *this;
		}

		//! \brief  return distance between points
		inline float distanceBetween(const point3& p) const
		{
		   vec3 sub = ( vec3)*this - ( vec3)p;
		  return sub.length();
		}


		/**  \brief  Change the distance between this point and the coefficient on pnt. k.
		If k is less than 1 then the convergence is made if more then removal    */
		point3& adjustDistancePoint(const point3& pnt, const float k)
		{
			 vec3 dv  = ( vec3)pnt - ( vec3)*this;
		  float dist = dv.length();
		   normal3 n  =  dv ;
		  moveAlongNormal( n,  dist - (dist * k) );
		  return *this;
		}


		//! \brief  transform coordinate
		point3& transform(const  mat44& m)
		{
			vec3 v = *this;
		   v.transform_coord(m);
		   *this = v;
			return *this;
		}


	};


}
