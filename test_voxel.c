#include "voxel.h"
#include <stdio.h>

int main() {
    // Create a new world
    printf("Creating world...\n");
    World* world = CreateWorld();
    
    if (!world) {
        printf("Failed to create world!\n");
        return 1;
    }
    
    // Test setting and getting blocks
    printf("Testing block operations...\n");
    
    // Set some blocks
    SetBlock(world, 10, 10, 10, BLOCK_GRASS);
    SetBlock(world, 11, 10, 10, BLOCK_STONE);
    SetBlock(world, 10, 11, 10, BLOCK_SAND);
    
    // Verify blocks were set correctly
    printf("Block at (10,10,10): %d (expect %d)\n", 
           GetBlock(world, 10, 10, 10), BLOCK_GRASS);
    printf("Block at (11,10,10): %d (expect %d)\n", 
           GetBlock(world, 11, 10, 10), BLOCK_STONE);
    printf("Block at (10,11,10): %d (expect %d)\n", 
           GetBlock(world, 10, 11, 10), BLOCK_SAND);
    printf("Block at (12,12,12): %d (expect %d)\n", 
           GetBlock(world, 12, 12, 12), BLOCK_EMPTY);
    
    // Test face visibility
    printf("\nTesting face visibility...\n");
    for (int i = 0; i < 6; i++) {
        printf("Face %d of block (10,10,10) visible: %s\n", 
               i, IsBlockFaceVisible(world, 10, 10, 10, i) ? "Yes" : "No");
    }
    
    // Test collision detection
    printf("\nTesting collision detection...\n");
    BoundingBox playerBox1 = { 
        { 9.5f, 9.5f, 9.5f }, 
        { 10.5f, 10.5f, 10.5f } 
    };
    BoundingBox playerBox2 = { 
        { 20.0f, 20.0f, 20.0f }, 
        { 21.0f, 21.0f, 21.0f } 
    };
    
    printf("Collision with box1: %s (expect collision)\n", 
           CheckCollision(world, playerBox1) ? "Yes" : "No");
    printf("Collision with box2: %s (expect no collision)\n", 
           CheckCollision(world, playerBox2) ? "Yes" : "No");
    
    // Clean up
    printf("\nCleaning up...\n");
    DestroyWorld(world);
    printf("Test completed successfully!\n");
    
    return 0;
}