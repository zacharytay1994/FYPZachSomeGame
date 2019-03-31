#pragma once

#include "Vec3.h"
#include "MatTemplate.h"
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
			Output(const Vecf4& pos, const Vecf2& texpos, const Vecf4& worldPos, const Vecf4& normal) // i dont really use this constructor
				:
				pos(pos),
				texpos(texpos),
				worldPos(worldPos),
				normal(normal)
			{}
			Output(const Vecf4& pos, const Vertex& vertex_in, const Vecf4 worldPos, const Vecf4& normal) 
				:
				pos(pos),
				texpos(vertex_in.texpos),
				worldPos(worldPos),
				normal(normal)
			{}
			// operators
			Output operator+(const Output& rhs) const {
				Vecf4 temppos = pos + rhs.pos;
				Vecf2 temptexpos = texpos + rhs.texpos;
				Vecf4 tempworldPos = worldPos + rhs.worldPos;
				Vecf4 tempnormal = normal + rhs.normal;
				return { temppos, temptexpos, tempworldPos, tempnormal };
			}
			Output operator+=(const Output& rhs) {
				return *this + rhs;
			}
			Output operator-(const Output& rhs) const {
				Vecf4 temppos = pos - rhs.pos;
				Vecf2 temptexpos = texpos - rhs.texpos;
				Vecf4 tempworldPos = worldPos - rhs.worldPos;
				Vecf4 tempnormal = normal - rhs.normal;
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
			Vecf4 pos;
			Vecf2 texpos;
			Vecf4 worldPos;
			Vecf4 normal;
		};
	public:
		Output operator()(const Vertex& vertex_in) {
			// Vecf3 temp
			Vecf3 preTransPos = vertex_in.pos;
			// transform vertices
			preTransPos.y = preTransPos.y + (amplitude * std::sin(time * freqScroll + preTransPos.x * frequency));
			preTransPos.y = preTransPos.y + (amplitude * std::sin(time * freqScroll + preTransPos.z * frequency));
			preTransPos.x = preTransPos.x + (amplitude * std::sin(time * freqScroll + preTransPos.y * frequency));
			preTransPos.x = preTransPos.x + (amplitude * std::sin(time * freqScroll + preTransPos.z * frequency));
			preTransPos.z = preTransPos.z + (amplitude * std::sin(time * freqScroll + preTransPos.x * frequency));
			preTransPos.z = preTransPos.z + (amplitude * std::sin(time * freqScroll + preTransPos.y * frequency));
			// calculate normals of vertices
			Vecf3 tangenty1 = { preTransPos.x, preTransPos.y, amplitude * std::cos(time * freqScroll + preTransPos.x * frequency) };
			Vecf3 tangenty2 = { preTransPos.y, preTransPos.z, amplitude * std::cos(time * freqScroll + preTransPos.z * frequency) };
			Vecf3 normal = tangenty2 % tangenty1;
			Vecf3 tangentx1 = { preTransPos.x, preTransPos.y, amplitude * std::cos(time * freqScroll + preTransPos.y * frequency) };
			Vecf3 tangentx2 = { preTransPos.z, preTransPos.x, amplitude * std::cos(time * freqScroll + preTransPos.z * frequency) };
			Vecf3 normal2 = tangentx1 % tangentx2;
			Vecf3 tangentz1 = { preTransPos.z, preTransPos.x, amplitude * std::cos(time * freqScroll + preTransPos.x * frequency) };
			Vecf3 tangentz2 = { preTransPos.y, preTransPos.z, amplitude * std::cos(time * freqScroll + preTransPos.y * frequency) };
			Vecf3 normal3 = tangentz1 % tangentz2;
			Vecf3 resultantNormal = (normal + normal2 + normal3) / 3;
			// transform from model space to world space
			// converting normal from vec3 to vec4 with w as 0 to negate translation
			Vecf4 postTransNormal = Vecf4(resultantNormal, 0.0f) * world;
			// transforming position
			Vecf4 tempPos = Vecf4(preTransPos) * worldProj;
			Vecf4 tempPosWorld = Vecf4(preTransPos) * world;
			
			// return vertex shader output
			return { tempPos, vertex_in, tempPosWorld, postTransNormal };
		}
		/*void BindRotation(const Matf3& rotation_in) {
			rotation = rotation_in;
		}
		void BindTranslation(const Vecf3& translation_in) {
			translation = translation_in;
		}*/
		void SetTime(float t) {
			time = t;
		}
		void SetTransValues(float frequencyIn, float amplitudeIn, float scrollIn) {
			frequency = frequencyIn;
			amplitude = amplitudeIn;
			freqScroll = scrollIn;
		}
		/*void BindTransformation(const Matf4& transformation_in) {
			transformation = transformation_in;
		}*/
		void BindWorld(const Matf4& transformation_in) {
			world = transformation_in;
			worldProj = world * proj;
		}
		void BindProjection(const Matf4& transformation_in) {
			proj = transformation_in;
			worldProj = world * proj;
		}
		const Matf4& GetProj() const {
			return proj;
		}
	private: // vertex shader attributes
		//Matf4 transformation;
		/*Matf3 rotation;
		Vecf3 translation;*/
		float time;
		float frequency = 0.0f;
		float amplitude = 0.0f;
		float freqScroll = 0.0f;

		// perspective projection transformations
		// world transform with no persp proj
		Matf4 world = Matf4::Identity();
		// persp proj transform
		Matf4 proj = Matf4::Identity();
		// both together
		Matf4 worldProj = Matf4::Identity();
	};

	class GeomShader {
	public:
		class Output {
		public:
			// constructors
			Output() = default;
			Output(const Vecf4 pos, const Vecf2& texpos, const Vecf4& worldPos, const Vecf4& normal)
				:
				pos(pos),
				texpos(texpos),
				worldPos(worldPos),
				normal(normal)
			{}
			Output(const Vecf4& pos, const VertexShader::Output& vertex_in)
				:
				pos(pos),
				texpos(vertex_in.texpos),
				worldPos(vertex_in.worldPos),
				normal(vertex_in.normal)
			{}
			// operators
			Output operator+(const Output& rhs) const {
				Vecf4 temppos = pos + rhs.pos;
				Vecf2 temptexpos = texpos + rhs.texpos;
				Vecf4 tempworldPos = worldPos + rhs.worldPos;
				Vecf3 tempnormal = normal + rhs.normal;
				return { temppos, temptexpos, tempworldPos, tempnormal };
			}
			Output operator+=(const Output& rhs) {
				return *this + rhs;
			}
			Output operator-(const Output& rhs) const {
				Vecf4 temppos = pos - rhs.pos;
				Vecf2 temptexpos = texpos - rhs.texpos;
				Vecf4 tempworldPos = worldPos - rhs.worldPos;
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
			Vecf4 pos;
			Vecf2 texpos;
			Vecf4 worldPos;
			Vecf4 normal;
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
		float ambientLight = 0.8f;

		// point light attributes
		Vecf3 pointLightPosition = { 0.0f, 0.0f, 2.5f };
		// linear and quadratic dont affect much
		float quadratic_attenuation = 2.619f;
		float linear_attenuation = 2.0f;
		// higher constant attenuation means less attenuation
		float constant_attenuation = 0.2f;

		// specular highlight attributes
		float specularScalar = 0.8f;
		// to alter fall off with angle
		float specularPower = 100.0f;
	};
public:
	PixelShader pixelShader;
	VertexShader vertexShader;
	GeomShader geomShader;
};
