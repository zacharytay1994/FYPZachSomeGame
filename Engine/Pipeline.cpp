#include "Pipeline.h"
#include "SurfaceDirectionalLighting.h"

Vecf3 Pipeline<SurfaceDirectionalLighting>::uprightVector = { 0.0f, 1.0f, 0.0f };
float Pipeline<SurfaceDirectionalLighting>::pitch = 0.0f;
float Pipeline<SurfaceDirectionalLighting>::yaw = 0.0f;
float Pipeline<SurfaceDirectionalLighting>::roll = 0.0f;

float Pipeline<SurfaceDirectionalLighting>::camWorldX = 0.0f;
float Pipeline<SurfaceDirectionalLighting>::camWorldZ = 0.0f;