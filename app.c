#include <stdio.h>
#include "raylib.h"
#define ASTEROIDS_LENGTH 10
#define BOSS_LASERS_LENGTH 5
#define DRIVER_LASERS_LENGTH 3
#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500
#define BOSS_TELEPORT_TIME 100
#define BOSS_LIFE_COUNT_TOTAL 10
//asteroids 
#define MIN_SPEED 1
#define MAX_SPEED 10
#define MIN_RADIUS 5 
#define MAX_RADIUS 10
#define MIN_X_POSITION 0
//laser
#define LASER_SPEED 10
//state enums
typedef enum {
  DEAD, 
  ALIVE
} PlayerStateType;

typedef enum {
  SPAWN,
  SHOOT
} LaserStateType;

//asteroid struct
typedef struct {
  float radius;
  Vector2 center;
  float speed;
} AsteroidType;

typedef struct {
  LaserStateType state;
  Vector2 start_position;
  Vector2 end_position;
  float speed;
} LaserType;

typedef struct {
  PlayerStateType state;
  int life_count;
  Vector2 v1;
  Vector2 v2;
  Vector2 v3;
  float speed;
} DriverType;

typedef struct {
  PlayerStateType state;
  float radius; 
  Vector2 center;
  float time_to_teleport;
  int life_count;
} BossType;


typedef struct {
  AsteroidType asteroids[ASTEROIDS_LENGTH];
  LaserType driver_laser;
  LaserType boss_lasers;
  BossType boss;
  DriverType driver;
} GameEntityType;

GameEntityType game_entity;

void init_driver(DriverType* driver);
void update_driver(DriverType* driver);
void init_asteroids(AsteroidType* asteroids);
void update_asteroids(AsteroidType* asteroids);
void respawn_asteroids(AsteroidType* asteroids);
void init_boss(BossType* boss);
void respawn_boss(BossType* boss);
void respawn_driver_laser(LaserType* driver_laser, DriverType* driver);
void shoot_driver_laser(LaserType* driver_laser);

int main(void) {
  InitAudioDevice();
  Sound explosion = LoadSound("SoundEffects/explosion.wav");
  Sound shoot = LoadSound("SoundEffects/laserShoot.wav");
  Sound bossExplosion = LoadSound("SoundEffects/bossExplosion.wav");
  SetSoundVolume(explosion, 0.1f);
  SetSoundVolume(bossExplosion, 0.5f);
  SetSoundVolume(shoot, 0.5f);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "SuperNhanAndLuiChat");
  SetWindowPosition(0,0);
  SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

  init_driver(&game_entity.driver);

  //this a pointer that point to the first struct 
  init_asteroids(game_entity.asteroids);

  init_boss(&game_entity.boss);

  int collisionCount = 0;
  bool isExit = false;
  LaserType* driver_laser = &game_entity.driver_laser;
  BossType* boss = &game_entity.boss;
  DriverType* driver = &game_entity.driver;
  AsteroidType* asteroids = game_entity.asteroids;
  while (!WindowShouldClose() && !isExit)    // Detect window close button or ESC key
  {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText(TextFormat("%d", collisionCount), 20, 20, 10, BLUE);
    DrawText(TextFormat("%d", driver->life_count), 40, 20, 10, RED);
    DrawText(TextFormat("%d", boss->time_to_teleport), 60, 20, 10, PURPLE);
    DrawText(TextFormat("%d", boss->life_count), 80, 20, 10, PURPLE);
    switch (driver->state) {
      case ALIVE : 

        update_asteroids(game_entity.asteroids);
        update_driver(&game_entity.driver);

        //for loop through astroids 
        for (int i = 0; i < ASTEROIDS_LENGTH; i++) {
          if (CheckCollisionPointCircle(driver->v3, asteroids[i].center, asteroids[i].radius)){
            PlaySound(explosion);
            collisionCount += 1; 
            driver->life_count -= 1;
            respawn_asteroids(&asteroids[i]);
          };

          if (CheckCollisionPointCircle(driver_laser->start_position, asteroids[i].center, asteroids[i].radius)){
            respawn_asteroids(&asteroids[i]);
            driver_laser->state = SPAWN;
          }
        }

        if (driver->life_count == 0) driver->state = DEAD;
        if (boss->state == ALIVE) {
          boss->time_to_teleport -= 1;
          if (boss->time_to_teleport == 0) {
            boss->time_to_teleport = 200;
            respawn_boss(&game_entity.boss);
          }
          if (CheckCollisionPointCircle(driver_laser->start_position,  boss->center, boss->radius)) {
            PlaySound(bossExplosion);
            boss->life_count -= 1;
            respawn_boss(&game_entity.boss);
          };
          if (boss->life_count == 0) boss->state = DEAD;
        }
        if (driver_laser->state == SPAWN) {
          respawn_driver_laser(&game_entity.driver_laser, &game_entity.driver);
          if (IsKeyPressed(KEY_A)) {
            PlaySound(shoot);
            driver_laser->state = SHOOT;
          }
        }
        if(driver_laser->state == SHOOT) {
          shoot_driver_laser(&game_entity.driver_laser);
           if (driver_laser->end_position.y == 0) driver_laser->state = SPAWN;
        }
        
        break;
      case DEAD:
        DrawText(TextFormat("GAME OVER"), 100, SCREEN_HEIGHT/2, 50, ORANGE);
        DrawText(TextFormat("Press [R] to Restart"), 100, (SCREEN_HEIGHT/2) + 50, 20, ORANGE);
        DrawText(TextFormat("Press [E] to Exit"), 100, (SCREEN_HEIGHT/2) + 100, 20, ORANGE);
        if (IsKeyPressed(KEY_E)) isExit = true; 
        if (IsKeyPressed(KEY_R)) {
          driver->state = ALIVE;
          collisionCount = 0;
          driver->state = 3;
          boss->center.x = (float)GetRandomValue(0, SCREEN_WIDTH);
          boss->center.y = (float)GetRandomValue(100, (SCREEN_HEIGHT/2) - 120);
          boss->life_count = BOSS_LIFE_COUNT_TOTAL;
          boss->state = ALIVE;
        }
        break;
    }
    EndDrawing();
  }

  CloseWindow();        // Close window and OpenGL context
  CloseAudioDevice();
  return 0;
}

void shoot_driver_laser(LaserType* driver_laser) {
   driver_laser->start_position.y -= LASER_SPEED;
   driver_laser->end_position.y -= LASER_SPEED;
}

void respawn_driver_laser(LaserType* driver_laser, DriverType* driver) {
  driver_laser->start_position = driver->v3;
  driver_laser->end_position.x = driver->v3.x;
  driver_laser->end_position.y = driver->v3.y + 10;
  DrawLineV(driver_laser->start_position, driver_laser->end_position, GREEN);
}
void respawn_boss(BossType* boss) {
  boss->center.x = (float)GetRandomValue(0, SCREEN_WIDTH);
  boss->center.y = (float)GetRandomValue(100, (SCREEN_HEIGHT/2) - 120);
}

void init_driver(DriverType* driver) {
  driver->state = ALIVE; 
  driver->life_count = 3;
  driver->v1 = (Vector2){(SCREEN_WIDTH/2) - 10, SCREEN_HEIGHT/2};
  driver->v2 = (Vector2){(SCREEN_WIDTH/2) + 10, SCREEN_HEIGHT/2};
  driver->v3 = (Vector2){SCREEN_WIDTH/2, (SCREEN_HEIGHT/2) - 10};
  driver->speed = 1;
  DrawTriangle(driver->v1, driver->v2, driver->v3, RED);
}
void init_boss (BossType* boss) {
  boss->radius = 30;
  boss->center = (Vector2){(float)GetRandomValue(0, SCREEN_WIDTH), (float)GetRandomValue(0, (SCREEN_HEIGHT/2) - 120)};
  boss->time_to_teleport = BOSS_TELEPORT_TIME;
  boss->life_count = BOSS_LIFE_COUNT_TOTAL;
  DrawCircle((int)boss->center.x, (int)boss->center.y, boss->radius, PURPLE);
}

void update_driver(DriverType* driver) {
  if (IsKeyDown(KEY_H)) {
    driver->v1.x -= driver->speed;
    driver->v2.x -= driver->speed;
    driver->v3.x -= driver->speed;
  }
  if (IsKeyDown(KEY_L)) {
    driver->v1.x += driver->speed;
    driver->v2.x += driver->speed;
    driver->v3.x += driver->speed;
  }
}

void init_asteroids(AsteroidType* asteroids) {
  //loop through asteroids to init each asteroid
  for (int i = 0; i < ASTEROIDS_LENGTH; i++) {
    asteroids[i].radius = (float)GetRandomValue(MIN_RADIUS, MAX_RADIUS);
    asteroids[i].center.x = (float)GetRandomValue(MIN_X_POSITION, SCREEN_WIDTH);
    asteroids[i].center.y = 0;
    asteroids[i].speed = (float)GetRandomValue(MIN_SPEED, MAX_SPEED);
  }
}

void update_asteroids(AsteroidType* asteroids) {
  for (int i = 0; i < ASTEROIDS_LENGTH; i++) {
    asteroids[i].center.y = asteroids[i].center.y + asteroids[i].speed;
    if (asteroids[i].center.y >= SCREEN_HEIGHT + asteroids[i].radius) {
      respawn_asteroids(&asteroids[i]);
    }
    DrawCircle((int)asteroids[i].center.x, (int)asteroids[i].center.y, asteroids[i].radius, (Color){102, 153, 204, 255});
  }
}

void respawn_asteroids(AsteroidType* asteroid) {
  asteroid->center.x = (float)GetRandomValue(MIN_X_POSITION, SCREEN_WIDTH);
  asteroid->center.y = 0 - 20;
  asteroid->radius = (float)GetRandomValue(MIN_RADIUS, MAX_RADIUS);
  asteroid->speed = (float)GetRandomValue(MIN_SPEED, MAX_SPEED);
}
