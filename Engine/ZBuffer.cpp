#include "ZBuffer.h"
#include "DebugWorld.h"
#include "Pipeline.h"

bool ZBuffer::FillReflectionBuffer(int x, int y, float depth, Color color, const Vecf3 & clipCoord)
{
	// check if yClipCoord is below plane
	// since plane is horizontal, perpendicular vector to plane is {0.0f, 1.0f, 0.0f}
	// dot with ycoord vector from plane
	Vecf3 pointToCheckVector = clipCoord - Pipeline<SurfaceDirectionalLighting>::pointOnWaterPlane;
	float planeCheck = (pointToCheckVector) * Pipeline<SurfaceDirectionalLighting>::perpendicularVectorToWaterPlane;
	if (planeCheck < 0) {
		float& depthAtPoint = AtReflect(x, y);
		if (depth < depthAtPoint) {
			depthAtPoint = depth;
			reflectionBuffer[y*width + (width - x)] = color;
			return true;
		}
		return false;
	}
	return false;
}
