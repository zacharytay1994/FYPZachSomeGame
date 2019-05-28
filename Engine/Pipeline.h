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
private:
	void ProcessVertices(std::vector<Vertex>& vertices, std::vector<size_t>& indices) {
		// create holder vector of output vertices
		std::vector<outputVertexStruct> verticesOut(vertices.size());

		// transform vertices using vertex shader
		std::transform(vertices.begin(), vertices.end(), verticesOut.begin(), effect.vertexShader);

		// pass it on
		AssembleTriangles(verticesOut, indices);
	}
	void AssembleTriangles(const std::vector<outputVertexStruct>& vertices, const std::vector<size_t>& indices) {
		// calculate origin with perspective projection matrix
		const Vecf4 eyepos = Vecf4{ 0.0f, 0.0f, 0.0f, 1.0f } *effect.vertexShader.GetProj();
		// loops and create triangles
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

			// loop for x
			Color tempColor;
			outputGeom modelSpace;
			//outputGeom modelSpace;
			for (int x = xStart; x < xEnd; x++, leftToRight = leftToRight + changeX) {
				// get z value
				const float zValue = 1.0f/leftToRight.pos.w;
				// bring output vertex back into orthographic space
				outputGeom passIn = leftToRight * zValue;
				if (!isWater) {
					tempColor = effect.pixelShader(passIn, false, screenWidth, screenHeight);
				}
				else {
					tempColor = effect.pixelShader(passIn/zValue, true, screenWidth, screenHeight);
				}
				assert(passIn.isReflection == true || passIn.isReflection == false);
				if (!passIn.isReflection) {
					assert(x > 0 && x < screenWidth && y>0 && y < screenHeight);
					if (zBuffer->TestAndSetZ(x, y, zValue, passIn.texpos)) {
						// getting color from orthographic texture coordinates
						gfx.PutPixel(x, y, tempColor);
						//else {
						//	// filling the reflection buffer used in water
						//	modelSpace = trans.TransformClipToModel(leftToRight);
						//	zBuffer->FillReflectionBuffer(x, y, tempColor, modelSpace.pos.y);
						//}
					}
				}
				else {
					// to get the model space coordinates of y for the clipping plane
					modelSpace = trans.TransformClipToModel(leftToRight);
					zBuffer->FillReflectionBuffer(x, y, zValue, tempColor, modelSpace.pos);
				}
			}
		}
	}
	Vecf3 CalculateUprightVector() {
		Matf4 rotationMatrix = Matf4::RotationZ(roll) * Matf4::RotationX(pitch) * Matf4::RotationY(yaw);
		Vecf3 test = (Vecf3)((Vecf4)uprightVector * rotationMatrix);
		return (Vecf3)((Vecf4)uprightVector * rotationMatrix);
	}
public:
	Effect effect;
	bool toDraw = true;
	bool isWater = false;
	// orientation
	static Vecf3 uprightVector;
	static float pitch;
	static float yaw;
	static float roll;
	static float camWorldX;
	static float camWorldZ;
private:
	Graphics& gfx;
	float screenWidth = gfx.ScreenWidth;
	float screenHeight = gfx.ScreenHeight;
	NDCTransformer<outputGeom> trans;
	std::shared_ptr<ZBuffer> zBuffer;
	bool isOccupied = false;
	bool visualize = false;
};