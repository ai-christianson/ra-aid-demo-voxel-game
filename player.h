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

// Water physics constants
#define PLAYER_BUOYANCY 0.3f      // Buoyancy force in water
#define WATER_MOVEMENT_FACTOR 0.6f // Movement speed reduction in water
#define PLAYER_SWIM_SPEED 0.15f   // Speed for vertical swimming movement
#define WATER_MAX_VERTICAL_SPEED 0.4f // Maximum vertical velocity in water
#define SURFACE_BUOYANCY_FACTOR 1.2f // Stronger buoyancy near the surface
#define UNDERWATER_GRAVITY_FACTOR 0.3f // Reduced gravity when fully underwater
#define SURFACE_GRAVITY_FACTOR 0.6f // Gravity when at water surface (higher than underwater)
#define WATER_TRANSITION_RATE 0.02f // Speed of transition between water states

// Player structure
typedef struct {
    Vector3 position;        // Player position in the world
    Vector3 velocity;        // Current movement velocity
    Vector3 size;            // Player collision box size
    float rotationAngle;     // Player rotation (yaw)
    float pitchAngle;        // Camera pitch
    bool isOnGround;         // Whether the player is on the ground
    bool isJumping;          // Whether the player is currently jumping
    bool isInWater;          // Whether the player is in water
    bool isFullyUnderwater;  // Whether the player's head is underwater
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