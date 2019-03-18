#pragma once

#include "Vec3.h"
#include "Graphics.h"
#include "NDCTransformer.h"
#include "Surface.h"
#include "Mat3.h"
#include "IndexedTriangleList.h"
#include "Triangle.h"
#include <string>

class Pipeline {
public:
	class Vertex {
	public:
		Vertex() = default;
		Vertex(const Vecf3& pos, const Vecf2& texpos)
			:
			pos(pos),
			texpos(texpos)
		{}
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

public:
	Pipeline(Graphics& gfx)
		:
		gfx(gfx)
	{}
	void Draw(IndexedTriangleList<Vertex> triangleList) { ProcessVertices(triangleList.vertices, triangleList.indices); }
	void BindRotation(const Matf3& rotation_in) {
		rotation = rotation_in;
	}
	void BindTranslation(const Vecf3& translation_in) {
		translation = translation_in;
	}
	void BindTexture(const std::string& filename) {
		texture = std::make_unique<Surface>(Surface(filename));
	}
private:
	void ProcessVertices(std::vector<Vertex>& vertices, std::vector<size_t>& indices) {
		// create holder vector
		std::vector<Vertex> verticesOut;

		// transform vertices
		for (Vertex v : vertices) {
			verticesOut.emplace_back(v.pos * rotation + translation, v.texpos);
		}

		// pass it on
		AssembleTriangles(verticesOut, indices);
	}
	void AssembleTriangles(std::vector<Vertex>& vertices, std::vector<size_t>& indices) {
		// loops and create triangles
		float testval = indices.size();
		for (size_t i = 0, end = indices.size() / 3; i < end; i++) {
			// get vertices by index
			const Vertex& v0 = vertices[indices[i * 3]];
			const Vertex& v1 = vertices[indices[i * 3 + 1]];
			const Vertex& v2 = vertices[indices[i * 3 + 2]];

			// backface cull
			if ((v1.pos - v0.pos) % (v2.pos - v0.pos) * v0.pos <= 0.0f) {
				ProcessTriangle( v0, v1, v2 );
			}
		}
	}
	void ProcessTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) { 
		PerpectiveTransform(Triangle<Vertex>{v0, v1, v2});
	} // makeshift geom shader
	void PerpectiveTransform(Triangle<Vertex>& triangle) { 
		// NDC transform
		trans.Transform(triangle.v0.pos);
		trans.Transform(triangle.v1.pos);
		trans.Transform(triangle.v2.pos);

		DrawTriangle(triangle); 
	}
	void DrawTriangle(Triangle<Vertex>& triangle) {
		// get pointers to vertices
		const Vertex* v0 = &triangle.v0;
		const Vertex* v1 = &triangle.v1;
		const Vertex* v2 = &triangle.v2;

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
			const Vertex v3 = interpolate<Vertex>(*v0, *v2, alpha);

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
	void DrawFlatBottomTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
		// get left and right rateofchange
		const Vertex changeLeft = (v1 - v0) / (v2.pos.y - v0.pos.y);
		const Vertex changeRight = (v2 - v0) / (v2.pos.y - v0.pos.y);

		// get y start and end
		const int yStart = (int)ceil(v0.pos.y - 0.5f);
		const int yEnd = (int)ceil(v2.pos.y - 0.5f);

		// create left and right interpolant
		Vertex leftInterpolant = v0;
		Vertex rightInterpolant = v0;

		// pre-step interpolants
		const float yPrestep = ((float)yStart + 0.5f - v0.pos.y);
		leftInterpolant += changeLeft * yPrestep;
		rightInterpolant += changeRight * yPrestep;

		DrawFlatTriangle(yStart, yEnd, v1, v2, leftInterpolant, rightInterpolant, changeLeft, changeRight);
	}
	void DrawFlatTopTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
		// get left and right rateofchange
		const Vertex changeLeft = (v2 - v0) / (v2.pos.y - v0.pos.y);
		const Vertex changeRight = (v2 - v1) / (v2.pos.y - v0.pos.y);

		// get y start and end
		const int yStart = (int)ceil(v0.pos.y - 0.5f);
		const int yEnd = (int)ceil(v2.pos.y - 0.5f);

		// create left and right interpolant
		Vertex leftInterpolant = v0;
		Vertex rightInterpolant = v1;

		// pre-step interpolants
		const float yPrestep = ((float)yStart + 0.5f - v0.pos.y);
		leftInterpolant += changeLeft * yPrestep;
		rightInterpolant += changeRight * yPrestep;

		DrawFlatTriangle(yStart, yEnd, v0, v1, leftInterpolant, rightInterpolant, changeLeft, changeRight);
	}
	void DrawFlatTriangle(const int& yStart, const int& yEnd, const Vertex& v0, const Vertex& v1,
		Vertex& leftInterpolant, Vertex& rightInterpolant, const Vertex& changeLeft, const Vertex& changeRight) {
		// loop through y down the triangle
		for (int y = yStart; y < yEnd; y++, leftInterpolant = leftInterpolant + changeLeft, rightInterpolant = rightInterpolant + changeRight) {
			// get x rateofchange
			const Vertex changeX = (v1 - v0) / (v1.pos.x - v0.pos.x);

			// get x start and end
			const int xStart = (int)ceil(leftInterpolant.pos.x - 0.5f);
			const int xEnd = (int)ceil(rightInterpolant.pos.x - 0.5f);

			float testvalue = rightInterpolant.pos.x - leftInterpolant.pos.x;

			// create left to right interpolant
			Vertex leftToRight = leftInterpolant;

			// pre-step interpolant
			float xPrestep = ((float)xStart + 0.5f) - leftInterpolant.pos.x;
			leftToRight += changeX * xPrestep;

			// get tex info and clamp
			float texWidth = (float)texture->GetWidth();
			float texHeight = (float)texture->GetHeight();
			float widthClamp = texWidth - 1.0f;
			float heightClamp = texHeight - 1.0f;

			// loop for x
			for (int x = xStart; x < xEnd; x++, leftToRight = leftToRight + changeX) {
				gfx.PutPixel(x, y, texture->GetPixel(
					(int)std::min(leftToRight.texpos.x * texWidth, widthClamp),
					(int)std::min(leftToRight.texpos.y * texHeight, heightClamp)
				));
			}
		}
	}
private:
	Graphics& gfx;
	NDCTransformer trans;
	Matf3 rotation;
	Vecf3 translation;
	std::unique_ptr<Surface> texture;
};