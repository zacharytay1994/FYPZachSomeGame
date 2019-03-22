#pragma once
#include "Vec3.h"
#include "Surface.h"
#include "DefaultVertexShader.h"
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
	
	//typedef DefaultVertexShader<Vertex> VertexShader;
	class VertexShader {
	public:
		typedef Vertex Output;
	public:
		void BindRotation(const Matf3& rotation_in) {
			rotation = rotation_in;
		}
		void BindTranslation(const Vecf3& translation_in) {
			translation = translation_in;
		}
		void SetTime(float t) {
			time = t;
		}
		Output operator()(const Vertex& vertex_in) const {
			Vecf3 tempPos = vertex_in.pos * rotation + translation;
			tempPos.y = tempPos.y + (amplitude * std::sin(time * freqScroll + tempPos.x * frequency));
			return { tempPos, vertex_in };
		}
	private:
		Matf3 rotation;
		Vecf3 translation;
		float time;
		float frequency = 10.0f;
		float amplitude = 0.15f;
		float freqScroll = 5.0f;
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

	class GeomShader {
	public:
		typedef VertexShader::Output Output;
	public:
		Triangle<Output> operator()(const VertexShader::Output& v0, const VertexShader::Output& v1, const VertexShader::Output& v2, unsigned int triangleIndex) const {
			Vecf3 surfaceNormal = (v1.pos - v0.pos) % (v2.pos - v0.pos);
			Vecf3 v0out = v0.pos + surfaceNormal * 7 * abs(sin(time));
			Vecf3 v1out = v1.pos + surfaceNormal * 7 * abs(sin(time));
			Vecf3 v2out = v2.pos + surfaceNormal * 7 * abs(sin(time));
			return { {v0out, v0.texpos}, {v1out, v1.texpos}, {v2out, v2.texpos} };
		}
		void BindTime(float t) {
			time = t;
		}
	private:
		float time;
	};
// member variables of TextureEffect
public:
	PixelShader pixelShader;
	VertexShader vertexShader;
	GeomShader geomShader;
};