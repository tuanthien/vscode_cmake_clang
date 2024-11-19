
#include "raylib.h"
#include "raymath.h"
#include <math.h>

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)
#define MAX_ENEMIES (100)
#define MAX_PROJECTILES (100)

#define DEZZNUTZ
const char *WINDOW_TITLE = "Window title";

typedef struct Enemy
{
  Vector2 pos;
  float colliderRadius;
} Enemy;
static int enemyCount = 0;
static Enemy enemies[MAX_ENEMIES] = {0};

typedef struct Projectile
{
  Vector2 pos;
  Vector2 dir;
  float speed;
  float radius;
  float time;
} Projectile;
static int projectileCount = 0;
static Projectile projectiles[MAX_PROJECTILES] = {0};

typedef struct GameState
{
  Vector2 playerPos;
  float shootTimer;
  float shootDelay;
  float projSpeed;
  float projRadius;
  int killCount;
  float playerRadius;
} GameState;

static const GameState DefaultState =
    {
        .playerPos = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f},
        .shootDelay = 0.3f,
        .projSpeed = 140.0f,
        .projRadius = 4.0f,
        .playerRadius = 20.0f,
};

int main()
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
  SetTargetFPS(60);

  Texture2D texture = LoadTexture("assets/test.png"); // Check README.md for how this works
  static GameState gameState = DefaultState;

  while (!WindowShouldClose())
  {
    BeginDrawing();

    ClearBackground(BLACK);

    static float gameTime = 0;
    gameTime += GetFrameTime();

    const int texture_x = SCREEN_WIDTH / 2 - texture.width / 2;
    const int texture_y = SCREEN_HEIGHT / 2 - texture.height / 2;

    const float speed = 100.0f;
    const float enemySpeed = 40 + gameTime / 10.0f;

    // Move the Player
    {
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
      {
        while (gameState.shootTimer >= gameState.shootDelay)
        {
          if (projectileCount >= MAX_PROJECTILES)
          {
            break;
          }

          Projectile projectile = {
              .pos = gameState.playerPos,
              .dir = Vector2Normalize(Vector2Subtract(GetMousePosition(), gameState.playerPos)),
              .speed = gameState.projSpeed,
              .radius = gameState.projRadius};
          projectiles[projectileCount++] = projectile;
          gameState.shootTimer -= gameState.shootDelay;
        }
      }

      if (gameState.shootTimer < gameState.shootDelay)
      {
        gameState.shootTimer += GetFrameTime();
      }

      if (IsKeyDown(KEY_A))
      {
        gameState.playerPos.x -= speed * GetFrameTime();
      }
      if (IsKeyDown(KEY_D))
      {
        gameState.playerPos.x += speed * GetFrameTime();
      }
      if (IsKeyDown(KEY_W))
      {
        gameState.playerPos.y -= speed * GetFrameTime();
      }
      if (IsKeyDown(KEY_S))
      {
        gameState.playerPos.y += speed * GetFrameTime();
      }
    }
    // DrawTexture(texture, texture_x, texture_y, WHITE);
    // DrawTextureEx(texture, pos, 0, 0.1f, WHITE);
    DrawCircle(gameState.playerPos.x, gameState.playerPos.y, gameState.playerRadius, GREEN);

    // Update Projectiles
    {
      for (int projIdx = 0; projIdx < projectileCount; projIdx++)
      {
        Projectile *proj = &projectiles[projIdx];
        proj->time += GetFrameTime();

        if (proj->time > 50)
        {
          *proj = projectiles[--projectileCount];
          continue;
        }
        proj->pos.x += proj->dir.x * proj->speed * GetFrameTime();
        proj->pos.y += proj->dir.y * proj->speed * GetFrameTime();

        DrawCircle(proj->pos.x, proj->pos.y, proj->radius, YELLOW);

        for (int enemyIdx = 0; enemyIdx < enemyCount; enemyIdx++)
        {
          Enemy *enemy = &enemies[enemyIdx];
          if (CheckCollisionCircles(proj->pos, proj->radius, enemy->pos, enemy->colliderRadius))
          {
            // Swap enemy out
            *enemy = enemies[--enemyCount];
            gameState.killCount++;

            if (gameState.killCount % 5 == 0)
            {
              gameState.playerRadius += 1.0f;
            }

            if (gameState.killCount % 10 == 0)
            {
              gameState.projRadius += 1.0f;
            }
            if (gameState.killCount > 10 && gameState.killCount % 15 == 0)
            {
              gameState.projSpeed += 10.0f;
            }
            if (gameState.killCount >= 20 && gameState.killCount % 20 == 0)
            {
              gameState.shootDelay -= 0.05f;
            }

            // Swap out Projectile
            *proj = projectiles[--projectileCount];

            break;
          }
        }
      }
    }

    // Spawn and Move Enemies
    {
      static float spawnTimer = 0;
      spawnTimer += GetFrameTime();
      const float spawnFrequency = 1.0f / (gameTime / 5.0f);
      const float minDistance = SCREEN_WIDTH / 2.0f;

      while (spawnTimer >= spawnFrequency)
      {
        if (enemyCount >= MAX_ENEMIES)
        {
          break;
        }

        const float angleRadians = GetRandomValue(0, 360) * PI / 180;
        Enemy enemy =
            {
                .pos = {SCREEN_WIDTH / 2.0f + cosf(angleRadians) * minDistance,
                        SCREEN_HEIGHT / 2.0f + sinf(angleRadians) * minDistance},
                .colliderRadius = 10};
        enemies[enemyCount++] = enemy;
        spawnTimer -= spawnFrequency;
      }

      for (int enemyIdx = 0; enemyIdx < enemyCount; enemyIdx++)
      {
        Enemy *enemy = &enemies[enemyIdx];

        // Move the enemy
        Vector2 dir = Vector2Normalize(Vector2Subtract(gameState.playerPos, enemy->pos));
        const float frameTime = GetFrameTime();
        enemy->pos.x += dir.x * enemySpeed * frameTime;
        enemy->pos.y += dir.y * enemySpeed * frameTime;

        if (CheckCollisionCircles(enemy->pos, enemy->colliderRadius, gameState.playerPos, gameState.playerRadius))
        {
          // Game Over
          gameState = DefaultState;
          projectileCount = 0;
          enemyCount = 0;
          break;
        }

        DrawCircle(enemy->pos.x, enemy->pos.y, enemy->colliderRadius, RED);
      }
    }

    const char *text = "OMG! IT WORKS!";
    const Vector2 text_size = MeasureTextEx(GetFontDefault(), text, 20, 1);
    DrawText(text, SCREEN_WIDTH / 2 - text_size.x / 2, texture_y + texture.height + text_size.y + 10, 20, BLACK);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}