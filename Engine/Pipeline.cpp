#include "Pipeline.h"
#include "SurfaceDirectionalLighting.h"
#include "DebugWorld.h"

Vecf3 Pipeline<SurfaceDirectionalLighting>::uprightVector = { 0.0f, 1.0f, 0.0f };
float Pipeline<SurfaceDirectionalLighting>::pitch = 0.0f;
float Pipeline<SurfaceDirectionalLighting>::yaw = 0.0f;
float Pipeline<SurfaceDirectionalLighting>::roll = 0.0f;

float Pipeline<SurfaceDirectionalLighting>::camWorldX = 0.0f;
float Pipeline<SurfaceDirectionalLighting>::camWorldZ = 0.0f;

Vecf3 Pipeline<SurfaceDirectionalLighting>::pointOnWaterPlane = { 0.0f, 0.0f, 0.0f };
Vecf3 Pipeline<SurfaceDirectionalLighting>::perpendicularVectorToWaterPlane = { 0.0f, 0.0f, 0.0f };

bool Pipeline<SurfaceDirectionalLighting>::IsAbovePlane(const Vecf3& coord) {
	Vecf3 pointToCheckVector = coord - DebugWorld::pointOnPlane;
	float planeCheck = (pointToCheckVector)* DebugWorld::uprightVector;
	if (planeCheck > 0) {
		return true;
	}
	return false;
}