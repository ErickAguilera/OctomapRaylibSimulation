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
 
#include "raylib.h"
#include "raymath.h"
#include <iostream>

int main(void) {
  // -- Raylib Window Initialization
  const int screenWidth = 800;
  const int screenHeight = 450;
  InitWindow(screenWidth, screenHeight, "Model-Ray Collision");
  SetTargetFPS(60);

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
  model.materials[ 0 ].maps[ MATERIAL_MAP_DIFFUSE ].texture = texture;            // Set map diffuse texture
  Vector3 modelPos = {0.0f, 0.0f, 0.0f};                    // Set model position

  // -- Aux variables
  Vector3 origin = {0.0f, 0.0f, 0.0f};
  Vector2 centerPos = {screenWidth / 2.0f, screenHeight / 2.0f};
  Ray centerRay = {0};
  RayCollision centerCollision;

  // Main game loop
  while (!WindowShouldClose()) {
    // Update camera
    UpdateCamera(&camera, CAMERA_FIRST_PERSON);
    centerRay = GetMouseRay(centerPos, camera);

    // Get the collision info between the ray and the cube
    centerCollision = GetRayCollisionMesh(centerRay, model.meshes[ 0 ], MatrixIdentity());

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
  UnloadTexture(texture);
  UnloadModel(model);
  CloseWindow();

  return 0;
}

// -- TODO --
/*
 *  - Get bounding box of model and position the camera in one of the vertex
 *  - Try multiple camera movements and test the ray-collision
 *  - Implement the Octomap library to insert elements into the structure
 *  - Create a "point cloud" by shooting rays from the camera in different directions
 */