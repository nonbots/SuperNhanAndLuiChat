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
  Vector2 center;
  int currentSpeed;
} asteroid_t;

typedef struct driver {
  Vector2 v1;
  Vector2 v2;
  Vector2 v3;
  int currentSpeed;
} driver_t;

void initAsteroid(asteroid_t* ast);
void updateAsteroid(asteroid_t* ast);

void initDriver(driver_t* ast);
void updateDriver(driver_t* ast);

int main(void) {
  InitWindow(screenWidth, screenHeight, "raylib [shapes] example - basic shapes drawing");
  SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

  //create an array of asteroid instances 
  asteroid_t asteroids[asteroidsLength];
  //create an instance of driver 
  driver_t superNhan = {(Vector2){(screenWidth/2) - 10,screenHeight},  (Vector2){(screenWidth/2) + 10, screenHeight},(Vector2){screenWidth/2, screenHeight - 10}};
  //loop through asteroids to init each asteroid
  for (int i = 0; i < asteroidsLength; i++) {
    initAsteroid(&asteroids[i]);
  }

 // Main game loop
  while (!WindowShouldClose())    // Detect window close button or ESC key
  {
    BeginDrawing();
    ClearBackground((Color){0,255,0,255});
    DrawTriangle(superNhan.v1, superNhan.v2, superNhan.v3, RED);
    for (int j = 0; j < asteroidsLength; j++) {
      updateAsteroid(&asteroids[j]);
   //   CheckCollisionPointCircle(superNhan.v3, asteroids[j], float radius
    }

    updateDriver(&superNhan);
    EndDrawing();
  }

  CloseWindow();        // Close window and OpenGL context
  return 0;
}

void updateDriver(driver_t* super) {
  if (IsKeyDown(KEY_H)) {
    super->v1.x -= 1;
    super->v2.x -= 1;
    super->v3.x -= 1;
  }
  if (IsKeyDown(KEY_L)) {
    super->v1.x += 1;
    super->v2.x += 1;
    super->v3.x += 1;
  }
}
void initAsteroid(asteroid_t* ast) {
  ast->radius = GetRandomValue(minRadius, maxRadius);
  ast->center.x = GetRandomValue(minXPosition, maxXPosition);
  ast->center.y = 0;
  ast->currentSpeed = GetRandomValue(minSpeed,maxSpeed);
}
void updateAsteroid(asteroid_t* ast) {
  if (ast->center.y >= screenHeight + ast->radius) {
    ast->center.x = GetRandomValue(minXPosition, maxXPosition);
    ast->center.y = 0 - ast->radius;
    ast->radius = GetRandomValue(minRadius, maxRadius);
    ast->currentSpeed = GetRandomValue(minSpeed, maxSpeed);
  }
  ast->center.y = ast->center.y + ast->currentSpeed;
  DrawCircle(ast->center.x, ast->center.y, ast->radius, (Color){102, 153, 204, 255});
}

