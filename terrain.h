#ifndef TERRAIN_H
#define TERRAIN_H

#include "voxel.h"

// Noise generation parameters
#define NOISE_SCALE 0.1f        // Controls the "zoom" of the noise pattern
#define TERRAIN_HEIGHT_SCALE 20  // Controls the vertical scale of the terrain
#define TERRAIN_HEIGHT_OFFSET 10 // Base height offset

// Terrain type thresholds
#define SAND_HEIGHT_THRESHOLD 12  // Below this height, use sand instead of grass
#define BEACH_NOISE_THRESHOLD 0.3f // Secondary noise threshold for creating sand patches

// Function prototypes for noise generation
float GenerateNoise2D(float x, float z, float scale);
float Interpolate(float a, float b, float t);
float SmoothFade(float t); // For smooth interpolation

// Function prototypes for terrain generation
void GenerateHeightMap(World* world, float* heightMap);
void GenerateTerrain(World* world);

#endif // TERRAIN_H