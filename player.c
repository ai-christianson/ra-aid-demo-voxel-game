#include "player.h"
#include <stdlib.h>
#include <math.h>

// Create and initialize a new player
Player* CreatePlayer(World* world) {
    Player* player = (Player*)malloc(sizeof(Player));
    
    if (player) {
        // Initialize player position above the center of the world
        player->position = (Vector3){ 
            WORLD_SIZE_X / 2.0f,  // Center X 
            WORLD_SIZE_Y * 0.75f, // High up in the world
            WORLD_SIZE_Z / 2.0f   // Center Z
        };
        
        // Initialize player velocity
        player->velocity = (Vector3){ 0.0f, 0.0f, 0.0f };
        
        // Set player size for collision detection
        player->size = (Vector3){ PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_DEPTH };
        
        // Initialize rotation (looking forward along Z-axis)
        player->rotationAngle = 0.0f;
        player->pitchAngle = 0.0f;
        
        // Initialize state
        player->isOnGround = false;
        player->isJumping = false;
        player->isInWater = false;
    }
    
    return player;
}

// Free player memory
void DestroyPlayer(Player* player) {
    if (player) {
        free(player);
    }
}

// Update player state (called once per frame)
void UpdatePlayer(Player* player, World* world) {
    // First handle user input
    HandlePlayerInput(player);
    
    // Then update physics
    UpdatePlayerPhysics(player, world);
}

// Handle keyboard and mouse input for player controls
void HandlePlayerInput(Player* player) {
    if (!player) return;
    
    // Reset lateral velocity
    player->velocity.x = 0;
    player->velocity.z = 0;
    
    // Get mouse movement for camera rotation (yaw and pitch)
    Vector2 mouseDelta = GetMouseDelta();
    
    // Update rotation angles based on mouse movement
    player->rotationAngle -= mouseDelta.x * MOUSE_SENSITIVITY;
    player->pitchAngle -= mouseDelta.y * MOUSE_SENSITIVITY;
    
    // Clamp pitch to prevent camera flipping
    if (player->pitchAngle > 1.5f) player->pitchAngle = 1.5f;
    if (player->pitchAngle < -1.5f) player->pitchAngle = -1.5f;
    
    // Calculate forward and right vectors based on player rotation
    Vector3 forward = { 
        sinf(player->rotationAngle), 
        0, 
        cosf(player->rotationAngle) 
    };
    
    Vector3 right = { 
        sinf(player->rotationAngle + PI/2), 
        0, 
        cosf(player->rotationAngle + PI/2) 
    };
    
    // Calculate the movement speed (reduced in water)
    float moveSpeed = player->isInWater ? PLAYER_MOVE_SPEED * WATER_MOVEMENT_FACTOR : PLAYER_MOVE_SPEED;
    
    // Move forward/backward (W/S keys)
    if (IsKeyDown(KEY_W)) {
        player->velocity.x += forward.x * moveSpeed;
        player->velocity.z += forward.z * moveSpeed;
    }
    if (IsKeyDown(KEY_S)) {
        player->velocity.x -= forward.x * moveSpeed;
        player->velocity.z -= forward.z * moveSpeed;
    }
    
    // Strafe left/right (A/D keys)
    if (IsKeyDown(KEY_A)) {
        player->velocity.x += right.x * moveSpeed;
        player->velocity.z += right.z * moveSpeed;
    }
    if (IsKeyDown(KEY_D)) {
        player->velocity.x -= right.x * moveSpeed;
        player->velocity.z -= right.z * moveSpeed;
    }
    
    // Jump (Space key)
    if (IsKeyPressed(KEY_SPACE) && player->isOnGround) {
        player->velocity.y = PLAYER_JUMP_FORCE;
        player->isJumping = true;
        player->isOnGround = false;
    }
}

// Update player physics including gravity and collision
void UpdatePlayerPhysics(Player* player, World* world) {
    if (!player || !world) return;
    
    // Store old position for collision resolution
    Vector3 oldPosition = player->position;
    
    // Check if player is in water
    int playerX = (int)player->position.x;
    int playerY = (int)player->position.y;
    int playerZ = (int)player->position.z;
    int headY = (int)(player->position.y + player->size.y * 0.9f); // Check at head level
    
    // Get block at player's position
    BlockType blockAtPlayer = GetBlock(world, playerX, playerY, playerZ);
    BlockType blockAtHead = GetBlock(world, playerX, headY, playerZ);
    
    // Update water state
    player->isInWater = (blockAtPlayer == BLOCK_WATER || blockAtHead == BLOCK_WATER);
    
    // Apply appropriate physics based on environment
    if (player->isInWater) {
        // Apply buoyancy force (opposite of gravity) when in water
        if (!player->isOnGround) {
            player->velocity.y += PLAYER_BUOYANCY;
        }
        
        // Apply a smaller gravity force in water (buoyancy counteracts some gravity)
        player->velocity.y -= PLAYER_GRAVITY * 0.5f;
        
        // Cap vertical velocity in water for smooth floating
        if (player->velocity.y > 0.1f) player->velocity.y = 0.1f;
        if (player->velocity.y < -0.1f) player->velocity.y = -0.1f;
    } else {
        // Apply normal gravity if not in water and not on ground
        if (!player->isOnGround) {
            player->velocity.y -= PLAYER_GRAVITY;
        }
    }
    
    // Update position based on velocity
    player->position.x += player->velocity.x;
    player->position.y += player->velocity.y;
    player->position.z += player->velocity.z;
    
    // Get player bounding box at new position
    BoundingBox playerBox = GetPlayerBoundingBox(player);
    
    // Check collision with world
    if (CheckCollision(world, playerBox)) {
        // Collision occurred, resolve it
        
        // Try to resolve X axis collision first
        player->position.x = oldPosition.x;
        playerBox = GetPlayerBoundingBox(player);
        
        // If still colliding, try Z axis
        if (CheckCollision(world, playerBox)) {
            player->position.z = oldPosition.z;
            playerBox = GetPlayerBoundingBox(player);
            
            // If still colliding, must be Y axis
            if (CheckCollision(world, playerBox)) {
                player->position.y = oldPosition.y;
                
                // If we were moving down, we've hit the ground
                if (player->velocity.y < 0) {
                    player->isOnGround = true;
                }
                
                // Stop vertical movement
                player->velocity.y = 0;
            }
        }
    } else {
        // No collision, check if we're still on ground
        
        // Create a "feet sensor" box slightly below the player
        Vector3 feetPosition = player->position;
        feetPosition.y -= 0.1f;  // Small distance below player
        
        // Create a small box for the feet
        BoundingBox feetBox = {
            (Vector3){ feetPosition.x - player->size.x/2, feetPosition.y, feetPosition.z - player->size.z/2 },
            (Vector3){ feetPosition.x + player->size.x/2, feetPosition.y + 0.1f, feetPosition.z + player->size.z/2 }
        };
        
        // Check if feet are touching ground
        player->isOnGround = CheckCollision(world, feetBox);
    }
    
    // Ensure player doesn't fall through the bottom of the world
    if (player->position.y < 0) {
        player->position.y = 0;
        player->velocity.y = 0;
        player->isOnGround = true;
    }
}

// Get the bounding box for the player at their current position
BoundingBox GetPlayerBoundingBox(Player* player) {
    BoundingBox box;
    
    // Player box is centered on X and Z, but bottom-aligned on Y
    box.min = (Vector3){ 
        player->position.x - player->size.x/2,  // Left
        player->position.y,                     // Bottom
        player->position.z - player->size.z/2   // Back
    };
    
    box.max = (Vector3){ 
        player->position.x + player->size.x/2,  // Right
        player->position.y + player->size.y,    // Top
        player->position.z + player->size.z/2   // Front
    };
    
    return box;
}

// Update camera position and orientation based on player
void UpdateCameraFromPlayer(Camera* camera, Player* player) {
    if (!camera || !player) return;
    
    // Set camera position to player's head position (offset a bit from player position)
    camera->position = (Vector3){ 
        player->position.x,
        player->position.y + player->size.y * 0.9f, // Place at eye level (90% of height)
        player->position.z 
    };
    
    // Calculate the look direction based on player rotation and pitch
    Vector3 lookDirection = {
        sinf(player->rotationAngle) * cosf(player->pitchAngle),
        sinf(player->pitchAngle),
        cosf(player->rotationAngle) * cosf(player->pitchAngle)
    };
    
    // Set camera target by adding the direction vector to the position
    camera->target = (Vector3){ 
        camera->position.x + lookDirection.x,
        camera->position.y + lookDirection.y,
        camera->position.z + lookDirection.z
    };
    
    // Keep the up vector as (0, 1, 0) for normal orientation
    camera->up = (Vector3){ 0.0f, 1.0f, 0.0f };
}