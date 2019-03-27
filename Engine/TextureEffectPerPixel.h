#pragma once

#include "Vec3.h"
#include "Mat3.h"
#include "Triangle.h"
#include "Surface.h"

#include <memory>
#include <string>

class TextureEffectPerPixel {
public:
	class Vertex {
	public:
		// constructors
		Vertex() = default;
		Vertex( const Vecf3& pos, const Vecf2& texpos ) 
			:
			pos(pos),
			texpos(texpos)
		{}
		Vertex(const Vecf3& pos, const Vertex& vert_in) 
			:
			pos(pos),
			texpos(vert_in.texpos)
		{}
		// operators
		Vertex operator+(const Vertex& rhs) const {
			Vecf3 temppos = pos + rhs.pos;
			Vecf2 temptexpos = texpos + rhs.texpos;
			return { temppos, temptexpos };
		}
		Vertex operator+=(const Vertex& rhs) {
			return *this + rhs;
		}
		Vertex operator-(const Vertex& rhs) const {
			Vecf3 temppos = pos - rhs.pos;
			Vecf2 temptexpos = texpos - rhs.texpos;
			return { temppos, temptexpos };
		}
		Vertex operator-=(const Vertex& rhs) {
			return *this - rhs;
		}
		Vertex operator*(float val) const {
			return Vertex(pos * val, texpos * val);
		}
		Vertex& operator/(float val) {
			pos = pos / val;
			texpos = texpos / val;
			return *this;
		}
	public:
		// attributes
		Vecf3 pos;
		Vecf2 texpos;
	};
	class VertexShader {
	public:
		class Output {
		public:
			// constructors
			Output() = default;
			Output(const Vecf3& pos, const Vecf2& texpos, const Vecf3& worldPos, const Vecf3& normal)
				:
				pos(pos),
				texpos(texpos),
				worldPos(worldPos),
				normal(normal)
			{}
			Output(const Vecf3& pos, const Vertex& vertex_in, const Vecf3& normal) 
				:
				pos(pos),
				texpos(vertex_in.texpos),
				worldPos(pos),
				normal(normal)
			{}
			// operators
			Output operator+(const Output& rhs) const {
				Vecf3 temppos = pos + rhs.pos;
				Vecf2 temptexpos = texpos + rhs.texpos;
				Vecf3 tempworldPos = worldPos + rhs.worldPos;
				Vecf3 tempnormal = normal + rhs.normal;
				return { temppos, temptexpos, tempworldPos, tempnormal };
			}
			Output operator+=(const Output& rhs) {
				return *this + rhs;
			}
			Output operator-(const Output& rhs) const {
				Vecf3 temppos = pos - rhs.pos;
				Vecf2 temptexpos = texpos - rhs.texpos;
				Vecf3 tempworldPos = worldPos - rhs.worldPos;
				Vecf3 tempnormal = normal - rhs.normal;
				return { temppos, temptexpos, tempworldPos, tempnormal };
			}
			Output operator-=(const Output& rhs) {
				return *this - rhs;
			}
			Output operator*(float val) const {
				return Output(pos * val, texpos * val, worldPos * val, normal * val);
			}
			Output& operator/(float val) {
				pos = pos / val;
				texpos = texpos / val;
				worldPos = worldPos / val;
				normal = normal / val;
				return *this;
			}
		public:
			Vecf3 pos;
			Vecf2 texpos;
			Vecf3 worldPos;
			Vecf3 normal;
		};
	public:
		Output operator()(const Vertex& vertex_in) {
			// transform from model space to world space
			Vecf3 tempPos = vertex_in.pos * rotation + translation;
			// transform vertices
			tempPos.y = tempPos.y + (amplitude * std::sin(time * freqScroll + tempPos.x * frequency));
			tempPos.y = tempPos.y + (amplitude * std::sin(time * freqScroll + tempPos.z * frequency));
			tempPos.x = tempPos.x + (amplitude * std::sin(time * freqScroll + tempPos.y * frequency));
			tempPos.x = tempPos.x + (amplitude * std::sin(time * freqScroll + tempPos.z * frequency));
			tempPos.z = tempPos.z + (amplitude * std::sin(time * freqScroll + tempPos.x * frequency));
			tempPos.z = tempPos.z + (amplitude * std::sin(time * freqScroll + tempPos.y * frequency));
			// calculate normals of vertices
			Vecf3 tangenty1 = { tempPos.x, tempPos.y, amplitude * std::cos(time * freqScroll + tempPos.x * frequency) };
			Vecf3 tangenty2 = { tempPos.y, tempPos.z, amplitude * std::cos(time * freqScroll + tempPos.z * frequency) };
			Vecf3 normal = tangenty2 % tangenty1;
			Vecf3 tangentx1 = { tempPos.x, tempPos.y, amplitude * std::cos(time * freqScroll + tempPos.y * frequency) };
			Vecf3 tangentx2 = { tempPos.z, tempPos.x, amplitude * std::cos(time * freqScroll + tempPos.z * frequency) };
			Vecf3 normal2 = tangentx1 % tangentx2;
			Vecf3 tangentz1 = { tempPos.z, tempPos.x, amplitude * std::cos(time * freqScroll + tempPos.x * frequency) };
			Vecf3 tangentz2 = { tempPos.y, tempPos.z, amplitude * std::cos(time * freqScroll + tempPos.y * frequency) };
			Vecf3 normal3 = tangentz1 % tangentz2;

			Vecf3 resultantNormal = (normal + normal2 + normal3);
			// return vertex shader output
			return { tempPos, vertex_in, resultantNormal.GetNormalized() };
		}
		void BindRotation(const Matf3& rotation_in) {
			rotation = rotation_in;
		}
		void BindTranslation(const Vecf3& translation_in) {
			translation = translation_in;
		}
		void SetTime(float t) {
			time = t;
		}
		void SetTransValues(float frequencyIn, float amplitudeIn, float scrollIn) {
			frequency = frequencyIn;
			amplitude = amplitudeIn;
			freqScroll = scrollIn;
		}
	private: // vertex shader attributes
		Matf3 rotation;
		Vecf3 translation;
		float time;
		float frequency = 8.0f;
		float amplitude = 0.10f;
		float freqScroll = 1.0f;
	};

	class GeomShader {
	public:
		class Output {
		public:
			// constructors
			Output() = default;
			Output(const Vecf3& pos, const Vecf2& texpos, const Vecf3& worldPos, const Vecf3& normal)
				:
				pos(pos),
				texpos(texpos),
				worldPos(worldPos),
				normal(normal)
			{}
			Output(const Vecf3& pos, const VertexShader::Output& vertex_in)
				:
				pos(pos),
				texpos(vertex_in.texpos),
				worldPos(vertex_in.worldPos),
				normal(vertex_in.normal)
			{}
			// operators
			Output operator+(const Output& rhs) const {
				Vecf3 temppos = pos + rhs.pos;
				Vecf2 temptexpos = texpos + rhs.texpos;
				Vecf3 tempworldPos = worldPos + rhs.worldPos;
				Vecf3 tempnormal = normal + rhs.normal;
				return { temppos, temptexpos, tempworldPos, tempnormal };
			}
			Output operator+=(const Output& rhs) {
				return *this + rhs;
			}
			Output operator-(const Output& rhs) const {
				Vecf3 temppos = pos - rhs.pos;
				Vecf2 temptexpos = texpos - rhs.texpos;
				Vecf3 tempworldPos = worldPos - rhs.worldPos;
				Vecf3 tempnormal = normal - rhs.normal;
				return { temppos, temptexpos, tempworldPos, tempnormal };
			}
			Output operator-=(const Output& rhs) {
				return *this - rhs;
			}
			Output operator*(float val) const {
				return Output(pos * val, texpos * val, worldPos * val, normal * val);
			}
			Output& operator/(float val) {
				pos = pos / val;
				texpos = texpos / val;
				worldPos = worldPos / val;
				normal = normal / val;
				return *this;
			}
		public:
			Vecf3 pos;
			Vecf2 texpos;
			Vecf3 worldPos;
			Vecf3 normal;
		};
	public:
		Triangle<Output> operator()(const VertexShader::Output& v0, const VertexShader::Output& v1, const VertexShader::Output& v2) {
			return { {v0.pos, v0},
			{v1.pos, v1},
			{v2.pos, v2} };
		}
	private:
	};

	class PixelShader {
	public:
		template<class Input>
		Color operator()(const Input& input) const {
			// calculate normal
			const Vecf3 normalizedNormal = input.normal.GetNormalized();
			// calculate light intensity per pixel lighting
			const Vecf3 lightToVertex = pointLightPosition - input.worldPos;
			// calcculate distance to light
			const float dist = lightToVertex.Len();
			// calculate direction normalize
			const Vecf3 pointLightDir = lightToVertex / dist;
			// calculate attenuation
			const float attenuation = 1.0f / (constant_attenuation + linear_attenuation * dist + quadratic_attenuation * sq(dist));

			// calculate specular highlight
			// projection of light dir vector on n hat
			const float lProjection = lightToVertex * normalizedNormal;
			// normal * scalar
			const Vecf3 normalScaled = normalizedNormal * lProjection;
			// calculate reflection vector
			const Vecf3 reflectionVector = (normalScaled * 2.0f) - lightToVertex;
			// calculate specular intensity
			const float s = std::pow(std::max(0.0f, reflectionVector.GetNormalized() * -input.worldPos.GetNormalized()), specularPower);
			const float specularIntensity = diffuseLight * specularScalar * s;

			// calculate intensity
			const float intensity = std::min(1.0f, std::max(0.0f, (-pointLightDir * normalizedNormal)) * diffuseLight * attenuation + ambientLight + specularIntensity);

			Vecf3 colorReturn = (Vecf3)texture->GetPixel(
				(int)std::min(input.texpos.x * tex_width, width_clamp),
				(int)std::min(input.texpos.y * tex_height, height_clamp));
			return (Color)(colorReturn * intensity);
		}
		void BindTexture(const std::string& filename) {
			texture = std::make_unique<Surface>(Surface(filename));
			tex_width = (float)texture->GetWidth();
			tex_height = (float)texture->GetHeight();
			width_clamp = tex_width - 1.0f;
			height_clamp = tex_height - 1.0f;
		}
		void SetLightPosition(const Vecf3& position) {
			pointLightPosition = position;
		}
	private:
		// texture attributes
		std::unique_ptr<Surface> texture;
		float tex_width;
		float tex_height;
		float width_clamp;
		float height_clamp;

		// diffuse light intensity
		float diffuseLight = 1.0f;
		// ambient light around assumed
		float ambientLight = 0.2f;

		// point light attributes
		Vecf3 pointLightPosition = { 0.0f, 0.0f, 2.5f };
		float quadratic_attenuation = 4.619f;
		float linear_attenuation = 0.2f;
		float constant_attenuation = 0.2f;

		// specular highlight attributes
		float specularScalar = 0.8f;
		// to alter fall off with angle
		float specularPower = 60.0f;
	};
public:
	PixelShader pixelShader;
	VertexShader vertexShader;
	GeomShader geomShader;
};
