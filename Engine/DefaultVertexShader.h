#pragma once

#include "Mat3.h"
#include "Vec3.h"

template<class T>
class DefaultVertexShader {
public:
	// redundant for now as no new vertex generated
	typedef T Output;
public:
	void BindRotation(const Matf3& rotation_in) {
		rotation = rotation_in;
	}
	void BindTranslation(const Vecf3& translation_in) {
		translation = translation_in;
	}
	Output operator()(const T& vertex_in) const {
		Vecf3 tempPos = vertex_in.pos * rotation + translation;
		return { tempPos, vertex_in };
	}
private:
	Matf3 rotation;
	Vecf3 translation;
};