#include <stdio.h>
#include "raylib.h"
#define ASTEROIDSLENGTH 10
#define SCREENWIDTH 500
#define SCREENHEIGHT 500
const int minSpeed = 1;
const int maxSpeed = 10;
const int minRadius = 5;
const int maxRadius = 20;
const int minXPosition = 0;
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

typedef enum {
  Dead, 
  Alive
} player_state_t;

//create an array of asteroid instances 
asteroid_t asteroids[ASTEROIDSLENGTH];

player_state_t player_state = Alive;

void initAsteroid(asteroid_t* ast);
void updateAsteroid(asteroid_t* ast);
void respawnAsteroid(asteroid_t*ast);

void initDriver(driver_t* ast);
void updateDriver(driver_t* ast);
void initAlive(int* collisionCount, int* livesCount, Sound explosion);

//create an instance of driver 
driver_t superNhan = {
  {(SCREENWIDTH/2) - 10, SCREENHEIGHT/2},  
  {(SCREENWIDTH/2) + 10, SCREENHEIGHT/2},
  {SCREENWIDTH/2, (SCREENHEIGHT/2) - 10}
};

int main(void) {
  InitAudioDevice();
  Sound explosion = LoadSound("SoundEffects/explosion.wav");
  SetSoundVolume(explosion, 0.1);
  InitWindow(SCREENWIDTH, SCREENHEIGHT, "SuperNhanAndLuiChat");
  SetWindowPosition(0,0);
  SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

  //loop through asteroids to init each asteroid
  for (int i = 0; i < ASTEROIDSLENGTH; i++) {
    initAsteroid(&asteroids[i]);
  }

  int collisionCount = 0;
  int livesCount = 3;
 // Main game loop
  while (!WindowShouldClose())    // Detect window close button or ESC key
  {
      BeginDrawing();
      ClearBackground(BLACK);
      DrawText(TextFormat("%d", collisionCount), 20, 20, 10, BLUE);
      DrawText(TextFormat("%d", livesCount), 40, 20, 10, RED);
    switch (player_state) {
      case Alive : 
        initAlive(&collisionCount, &livesCount, explosion);
        break;
      case Dead:
        DrawText(TextFormat("GAME OVER"), 100, SCREENHEIGHT/2, 50, ORANGE);
        DrawText(TextFormat("Press [R] to Restart"), 100, (SCREENHEIGHT/2) + 50, 20, ORANGE);
        if (IsKeyPressed(KEY_R)) {
          player_state = Alive;
          collisionCount = 0;
          livesCount = 3;
        }
    }
    EndDrawing();
  }

  CloseWindow();        // Close window and OpenGL context
  CloseAudioDevice();
  return 0;
}
void initAlive (int* collisionCount, int* livesCount, Sound explosion) {
  DrawTriangle(superNhan.v1, superNhan.v2, superNhan.v3, RED);
  for (int j = 0; j < ASTEROIDSLENGTH; j++) {
    updateAsteroid(&asteroids[j]);
    if (CheckCollisionPointCircle(superNhan.v3, asteroids[j].center, asteroids[j].radius)){
      PlaySound(explosion);
      *collisionCount += 1; 
      *livesCount -= 1;
      respawnAsteroid(&asteroids[j]);
    };
  if (*livesCount == 0) player_state = Dead;
  }
  updateDriver(&superNhan);
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
  ast->center.x = GetRandomValue(minXPosition, SCREENWIDTH);
  ast->center.y = 0;
  ast->currentSpeed = GetRandomValue(minSpeed,maxSpeed);
}
void updateAsteroid(asteroid_t* ast) {
  if (ast->center.y >= SCREENHEIGHT + ast->radius) {
    respawnAsteroid(ast);
  }
  ast->center.y = ast->center.y + ast->currentSpeed;
  DrawCircle(ast->center.x, ast->center.y, ast->radius, (Color){102, 153, 204, 255});
}

void respawnAsteroid(asteroid_t* ast) {
  ast->center.x = GetRandomValue(minXPosition, SCREENWIDTH);
  ast->center.y = 0 - 20;
  ast->radius = GetRandomValue(minRadius, maxRadius);
  ast->currentSpeed = GetRandomValue(minSpeed, maxSpeed);
}
