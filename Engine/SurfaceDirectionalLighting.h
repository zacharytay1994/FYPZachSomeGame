#pragma once

#include "Vec3.h"
#include "MatTemplate.h"
#include "Triangle.h"
#include "Surface.h"

#include <memory>
#include <string>

class SurfaceDirectionalLighting {
public:
	class Vertex {
	public:
		// constructors
		Vertex() = default;
		Vertex(const Vecf3& pos, const Vecf2& texpos)
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
			Output(const Vecf4& pos, const Vecf2& texpos) // i dont really use this constructor
				:
				pos(pos),
				texpos(texpos)
			{}
			Output(const Vecf4& pos, const Vertex& vertex_in)
				:
				pos(pos),
				texpos(vertex_in.texpos)
			{}
			// operators
			Output operator+(const Output& rhs) const {
				Vecf4 temppos = pos + rhs.pos;
				Vecf2 temptexpos = texpos + rhs.texpos;
				return { temppos, temptexpos };
			}
			Output operator+=(const Output& rhs) {
				return *this + rhs;
			}
			Output operator-(const Output& rhs) const {
				Vecf4 temppos = pos - rhs.pos;
				Vecf2 temptexpos = texpos - rhs.texpos;
				return { temppos, temptexpos };
			}
			Output operator-=(const Output& rhs) {
				return *this - rhs;
			}
			Output operator*(float val) const {
				return Output(pos * val, texpos * val);
			}
			Output& operator/(float val) {
				pos = pos / val;
				texpos = texpos / val;
				return *this;
			}
		public:
			Vecf4 pos;
			Vecf2 texpos;
		};
		Output operator()(const Vertex& vertex_in) {
			Vecf4 tempPos = Vecf4(vertex_in.pos) * worldViewProj;
			return { tempPos, vertex_in };
		}
		void BindWorld(const Matf4& transformation_in) {
			world = transformation_in;
			worldView = world * view;
			worldViewProj = worldView * proj;
		}
		void BindView(const Matf4& transformation_in) {
			view = transformation_in;
			worldView = world * view;
			worldViewProj = worldView * proj;
		}
		void BindProjection(const Matf4& transformation_in) {
			proj = transformation_in;
			worldViewProj = worldView * proj;
		}
		const Matf4& GetProj() const {
			return proj;
		}
	private:
		// world transform
		Matf4 world = Matf4::Identity();
		// view transform
		Matf4 view = Matf4::Identity();
		// world view transform
		Matf4 worldView = Matf4::Identity();
		// persp proj transform
		Matf4 proj = Matf4::Identity();
		// both together
		Matf4 worldViewProj = Matf4::Identity();
	};

	class GeomShader {
	public:
		class Output {
		public:
			// constructors
			Output() = default;
			Output(const Vecf4 pos, const Vecf2& texpos, float intensity)
				:
				pos(pos),
				texpos(texpos),
				intensity(intensity)
			{}
			Output(const Vecf4& pos, const VertexShader::Output& vertex_in, float intensity)
				:
				pos(pos),
				texpos(vertex_in.texpos),
				intensity(intensity)
			{}
			// operators
			Output operator+(const Output& rhs) const {
				Vecf4 temppos = pos + rhs.pos;
				Vecf2 temptexpos = texpos + rhs.texpos;
				return { temppos, temptexpos, intensity };
			}
			Output operator+=(const Output& rhs) {
				return *this + rhs;
			}
			Output operator-(const Output& rhs) const {
				Vecf4 temppos = pos - rhs.pos;
				Vecf2 temptexpos = texpos - rhs.texpos;
				return { temppos, temptexpos, intensity };
			}
			Output operator-=(const Output& rhs) {
				return *this - rhs;
			}
			Output operator*(float val) const {
				return Output(pos * val, texpos * val, intensity);
			}
			Output& operator/(float val) {
				pos = pos / val;
				texpos = texpos / val;
				return *this;
			}
		public:
			Vecf4 pos;
			Vecf2 texpos;
			float intensity;
		};
	public:
		Triangle<Output> operator()(const VertexShader::Output& v0, const VertexShader::Output& v1, const VertexShader::Output& v2) {
			// calculating surface normal
			surfaceNormal = ((v0.pos - v1.pos).GetNormalized() % (v2.pos - v1.pos)).GetNormalized();
			// calculating intensity
			float intensity = std::min(1.0f, std::max(0.0f, (-directionalLight * surfaceNormal)) * diffuseLight + ambientLight);
			return { {v0.pos, v0, intensity},
			{v1.pos, v1, intensity},
			{v2.pos, v2, intensity} };
		}

		void BindDirectionalLight(const Vecf3& directionalLightIn) {
			directionalLight = directionalLightIn;
		}

		void BindAmbientLight(const float& ambientLightIn) {
			ambientLight = ambientLightIn;
		}

		Vecf3 GetSurfaceNormal() {
			return surfaceNormal;
		}
	private:
		Vecf3 surfaceNormal;
		Vecf3 directionalLight = { 0.0f, 0.9f, 0.0f };
		float diffuseLight = 1.0f;
		float ambientLight = 0.2f;
	};

	class PixelShader {
	public:
		template<class Input>
		Color operator()(const Input& input) const {
			Vecf3 colorReturn = (Vecf3)texture->GetPixel(
				(int)std::min(input.texpos.x * tex_width, width_clamp),
				(int)std::min(input.texpos.y * tex_height, height_clamp));
			return (Color)(colorReturn * input.intensity);
		}
		void BindTexture(const std::string& filename) {
			texture = std::make_unique<Surface>(Surface(filename));
			tex_width = (float)texture->GetWidth();
			tex_height = (float)texture->GetHeight();
			width_clamp = tex_width - 1.0f;
			height_clamp = tex_height - 1.0f;
		}
		void BindSurface(Surface& surface) {
			texture = std::make_unique<Surface>(surface);
			tex_width = (float)texture->GetWidth();
			tex_height = (float)texture->GetHeight();
			width_clamp = tex_width - 1.0f;
			height_clamp = tex_height - 1.0f;
		}
	private:
		// texture attributes
		std::unique_ptr<Surface> texture;
		float tex_width;
		float tex_height;
		float width_clamp;
		float height_clamp;
	};
public:
	PixelShader pixelShader;
	VertexShader vertexShader;
	GeomShader geomShader;
};