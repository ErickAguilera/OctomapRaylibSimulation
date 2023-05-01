/* FILE INFO
 * File:	octomap-sim-ray.cpp	
 * Author:	Erick Yair A. C.
 * Mail:	erickyairac@hotmail.com
 *
 * Created: 2023-05-1, Mon, 6:37
 * Mod:        
 * 
 * Activity: Program to simulate a camera and a model to reconstruct it using octomap.
 */

#include <iostream>
#include "raylib.h"
#include "raymath.h"
#include <octomap/octomap.h>

using namespace octomap;

#define GRID_SIZE 100
#define TREE_MAX_LEVEL 15   // 16 should be the resolution of the tree
#define TREE_MIN_LEVEL 8
#define TREE_DEPTH 12

static const Color COLOR_OCCUPIED = {0, 121, 241, 220};
static const Color COLOR_FREE = {0, 228, 48, 120};


int main(void) {
  // -- Create an Octomap OcTree object
  OcTree tree(0.5);  // Resolution will be overwritten with the loaded object

  // -- Raylib Window Initialization
  const int screenWidth = 800;
  const int screenHeight = 450;
  InitWindow(screenWidth, screenHeight, "Model-Ray Collision");
  SetTargetFPS(60);
  InitAudioDevice();      // Initialize audio device
  Sound fxWav = LoadSound("res/audio/coin.wav");         // Load WAV audio file

  // -- Define the camera
  Camera3D camera = {0};
  camera.position = (Vector3) {50.0f, 50.0f, 50.0f};
  camera.target = (Vector3) {0.0f, 0.0f, 0.0f};
  camera.up = (Vector3) {0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  DisableCursor();

  // -- Load model and texture
  Model model = LoadModel("res/models/obj/castle.obj");                 // Load model
  Texture2D texture = LoadTexture("res/models/obj/castle_diffuse.png"); // Load model texture
  model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;            // Set map diffuse texture
  Vector3 modelPos = {0.0f, 0.0f, 0.0f};                    // Set model position

  // -- Aux variables
  Vector3 origin = {0.0f, 0.0f, 0.0f};
  Vector2 centerPos = {screenWidth / 2.0f, screenHeight / 2.0f};
  Ray centerRay = {0};
  RayCollision centerCollision;
  point3d rayOrigin, rayEnd;

  // Main game loop
  while (!WindowShouldClose()) {
    // Update camera
    UpdateCamera(&camera, CAMERA_FIRST_PERSON);
    centerRay = GetMouseRay(centerPos, camera);

    // Get the collision info between the ray and the cube
    centerCollision = GetRayCollisionMesh(centerRay, model.meshes[0], MatrixIdentity());
    // TODO: Make the following an interface
    rayOrigin.x() = camera.position.x;
    rayOrigin.y() = camera.position.y;
    rayOrigin.z() = camera.position.z;
    rayEnd.x() = centerCollision.point.x;
    rayEnd.y() = centerCollision.point.y;
    rayEnd.z() = centerCollision.point.z;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && centerCollision.hit) {
      tree.insertRay(rayOrigin, rayEnd);
      PlaySound(fxWav);
    }

    //  -- BEGIN DRAWING
    BeginDrawing();
    ClearBackground(RAYWHITE);
    // 3D Start
    BeginMode3D(camera);
    DrawGrid(100, 1.0f);
    DrawModel(model, origin, 1.0f, WHITE);
    if (centerCollision.hit) {
      DrawSphere(centerCollision.point, 1.0f, YELLOW);
    }
    EndMode3D();
    // 3D End
    DrawCircle(static_cast<int>(centerPos.x), static_cast<int>(centerPos.y), 5.0f, GREEN);
    DrawFPS(10, 10);
    EndDrawing();
  } // while loop end

  // De-Initialization
  UnloadSound(fxWav);     // Unload sound data
  CloseAudioDevice();     // Close audio device
  UnloadTexture(texture);
  UnloadModel(model);
  CloseWindow();

  // Save tree
  tree.writeBinary("test.bt");

  return 0;
}

// -- TODO --
/*
 *  - Get bounding box of model and position the camera in one of the vertex
 *  - Create a "point cloud" by shooting rays from the camera in different directions
 */