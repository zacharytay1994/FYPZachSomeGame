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
				//worldPos(worldPos)
			{}
			// new copy constructor to copy all but pos
			Output(const Vecf4 pos_in, const Vertex& vert_in, Vecf4 normal)
				:
				pos(pos_in),
				texpos(vert_in.texpos),
				//worldPos(worldPos),
				normal(normal)
			{
			}
			Output operator-(const Output& rhs) const {
				return Output(pos - rhs.pos, texpos - rhs.texpos, normal - rhs.normal);
			}
			Output& operator/(float val) {
				pos = pos / val;
				texpos = texpos / val;
				//worldPos = worldPos / val;
				return *this;
			}
			Output operator+(const Output& rhs) const {
				Vecf3 temppos = pos + rhs.pos;
				Vecf2 temptexpos = texpos + rhs.texpos;
				Output temp = { temppos, temptexpos, normal + rhs.normal};
				return temp;
			}
			Output operator+=(const Output& rhs) {
				return *this + rhs;
			}
			Output operator*(float val) const {
				return Output(pos * val, texpos * val, normal);
			}
		public:
			Vecf4 pos;
			Vecf2 texpos;
			Vecf3 normal;
			//Vecf3 worldPos;
		};
	public:
		/*void BindRotation(const Matf3& rotation_in) {
			rotation = rotation_in;
		}
		void BindTranslation(const Vecf3& translation_in) {
			translation = translation_in;
		}*/
		void BindTransformation(const Matf4& transformation_in) {
			transformation = transformation_in;
		}
		void SetTime(float t) {
			time = t;
		}
		void SetTransValues(float frequencyIn, float amplitudeIn, float scrollIn) {
			frequency = frequencyIn;
			amplitude = amplitudeIn;
			freqScroll = scrollIn;
		}
		Output operator()(const Vertex& vertex_in) const {
			Vecf4 tempPos = Vecf4(vertex_in.pos) * transformation;
			tempPos.y = tempPos.y + (amplitude * std::sin(time * freqScroll + tempPos.x * frequency));
			tempPos.y = tempPos.y + (amplitude * std::sin(time * freqScroll + tempPos.z * frequency));
			tempPos.x = tempPos.x + (amplitude * std::sin(time * freqScroll + tempPos.y * frequency));
			tempPos.x = tempPos.x + (amplitude * std::sin(time * freqScroll + tempPos.z * frequency));
			tempPos.z = tempPos.z + (amplitude * std::sin(time * freqScroll + tempPos.x * frequency));
			tempPos.z = tempPos.z + (amplitude * std::sin(time * freqScroll + tempPos.y * frequency));
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
			return { tempPos, vertex_in, resultantNormal.GetNormalized() };
		}
	private:
		Matf4 transformation;
		/*Matf3 rotation;
		Vecf3 translation;*/
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
				//worldPos(worldPos)
			{}
			// new copy constructor to copy all but pos
			Output(const Vecf3 pos_in, const Output& vert_in)
				:
				pos(pos_in),
				texpos(vert_in.texpos),
				intensity(vert_in.intensity),
				normal(vert_in.normal)
				//worldPos(vert_in.worldPos)
			{
			}
			Output operator-(const Output& rhs) const {
				return Output(pos - rhs.pos, texpos - rhs.texpos, intensity - rhs.intensity, normal);
			}
			Output& operator/(float val) {
				pos = pos / val;
				texpos = texpos / val;
				intensity = intensity / val;
				
				//worldPos = worldPos / val;
				return *this;
			}
			Output operator+(const Output& rhs) const {
				Vecf3 temppos = pos + rhs.pos;
				Vecf2 temptexpos = texpos + rhs.texpos;
				float tempintensity = intensity + rhs.intensity;
				//Vecf3 tempWorldPos = worldPos + rhs.worldPos;
				Output temp = { temppos, temptexpos, tempintensity, normal};
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
			//Vecf3 worldPos;
		};
	public:
		Triangle<Output> operator()(const VertexShader::Output& v0, const VertexShader::Output& v1, const VertexShader::Output& v2) const {
			/* calculate surface normal
			Vecf3 surfaceNormal = ((v1.pos - v0.pos) % (v2.pos - v0.pos)).GetNormalized();
			 exploding stuff*/
			/*Vecf3 v0out = v0.pos + surfaceNormal * 0.2 * abs(sin(time / 2));
			Vecf3 v1out = v1.pos + surfaceNormal * 0.2 * abs(sin(time / 2));
			Vecf3 v2out = v2.pos + surfaceNormal * 0.2 * abs(sin(time / 2));*/
			Vecf3 v0out = v0.pos;
			Vecf3 v1out = v1.pos;
			Vecf3 v2out = v2.pos;

			// calculating point light direction
			const Vecf3 lightToVertexVector0 = pointLightPosition - v0.pos;
			const Vecf3 lightToVertexVector1 = pointLightPosition - v1.pos;
			const Vecf3 lightToVertexVector2 = pointLightPosition - v2.pos;
			// calculating light distance
			const float lightDistance0 = lightToVertexVector0.Len();
			const float lightDistance1 = lightToVertexVector1.Len();
			const float lightDistance2 = lightToVertexVector2.Len();
			// calculate normalized direction of light
			const Vecf3 lightDirection0 = lightToVertexVector0 / lightDistance0;
			const Vecf3 lightDirection1 = lightToVertexVector1 / lightDistance1;
			const Vecf3 lightDirection2 = lightToVertexVector2 / lightDistance2;
			// calculate attenuation 
			const float attenuation0 = 1.0f / (constant_attenuation + linear_attenuation * lightDistance0 + quadratic_attenuation * sq(lightDistance0));
			const float attenuation1 = 1.0f / (constant_attenuation + linear_attenuation * lightDistance1 + quadratic_attenuation * sq(lightDistance1));
			const float attenuation2 = 1.0f / (constant_attenuation + linear_attenuation * lightDistance2 + quadratic_attenuation * sq(lightDistance2));

			// calculating intensity based on angle
			// vertex normal gouraud shading
			float v0intensityIn = 1.0f;
			float v1intensityIn = 1.0f;
			float v2intensityIn = 1.0f;
			if (!isLight) {
				v0intensityIn = std::min(1.0f, std::max(0.0f, (-lightDirection0 * v0.normal)) * diffuseLight * attenuation0 + ambientLight);
				v1intensityIn = std::min(1.0f, std::max(0.0f, (-lightDirection1 * v1.normal)) * diffuseLight * attenuation1 + ambientLight);
				v2intensityIn = std::min(1.0f, std::max(0.0f, (-lightDirection2 * v2.normal)) * diffuseLight * attenuation2 + ambientLight);
			}
			// face normal flat shading
			//float intensityIn = std::min(1.0f, std::max(0.0f, (-directionLight * surfaceNormal)) * diffuseLight + ambientLight);
			return { {v0out, v0.texpos, v0intensityIn, v0.normal}, {v1out, v1.texpos, v1intensityIn, v1.normal}, {v2out, v2.texpos, v2intensityIn, v2.normal} };
			/*return { {v0.pos, v0.texpos, 1.0f, v0.normal, v0.worldPos},
			{v1.pos, v1.texpos, 1.0f, v1.normal, v1.worldPos},
			{v2.pos, v2.texpos, 1.0f, v2.normal, v2.worldPos} };*/
		}
		void BindTime(float t) {
			time = t;
		}
		void SetLightPosition(const Vecf3& position) {
			pointLightPosition = position;
		}
		void SetIsLight(bool isLightIn) {
			isLight = isLightIn;
		}
	private:
		float time;
		// direction vector of light
		Vecf3 directionLight = { 0.0f, 0.0f, 1.0f };
		// intensity of light, full stream white 100%
		float diffuseLight = 1.0f;
		// ambient light around assumed
		float ambientLight = 0.1f;

		// point light attributes
		Vecf3 pointLightPosition = { 0.0f, 0.0f, 2.5f };
		float quadratic_attenuation = 2.619f;
		float linear_attenuation = 1.0f;
		float constant_attenuation = 0.382f;

		// temp boolean for test
		bool isLight = false;
	};

	class PixelShader {
	public:
		template<class Input>
		// overload () operator to return a color from input, same as previously in pipeline
		Color operator()(const Input& input) const {

			//// calculate light intensity per pixel lighting
			//const Vecf3 lightToVertex = pointLightPosition - input.worldPos;
			//// calcculate distance to light
			//const float dist = lightToVertex.Len();
			//// calculate direction normalize
			//const Vecf3 pointLightDir = lightToVertex / dist;
			//// calculate attenuation
			//const float attenuation = 1.0f / (constant_attenuation + quadratic_attenuation * sq(dist));
			//// calculate intensity
			//const float intens = std::min(1.0f, diffuseLight * attenuation *std::max(0.0f, input.normal * pointLightDir) + ambientLight);
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
		void SetLightPosition(const Vecf3& position) {
			pointLightPosition = position;
		}
		// member variables of pixelshader
	private:
		std::unique_ptr<Surface> texture;
		float tex_width;
		float tex_height;
		float width_clamp;
		float height_clamp;

		// calculating per pixel attributes
		float diffuseLight = 1.0f;
		// ambient light around assumed
		float ambientLight = 0.2f;

		// point light attributes
		Vecf3 pointLightPosition = { 0.0f, 0.0f, 2.5f };
		float quadratic_attenuation = 2.619f;
		//float linear_attenuation = 1.0f;
		float constant_attenuation = 1.0f;
	};

// member variables of TextureEffect
public:
	PixelShader pixelShader;
	VertexShader vertexShader;
	GeomShader geomShader;
};