/* FILE INFO
 * File:	octomap-viz.cpp	
 * Author:	Erick Yair A. C.
 * Mail:	erickyairac@hotmail.com
 *
 * Created: 2023-05-1, Mon, 6:26
 * Mod:        
 * 
 * Activity: Visualizer to load octomap-created files
 */

#include "raylib.h"
#include <octomap/octomap.h>

using namespace octomap;

#define GRID_SIZE 100
#define TREE_MAX_LEVEL 15   // 16 should be the resolution of the tree
#define TREE_MIN_LEVEL 8
#define TREE_DEPTH 12

static const Color COLOR_OCCUPIED = {0, 121, 241, 220};
static const Color COLOR_FREE = {0, 228, 48, 120};

// USAGE:
//      octomap-sim-ray [file.bt/file.ot]
int main(int argc, char **argv) {
  // -- Create an Octomap OcTree object
  OcTree tree(0.1);  // Resolution will be overwritten with the loaded object
  std::cout << argv[1] << std::endl;
  // Load the binary file into the OcTree
  if (!tree.readBinary(argv[1])) {
    std::cerr << "Failed to read octree file!" << std::endl;
    return -1;
  }

  // -- raylib window init
  const int screenWidth = 800;
  const int screenHeight = 450;
  InitWindow(screenWidth, screenHeight, "Octomap Viz");
  SetTargetFPS(60);

  // Initialize the camera
  Camera3D camera = {0};
  camera.position = (Vector3) {2.5f, 2.5f, 0.0f};   // Camera position
  camera.target = (Vector3) {0.0f, 1.0f, 1.0f};     // Camera looking at point
  camera.up = (Vector3) {0.0f, 1.0f, 0.0f};         // Camera up vector (rotation towards target)
  camera.fovy = 90.0f;                              // Camera field-of-view Y
  camera.projection = CAMERA_PERSPECTIVE;           // Camera projection type
  DisableCursor();                                  // Hide cursor and restrict movement within the screen

  // Specify the initial desired tree-depth level
  int level = TREE_DEPTH;
  // Create an iterator to iterate over all leaf nodes at the desired depth level
  octomap::OcTree::iterator it = tree.begin(level);
  octomap::OcTree::iterator end = tree.end();
  auto voxelSize = static_cast<float>(it.getSize());

  // Create an iterator to iterate over all leaf nodes at the desired depth level
  octomap::OcTree::iterator parent_it = tree.begin(level - 1);
  octomap::OcTree::iterator parent_end = tree.end();
  auto parentVoxelSize = static_cast<float>(parent_it.getSize());

  // Auxiliary flags
  bool drawFree = true;
  bool drawParentBox = false;
  bool drawGrid = true;
  bool isOccupied = false;

  // -- Main loop
  // Set window close button or ESC key to end loop
  while (!WindowShouldClose()) {
    // Update camera
    UpdateCamera(&camera, CAMERA_PERSPECTIVE);
    // Update Input
    if (IsKeyPressed(KEY_KP_ADD)) level = (level == TREE_MAX_LEVEL) ? level : level + 1;
    if (IsKeyPressed(KEY_KP_SUBTRACT)) level = (level == TREE_MIN_LEVEL) ? level : level - 1;
    if (IsKeyPressed(KEY_F)) drawFree = !drawFree;
    if (IsKeyPressed(KEY_P)) drawParentBox = !drawParentBox;
    if (IsKeyPressed(KEY_G)) drawGrid = !drawGrid;

    // Update structures
    it = tree.begin(level);
    end = tree.end();
    parent_it = tree.begin(level - 1);
    parent_end = tree.end();

    // -- Draw --
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode3D(camera);
    if (drawGrid) DrawGrid(GRID_SIZE, voxelSize);
    // -- DRAW VOXELS
    for (; it != end; ++it) {
      // Check if node is occupied or free
      isOccupied = tree.isNodeOccupied(*it);
      // Don't draw free nodes
      if (!isOccupied)
        if (!drawFree)
          continue;
      // Calculate the cube position
      octomap::point3d coordinate = it.getCoordinate();
      Vector3 cubePos = {
          coordinate.x(),
          coordinate.y(),
          coordinate.z(),
      };
      // Draw the cube!
      Color cubeColor = (isOccupied ? COLOR_OCCUPIED : COLOR_FREE);
      voxelSize = static_cast<float>(it.getSize());
      DrawCube(cubePos, voxelSize, voxelSize, voxelSize, cubeColor);
      DrawCubeWires(cubePos, voxelSize, voxelSize, voxelSize, BLACK);
    }
    // -- DRAW PARENT VOXELS
    // They are the voxels that are parents to the voxels of the current tree depth
    if (drawParentBox) {
      for (; parent_it != parent_end; ++parent_it) {
        // Calculate the cube position
        octomap::point3d coordinate = parent_it.getCoordinate();
        Vector3 cubePos = {
            coordinate.x(),
            coordinate.y(),
            coordinate.z(),
        };
        parentVoxelSize= static_cast<float>(parent_it.getSize());
        DrawCubeWires(cubePos, parentVoxelSize, parentVoxelSize, parentVoxelSize, BLACK);
      }
    }
    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
  } // Main loop end
  // De-Initialization
  CloseWindow();        // Close window and OpenGL context
  return 0;
}