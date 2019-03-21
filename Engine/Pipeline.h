#pragma once

#include "Vec3.h"
#include "Graphics.h"
#include "NDCTransformer.h"
#include "Surface.h"
#include "Mat3.h"
#include "IndexedTriangleList.h"
#include "Triangle.h"
#include "ZBuffer.h"
#include "TextureEffect.h"
#include <string>

template<class Effect> 
class Pipeline {
public:
	// gets Vertex from effect class
	typedef typename Effect::Vertex Vertex;
	// output vertex from vertex shader
	typedef typename Effect::VertexShader::Output outputVertex;
public:
	Pipeline(Graphics& gfx)
		:
		gfx(gfx),
		zBuffer(gfx.ScreenWidth, gfx.ScreenHeight)
	{}
	void Draw(IndexedTriangleList<Vertex> triangleList) { ProcessVertices(triangleList.vertices, triangleList.indices); }
	void BeginFrame() {
		zBuffer.Clear();
	}
private:
	void ProcessVertices(std::vector<Vertex>& vertices, std::vector<size_t>& indices) {
		// create holder vector of output vertices
		std::vector<outputVertex> verticesOut(vertices.size());

		// transform vertices using vertex shader
		std::transform(vertices.begin(), vertices.end(), verticesOut.begin(), effect.vertexShader);

		// pass it on
		AssembleTriangles(verticesOut, indices);
	}
	void AssembleTriangles(const std::vector<outputVertex>& vertices, const std::vector<size_t>& indices) {
		// loops and create triangles
		for (size_t i = 0, end = indices.size() / 3; i < end; i++) {
			// get vertices by index
			const outputVertex& v0 = vertices[indices[i * 3]];
			const outputVertex& v1 = vertices[indices[i * 3 + 1]];
			const outputVertex& v2 = vertices[indices[i * 3 + 2]];

			// backface cull
			float testVal = ((v1.pos - v0.pos) % (v2.pos - v0.pos)) * v0.pos;
			if ((v1.pos - v0.pos) % (v2.pos - v0.pos) * v0.pos <= 0.0f) {
				ProcessTriangle( v0, v1, v2 );
			}
		}
	}
	void ProcessTriangle(const outputVertex& v0, const outputVertex& v1, const outputVertex& v2) {
		PerpectiveTransform(Triangle<outputVertex>{v0, v1, v2});
	} // makeshift geom shader
	void PerpectiveTransform(Triangle<outputVertex>& triangle) {
		// NDC transform
		trans.Transform(triangle.v0);
		trans.Transform(triangle.v1);
		trans.Transform(triangle.v2);

		DrawTriangle(triangle);
		gfx.DrawLine(triangle.v0.pos, triangle.v1.pos, Colors::White);
		gfx.DrawLine(triangle.v0.pos, triangle.v2.pos, Colors::White);
		gfx.DrawLine(triangle.v1.pos, triangle.v2.pos, Colors::White);
	}
	void DrawTriangle(Triangle<outputVertex>& triangle) {
		// get pointers to vertices
		const outputVertex* v0 = &triangle.v0;
		const outputVertex* v1 = &triangle.v1;
		const outputVertex* v2 = &triangle.v2;

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
			const outputVertex v3 = interpolate<outputVertex>(*v0, *v2, alpha);

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
	void DrawFlatBottomTriangle(const outputVertex& v0, const outputVertex& v1, const outputVertex& v2) {
		// get left and right rateofchange
		const outputVertex changeLeft = (v1 - v0) / (v2.pos.y - v0.pos.y);
		const outputVertex changeRight = (v2 - v0) / (v2.pos.y - v0.pos.y);

		// get y start and end
		const int yStart = (int)ceil(v0.pos.y - 0.5f);
		const int yEnd = (int)ceil(v2.pos.y - 0.5f);

		// create left and right interpolant
		outputVertex leftInterpolant = v0;
		outputVertex rightInterpolant = v0;

		// pre-step interpolants
		const float yPrestep = ((float)yStart + 0.5f - v0.pos.y);
		leftInterpolant += changeLeft * yPrestep;
		rightInterpolant += changeRight * yPrestep;

		DrawFlatTriangle(yStart, yEnd, v1, v2, leftInterpolant, rightInterpolant, changeLeft, changeRight);
	}
	void DrawFlatTopTriangle(const outputVertex& v0, const outputVertex& v1, const outputVertex& v2) {
		// get left and right rateofchange
		const outputVertex changeLeft = (v2 - v0) / (v2.pos.y - v0.pos.y);
		const outputVertex changeRight = (v2 - v1) / (v2.pos.y - v0.pos.y);

		// get y start and end
		const int yStart = (int)ceil(v0.pos.y - 0.5f);
		const int yEnd = (int)ceil(v2.pos.y - 0.5f);

		// create left and right interpolant
		outputVertex leftInterpolant = v0;
		outputVertex rightInterpolant = v1;

		// pre-step interpolants
		const float yPrestep = ((float)yStart + 0.5f - v0.pos.y);
		leftInterpolant += changeLeft * yPrestep;
		rightInterpolant += changeRight * yPrestep;

		DrawFlatTriangle(yStart, yEnd, v0, v1, leftInterpolant, rightInterpolant, changeLeft, changeRight);
	}
	void DrawFlatTriangle(const int& yStart, const int& yEnd, const outputVertex& v0, const outputVertex& v1,
		outputVertex& leftInterpolant, outputVertex& rightInterpolant, const outputVertex& changeLeft, const outputVertex& changeRight) {
		// loop through y down the triangle
		for (int y = yStart; y < yEnd; y++, leftInterpolant = leftInterpolant + changeLeft, rightInterpolant = rightInterpolant + changeRight) {
			// get x rateofchange
			const outputVertex changeX = (v1 - v0) / (v1.pos.x - v0.pos.x);

			// get x start and end
			const int xStart = (int)ceil(leftInterpolant.pos.x - 0.5f);
			const int xEnd = (int)ceil(rightInterpolant.pos.x - 0.5f);

			float testvalue = rightInterpolant.pos.x - leftInterpolant.pos.x;

			// create left to right interpolant
			outputVertex leftToRight = leftInterpolant;

			// pre-step interpolant
			float xPrestep = ((float)xStart + 0.5f) - leftInterpolant.pos.x;
			leftToRight += changeX * xPrestep;

			// loop for x
			for (int x = xStart; x < xEnd; x++, leftToRight = leftToRight + changeX) {
				// get z value
				const float zValue = 1.0f/leftToRight.pos.z;
				if (zBuffer.TestAndSet(x, y, zValue)) {
					// bring texture coordinates back to orthographic space
					const outputVertex passIn = leftToRight * zValue;
					gfx.PutPixel(x, y, effect.pixelShader(passIn));
				}
			}
		}
	}
public:
	Effect effect;
private:
	Graphics& gfx;
	NDCTransformer<Vertex> trans;
	ZBuffer zBuffer;
};