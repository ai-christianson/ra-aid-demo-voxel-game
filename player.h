#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "voxel.h"

// Player constants
#define PLAYER_MOVE_SPEED 0.1f
#define PLAYER_JUMP_FORCE 0.15f
#define PLAYER_GRAVITY 0.005f
#define PLAYER_HEIGHT 1.8f
#define PLAYER_WIDTH 0.6f
#define PLAYER_DEPTH 0.6f
#define MOUSE_SENSITIVITY 0.003f

// Player structure
typedef struct {
    Vector3 position;        // Player position in the world
    Vector3 velocity;        // Current movement velocity
    Vector3 size;            // Player collision box size
    float rotationAngle;     // Player rotation (yaw)
    float pitchAngle;        // Camera pitch
    bool isOnGround;         // Whether the player is on the ground
    bool isJumping;          // Whether the player is currently jumping
} Player;

// Function prototypes
Player* CreatePlayer(World* world);
void DestroyPlayer(Player* player);
void UpdatePlayer(Player* player, World* world);
void HandlePlayerInput(Player* player);
void UpdatePlayerPhysics(Player* player, World* world);
BoundingBox GetPlayerBoundingBox(Player* player);
void UpdateCameraFromPlayer(Camera* camera, Player* player);

#endif // PLAYER_H