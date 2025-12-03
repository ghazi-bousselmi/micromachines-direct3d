#ifndef _3DUTILS_INCLUDED

#define _3DUTILS_INCLUDED

#include "d3d8.h"
#include "d3dx8mesh.h"
#include "d3d8types.h"		
#include "d3dx8tex.h"


inline D3DVECTOR abs(D3DVECTOR& v)
{
	D3DVECTOR v1;
	v1.x=float(fabs(v.x));
	v1.y=float(fabs(v.y));
	v1.z=float(fabs(v.z));
	return v1;
}

inline D3DVECTOR operator *(D3DVECTOR& v,D3DXMATRIX& m)
{
	D3DVECTOR v1;
	v1.x=v.x*m._11+v.y*m._21+v.z*m._31+m._41;
	v1.y=v.x*m._12+v.y*m._22+v.z*m._32+m._42;
	v1.z=v.x*m._13+v.y*m._23+v.z*m._33+m._43;
	return v1;
}

inline D3DVECTOR operator *(float f,D3DVECTOR& v)
{
	D3DVECTOR v1;
	v1.x=v.x*f;
	v1.y=v.y*f;
	v1.z=v.z*f;
	return v1;
}

inline D3DVECTOR operator *(D3DVECTOR& v,float f)
{
	D3DVECTOR v1;
	v1.x=v.x*f;
	v1.y=v.y*f;
	v1.z=v.z*f;
	return v1;
}

inline float operator *(D3DVECTOR& v1,D3DVECTOR& v2)
{
	float f;
	f=v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
	return f;
}

inline D3DVECTOR operator /(D3DVECTOR& v,float f)
{
	D3DVECTOR v1;
	v1.x=v.x/f;
	v1.y=v.y/f;
	v1.z=v.z/f;
	return v1;
}

inline float operator ~(D3DVECTOR v)
{
	return (float)sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

inline D3DVECTOR operator ^(D3DVECTOR v1,D3DVECTOR v2)
{
	D3DVECTOR v;
	v.x=v1.y*v2.z-v1.z*v2.y;
	v.y=v1.z*v2.x-v1.x*v2.z;
	v.z=v1.x*v2.y-v1.y*v2.x;
	return v;
}

inline D3DVECTOR operator +(D3DVECTOR v1,D3DVECTOR v2)
{
	D3DVECTOR v;
	v.x=v1.x+v2.x;
	v.y=v1.y+v2.y;
	v.z=v1.z+v2.z;
	return v;
}

inline D3DVECTOR operator -(D3DVECTOR v1,D3DVECTOR v2)
{
	D3DVECTOR v;
	v.x=v1.x-v2.x;
	v.y=v1.y-v2.y;
	v.z=v1.z-v2.z;
	return v;
}


inline D3DVECTOR D3DVEC(float x, float y , float z )
{
	D3DVECTOR v;
	v.x=x;
	v.y=y;
	v.z=z;
	return v;
}

inline D3DVECTOR operator +=(D3DVECTOR& v,D3DVECTOR& m)
{
	D3DVECTOR v1;
	v1.x=v.x+m.x;
	v1.y=v.y+m.y;
	v1.z=v.z+m.z;
	return v1;
}

inline D3DVECTOR operator -=(D3DVECTOR& v,D3DVECTOR& m)
{
	D3DVECTOR v1;
	v1.x=v.x-m.x;
	v1.y=v.y-m.y;
	v1.z=v.z-m.z;
	return v1;
}







#endif