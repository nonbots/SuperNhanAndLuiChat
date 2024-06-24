#include <stdio.h>
#include "raylib.h"

const int screenWidth = 500;
const int screenHeight = 500;
const int minSpeed = 1;
const int maxSpeed = 10;
const int minRadius = 5;
const int maxRadius = 20;
const int minXPosition = 0;
const int maxXPosition = screenWidth;
const int asteroidsLength = 10;
//asteroid struct
typedef struct asteroid {
  int radius;
  int xPosition;
  int yPosition;
  int currentSpeed;
} asteroid_t;

void initAsteroid(asteroid_t* ast) {

  ast->radius = GetRandomValue(minRadius, maxRadius);
  ast->xPosition = GetRandomValue(minXPosition, maxXPosition);
  ast->yPosition = 0; 
  ast->currentSpeed = GetRandomValue(minSpeed,maxSpeed);
}
void updateAsteroid(asteroid_t* ast) {
  if (ast->yPosition >= screenHeight + ast->radius) {
    ast->yPosition = 0 - ast->radius;
    ast->xPosition = GetRandomValue(minXPosition, maxXPosition);
    ast->radius = GetRandomValue(minRadius, maxRadius);
    ast->currentSpeed = GetRandomValue(minSpeed, maxSpeed);
  }
  ast->yPosition = ast->yPosition + ast->currentSpeed;
  DrawCircle(ast->xPosition, ast->yPosition, ast->radius, (Color){102, 153, 204, 255});
}

int main(void) {
  InitWindow(screenWidth, screenHeight, "raylib [shapes] example - basic shapes drawing");
  SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

  //create an array of asteroid instances 
  asteroid_t asteroids[asteroidsLength];
  //loop through asteroids to init each asteroid
  for (int i = 0; i < asteroidsLength; i++) {
    initAsteroid(&asteroids[i]);
  }
 // Main game loop
  while (!WindowShouldClose())    // Detect window close button or ESC key
  {
    BeginDrawing();
    ClearBackground((Color){0,255,0,255});
    for (int j = 0; j < asteroidsLength; j++) {
      updateAsteroid(&asteroids[j]);
    }
    EndDrawing();
  }

  CloseWindow();        // Close window and OpenGL context
  return 0;
}
