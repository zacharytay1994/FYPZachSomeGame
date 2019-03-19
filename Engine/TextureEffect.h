#pragma once
#include "Vec3.h"
#include "Surface.h"
#include <string>

class TextureEffect {
public:
	// Vertex class moved here
	class Vertex {
	public:
		Vertex() = default;
		Vertex(const Vecf3& pos, const Vecf2& texpos)
			:
			pos(pos),
			texpos(texpos)
		{}
		// new copy constructor to copy all but pos
		Vertex(const Vecf3 pos_in, const Vertex& vert_in) 
			:
			pos(pos_in),
			texpos(vert_in.texpos)
		{
		}
		Vertex operator-(const Vertex& rhs) const {
			return Vertex(pos - rhs.pos, texpos - rhs.texpos);
		}
		Vertex& operator/(float val) {
			pos = pos / val;
			texpos = texpos / val;
			return *this;
		}
		Vertex operator+(const Vertex& rhs) const {
			Vecf3 temppos = pos + rhs.pos;
			Vecf2 temptexpos = texpos + rhs.texpos;
			Vertex temp = { temppos, temptexpos };
			return temp;
		}
		Vertex operator+=(const Vertex& rhs) {
			return *this + rhs;
		}
		Vertex operator*(float val) const {
			return Vertex(pos * val, texpos * val);
		}
	public:
		Vecf3 pos;
		Vecf2 texpos;
	private:
	};
	
	class PixelShader {
	public:
		template<class Input>
		// overload () operator to return a color from input, same as previously in pipeline
		Color operator()(const Input& input) const {
			return texture->GetPixel(
				(int)std::min(input.texpos.x * tex_width, width_clamp),
				(int)std::min(input.texpos.y * tex_height, height_clamp));
		}
		// bind texture
		void BindTexture(const std::string& filename) {
			texture = std::make_unique<Surface>(Surface(filename));
			tex_width = (float)texture->GetWidth();
			tex_height = (float)texture->GetHeight();
			width_clamp = tex_width - 1.0f;
			height_clamp = tex_height - 1.0f;
		}
		// member variables of pixelshader
	private:
		std::unique_ptr<Surface> texture;
		float tex_width;
		float tex_height;
		float width_clamp;
		float height_clamp;
	};
// member variables of TextureEffect
public:
	PixelShader pixelShader;
};