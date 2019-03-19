#pragma once
#include "Vec3.h"
#include "Surface.h"
#include <string>

class ColorEffect {
public:
	// Vertex class moved here
	class Vertex {
	public:
		Vertex() = default;
		Vertex(const Vecf3& pos)
			:
			pos(pos)
		{}
		Vertex(const Vecf3& pos, const Vecf3& color)
			:
			pos(pos),
			color(color)
		{}
		// new copy constructor to copy all but pos
		Vertex(const Vecf3 pos_in, const Vertex& vert_in)
			:
			pos(pos_in),
			color(vert_in.color)
		{
		}
		Vertex operator-(const Vertex& rhs) const {
			return Vertex(pos - rhs.pos, color - rhs.color);
		}
		Vertex& operator/(float val) {
			pos = pos / val;
			color = color / val;
			return *this;
		}
		Vertex operator+(const Vertex& rhs) const {
			Vecf3 temppos = pos + rhs.pos;
			Vecf3 tempcolor = color + rhs.color;
			Vertex temp = { temppos, tempcolor };
			return temp;
		}
		Vertex operator+=(const Vertex& rhs) {
			return *this + rhs;
		}
		Vertex operator*(float val) const {
			return Vertex(pos * val, color * val);
		}
	public:
		Vecf3 pos;
		Vecf3 color;
	private:
	};

	class PixelShader {
	public:
		template<class Input>
		// overload () operator to return a color from input, same as previously in pipeline
		Color operator()(const Input& input) const {
			return Color(input.color);
		}
	};
	// member variables of TextureEffect
public:
	PixelShader pixelShader;
};