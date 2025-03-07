#include "voxel.h"
#include <stdlib.h>
#include <string.h>

// Direction vectors for the 6 faces of a block
// Order: +X, -X, +Y, -Y, +Z, -Z
const int DIRECTION_VECTORS[6][3] = {
    { 1, 0, 0 }, // Right
    {-1, 0, 0 }, // Left
    { 0, 1, 0 }, // Up
    { 0,-1, 0 }, // Down
    { 0, 0, 1 }, // Front
    { 0, 0,-1 }  // Back
};

// Check if a block type is transparent
bool IsBlockTransparent(BlockType blockType) {
    return blockType == BLOCK_EMPTY || blockType == BLOCK_JELLO;
}

// Create a new empty world
World* CreateWorld(void) {
    World* world = (World*)malloc(sizeof(World));
    
    if (world) {
        // Initialize all blocks to empty
        memset(world->blocks, BLOCK_EMPTY, sizeof(world->blocks));
    }
    
    return world;
}

// Free the world's memory
void DestroyWorld(World* world) {
    if (world) {
        free(world);
    }
}

// Check if a position is within world bounds
bool IsValidBlockPosition(int x, int y, int z) {
    return (x >= 0 && x < WORLD_SIZE_X &&
            y >= 0 && y < WORLD_SIZE_Y &&
            z >= 0 && z < WORLD_SIZE_Z);
}

// Get the block type at a specific position
BlockType GetBlock(World* world, int x, int y, int z) {
    if (!world || !IsValidBlockPosition(x, y, z)) {
        return BLOCK_EMPTY;
    }
    
    return world->blocks[x][y][z];
}

// Set a block at a specific position
void SetBlock(World* world, int x, int y, int z, BlockType type) {
    if (world && IsValidBlockPosition(x, y, z)) {
        world->blocks[x][y][z] = type;
    }
}

// Check if a specific face of a block is visible (adjacent to an empty block)
bool IsBlockFaceVisible(World* world, int x, int y, int z, int faceDir) {
    if (!world || !IsValidBlockPosition(x, y, z)) {
        return false;
    }
    
    // If the block itself is empty, no faces are visible
    if (GetBlock(world, x, y, z) == BLOCK_EMPTY) {
        return false;
    }
    
    // Check if the adjacent block in the direction of the face is empty or out of bounds
    int nx = x + DIRECTION_VECTORS[faceDir][0];
    int ny = y + DIRECTION_VECTORS[faceDir][1];
    int nz = z + DIRECTION_VECTORS[faceDir][2];
    
    // Faces at the edge of the world are visible
    if (!IsValidBlockPosition(nx, ny, nz)) {
        return true;
    }
    
    // Get the adjacent block
    BlockType adjacentBlock = GetBlock(world, nx, ny, nz);
    
    // A face is visible if:
    // 1. The adjacent block is empty, or
    // 2. The adjacent block is transparent (like jello) and the current block is not transparent
    return adjacentBlock == BLOCK_EMPTY || 
           (IsBlockTransparent(adjacentBlock) && !IsBlockTransparent(GetBlock(world, x, y, z)));
}

// Get a bounding box for a specific block
BoundingBox GetBlockBoundingBox(int x, int y, int z) {
    BoundingBox box;
    
    // Each block is 1x1x1 units
    box.min = (Vector3){ (float)x, (float)y, (float)z };
    box.max = (Vector3){ (float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f };
    
    return box;
}

// Simple collision detection between player and world
bool CheckCollision(World* world, BoundingBox playerBox) {
    if (!world) {
        return false;
    }
    
    // Determine the range of blocks to check based on player position
    int minX = (int)playerBox.min.x;
    int minY = (int)playerBox.min.y;
    int minZ = (int)playerBox.min.z;
    int maxX = (int)playerBox.max.x + 1;
    int maxY = (int)playerBox.max.y + 1;
    int maxZ = (int)playerBox.max.z + 1;
    
    // Clamp to world bounds
    minX = (minX < 0) ? 0 : minX;
    minY = (minY < 0) ? 0 : minY;
    minZ = (minZ < 0) ? 0 : minZ;
    maxX = (maxX >= WORLD_SIZE_X) ? WORLD_SIZE_X - 1 : maxX;
    maxY = (maxY >= WORLD_SIZE_Y) ? WORLD_SIZE_Y - 1 : maxY;
    maxZ = (maxZ >= WORLD_SIZE_Z) ? WORLD_SIZE_Z - 1 : maxZ;
    
    // Check collision with each block in range
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            for (int z = minZ; z <= maxZ; z++) {
                BlockType blockType = GetBlock(world, x, y, z);
                
                // Skip empty blocks and jello blocks
                if (blockType == BLOCK_EMPTY || blockType == BLOCK_JELLO) {
                    continue;
                }
                
                // Check collision with this block
                BoundingBox blockBox = GetBlockBoundingBox(x, y, z);
                if (CheckCollisionBoxes(playerBox, blockBox)) {
                    return true;
                }
            }
        }
    }
    
    return false;
}