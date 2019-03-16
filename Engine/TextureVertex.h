#pragma once

#include "Vec3.h"
#include "Vec2.h"

class TextureVertex {
public:
	TextureVertex(const Vecf3& pos, const Vecf2& texpos) 
		:
		pos(pos),
		texpos(texpos)
	{

	}
	TextureVertex operator-(const TextureVertex& rhs) const {
		return TextureVertex( pos - rhs.pos, texpos - rhs.texpos );
	}
	TextureVertex& operator/(float val) {
		pos = pos / val;
		texpos = texpos / val;
		return *this;
	}
	TextureVertex& operator+(const TextureVertex& rhs) {
		pos = pos + rhs.pos;
		texpos = texpos + rhs.texpos;
		return *this;
	}
	TextureVertex& operator+=(const TextureVertex& rhs) {
		return *this + rhs;
	}
	TextureVertex operator*(float val) const {
		return TextureVertex( pos * val, texpos * val );
	}
	// v0 interpolate v2
	TextureVertex InterpolateTo(const TextureVertex& v2, const float& dy) const {
		// get change in y
		const float _dy = v2.pos.y - pos.y;
		// get dx/dy
		const float _dxoverdy = (v2.pos.x - pos.x) / _dy;
		// calculate posdx per unit y
		const float _dxperdy = dy * _dxoverdy;
		// get texx per unit y
		const Vecf2 _texdxperdy = ((v2.texpos - texpos) / _dy) * dy;

		return { {pos.x + _dxperdy,pos.y + dy,pos.z},{texpos.x + _texdxperdy.x, texpos.y + _texdxperdy.y} };
		
	}

public:
	Vecf3 pos;
	Vecf2 texpos;
};
