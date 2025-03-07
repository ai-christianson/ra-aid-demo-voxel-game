#include "terrain.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Pseudo-random hash function for noise generation
static int Hash(int x, int z) {
    int hash = x * 73856093 ^ z * 19349663;
    hash = hash % 100000;
    return hash;
}

// Linear interpolation helper
float Interpolate(float a, float b, float t) {
    return a + t * (b - a);
}

// Fade function for smoother noise (cubic fade)
float SmoothFade(float t) {
    return t * t * (3.0f - 2.0f * t);
}

// Generate 2D noise similar to Perlin noise (simplified implementation)
float GenerateNoise2D(float x, float z, float scale) {
    // Scale the coordinates
    x *= scale;
    z *= scale;
    
    // Get grid cell coordinates
    int x0 = (int)floor(x);
    int z0 = (int)floor(z);
    int x1 = x0 + 1;
    int z1 = z0 + 1;
    
    // Fractional part of coordinates
    float sx = x - (float)x0;
    float sz = z - (float)z0;
    
    // Apply fade to smooth transitions
    float sx_fade = SmoothFade(sx);
    float sz_fade = SmoothFade(sz);
    
    // Generate random values at the corners of the cell
    float n00 = (float)Hash(x0, z0) / 100000.0f;
    float n10 = (float)Hash(x1, z0) / 100000.0f;
    float n01 = (float)Hash(x0, z1) / 100000.0f;
    float n11 = (float)Hash(x1, z1) / 100000.0f;
    
    // Bilinear interpolation
    float nx0 = Interpolate(n00, n10, sx_fade);
    float nx1 = Interpolate(n01, n11, sx_fade);
    float nxz = Interpolate(nx0, nx1, sz_fade);
    
    // Map to [-1, 1] range
    return 2.0f * nxz - 1.0f;
}

// Generate a height map for the terrain
void GenerateHeightMap(World* world, float* heightMap) {
    // Seed random number generator
    srand((unsigned int)time(NULL));
    
    // Generate primary noise for height
    for (int x = 0; x < WORLD_SIZE_X; x++) {
        for (int z = 0; z < WORLD_SIZE_Z; z++) {
            int idx = x + z * WORLD_SIZE_X;
            
            // Base terrain using primary noise
            float noise = GenerateNoise2D((float)x, (float)z, NOISE_SCALE);
            
            // Add some smaller scale noise for detail
            noise += 0.5f * GenerateNoise2D((float)x, (float)z, NOISE_SCALE * 2.0f);
            noise += 0.25f * GenerateNoise2D((float)x, (float)z, NOISE_SCALE * 4.0f);
            
            // Normalize and scale
            noise = (noise + 1.0f) * 0.5f; // Map from [-1,1] to [0,1]
            
            // Convert to height value
            heightMap[idx] = noise * TERRAIN_HEIGHT_SCALE + TERRAIN_HEIGHT_OFFSET;
        }
    }
}

// Generate the terrain based on the height map
void GenerateTerrain(World* world) {
    if (!world) return;
    
    // Create height map
    float* heightMap = (float*)malloc(WORLD_SIZE_X * WORLD_SIZE_Z * sizeof(float));
    if (!heightMap) return;
    
    // Generate the height map
    GenerateHeightMap(world, heightMap);
    
    // Secondary noise map for sand patches
    float* sandNoise = (float*)malloc(WORLD_SIZE_X * WORLD_SIZE_Z * sizeof(float));
    if (!sandNoise) {
        free(heightMap);
        return;
    }
    
    // Generate sand distribution noise
    for (int x = 0; x < WORLD_SIZE_X; x++) {
        for (int z = 0; z < WORLD_SIZE_Z; z++) {
            int idx = x + z * WORLD_SIZE_X;
            sandNoise[idx] = GenerateNoise2D((float)x * 2.5f, (float)z * 2.5f, NOISE_SCALE * 3.0f);
            // Normalize to [0,1]
            sandNoise[idx] = (sandNoise[idx] + 1.0f) * 0.5f;
        }
    }
    
    // Generate blocks based on height map
    for (int x = 0; x < WORLD_SIZE_X; x++) {
        for (int z = 0; z < WORLD_SIZE_Z; z++) {
            int idx = x + z * WORLD_SIZE_X;
            float height = heightMap[idx];
            int intHeight = (int)floorf(height);
            
            // Set blocks from bottom up to the height
            for (int y = 0; y < WORLD_SIZE_Y; y++) {
                // Empty above terrain height
                if (y > intHeight) {
                    SetBlock(world, x, y, z, BLOCK_EMPTY);
                }
                // Surface layer
                else if (y == intHeight) {
                    // Determine block type based on height and secondary noise
                    if (y < SAND_HEIGHT_THRESHOLD || sandNoise[idx] > BEACH_NOISE_THRESHOLD) {
                        SetBlock(world, x, y, z, BLOCK_SAND);
                    } else {
                        SetBlock(world, x, y, z, BLOCK_GRASS);
                    }
                }
                // Underground (1-3 blocks deep is still surface material, deeper is stone)
                else if (y >= intHeight - 3) {
                    if (y < SAND_HEIGHT_THRESHOLD || sandNoise[idx] > BEACH_NOISE_THRESHOLD) {
                        SetBlock(world, x, y, z, BLOCK_SAND);
                    } else {
                        SetBlock(world, x, y, z, BLOCK_GRASS);
                    }
                }
                // Deep underground
                else {
                    SetBlock(world, x, y, z, BLOCK_STONE);
                }
            }
        }
    }
    
    // Add jello at or below WATER_LEVEL
    for (int x = 0; x < WORLD_SIZE_X; x++) {
        for (int z = 0; z < WORLD_SIZE_Z; z++) {
            int idx = x + z * WORLD_SIZE_X;
            float height = heightMap[idx];
            int intHeight = (int)floorf(height);
            
            // Add jello from terrain height up to WATER_LEVEL
            for (int y = intHeight + 1; y <= WATER_LEVEL; y++) {
                // Only replace empty blocks with jello
                if (GetBlock(world, x, y, z) == BLOCK_EMPTY) {
                    SetBlock(world, x, y, z, BLOCK_JELLO);
                }
            }
        }
    }
    
    // Cleanup
    free(heightMap);
    free(sandNoise);
}