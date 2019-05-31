#include "Pipeline.h"
#include "SurfaceDirectionalLighting.h"

Vecf3 Pipeline<SurfaceDirectionalLighting>::pointOnWaterPlane = { 0.0f, 0.0f, 0.0f };
Vecf3 Pipeline<SurfaceDirectionalLighting>::perpendicularVectorToWaterPlane = { 0.0f, 0.0f, 0.0f };