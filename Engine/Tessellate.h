#pragma once

#include "IndexedTriangleList.h"

class Tessallate {
public:
	template <class T>
	static IndexedTriangleList<T> GetTessellate(const IndexedTriangleList<T> triList) {
		std::vector<T> vertexOut;
		std::vector<size_t> indexOut;

		size_t triNumber = 0;
		// loop through triList
		for (size_t i = 0; i < triList.indices.size() / 3; i++, triNumber++) {
			const T& v0 = triList.vertices[triList.indices[i * 3]];
			const T& v1 = triList.vertices[triList.indices[i * 3 + 1]];
			const T& v2 = triList.vertices[triList.indices[i * 3 + 2]];

			const T edgeCenter1 = v0 + ((v1 - v0) / 2);
			const T edgeCenter2 = v0 + ((v2 - v0) / 2);
			const T edgeCenter3 = v1 + ((v2 - v1) / 2);

			vertexOut.push_back(v0);
			vertexOut.push_back(v1);
			vertexOut.push_back(v2);
			vertexOut.push_back(edgeCenter1);
			vertexOut.push_back(edgeCenter2);
			vertexOut.push_back(edgeCenter3);

			// indexing
			std::vector<size_t> tempVec = { 
				0 + triNumber * 6,3 + triNumber * 6,4 + triNumber * 6,
				3 + triNumber * 6,5 + triNumber * 6,4 + triNumber * 6,
				3 + triNumber * 6,1 + triNumber * 6,5 + triNumber * 6,
				5 + triNumber * 6,2 + triNumber * 6,4 + triNumber * 6 };
			for (size_t j = 0; j < tempVec.size(); j++) {
				indexOut.push_back(tempVec[j]);
			}
		}
		return { vertexOut, indexOut };
	}
private:
};