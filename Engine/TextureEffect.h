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
		class Output {
		public:
			Output() = default;
			Output(const Vecf3& pos, const Vecf2& texpos, const Vecf3& normal)
				:
				pos(pos),
				texpos(texpos),
				normal(normal)
			{}
			// new copy constructor to copy all but pos
			Output(const Vecf3 pos_in, const Vertex& vert_in, Vecf3 normal)
				:
				pos(pos_in),
				texpos(vert_in.texpos),
				normal(normal)
			{
			}
			Output operator-(const Output& rhs) const {
				return Output(pos - rhs.pos, texpos - rhs.texpos, normal - rhs.normal);
			}
			Output& operator/(float val) {
				pos = pos / val;
				texpos = texpos / val;
				return *this;
			}
			Output operator+(const Output& rhs) const {
				Vecf3 temppos = pos + rhs.pos;
				Vecf2 temptexpos = texpos + rhs.texpos;
				Output temp = { temppos, temptexpos, normal + rhs.normal };
				return temp;
			}
			Output operator+=(const Output& rhs) {
				return *this + rhs;
			}
			Output operator*(float val) const {
				return Output(pos * val, texpos * val, normal);
			}
		public:
			Vecf3 pos;
			Vecf2 texpos;
			Vecf3 normal;
		};
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
			Vecf3 tangenty1 = { tempPos.x, tempPos.y, amplitude * -abs(std::cos(time * freqScroll + tempPos.x * frequency)) };
			tempPos.y = tempPos.y + (amplitude * std::sin(time * freqScroll + tempPos.z * frequency));
			Vecf3 tangenty2 = { tempPos.x, tempPos.y, amplitude * -abs(std::cos(time * freqScroll + tempPos.z * frequency)) };
			Vecf3 normal = tangenty1 % tangenty2;
			tempPos.x = tempPos.x + (amplitude * std::sin(time * freqScroll + tempPos.y * frequency));
			Vecf3 tangentx1 = { tempPos.x, tempPos.y, amplitude * -abs(std::cos(time * freqScroll + tempPos.y * frequency)) };
			tempPos.x = tempPos.x + (amplitude * std::sin(time * freqScroll + tempPos.z * frequency));
			Vecf3 tangentx2 = { tempPos.x, tempPos.y, amplitude * -abs(std::cos(time * freqScroll + tempPos.z * frequency)) };
			Vecf3 normal2 = tangentx1 % tangentx2;
			tempPos.z = tempPos.z + (amplitude * std::sin(time * freqScroll + tempPos.x * frequency));
			Vecf3 tangentz1 = { tempPos.x, tempPos.y, amplitude * -abs(std::cos(time * freqScroll + tempPos.x * frequency)) };
			tempPos.z = tempPos.z + (amplitude * std::sin(time * freqScroll + tempPos.y * frequency));
			Vecf3 tangentz2 = { tempPos.x, tempPos.y, amplitude * -abs(std::sin(time * freqScroll + tempPos.y * frequency)) };
			Vecf3 normal3 = tangentz1 % tangentz2;

			Vecf3 resultantNormal = normal + normal2 + normal3;
			return { tempPos, vertex_in, resultantNormal.GetNormalized() };
		}
	private:
		Matf3 rotation;
		Vecf3 translation;
		float time;
		float frequency = 8.0f;
		float amplitude = 0.10f;
		float freqScroll = 2.0f;
	};

	class GeomShader {
	public:
		class Output {
		public:
			Output() = default;
			Output(const Vecf3& pos, const Vecf2& texpos, const float& intensity, const Vecf3& normal)
				:
				pos(pos),
				texpos(texpos),
				intensity(intensity),
				normal(normal)
			{}
			// new copy constructor to copy all but pos
			Output(const Vecf3 pos_in, const Output& vert_in)
				:
				pos(pos_in),
				texpos(vert_in.texpos),
				intensity(vert_in.intensity),
				normal(vert_in.normal)
			{
			}
			Output operator-(const Output& rhs) const {
				return Output(pos - rhs.pos, texpos - rhs.texpos, intensity - rhs.intensity, normal);
			}
			Output& operator/(float val) {
				pos = pos / val;
				texpos = texpos / val;
				intensity = intensity / val;
				return *this;
			}
			Output operator+(const Output& rhs) const {
				Vecf3 temppos = pos + rhs.pos;
				Vecf2 temptexpos = texpos + rhs.texpos;
				float tempintensity = intensity + rhs.intensity;
				Output temp = { temppos, temptexpos, tempintensity, normal };
				return temp;
			}
			Output operator+=(const Output& rhs) {
				return *this + rhs;
			}
			Output operator*(float val) const {
				return Output(pos * val, texpos * val, intensity * val, normal);
			}
		public:
			Vecf3 pos;
			Vecf2 texpos;
			Vecf3 normal;
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
			// vertex normal gouraud shading
			float v0intensityIn = std::min(1.0f, std::max(0.0f, (-directionLight * v0.normal)) * diffuseLight + ambientLight);
			float v1intensityIn = std::min(1.0f, std::max(0.0f, (-directionLight * v1.normal)) * diffuseLight + ambientLight);
			float v2intensityIn = std::min(1.0f, std::max(0.0f, (-directionLight * v2.normal)) * diffuseLight + ambientLight);
			// face normal flat shading
			//float intensityIn = std::min(1.0f, std::max(0.0f, (-directionLight * surfaceNormal)) * diffuseLight + ambientLight);
			return { {v0out, v0.texpos, v0intensityIn, v0.normal}, {v1out, v1.texpos, v1intensityIn, v1.normal}, {v2out, v2.texpos, v2intensityIn, v2.normal} };
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