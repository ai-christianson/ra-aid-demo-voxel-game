#ifndef VOXEL_H
#define VOXEL_H

#include <raylib.h>
#include <stdbool.h>

// Block types enum
typedef enum {
    BLOCK_EMPTY = 0,
    BLOCK_GRASS,
    BLOCK_SAND,
    BLOCK_STONE,
    BLOCK_JELLO,
    BLOCK_TYPE_COUNT
} BlockType;

// World dimensions
#define WORLD_SIZE_X 64
#define WORLD_SIZE_Y 64
#define WORLD_SIZE_Z 64

// World structure
typedef struct {
    BlockType blocks[WORLD_SIZE_X][WORLD_SIZE_Y][WORLD_SIZE_Z];
} World;

// Function prototypes for world creation and management
World* CreateWorld(void);
void DestroyWorld(World* world);

// Block access and modification
BlockType GetBlock(World* world, int x, int y, int z);
void SetBlock(World* world, int x, int y, int z, BlockType type);
bool IsValidBlockPosition(int x, int y, int z);

// Collision detection
bool CheckCollision(World* world, BoundingBox playerBox);
BoundingBox GetBlockBoundingBox(int x, int y, int z);

// Rendering optimization
bool IsBlockFaceVisible(World* world, int x, int y, int z, int faceDir);
bool IsBlockTransparent(BlockType blockType);

#endif // VOXEL_H