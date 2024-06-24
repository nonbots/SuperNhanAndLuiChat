#include <stdio.h>
#include "raylib.h"

const int screenWidth = 500;
const int screenHeight = 500;
const int minSpeed = 1;
const int maxSpeed = 20;
const int minRadius = 5;
const int maxRadius = 20;
const int minXPosition = 0;
const int maxXPosition = screenWidth;
//asteriod struct
typedef struct asteroid {
  int radius;
  int xPosition;
  int yPosition;
  int currentSpeed;
} asteroid_t;


void drawAsteroid(asteroid_t* ast) {
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

  asteroid_t a = { 
    GetRandomValue(minRadius, maxRadius), 
    GetRandomValue(minXPosition, maxXPosition),
    0, 
    GetRandomValue(minSpeed,maxSpeed) 
  };
  // Main game loop
  while (!WindowShouldClose())    // Detect window close button or ESC key
  {
    BeginDrawing();
    ClearBackground((Color){0,255,0,255});
    drawAsteroid(&a);
    EndDrawing();
  }

  CloseWindow();        // Close window and OpenGL context
  return 0;
}
