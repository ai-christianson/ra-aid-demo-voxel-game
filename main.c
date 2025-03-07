#include "raylib.h"
#include "voxel.h"
#include "player.h"
#include "terrain.h"

// Window dimensions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GAME_TITLE "Simple Voxel Game"

// Color definitions for different block types
const Color BLOCK_COLORS[BLOCK_TYPE_COUNT] = {
    { 0, 0, 0, 0 },        // BLOCK_EMPTY (transparent)
    { 34, 139, 34, 255 },  // BLOCK_GRASS (forest green)
    { 210, 180, 140, 255 },// BLOCK_SAND (tan)
    { 128, 128, 128, 255 },// BLOCK_STONE (gray)
    { 223, 64, 64, 150 }  // BLOCK_JELLO (semi-transparent red)
};

// Define render distance (how far to render blocks)
#define RENDER_DISTANCE 48

// Draw a single block face
void DrawBlockFace(int x, int y, int z, int faceDir, Color color) {
    // Define corners of a cube
    const float vertices[8][3] = {
        { x + 0.0f, y + 0.0f, z + 0.0f }, // 0: bottom-left-back
        { x + 1.0f, y + 0.0f, z + 0.0f }, // 1: bottom-right-back
        { x + 1.0f, y + 1.0f, z + 0.0f }, // 2: top-right-back
        { x + 0.0f, y + 1.0f, z + 0.0f }, // 3: top-left-back
        { x + 0.0f, y + 0.0f, z + 1.0f }, // 4: bottom-left-front
        { x + 1.0f, y + 0.0f, z + 1.0f }, // 5: bottom-right-front
        { x + 1.0f, y + 1.0f, z + 1.0f }, // 6: top-right-front
        { x + 0.0f, y + 1.0f, z + 1.0f }  // 7: top-left-front
    };

    // Define the indices for each face (CCW winding)
    const int faceIndices[6][4] = {
        { 1, 2, 6, 5 }, // +X face
        { 0, 4, 7, 3 }, // -X face
        { 3, 7, 6, 2 }, // +Y face
        { 0, 1, 5, 4 }, // -Y face
        { 4, 5, 6, 7 }, // +Z face
        { 0, 3, 2, 1 }  // -Z face
    };

    // Slightly adjust color based on face direction for better visibility
    Color faceColor = color;
    switch (faceDir) {
        case 0: // +X (right)
            faceColor.r = (unsigned char)(color.r * 0.9f);
            faceColor.g = (unsigned char)(color.g * 0.9f);
            faceColor.b = (unsigned char)(color.b * 0.9f);
            break;
        case 1: // -X (left)
            faceColor.r = (unsigned char)(color.r * 0.8f);
            faceColor.g = (unsigned char)(color.g * 0.8f);
            faceColor.b = (unsigned char)(color.b * 0.8f);
            break;
        case 2: // +Y (top)
            // Keep original color for top faces
            break;
        case 3: // -Y (bottom)
            faceColor.r = (unsigned char)(color.r * 0.7f);
            faceColor.g = (unsigned char)(color.g * 0.7f);
            faceColor.b = (unsigned char)(color.b * 0.7f);
            break;
        case 4: // +Z (front)
            faceColor.r = (unsigned char)(color.r * 0.85f);
            faceColor.g = (unsigned char)(color.g * 0.85f);
            faceColor.b = (unsigned char)(color.b * 0.85f);
            break;
        case 5: // -Z (back)
            faceColor.r = (unsigned char)(color.r * 0.75f);
            faceColor.g = (unsigned char)(color.g * 0.75f);
            faceColor.b = (unsigned char)(color.b * 0.75f);
            break;
    }

    // Get vertices for this face
    Vector3 v0 = { vertices[faceIndices[faceDir][0]][0], vertices[faceIndices[faceDir][0]][1], vertices[faceIndices[faceDir][0]][2] };
    Vector3 v1 = { vertices[faceIndices[faceDir][1]][0], vertices[faceIndices[faceDir][1]][1], vertices[faceIndices[faceDir][1]][2] };
    Vector3 v2 = { vertices[faceIndices[faceDir][2]][0], vertices[faceIndices[faceDir][2]][1], vertices[faceIndices[faceDir][2]][2] };
    Vector3 v3 = { vertices[faceIndices[faceDir][3]][0], vertices[faceIndices[faceDir][3]][1], vertices[faceIndices[faceDir][3]][2] };

    // Draw two triangles to form the face quad
    DrawTriangle3D(v0, v1, v2, faceColor);
    DrawTriangle3D(v0, v2, v3, faceColor);
}

// Render the voxel world
void RenderWorld(World* world, Player* player) {
    if (!world || !player) return;

    // Calculate the maximum distance to render blocks
    int renderHalfDistance = RENDER_DISTANCE / 2;

    // Convert player position to integer coordinates
    int playerX = (int)player->position.x;
    int playerY = (int)player->position.y;
    int playerZ = (int)player->position.z;

    // Calculate visible range
    int startX = playerX - renderHalfDistance;
    int startY = playerY - renderHalfDistance;
    int startZ = playerZ - renderHalfDistance;
    int endX = playerX + renderHalfDistance;
    int endY = playerY + renderHalfDistance;
    int endZ = playerZ + renderHalfDistance;

    // Clamp to world bounds
    startX = (startX < 0) ? 0 : startX;
    startY = (startY < 0) ? 0 : startY;
    startZ = (startZ < 0) ? 0 : startZ;
    endX = (endX >= WORLD_SIZE_X) ? WORLD_SIZE_X - 1 : endX;
    endY = (endY >= WORLD_SIZE_Y) ? WORLD_SIZE_Y - 1 : endY;
    endZ = (endZ >= WORLD_SIZE_Z) ? WORLD_SIZE_Z - 1 : endZ;

    // First pass: Render opaque blocks
    for (int x = startX; x <= endX; x++) {
        for (int y = startY; y <= endY; y++) {
            for (int z = startZ; z <= endZ; z++) {
                // Get the block type
                BlockType blockType = GetBlock(world, x, y, z);

                // Skip empty blocks and transparent blocks (will be rendered in second pass)
                if (blockType == BLOCK_EMPTY || IsBlockTransparent(blockType)) continue;

                // Get the color for this block type
                Color blockColor = BLOCK_COLORS[blockType];

                // Check and draw each visible face
                for (int faceDir = 0; faceDir < 6; faceDir++) {
                    if (IsBlockFaceVisible(world, x, y, z, faceDir)) {
                        DrawBlockFace(x, y, z, faceDir, blockColor);
                    }
                }
            }
        }
    }

    // Second pass: Render transparent blocks
    // Enable alpha blending for transparent objects
    BeginBlendMode(BLEND_ALPHA);
    for (int x = startX; x <= endX; x++) {
        for (int y = startY; y <= endY; y++) {
            for (int z = startZ; z <= endZ; z++) {
                // Get the block type
                BlockType blockType = GetBlock(world, x, y, z);

                // Only render transparent blocks (not empty and transparent)
                if (blockType == BLOCK_EMPTY || !IsBlockTransparent(blockType)) continue;

                // Get the color for this block type
                Color blockColor = BLOCK_COLORS[blockType];

                // Check and draw each visible face
                for (int faceDir = 0; faceDir < 6; faceDir++) {
                    if (IsBlockFaceVisible(world, x, y, z, faceDir)) {
                        DrawBlockFace(x, y, z, faceDir, blockColor);
                    }
                }
            }
        }
    }
    EndBlendMode();
}

// Draw a simple crosshair in the center of the screen
void DrawCrosshair() {
    int centerX = GetScreenWidth() / 2;
    int centerY = GetScreenHeight() / 2;
    
    // Draw a simple plus sign
    DrawLine(centerX - 10, centerY, centerX + 10, centerY, WHITE);
    DrawLine(centerX, centerY - 10, centerX, centerY + 10, WHITE);
}

int main(void) {
    // Initialize the window and OpenGL context
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
    
    // Target 60 frames per second
    SetTargetFPS(60);
    
    // Disable cursor for first-person mouse look
    DisableCursor();
    
    // Create and initialize the voxel world
    World* world = CreateWorld();
    GenerateTerrain(world);
    
    // Create and initialize the player
    Player* player = CreatePlayer(world);
    
    // Initialize the camera for a 3D perspective view
    Camera camera = { 0 };
    camera.position = (Vector3){ WORLD_SIZE_X / 2.0f, WORLD_SIZE_Y * 0.75f, WORLD_SIZE_Z / 2.0f };
    camera.target = (Vector3){ WORLD_SIZE_X / 2.0f, 0.0f, WORLD_SIZE_Z / 2.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };         // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                               // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;            // Camera projection type
    
    // Main game loop
    while (!WindowShouldClose()) {
        // Update game logic
        
        // Update player physics and handle input
        UpdatePlayer(player, world);
        
        // Update camera based on player position and orientation
        UpdateCameraFromPlayer(&camera, player);
        
        // Begin drawing
        BeginDrawing();
            ClearBackground(SKYBLUE);
            
            // Draw 3D elements
            BeginMode3D(camera);
                // Render the voxel world
                RenderWorld(world, player);
            EndMode3D();
            
            // Draw 2D UI elements
            DrawFPS(10, 10);
            DrawText("WASD - Move, SPACE - Jump, Mouse - Look", 10, 30, 20, BLACK);
            DrawCrosshair();
            
        EndDrawing();
    }
    
    // Cleanup resources
    DestroyPlayer(player);
    DestroyWorld(world);
    
    // Re-enable cursor before closing
    EnableCursor();
    
    // Close the window
    CloseWindow();
    
    return 0;
}