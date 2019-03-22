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

	class GeomShader {
	public:
		class Output {
		public:
			Output() = default;
			Output(const Vecf3& pos, const Vecf2& texpos, const float& intensity)
				:
				pos(pos),
				texpos(texpos),
				intensity(intensity)
			{}
			// new copy constructor to copy all but pos
			Output(const Vecf3 pos_in, const Vertex& vert_in)
				:
				pos(pos_in),
				texpos(vert_in.texpos)
			{
			}
			Output operator-(const Output& rhs) const {
				return Output(pos - rhs.pos, texpos - rhs.texpos, rhs.intensity);
			}
			Output& operator/(float val) {
				pos = pos / val;
				texpos = texpos / val;
				return *this;
			}
			Output operator+(const Output& rhs) const {
				Vecf3 temppos = pos + rhs.pos;
				Vecf2 temptexpos = texpos + rhs.texpos;
				Output temp = { temppos, temptexpos, intensity };
				return temp;
			}
			Output operator+=(const Output& rhs) {
				return *this + rhs;
			}
			Output operator*(float val) const {
				return Output(pos * val, texpos * val, intensity);
			}
		public:
			Vecf3 pos;
			Vecf2 texpos;
			float intensity;
		};
	public:
		Triangle<Output> operator()(const VertexShader::Output& v0, const VertexShader::Output& v1, const VertexShader::Output& v2, unsigned int triangleIndex) const {
			// calculate surface normal
			Vecf3 surfaceNormal = ((v1.pos - v0.pos) % (v2.pos - v0.pos)).GetNormalized();
			// exploding stuff
			/*Vecf3 v0out = v0.pos + surfaceNormal * 0.2 * abs(sin(time / 2));
			Vecf3 v1out = v1.pos + surfaceNormal * 0.2 * abs(sin(time / 2));
			Vecf3 v2out = v2.pos + surfaceNormal * 0.2 * abs(sin(time / 2));*/
			Vecf3 v0out = v0.pos;
			Vecf3 v1out = v1.pos;
			Vecf3 v2out = v2.pos;
			// calculating intensity based on angle
			float intensityIn = std::min(1.0f, std::max(0.0f, (-directionLight * surfaceNormal)) * diffuseLight + ambientLight);
			return { {v0out, v0.texpos, intensityIn}, {v1out, v1.texpos, intensityIn}, {v2out, v2.texpos, intensityIn} };
		}
		void BindTime(float t) {
			time = t;
		}
	private:
		float time;
		// direction vector of light
		Vecf3 directionLight = { 0.0f, 0.0f, 1.0f };
		// intensity of light, full stream white 100%
		float diffuseLight = 1.0f;
		// ambient light around assumed
		float ambientLight = 0.2f;
	};

	class PixelShader {
	public:
		template<class Input>
		// overload () operator to return a color from input, same as previously in pipeline
		Color operator()(const Input& input) const {
			Vecf3 colorReturn = (Vecf3)texture->GetPixel(
				(int)std::min(input.texpos.x * tex_width, width_clamp),
				(int)std::min(input.texpos.y * tex_height, height_clamp));
			return (Color)(colorReturn * input.intensity);
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
	VertexShader vertexShader;
	GeomShader geomShader;
};