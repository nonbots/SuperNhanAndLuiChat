#include <stdio.h>
#include <math.h>
#include "raylib.h"
#define ASTEROIDS_LENGTH 10
#define BOSS_LASERS_LENGTH 5
#define DRIVER_LASERS_LENGTH 3
#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500
#define BOSS_TELEPORT_TIME 100
#define SPAWN_BOOSTER_COUNTER 600
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
  float radius;
  Vector2 v1;
  Vector2 v2;
  Vector2 v3;
  Vector2 center;
  float rotation_v1;
  float rotation_v2;
  float rotation_v3;
  float speed;
  float rotation_speed;
} DriverType;

typedef struct {
  PlayerStateType state;
  float radius; 
  Vector2 center;
  int time_to_teleport;
  int life_count;
} BossType;

typedef struct {
  float radius;
  Vector2 center;
  float speed;
} BoosterType;

typedef struct {
  AsteroidType asteroids[ASTEROIDS_LENGTH];
  LaserType driver_laser;
  LaserType boss_lasers;
  BossType boss;
  DriverType driver;
  BoosterType booster;
} GameEntityType;

GameEntityType game_entity;
void init_booster(BoosterType* booster);
void boost_driver(DriverType* driver);
void update_booster(BoosterType* booster);
Vector2 get_vector_from_pivot (Vector2 pivot, int degrees, float radius);
void init_driver(DriverType* driver);
void update_driver(DriverType* driver);
void init_asteroids(AsteroidType* asteroids);
void update_asteroids(AsteroidType* asteroids);
void respawn_asteroids(AsteroidType* asteroids);
void init_boss(BossType* boss);
void respawn_boss(BossType* boss);
void respawn_driver_laser(LaserType* driver_laser, DriverType* driver);
void shoot_driver_laser(LaserType* driver_laser, float rotation);
void update_boss(BossType* boss);


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

  int spawn_booster_counter = 400; 
  int collisionCount = 0;
  bool isExit = false;
  LaserType* driver_laser = &game_entity.driver_laser;
  BossType* boss = &game_entity.boss;
  DriverType* driver = &game_entity.driver;
  AsteroidType* asteroids = game_entity.asteroids;
  BoosterType* booster = &game_entity.booster; 

  init_driver(driver);
  //this a pointer that point to the first struct 
  init_asteroids(asteroids);
  init_boss(boss);
  init_booster(booster);
  while (!WindowShouldClose() && !isExit)    // Detect window close button or ESC key
  {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText(TextFormat("%d", collisionCount), 20, 20, 10, BLUE);
    DrawText(TextFormat("%d", driver->life_count), 40, 20, 10, RED);
    DrawText(TextFormat("%d", boss->time_to_teleport), 60, 20, 10, PURPLE);
    DrawText(TextFormat("Booster Spawn Counter %d", spawn_booster_counter), 60, 20, 10, PURPLE);
    DrawText(TextFormat("%d", boss->life_count), 80, 20, 10, PURPLE);
    switch (driver->state) {

      case ALIVE : 
        spawn_booster_counter -= 1;
        update_asteroids(asteroids);
        update_driver(driver);

        if (spawn_booster_counter == 0) {
          init_booster(booster);
          spawn_booster_counter = SPAWN_BOOSTER_COUNTER;
        }
        update_booster(booster);
        //for loop through astroids 
        bool is_collision_driver_v1 = CheckCollisionPointCircle(driver->v1, booster->center, booster->radius);
        bool is_collision_driver_v2 = CheckCollisionPointCircle(driver->v2, booster->center, booster->radius);
        bool is_collision_driver_v3 = CheckCollisionPointCircle(driver->v3, booster->center, booster->radius);
        if ( is_collision_driver_v1 || is_collision_driver_v2 || is_collision_driver_v3) {
            booster->center = (Vector2){-booster->radius, -booster->radius};
            booster->speed = 0;
            boost_driver(driver);
        }
        for (int i = 0; i < ASTEROIDS_LENGTH; i++) {
          bool is_collision_v1 = CheckCollisionPointCircle(driver->v3, asteroids[i].center, asteroids[i].radius);
          bool is_collision_v2 = CheckCollisionPointCircle(driver->v1, asteroids[i].center, asteroids[i].radius);
          bool is_collision_v3 = CheckCollisionPointCircle(driver->v3, asteroids[i].center, asteroids[i].radius);

          if (is_collision_v1 || is_collision_v2 || is_collision_v3){
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
          update_boss(boss);
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
          if (IsKeyPressed(KEY_SPACE)) {
            PlaySound(shoot);
            driver_laser->state = SHOOT;
          }
        }
        if(driver_laser->state == SHOOT) {
          shoot_driver_laser(&game_entity.driver_laser, driver->rotation_v3);
           bool overTop = driver_laser->end_position.y <= 0;
           bool overBottom = driver_laser->end_position.y >= SCREEN_HEIGHT;
           bool overLeft = driver_laser->end_position.x <= 0;
           bool overRight = driver_laser->end_position.x >= SCREEN_WIDTH;
           if (overTop || overBottom || overLeft || overRight) driver_laser->state = SPAWN;
        }
        break;

      case DEAD:
        DrawText(TextFormat("GAME OVER"), 100, SCREEN_HEIGHT/2, 50, ORANGE);
        DrawText(TextFormat("Press [R] to Restart"), 100, (SCREEN_HEIGHT/2) + 50, 20, ORANGE);
        DrawText(TextFormat("Press [E] to Exit"), 100, (SCREEN_HEIGHT/2) + 100, 20, ORANGE);
        if (IsKeyPressed(KEY_E)) isExit = true; 
        if (IsKeyPressed(KEY_R)) {
          driver->state = ALIVE;
          driver->speed = 1;
          collisionCount = 0;
          driver->life_count = 3;
          boss->life_count = ALIVE;
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

void init_booster(BoosterType* booster) {
  booster->speed = 1;
  booster->radius = 10;
  float rand_x =GetRandomValue(booster->radius, SCREEN_WIDTH - booster-> radius);
  float y = -booster->radius;
  booster->center = (Vector2){rand_x, y};
}

void update_booster(BoosterType* booster) {
  booster->center.y = booster->center.y += booster->speed; 
  DrawCircle((int)booster->center.x, (int)booster->center.y, booster->radius, GREEN);
}
void boost_driver(DriverType* driver) {
  driver->speed = 5;
}
void shoot_driver_laser(LaserType* driver_laser, float rotation) {
  driver_laser->start_position = driver_laser->end_position;
  driver_laser->end_position = get_vector_from_pivot(driver_laser->start_position, rotation, 5);
  DrawLineV(driver_laser->start_position, driver_laser->end_position, GREEN);
}

void respawn_driver_laser(LaserType* driver_laser, DriverType* driver) {
  driver_laser->start_position = driver->v3;
  driver_laser->end_position = get_vector_from_pivot(driver->v3, driver->rotation_v3, 5);
  DrawLineV(driver_laser->start_position, driver_laser->end_position, GREEN);
}

void respawn_boss(BossType* boss) {
  boss->center.x = (float)GetRandomValue(0, SCREEN_WIDTH);
  float y_position = GetRandomValue(boss->radius, SCREEN_HEIGHT - boss->radius); 
  if (y_position > SCREEN_HEIGHT/2 - boss->radius * 2 && y_position < SCREEN_HEIGHT/2 + boss->radius * 2) {
    y_position -= SCREEN_HEIGHT/2 - boss->radius * 4;
  }
  boss->center.y = y_position;
  DrawCircle((int)boss->center.x, (int)boss->center.y, boss->radius, PURPLE);
}

void init_driver(DriverType* driver) {
  driver->state = ALIVE; 
  driver->life_count = 3;
  driver->center = (Vector2){SCREEN_WIDTH/2 , SCREEN_HEIGHT/2};
  driver->radius = 10;
  driver->speed = 1;
  driver->rotation_speed = 1;
  driver->rotation_v1 = 270;
  driver->rotation_v2 = 150;
  driver->rotation_v3 = 30;
  driver->v1 = get_vector_from_pivot(driver->center, driver->rotation_v1, driver->radius);
  driver->v2 = get_vector_from_pivot(driver->center, driver->rotation_v2, driver->radius);
  driver->v3 = get_vector_from_pivot(driver->center, driver->rotation_v3, driver->radius);
  DrawTriangle(driver->v1, driver->v2, driver->v3, RED);
}
void init_boss (BossType* boss) {
  boss->state = ALIVE;
  boss->radius = 25;
  float y_position = GetRandomValue(boss->radius, SCREEN_HEIGHT - boss->radius); 
  if (y_position > SCREEN_HEIGHT/2 - boss->radius * 2 && y_position < SCREEN_HEIGHT/2 + boss->radius * 2) {
    y_position -= SCREEN_HEIGHT/2 - boss->radius * 4;
  }
  boss->center = (Vector2){(float)GetRandomValue(boss->radius * 2, SCREEN_WIDTH - boss->radius), y_position};
  boss->time_to_teleport = BOSS_TELEPORT_TIME;
  boss->life_count = BOSS_LIFE_COUNT_TOTAL;
  DrawCircle((int)boss->center.x, (int)boss->center.y, boss->radius, PURPLE);
}
void update_boss (BossType* boss) {
  boss->center = boss->center;
  DrawCircle((int)boss->center.x, (int)boss->center.y, boss->radius, PURPLE);
}

void update_driver(DriverType* driver) {
  if (IsKeyDown(KEY_H)) {
    driver->center.x -= driver->speed;
  }
  if (IsKeyDown(KEY_L)) {
    driver->center.x += driver->speed;
  }
  if (IsKeyDown(KEY_D)) { // rotate right
    if ((int)driver->rotation_v1 % 360 == 0) driver->rotation_v1 = 0;
    if ((int)driver->rotation_v2 % 360 == 0) driver->rotation_v2 = 0;
    if ((int)driver->rotation_v3 % 360 == 0) driver->rotation_v3 = 0;
    driver->rotation_v1 += driver->rotation_speed;     
    driver->rotation_v2 += driver->rotation_speed;     
    driver->rotation_v3 += driver->rotation_speed;     
  }
  if (IsKeyDown(KEY_S)) { // rotate left
    if ((int)driver->rotation_v1 % 360 == 0) driver->rotation_v1 = 360;
    if ((int)driver->rotation_v2 % 360 == 0) driver->rotation_v2 = 360;
    if ((int)driver->rotation_v3 % 360 == 0) driver->rotation_v3 = 360;
    driver->rotation_v1 -= driver->rotation_speed;     
    driver->rotation_v2 -= driver->rotation_speed;     
    driver->rotation_v3 -= driver->rotation_speed;     
  }
  driver->v1 = get_vector_from_pivot(driver->center, driver->rotation_v1, driver->radius);
  driver->v2 = get_vector_from_pivot(driver->center, driver->rotation_v2, driver->radius);
  driver->v3 = get_vector_from_pivot(driver->center, driver->rotation_v3, driver->radius);

  printf("x: %f y: %f\n", driver->v1.x, driver->v1.y);
  printf("x: %f y: %f\n", driver->v2.x, driver->v2.y);
  printf("x: %f y: %f\n", driver->v3.x, driver->v3.y);
  printf("rotation_v1: %f\n", driver->rotation_v1);
  printf("rotation_v2: %f\n", driver->rotation_v2);
  printf("rotation_v3: %f\n", driver->rotation_v3);
  DrawTriangle(driver->v1, driver->v2, driver->v3, RED);
}

Vector2 get_vector_from_pivot (Vector2 pivot, int degrees, float radius ) {
  float cosx = (float)cos(degrees * (PI/180));
  float siny = (float)sin(degrees * (PI/180));
  return (Vector2){pivot.x + (radius * cosx), pivot.y + (radius * siny)};
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
