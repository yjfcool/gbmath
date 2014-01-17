﻿/** \file
* \brief   float/double operations .
*
*/

#pragma once

#ifndef __GBMATH_H__
    #error  DO NOT INCLUDE THIS FILE. USE:   #include <gbmath/_gbmath.h>
#endif

 

//! \brief   math operations and structures .
namespace gbmath
{
	 



//inline bool is_nan( float a )
//{
//	bool r = a==a;
//	return !( a == a );
//}

inline bool compare(float a, float b, float epsilon)
{
   return fabs(a - b) < epsilon;
}


//! \brief Check the value the correct value
inline bool check(float f)
{
   if( f == 0.0f ) return true;
   if(f < 0.0f)	   f = -f;
   if( (f <= FLT_MAX) && (f >= FLT_MIN) ) return true;
   return false;
}

//! \brief Check the value the correct value
inline bool check(double d)
{
   if( d == 0.0 ) 
	   return true;

   if(d < 0.0) 
	   d = -d;

   if( (d <= DBL_MAX) && (d >= DBL_MIN) ) 
	   return true;

   return false;
}


	//! \brief  x*x
	inline float sqr(float x) 
	{ 
		return x*x; 
	}

	inline float abs(float val)
	{
	    if(val<0.0f)
		{
			return -val;
		}

		return val;
	}


	template<typename T>
	//! \brief   Calculate the linear interpolation
	inline T lerp( float alpha, const T& v0, const T& v1 ) throw()
	{
		return( (T)(v0 + alpha * ( v1 - v0 )) );
	}


	template<class T>
	//! \brief   reduction in the range between
	inline T clamp( const T& v, const T& l, const T& u ) throw()
	{
		return min(u, max(l,v));
	}


	template<typename T>
	//! \brief   fill the buffer dest value val number num.
	inline void fill( T * dest, const T val, const int num ) throw()
	{
		for (int i = 0; i < num; i++)
		{
			dest[i] = val;
		}
	}

// 		template<typename T, const unsigned int num>
// 		//! \brief   copy
// 		inline void copy( T * dest, const T * val, const int num ) throw()
// 		{
// 			for (int i = 0; i < num; i++)
// 			{
// 				dest[i] = val[i];
// 			}
// 		}



/**  \brief   Returns the sign of <br>
     x>0 :  1   <br>
     x=0 :  0   <br>
     x<0 : -1	  */
inline float  sign (const float val)
{
    if(val==0.0f)
	  return 0.0f;

	if(val>0.0f)
	  return 1.0f;

	return -1.0f;
}

/** \brief Returns the sign of  <br>
 x>0 :  1   <br>
 x=0 :  0   <br>
 x<0 : -1	  */
inline int sign2(float x)
{
	return  (x>0)-(x<0);
}


//! \brief Clipping values ​​in a range between minimum and maximum inclusive. 
inline float clump(float value, float _min, float _max)
{
	float r = value;
	if(r < _min) r=_min;
	if(r > _max) r=_max;
	return r;
}

//! \brief   sine and cosine.
inline void sincos(const float a, float& outSin, float& outCos) 
{  
	outSin = sin(a); 
	outCos = cos(a); 
}

#if defined (_MSC_VER)
#ifdef GBMATH_USE_ASM
//! \brief assembler way to get the sine and cosine.
inline void sincosAsm (float a, float& s, float& c)
{
	__asm  
	{
			fld a;
			fsincos;
			mov eax, [c];
			fstp [eax];
			mov eax, [s];
			fstp [eax];
	};
};
#endif 
#endif

//! \brief Calculate the arc cotangent 
inline float arccot(float f) 
{
   return atan(1.0f / f);
}

//! \brief Calculate the arc tangent
inline float arctg(float y, float x)
{
	return atan2f(y, x);
}

/** \brief  Same as acos(x), but if x is out of range, it is "clamped"
to the nearest valid value.  The value returned is in range 0...pi,
the same as the standard C acos() function   
*/
inline float safeAcos(float x)
{
	// Check limit conditions
	if (x <= -1.0f)
	{
		return PI; //   constPi <float>(); // kPi;
	}
	if (x >= 1.0f)
	{
		return 0.0f;
	}

	// Value is in the domain - use standard C function
	return acos(x);
}


//!  \brief   cotangent 
inline float cotan(const float f) 
{  
	return   1.0f / tanf(f); 
}

//! \brief   Angle in radians to degrees
inline float radiansToDegree(float rad) 
{ 
	return rad * ( 180.0f /  PI ); 
}


//! \brief Angle in degrees to radians
inline float degreeToRadians(float dgr)  
{ 
	return dgr * (  PI / 180.0f); 
}

//! \brief Normalization of the angle of rotation angle.   Cast the value in the range -PI ... + PI .
inline float normalize_angle( float angle )
{
	static  const float _PI_ = 3.1415926535898f;
	static  const float TWO_PI = 6.2831853071795865f;

	return (angle - TWO_PI *  floor( ( angle + _PI_ ) / TWO_PI) );
}

//!  \brief  Linear interpolation  
inline float lerp(const float f1, const float f2, const float k) 
{ 
	return f1 + (f2 - f1) * k; 
}

//! \brief rounding . 
inline int round(float f) 
{  
	return (int)(f + (f > 0 ? 0.5f : -0.5f));  
}

#if defined (_MSC_VER)
#ifdef GBMATH_USE_ASM

//! \brief rounding asm . 
inline int roundAsm(float a)
{
	register int retval;
	__asm fld a
	__asm fistp retval
	return retval;
}

#endif
#endif


//! \brief Get an average of 3 values 
inline float   max3 ( float a, float b, float c ) 
{ 
	return a > b ? (a > c ? a : (b > c ? b : c)) : (b > c ? b : (a > c ? a : c));  
}



//! \brief  Compute and return the average (arithmetic) 
inline float average_arithmetic(const float* pf, const unsigned int num)
{
	float r = 0.0f;
	if(num==0) 
		return r;

	for(unsigned int c=0; c<num; c++)
	{
		r +=  *( pf + c );
	}

	return r / (float)num;
}

//!  \brief  Compute and return the geometric mean 
inline float average_geometric(const float* pf, const unsigned int num)
{
	float r = 1.0f;
	if(num==0) 
		return r;

	for(unsigned int c=0; c<num; c++)
	{
		r *=  *( pf + c );
	}

	return   pow( r, 1.0f/(float)num );
}

//! \brief random values ​​in the range ( 0.0f ... 1.0f )
float random();






}
