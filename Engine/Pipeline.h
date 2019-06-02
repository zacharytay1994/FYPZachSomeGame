#pragma once

#include "Vec3.h"
#include "Graphics.h"
#include "NDCTransformer.h"
#include "Surface.h"
#include "MatTemplate.h"
#include "IndexedTriangleList.h"
#include "Triangle.h"
#include "ZBuffer.h"
#include "TextureEffect.h"
#include <string>

#include <assert.h>

template<class Effect>
class Pipeline {
public:
	// gets Vertex from effect class
	typedef typename Effect::Vertex Vertex;
	// output vertex from vertex shader
	typedef typename Effect::VertexShader::Output outputVertex;
	// output struct from vertex shader
	typedef typename Effect::VertexShader::OutputStruct outputVertexStruct;
	// output vertex from geom shader
	typedef typename Effect::GeomShader::Output outputGeom;
public:
	Pipeline(Graphics& gfx)
		:
		Pipeline(gfx, std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight))
	{}
	Pipeline(Graphics& gfx, std::shared_ptr<ZBuffer> zBuffer)
		:
		gfx(gfx),
		zBuffer(std::move(zBuffer))
	{
		//assert(zBuffer->height == gfx.ScreenHeight && zBuffer->width == gfx.ScreenWidth);
	}
	void Draw(IndexedTriangleList<Vertex> triangleList) { ProcessVertices(triangleList.vertices, triangleList.indices); }
	void BeginFrame() {
		zBuffer->Clear();
	}
	void SetIsOccupied(const bool& boolIn) {
		isOccupied = boolIn;
	}
	void SetVisualize(const bool& boolIn) {
		visualize = boolIn;
	}
	void CalculateWaterStuff(const Vecf3& cameraPositionIn) {
		cameraPosition = cameraPositionIn;
		CalculatePlaneOrientation();
		BindFresnelValues();
	}
	float GetReflectiveIndex() {
		return reflectiveIndex;
	}
private:
	void ProcessVertices(std::vector<Vertex>& vertices, std::vector<size_t>& indices) {
		// create holder vector of output vertices
		std::vector<outputVertexStruct> verticesOut;

		if (!isWater) {
			std::vector<Vertex>::iterator end = vertices.end();
			for (std::vector<Vertex>::iterator start = vertices.begin(); start != end; std::advance(start, 1)) {
				verticesOut.push_back(effect.vertexShader(*start, false));
			}
			// transform vertices using vertex shader
			//std::transform(vertices.begin(), vertices.end(), verticesOut.begin(), effect.vertexShader);
		}
		else {
			std::vector<Vertex>::iterator end = vertices.end();
			for (std::vector<Vertex>::iterator start = vertices.begin(); start != end; std::advance(start, 1)) {
				verticesOut.push_back(effect.vertexShader(*start, true));
			}
			// transform vertices using vertex shader
			//std::transform(vertices.begin(), vertices.end(), verticesOut.begin(), effect.vertexShader);
		}

		//// if is rendering water, calculate plane orientation for clipping reflection
		//if (isWater) {
		//	/*CalculatePlaneOrientation();
		//	CalculateFresnel();*/
		//}
		// pass it on
		AssembleTriangles(verticesOut, indices);
	}
	void AssembleTriangles(const std::vector<outputVertexStruct>& vertices, const std::vector<size_t>& indices) {
		// calculate origin with perspective projection matrix
		const Vecf4 eyepos = Vecf4{ 0.0f, 0.0f, 0.0f, 1.0f } *effect.vertexShader.GetProj();

		for (size_t i = 0, end = indices.size() / 3; i < end; i++) {
			// get vertices by index
			const outputVertexStruct& v0 = vertices[indices[i * 3]];
			const outputVertexStruct& v1 = vertices[indices[i * 3 + 1]];
			const outputVertexStruct& v2 = vertices[indices[i * 3 + 2]];

			// backface cull basic vertices
			float testVal = ((v1.basicVertices.pos - v0.basicVertices.pos) % (v2.basicVertices.pos - v0.basicVertices.pos)) * Vecf3(v0.basicVertices.pos - eyepos);
			if ((v1.basicVertices.pos - v0.basicVertices.pos) % (v2.basicVertices.pos - v0.basicVertices.pos) * Vecf3(v0.basicVertices.pos - eyepos) <= 0.0f) {
				ProcessTriangle(v0.basicVertices, v1.basicVertices, v2.basicVertices);
			}
			// backface cull reflection vertices
			float reflectionTestVal = ((v1.reflectionVertices.pos - v0.reflectionVertices.pos) % (v2.reflectionVertices.pos - v0.reflectionVertices.pos)) * Vecf3(v0.reflectionVertices.pos - eyepos);
			if ((v1.reflectionVertices.pos - v0.reflectionVertices.pos) % (v2.reflectionVertices.pos - v0.reflectionVertices.pos) * Vecf3(v0.reflectionVertices.pos - eyepos) <= 0.0f) {
				ProcessTriangle(v0.reflectionVertices, v1.reflectionVertices, v2.reflectionVertices);
			}
		}
	}
	void ProcessTriangle(const outputVertex& v0, const outputVertex& v1, const outputVertex& v2) {
		ClipCulling(effect.geomShader(v0, v1, v2));
	} // makeshift geom shader

	// clip culling of triangles
	void ClipCulling(Triangle<outputGeom>& triangle) {
		// check on all planes if exceed boundaries
		if (triangle.v0.pos.x > triangle.v0.pos.w && triangle.v1.pos.x > triangle.v1.pos.w && triangle.v2.pos.x > triangle.v2.pos.w) {
			return;
		}
		if (triangle.v0.pos.x < -triangle.v0.pos.w && triangle.v1.pos.x < -triangle.v1.pos.w && triangle.v2.pos.x < -triangle.v2.pos.w) {
			return;
		}
		if (triangle.v0.pos.y > triangle.v0.pos.w && triangle.v1.pos.y > triangle.v1.pos.w && triangle.v2.pos.y > triangle.v2.pos.w) {
			return;
		}
		if (triangle.v0.pos.y < -triangle.v0.pos.w && triangle.v1.pos.y < -triangle.v1.pos.w && triangle.v2.pos.y < -triangle.v2.pos.w) {
			return;
		}
		if (triangle.v0.pos.z > triangle.v0.pos.w && triangle.v1.pos.z > triangle.v1.pos.w && triangle.v2.pos.z > triangle.v2.pos.w) {
			return;
		}
		if (triangle.v0.pos.z < 0.0f && triangle.v1.pos.z < 0.0f && triangle.v2.pos.z < 0.0f) {
			return;
		}

		// if 1 vertex v0 outside bounds
		const auto Clip1 = [this](outputGeom& v0, outputGeom& v1, outputGeom& v2) {
			// finding v3
			const float alphav3 = (-v0.pos.z) / (v1.pos.z - v0.pos.z);
			const outputGeom v3 = interpolate(v0, v1, alphav3);
			// finding v4
			const float alphav4 = (-v0.pos.z) / (v2.pos.z - v0.pos.z);
			const outputGeom v4 = interpolate(v0, v2, alphav4);
			// pass it on
			PerspectiveTransform(Triangle<outputGeom>{v3, v1, v2});
			PerspectiveTransform(Triangle<outputGeom>{v4, v3, v2});
		};

		// if 2 vertices outside bounds
		const auto Clip2 = [this](outputGeom& v0, outputGeom& v1, outputGeom& v2) {
			// finding v3
			const float alphav3 = (-v2.pos.z) / (v0.pos.z - v2.pos.z);
			v2 = interpolate(v2, v0, alphav3);
			// finding v4
			const float alphav4 = (-v1.pos.z) / (v0.pos.z - v1.pos.z);
			v1 = interpolate(v1, v0, alphav4);
			// pass it on
			PerspectiveTransform(Triangle<outputGeom>{v0, v1, v2});
		};

		// near plane clip test
		if (triangle.v0.pos.z < 0.0f) {
			// if v0 and v1 outside
			if (triangle.v1.pos.z < 0.0f) {
				Clip2(triangle.v2, triangle.v0, triangle.v1);
			}
			// if v0 and v2 outside
			else if (triangle.v2.pos.z < 0.0f) {
				Clip2(triangle.v1, triangle.v0, triangle.v2);
			}
			// only v0 outside
			else {
				Clip1(triangle.v0, triangle.v1, triangle.v2);
			}
		}
		else if (triangle.v1.pos.z < 0.0f) {
			// if v1 and v2 outside
			if (triangle.v2.pos.z < 0.0f) {
				Clip2(triangle.v0, triangle.v1, triangle.v2);
			}
			// only v1 outside
			else {
				Clip1(triangle.v1, triangle.v0, triangle.v2);
			}
		}
		// only v2 outside
		else if (triangle.v2.pos.z < 0.0f) {
			Clip1(triangle.v2, triangle.v0, triangle.v1);
		}
		// no vertices outside
		else {
			PerspectiveTransform(triangle);
		}
	}

	void PerspectiveTransform(Triangle<outputGeom>& triangle) {
		// NDC transform
		trans.Transform(triangle.v0);
		trans.Transform(triangle.v1);
		trans.Transform(triangle.v2);

		DrawTriangle(triangle);
		// drawline functions which I sometimes use for debugging
		/*gfx.DrawLine((Vecf2)triangle.v0.pos, (Vecf2)triangle.v1.pos, Colors::Red);
		gfx.DrawLine((Vecf2)triangle.v0.pos, (Vecf2)triangle.v2.pos, Colors::Red);
		gfx.DrawLine((Vecf2)triangle.v1.pos, (Vecf2)triangle.v2.pos, Colors::Red);*/
		/*gfx.DrawLine(triangle.v0.pos, triangle.v0.pos + triangle.v0.normal * 300, Colors::Red);
		gfx.DrawLine(triangle.v1.pos, triangle.v1.pos + triangle.v1.normal * 300, Colors::Red);
		gfx.DrawLine(triangle.v2.pos, triangle.v2.pos + triangle.v2.normal * 300, Colors::Red);*/
	}
	void DrawTriangle(Triangle<outputGeom>& triangle) {
		// get pointers to vertices
		const outputGeom* v0 = &triangle.v0;
		const outputGeom* v1 = &triangle.v1;
		const outputGeom* v2 = &triangle.v2;

		// sort vertices
		if (v0->pos.y > v1->pos.y) { std::swap(v0, v1); }
		if (v1->pos.y > v2->pos.y) { std::swap(v1, v2); }
		if (v0->pos.y > v1->pos.y) { std::swap(v0, v1); }

		// determine if flat top/bottom else
		if (v1->pos.y == v2->pos.y) {
			// draw flat bottom
			if (v1->pos.x > v2->pos.x) { std::swap(v1, v2); }
			DrawFlatBottomTriangle(*v0, *v1, *v2);
		}
		else if (v0->pos.y == v1->pos.y) {
			// draw flat top
			if (v0->pos.x > v1->pos.x) { std::swap(v0, v1); }
			DrawFlatTopTriangle(*v0, *v1, *v2);
		}
		else {
			// split triangle
			// interpolate v3
			const float alpha = (v1->pos.y - v0->pos.y) / (v2->pos.y - v0->pos.y);
			// Interpolate(source, destination, rateofchange)
			const outputGeom v3 = interpolate<outputGeom>(*v0, *v2, alpha);

			// determine if major left/right
			if (v3.pos.x > v1->pos.x) {
				// major right
				DrawFlatBottomTriangle(*v0, *v1, v3);
				DrawFlatTopTriangle(*v1, v3, *v2);
			}
			else {
				// major left
				DrawFlatBottomTriangle(*v0, v3, *v1);
				DrawFlatTopTriangle(v3, *v1, *v2);
			}
		}
	}
	void DrawFlatBottomTriangle(const outputGeom& v0, const outputGeom& v1, const outputGeom& v2) {
		// get left and right rateofchange
		const outputGeom changeLeft = (v1 - v0) / (v2.pos.y - v0.pos.y);
		const outputGeom changeRight = (v2 - v0) / (v2.pos.y - v0.pos.y);

		// get y start and end
		const int yStart = std::max((int)ceil(v0.pos.y - 0.5f), 0);
		const int yEnd = std::min((int)ceil(v2.pos.y - 0.5f), (int)Graphics::ScreenHeight - 1);

		// create left and right interpolant
		outputGeom leftInterpolant = v0;
		outputGeom rightInterpolant = v0;

		// pre-step interpolants
		const float yPrestep = ((float)yStart + 0.5f - v0.pos.y);
		leftInterpolant = leftInterpolant + (changeLeft * yPrestep);
		rightInterpolant = rightInterpolant + (changeRight * yPrestep);

		DrawFlatTriangle(yStart, yEnd, v1, v2, leftInterpolant, rightInterpolant, changeLeft, changeRight);
	}
	void DrawFlatTopTriangle(const outputGeom& v0, const outputGeom& v1, const outputGeom& v2) {
		// get left and right rateofchange
		const outputGeom changeLeft = (v2 - v0) / (v2.pos.y - v0.pos.y);
		const outputGeom changeRight = (v2 - v1) / (v2.pos.y - v0.pos.y);

		// get y start and end
		const int yStart = std::max((int)ceil(v0.pos.y - 0.5f), 0);
		const int yEnd = std::min((int)ceil(v2.pos.y - 0.5f), (int)Graphics::ScreenHeight - 1);

		// create left and right interpolant
		outputGeom leftInterpolant = v0;
		outputGeom rightInterpolant = v1;

		// pre-step interpolants
		const float yPrestep = ((float)yStart + 0.5f - v0.pos.y);
		leftInterpolant = leftInterpolant + (changeLeft * yPrestep);
		rightInterpolant = rightInterpolant + (changeRight * yPrestep);

		DrawFlatTriangle(yStart, yEnd, v0, v1, leftInterpolant, rightInterpolant, changeLeft, changeRight);
	}
	void DrawFlatTriangle(const int& yStart, const int& yEnd, const outputGeom& v0, const outputGeom& v1,
		outputGeom& leftInterpolant, outputGeom& rightInterpolant, const outputGeom& changeLeft, const outputGeom& changeRight) {
		// loop through y down the triangle
		for (int y = yStart; y < yEnd; y++, leftInterpolant = leftInterpolant + changeLeft, rightInterpolant = rightInterpolant + changeRight) {
			// get x rateofchange
			const outputGeom changeX = (v1 - v0) / (v1.pos.x - v0.pos.x);

			// get x start and end
			const int xStart = std::max((int)ceil(leftInterpolant.pos.x - 0.5f), 0);
			
			const int xEnd = std::min((int)ceil(rightInterpolant.pos.x - 0.5f), (int)Graphics::ScreenWidth - 1);

			float testvalue = rightInterpolant.pos.x - leftInterpolant.pos.x;

			// create left to right interpolant
			outputGeom leftToRight = leftInterpolant;

			// pre-step interpolant
			float xPrestep = ((float)xStart + 0.5f) - leftInterpolant.pos.x;
			leftToRight = leftToRight + (changeX * xPrestep);

			// color of to be sample
			Color tempColor;
			// vertex with coordinate in clip space from screen space
			outputGeom clipSpace;
			// loop for x
			for (int x = xStart; x < xEnd; x++, leftToRight = leftToRight + changeX) {
				// get z value
				const float zValue = 1.0f/leftToRight.pos.w;
				// bring output vertex back into orthographic space
				outputGeom passIn = leftToRight * zValue;
				if (!isWater) {
					// getting color from orthographic texture coordinates
					tempColor = effect.pixelShader(passIn, leftToRight.reflectiveIndex, false, screenWidth, screenHeight);
				}
				else if (isWater) {
					// getting color from  screen space texture coordinates, i.e. 
					// mapping reflection screenbuffer to water plane texture buffer
					tempColor = effect.pixelShader(passIn/zValue, leftToRight.reflectiveIndex, true, screenWidth, screenHeight);
				}
				assert(passIn.isReflection == true || passIn.isReflection == false);
				// if pipeline is not rendering a reflection coordinate
				if (!passIn.isReflection) {
					//assert(x > 0 && x < screenWidth && y>0 && y < screenHeight);
					// get clip space coordinates from screen space coordinates for water clipping plane comparison
					clipSpace = trans.TransformScreenToClip(leftToRight);
					// if pipeline is not rendering water and coordinates fall below the water plane,
					// add color to refraction buffer
					if (!AbovePlane(clipSpace.pos) && !isWater) {
						//Vecf3 alteredColor = Vecf3(tempColor) * (1 - leftToRight.reflectiveIndex);
						zBuffer->FillRefractionBuffer(x, y, zValue, tempColor/*Colors::MakeRGB((int)alteredColor.x, (int)alteredColor.y, (int)alteredColor.z)*/);
					}
					if (zBuffer->TestAndSetZ(x, y, zValue, passIn.texpos)) {
						gfx.PutPixel(x, y, tempColor);
					}
				}
				// if is a reflection but not of water, i.e. water can't reflect itself
				else if (!isWater) {
					// to get the model space coordinates of y for the clipping plane
					clipSpace = trans.TransformScreenToClip(leftToRight);
					// add color to reflection buffer
					//Vecf3 alteredColor = Vecf3(tempColor) * leftToRight.reflectiveIndex;
					zBuffer->FillReflectionBuffer(x, y, zValue, tempColor/*Colors::MakeRGB((int)alteredColor.x, (int)alteredColor.y, (int)alteredColor.z)*/, clipSpace.pos);
				}
			}
		}
	}
	// calculates perpendicular and point on plane for plane clipping
	void CalculatePlaneOrientation() {
		pointOnWaterPlane = effect.vertexShader.CalcPointTransform(centerPoint);
		perpendicularVectorToWaterPlane = effect.vertexShader.CalcPointTransform(uprightVec) - pointOnWaterPlane;
		// a little hacky scaler to offset unintended effect, still an unresolved problem
		perpendicularVectorToWaterPlane.y *= 2.3f;
	}
	// check if reflection point falls above the water surface, i.e. have to to be clipped
	bool AbovePlane(const Vecf3& coord) {
		Vecf3 pointToCheckVector = coord - pointOnWaterPlane;
		float planeCheck = (pointToCheckVector) * perpendicularVectorToWaterPlane;
		if (planeCheck > 0) {
			return true;
		}
		return false;
	}
	// calculate fresnel reflective index and binds
	void BindFresnelValues() {
		// camera vector
		effect.vertexShader.BindCameraPosition(cameraPosition);
		effect.vertexShader.BindPerpendicularToPlane(perpendicularVectorToWaterPlane);
		//reflectiveIndex =  cameraPosition.GetNormalized() * perpendicularVectorToWaterPlane.GetNormalized();
	}
public:
	Effect effect;
	bool isWater = false;
	// orientation
	Vecf3 uprightVec = { 0.0f, 0.5f, 0.0f };
	Vecf3 centerPoint = { 0.0f, 0.0f, 0.0f };
	static Vecf3 pointOnWaterPlane;
	static Vecf3 perpendicularVectorToWaterPlane;
	Vecf3 cameraPosition;
	float reflectiveIndex;
	float angleOfIncidence;
private:
	Graphics& gfx;
	float screenWidth = gfx.ScreenWidth;
	float screenHeight = gfx.ScreenHeight;
	NDCTransformer<outputGeom> trans;
	std::shared_ptr<ZBuffer> zBuffer;
	bool isOccupied = false;
	bool visualize = false;
};
