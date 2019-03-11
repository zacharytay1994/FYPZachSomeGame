/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	Vec2.h																				  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once

template <typename T>
class Vec2
{
public:
	Vec2()
	{}
	Vec2( T x,T y )
		:
		x( x ),
		y( y )
	{}
	Vec2( const Vec2& vect )
		:
		Vec2( vect.x,vect.y )
	{}
	template <typename T2>
	explicit operator Vec2<T2>() const
	{
		return{ (T2)x,(T2)y };
	}
	T		LenSq() const
	{
		return sq( *this );
	}
	T		Len() const
	{
		return sqrt( LenSq() );
	}
	Vec2&	Normalize()
	{
		const T length = Len();
		x /= length;
		y /= length;
		return *this;
	}
	Vec2	GetNormalized() const
	{
		Vec2 norm = *this;
		norm.Normalize();
		return norm;
	}
	Vec2	operator-() const
	{
		return Vec2( -x,-y );
	}
	Vec2&	operator=( const Vec2 &rhs )
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}
	Vec2&	operator+=( const Vec2 &rhs )
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	Vec2&	operator-=( const Vec2 &rhs )
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
	Vec2	operator+( const Vec2 &rhs ) const
	{
		return Vec2( *this ) += rhs;
	}
	Vec2	operator-( const Vec2 &rhs ) const
	{
		return Vec2( *this ) -= rhs;
	}
	Vec2&	operator*=( const T &rhs )
	{
		x *= rhs;
		y *= rhs;
		return *this;
	}
	Vec2	operator*( const T &rhs ) const
	{
		return Vec2( *this ) *= rhs;
	}
	Vec2&	operator/=( const T &rhs )
	{
		x /= rhs;
		y /= rhs;
		return *this;
	}
	Vec2	operator/( const T &rhs ) const
	{
		return Vec2( *this ) /= rhs;
	}
	bool	operator==( const Vec2 &rhs ) const
	{
		return x == rhs.x && y == rhs.y;
	}
	bool	operator!=( const Vec2 &rhs ) const
	{
		return !(*this == rhs);
	}
public:
	T x;
	T y;
};

typedef Vec2<float> Vecf2;
typedef Vec2<double> Vecd2;
typedef Vec2<int> Veci2;