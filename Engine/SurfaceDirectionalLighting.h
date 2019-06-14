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
			Output(const Vecf4& pos, const Vecf2& texpos, const bool& isReflection,
				const Vecf4& directionalLight, const float& reflectiveIndex, const Vecf3 modelPos) // i dont really use this constructor
				:
				pos(pos),
				texpos(texpos),
				isReflection(isReflection),
				directionalLight(directionalLight),
				reflectiveIndex(reflectiveIndex),
				modelPos(modelPos)
			{}
			Output(const Vecf4& pos, const Vertex& vertex_in, const bool& isReflection,
				const Vecf4& directionalLight, const float& reflectiveIndex, const Vecf3& modelPos)
				:
				pos(pos),
				texpos(vertex_in.texpos),
				isReflection(isReflection),
				directionalLight(directionalLight),
				reflectiveIndex(reflectiveIndex),
				modelPos(modelPos)
			{}
			// operators
			Output operator+(const Output& rhs) const {
				Vecf4 temppos = pos + rhs.pos;
				Vecf2 temptexpos = texpos + rhs.texpos;
				float reflectiveIndexOut = reflectiveIndex + rhs.reflectiveIndex;
				Vecf3 tempModelPos = modelPos + rhs.modelPos;
				return { temppos, temptexpos, isReflection, directionalLight, reflectiveIndexOut, tempModelPos };
			}
			Output operator+=(const Output& rhs) {
				return *this + rhs;
			}
			Output operator-(const Output& rhs) const {
				Vecf4 temppos = pos - rhs.pos;
				Vecf2 temptexpos = texpos - rhs.texpos;
				float reflectiveIndexOut = reflectiveIndex - rhs.reflectiveIndex;
				Vecf3 tempModelPos = modelPos - rhs.modelPos;
				return { temppos, temptexpos, isReflection, directionalLight, reflectiveIndexOut, tempModelPos };
			}
			Output operator-=(const Output& rhs) {
				return *this - rhs;
			}
			Output operator*(float val) const {
				return Output(pos * val, texpos * val, isReflection, directionalLight,
					reflectiveIndex * val, modelPos * val);
			}
			Output& operator/(float val) {
				pos = pos / val;
				texpos = texpos / val;
				reflectiveIndex = reflectiveIndex / val;
				modelPos = modelPos / val;
				return *this;
			}
		public:
			Vecf4 pos;
			Vecf2 texpos;
			bool isReflection;
			Vecf4 directionalLight;
			float reflectiveIndex;
			Vecf3 modelPos;
		};
		struct OutputStruct {
			Output basicVertices;
			Output reflectionVertices;
		};
		OutputStruct operator()(const Vertex& vertex_in, const bool& isWater) {
			Vecf4 tempPos = Vecf4(vertex_in.pos) * worldViewProj;
			float reflectiveIndex;
			// calculate reflectiveindex
			if (!isWater) {
				reflectiveIndex = 1.0f;
			}
			else {
				if (vertex_in.pos.x == cameraPosition.x && vertex_in.pos.z == cameraPosition.z) {
					int checking = 1;
				}
				Vecf3 viewingVector = (cameraPosition - vertex_in.pos).GetNormalized();
				reflectiveIndex = viewingVector * Vecf3(0.0f, 1.0f, 0.0f);
			}
			Vecf4 reflectionTempPos = Vecf4(vertex_in.pos) * reflectionWorldViewProj;
			Vecf3 worldAlteredModelPos = (Vecf3)((Vecf4)vertex_in.pos * world);
			Vecf3 reflectionTempModelPos = worldAlteredModelPos * -1;
			return { Output(tempPos, vertex_in, false, (Vecf4)directionalLightVec3*view, reflectiveIndex,worldAlteredModelPos),
				Output(reflectionTempPos, vertex_in, true, (Vecf4)directionalLightVec3*reflectionView, reflectiveIndex, reflectionTempModelPos)};
		}
		Vecf3 CalcPointTransform(const Vecf3& vec_in) {
			Vecf3 temp = (Vecf4)vec_in * worldViewProj;
			return temp;
		}
		Vecf4 CalcPointTransform(const Vecf4& vec_in) {
			return vec_in * worldViewProj;
		}
		void BindWorld(const Matf4& transformation_in) {
			world = transformation_in;
			// for basic vertices
			worldView = world * view;
			worldViewProj = worldView * proj;
			// for reflection vertices
			reflectionWorldView = world * reflectionView;
			reflectionWorldViewProj = reflectionWorldView * proj;
		}
		void BindView(const Matf4& transformation_in) {
			view = transformation_in;
			worldView = world * view;
			worldViewProj = worldView * proj;
		}
		void BindProjection(const Matf4& transformation_in) {
			proj = transformation_in;
			worldViewProj = worldView * proj;
			reflectionWorldViewProj = reflectionWorldView * proj;
		}
		const Matf4& GetProj() const {
			return proj;
		}
		// reflecton proj
		void BindReflectionView(const Matf4& transformation_in) {
			reflectionView = transformation_in;
			reflectionWorldView = world * reflectionView;
			reflectionWorldViewProj = reflectionWorldView * proj;
		}
		void BindCameraPosition(const Vecf3& cameraPositionIn) {
			cameraPosition = cameraPositionIn;
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
		// reflection view transform
		Matf4 reflectionView = Matf4::Identity();
		// reflection world view transform
		Matf4 reflectionWorldView = Matf4::Identity();
		// reflection world view proj
		Matf4 reflectionWorldViewProj = Matf4::Identity();
		// directional light
		Vecf3 directionalLightVec3 = { 0.0f, 50.0f, 0.0f };
		Vecf4 directionalLight;
		// cameraPosition
		Vecf3 cameraPosition;
	};

	class GeomShader {
	public:
		class Output {
		public:
			// constructors
			Output() = default;
			Output(const Vecf4 pos, const Vecf2& texpos, float intensity, const bool& isReflection,
				const Vecf4& directionalLight, const float& reflectiveIndex, const Vecf3& modelPos)
				:
				pos(pos),
				texpos(texpos),
				intensity(intensity),
				isReflection(isReflection),
				directionalLight(directionalLight),
				reflectiveIndex(reflectiveIndex),
				modelPos(modelPos)
			{}
			Output(const Vecf4& pos, const VertexShader::Output& vertex_in, float intensity)
				:
				pos(pos),
				texpos(vertex_in.texpos),
				intensity(intensity),
				isReflection(vertex_in.isReflection),
				directionalLight(vertex_in.directionalLight),
				reflectiveIndex(vertex_in.reflectiveIndex),
				modelPos(vertex_in.modelPos)
			{}
			// operators
			Output operator+(const Output& rhs) const {
				Vecf4 temppos = pos + rhs.pos;
				Vecf2 temptexpos = texpos + rhs.texpos;
				float reflectiveIndexOut = reflectiveIndex + rhs.reflectiveIndex;
				Vecf3 tempModelPos = modelPos + rhs.modelPos;
				return { temppos, temptexpos, intensity, isReflection, directionalLight, reflectiveIndexOut, tempModelPos };
			}
			Output operator+=(const Output& rhs) {
				return *this + rhs;
			}
			Output operator-(const Output& rhs) const {
				Vecf4 temppos = pos - rhs.pos;
				Vecf2 temptexpos = texpos - rhs.texpos;
				float reflectiveIndexOut = reflectiveIndex - rhs.reflectiveIndex;
				Vecf3 tempModelPos = modelPos - rhs.modelPos;
				return { temppos, temptexpos, intensity, isReflection, directionalLight, reflectiveIndexOut, tempModelPos };
			}
			Output operator-=(const Output& rhs) {
				return *this - rhs;
			}
			Output operator*(float val) const {
				return Output(pos * val, texpos * val, intensity, isReflection, directionalLight, reflectiveIndex * val, modelPos * val);
			}
			Output& operator/(float val) {
				pos = pos / val;
				texpos = texpos / val;
				reflectiveIndex = reflectiveIndex / val;
				modelPos = modelPos / val;
				return *this;
			}
		public:
			Vecf4 pos;
			Vecf2 texpos;
			float intensity;
			bool isReflection;
			Vecf4 directionalLight;
			float reflectiveIndex;
			Vecf3 modelPos;
		};
	public:
		Triangle<Output> operator()(const VertexShader::Output& v0, const VertexShader::Output& v1, const VertexShader::Output& v2) {
			// calculating surface normal
			surfaceNormal = ((v0.pos - v1.pos).GetNormalized() % (v2.pos - v1.pos)).GetNormalized();
			// calculating intensity
			float intensity = std::min(1.0f, std::max(0.0f, (-/*(Vecf3)v0.*/directionalLight * surfaceNormal)) * diffuseLight + ambientLight);
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
		Vecf3 cameraPosition;
		Vecf3 directionalLight = { 0.0f, 1.0f, 0.0f };
		float diffuseLight = 1.0f;
		float ambientLight = 0.2f;
	};

	class PixelShader {
	public:
		struct Texture {
			std::unique_ptr<Surface> texture;
			const float tex_width;
			const float tex_height;
			const float width_clamp;
			const float height_clamp;
		};
		enum class EntityType {
			Enemy,
			Turret,
			Building,
			Projectile,
			Arrow
		};
	public:
		template<class Input>
		Color operator()(const Input& input, const float& reflectiveIndex, const bool& check, const float& screenWidth, const float& screenHeight) const {
			if (!check) {
				if (staticTexture) {
					Vecf3 colorReturn = (Vecf3)texture->GetPixel(
						(int)std::min(input.texpos.x * tex_width, width_clamp),
						(int)std::min(input.texpos.y * tex_height, height_clamp));
					return (Color)(colorReturn * input.intensity);
				}
				// if calling instance requires different textures per entity
				else {
					Vecf3 colorReturn = (Vecf3)textureList[entityType].texture->GetPixel(
						(int)std::min(input.texpos.x * textureList[entityType].tex_width, textureList[entityType].width_clamp),
						(int)std::min(input.texpos.y * textureList[entityType].tex_height, textureList[entityType].height_clamp));
					return (Color)(colorReturn * input.intensity);
				}
			}
			else {
				int xIndex = (int)std::min((input.pos.x / screenWidth)*tex_width, width_clamp);
				int yIndex = (int)std::min((input.pos.y / screenHeight)*tex_height, height_clamp);
				Vecf3 reflectionColor = (Vecf3)(reflectionBufferSample[yIndex*texture->GetWidth()+xIndex]);
				Vecf3 refractionColor = (Vecf3)(refractionBufferSample[yIndex*texture->GetWidth()+xIndex]);
				// calculate specular light color
				Vecf3 returnColor = (reflectionColor * (1.0f - reflectiveIndex)) + (refractionColor * (reflectiveIndex));
				Color colorReturn = Colors::MakeRGB((int)returnColor.x, (int)returnColor.y, (int)returnColor.z);
				return colorReturn;
			}
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
		void BindBuffer(std::vector<Color> reflectionBuffer, std::vector<Color> refractionBuffer, const int& bufferWidth, const int& bufferHeight) {
			texture->Reset(bufferWidth, bufferHeight);
			tex_width = (float)texture->GetWidth();
			tex_height = (float)texture->GetHeight();
			width_clamp = tex_width - 1.0f;
			height_clamp = tex_height - 1.0f;
			reflectionBufferSample.clear();
			refractionBufferSample.clear();
			std::vector<Color>::iterator end = reflectionBuffer.end();
			for (std::vector<Color>::iterator start = reflectionBuffer.begin(); start != end; std::advance(start, 1)) {
				reflectionBufferSample.push_back(*start);
			}
			std::vector<Color>::iterator end2 = refractionBuffer.end();
			for (std::vector<Color>::iterator start2 = refractionBuffer.begin(); start2 != end2; std::advance(start2, 1)) {
				refractionBufferSample.push_back(*start2);
			}
		}
		void AddTexture(const std::string& filename) {
			BindTexture(filename);
			textureList.push_back({ std::make_unique<Surface>(Surface(filename)), tex_width, tex_height, width_clamp, height_clamp });
		}
		void SetTextureType(const EntityType& type) {
			entityType = static_cast<int>(type);
		}
		void SetStaticTexture(const bool& boolIn) {
			staticTexture = boolIn;
		}
	private:
		// texture attributes
		// array of textures for drawing entities with different textures
		std::vector<Texture> textureList;
		std::unique_ptr<Surface> texture;
		float tex_width;
		float tex_height;
		float width_clamp;
		float height_clamp;
		int entityType = 0;
		// whether calling entities require different textures
		bool staticTexture = true;
		// buffers
		std::vector<Color> reflectionBufferSample;
		std::vector<Color> refractionBufferSample;
	};
public:
	PixelShader pixelShader;
	VertexShader vertexShader;
	GeomShader geomShader;
};