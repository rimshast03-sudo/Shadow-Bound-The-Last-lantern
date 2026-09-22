#ifndef SENTRY_HPP
#define SENTRY_HPP


#include "camera.hpp"
#include "config.hpp"
#include "iGraphics.h"
#include "player.hpp"
#include "sounds.hpp"
#include "structs.hpp"
#include "textures.hpp"
#include <math.h>

inline int checkCollisionSentry(int x1, int y1, int w1, int h1, int x2, int y2,
                                int w2, int h2) {
  return !(x1 + w1 < x2 || x2 + w2 < x1 || y1 + h1 < y2 || y2 + h2 < y1);
}

void initSentries(struct Sentry sentries[]) {
  for (int i = 0; i < MAX_SENTRIES; i++) {
    sentries[i].active = 0;
    sentries[i].state = SENTRY_INACTIVE;
    sentries[i].frame = 0;
    sentries[i].vx = 0;
    sentries[i].vy = 0;
    sentries[i].subStateTimer = 0;
    sentries[i].animationTimer = 0;
    sentries[i].facingRight = 1;
    sentries[i].isAggro = 0;
  }
}

void spawnSentry(struct Sentry *sentry, int x, int y) {
  sentry->x = x;
  sentry->y = y;
  sentry->vx = 0;
  sentry->vy = 0;
  sentry->frame = 0;
  sentry->active = 1;
  sentry->state = SENTRY_WAKING;
  sentry->patrolStartX = x;
  sentry->subStateTimer = 0;
  sentry->animationTimer = 0;
  sentry->facingRight = 1;
  sentry->maxHealth = SENTRY_MAX_HEALTH;
  sentry->currentHealth = SENTRY_MAX_HEALTH;
  sentry->invincibilityTimer = 0;
  sentry->damageAnimTimer = 0;
  sentry->damageFrame = 0;
  sentry->isAggro = 0;
  sentry->attackCooldown = 0;
}

void updateSentries(struct Sentry sentries[], Player *player, struct Arrow arrows[], int gameState) {
  for (int i = 0; i < MAX_SENTRIES; i++) {
    if (!sentries[i].active && sentries[i].state == SENTRY_INACTIVE) {
      int spawnX;
      if (gameState == LEVEL3_STATE) {
        spawnX = SENTRY_SPAWN_L3_1_X;
        if (i == 1)
          spawnX = SENTRY_SPAWN_L3_2_X;
        else if (i == 2)
          spawnX = SENTRY_SPAWN_L3_3_X;
      } else {
        spawnX = SENTRY_SPAWN_1_X;
        if (i == 1)
          spawnX = SENTRY_SPAWN_2_X;
        else if (i == 2)
          spawnX = SENTRY_SPAWN_3_X;
      }

      int dx = player->x - spawnX;
      int dy = player->y - GROUND_Y;
      float spawnDistance = sqrt((double)(dx * dx + dy * dy));

      if (spawnDistance <= CREATURE_SPAWN_TRIGGER) {
        spawnSentry(&sentries[i], spawnX, GROUND_Y);
      }
    }
  }

  for (int i = 0; i < MAX_SENTRIES; i++) {
    if (!sentries[i].active)
      continue;

    struct Sentry *s = &sentries[i];

    int dx = player->x - s->x;
    int dy = player->y - s->y;
    float distance = sqrt((double)(dx * dx + dy * dy));

    int distanceFromStart = abs(s->x - s->patrolStartX);
    int isPlayerInChaseRange = (distanceFromStart <= SENTRY_MAX_CHASE_DIST);

  
    if (!s->isAggro && distance <= SENTRY_DETECTION_RANGE && isPlayerInChaseRange) {
      s->isAggro = 1;
    } else if (s->isAggro &&
               (distance > SENTRY_DETECTION_RANGE + AGGRO_LOSE_BUFFER || !isPlayerInChaseRange)) {
      s->isAggro = 0;
    }
    int isDetected = s->isAggro;

    if (s->y > GROUND_Y || s->vy != 0) {
      s->vy -= GRAVITY;
      s->y += s->vy;

      if (s->y <= GROUND_Y) {
        s->y = GROUND_Y;
        s->vy = 0;

        if (s->state == SENTRY_JUMP_ATTACK) {
          s->state = s->facingRight ? SENTRY_SLASH_RIGHT : SENTRY_SLASH_LEFT;
          s->subStateTimer = 0;
          s->vx = 0;
        } else if (s->state == SENTRY_DYING_AIR) {
          s->state = SENTRY_DYING;
          s->subStateTimer = 0;
          s->frame = 0;
        }
      }
    }

    s->subStateTimer++;
    s->animationTimer++;
    if (s->attackCooldown > 0)
      s->attackCooldown--;

    switch (s->state) {
    case SENTRY_WAKING:
      if (s->subStateTimer >= SENTRY_WAKE_FRAMES * 15) {
        s->state = SENTRY_IDLE_STATE;
        s->subStateTimer = 0;
      }
      break;

    case SENTRY_IDLE_STATE:
      if (s->subStateTimer >= SENTRY_IDLE_FRAMES * 20) {
        s->state = s->facingRight ? SENTRY_WALK_RIGHT : SENTRY_WALK_LEFT;
        s->vx = s->facingRight ? SENTRY_SPEED : -SENTRY_SPEED;
        s->subStateTimer = 0;
      }
      if (isDetected) {
        s->state = s->facingRight ? SENTRY_RUN_RIGHT : SENTRY_RUN_LEFT;
        s->vx = s->facingRight ? SENTRY_RUN_SPEED : -SENTRY_RUN_SPEED;
      }
      break;

    case SENTRY_WALK_RIGHT:
      s->x += s->vx;
      if (s->x >= s->patrolStartX + SENTRY_PATROL_DISTANCE) {
        s->state = SENTRY_TURNING_STATE;
        s->subStateTimer = 0;
        s->vx = 0;
      }
      if (!isDetected && rand() % SENTRY_IDLE_CHANCE == 0) {
        s->state = SENTRY_IDLE_STATE;
        s->subStateTimer = 0;
        s->vx = 0;
      }
      if (isDetected) {
        s->state = SENTRY_RUN_RIGHT;
        s->vx = SENTRY_RUN_SPEED;
      }
      break;

    case SENTRY_WALK_LEFT:
      s->x += s->vx;
      if (s->x <= s->patrolStartX - SENTRY_PATROL_DISTANCE) {
        s->state = SENTRY_TURNING_STATE;
        s->subStateTimer = 0;
        s->vx = 0;
      }
      if (!isDetected && rand() % SENTRY_IDLE_CHANCE == 0) {
        s->state = SENTRY_IDLE_STATE;
        s->subStateTimer = 0;
        s->vx = 0;
      }
      if (isDetected) {
        s->state = SENTRY_RUN_LEFT;
        s->vx = -SENTRY_RUN_SPEED;
      }
      break;

    case SENTRY_RUN_RIGHT:
      s->x += s->vx;
      if (!isDetected) {
        if (s->x > s->patrolStartX) {
          s->state = SENTRY_WALK_LEFT;
          s->vx = -SENTRY_SPEED;
        } else {
          s->state = SENTRY_WALK_RIGHT;
          s->vx = SENTRY_SPEED;
        }
      }
      if (dx < -50) {
        s->facingRight = 0;
        s->state = SENTRY_RUN_LEFT;
        s->vx = -SENTRY_RUN_SPEED;
      }
      break;

    case SENTRY_RUN_LEFT:
      s->x += s->vx;
      if (!isDetected) {
        if (s->x > s->patrolStartX) {
          s->state = SENTRY_WALK_LEFT;
          s->vx = -SENTRY_SPEED;
        } else {
          s->state = SENTRY_WALK_RIGHT;
          s->vx = SENTRY_SPEED;
        }
      }
      if (dx > 50) {
        s->facingRight = 1;
        s->state = SENTRY_RUN_RIGHT;
        s->vx = SENTRY_RUN_SPEED;
      }
      break;

    case SENTRY_TURNING_STATE:
      if (s->subStateTimer >= SENTRY_TURNL_FRAMES * 15) {
        s->facingRight = !s->facingRight;
        s->state = s->facingRight ? SENTRY_WALK_RIGHT : SENTRY_WALK_LEFT;
        s->vx = s->facingRight ? SENTRY_SPEED : -SENTRY_SPEED;
        s->subStateTimer = 0;
      }
      if (isDetected) {
        s->state = s->facingRight ? SENTRY_RUN_RIGHT : SENTRY_RUN_LEFT;
        s->vx = s->facingRight ? SENTRY_RUN_SPEED : -SENTRY_RUN_SPEED;
      }
      break;

    case SENTRY_ATTACK_LEFT:
    case SENTRY_ATTACK_RIGHT:
      if (dx > 0)
        s->vx = SENTRY_ATTACK_SPEED;
      else if (dx < 0)
        s->vx = -SENTRY_ATTACK_SPEED;
      else
        s->vx = 0;
      s->x += s->vx;

      if (s->subStateTimer >= SENTRY_ATTACKL_FRAMES * 6) {
        
        if (s->isAggro) {
          s->state = s->facingRight ? SENTRY_RUN_RIGHT : SENTRY_RUN_LEFT;
          s->vx = s->facingRight ? SENTRY_RUN_SPEED : -SENTRY_RUN_SPEED;
        } else {
          s->state = s->facingRight ? SENTRY_WALK_RIGHT : SENTRY_WALK_LEFT;
          s->vx = s->facingRight ? SENTRY_SPEED : -SENTRY_SPEED;
        }
        s->subStateTimer = 0;
        s->attackCooldown = SENTRY_ATTACK_COOLDOWN;
      }
      break;

    case SENTRY_JUMP_ATTACK:
      s->x += s->vx;
      break;

    case SENTRY_SLASH_LEFT:
    case SENTRY_SLASH_RIGHT:
      if (s->subStateTimer >= SENTRY_SLASHL_FRAMES * 8) {
        if (s->isAggro) {
          s->state = s->facingRight ? SENTRY_RUN_RIGHT : SENTRY_RUN_LEFT;
          s->vx = s->facingRight ? SENTRY_RUN_SPEED : -SENTRY_RUN_SPEED;
        } else {
          s->state = SENTRY_IDLE_STATE;
          s->vx = 0;
        }
        s->subStateTimer = 0;
        s->attackCooldown = SENTRY_ATTACK_COOLDOWN;
      }
      break;

    case SENTRY_DYING:
      if (s->subStateTimer >= SENTRY_DEATH_FRAMES * 20) {
        s->active = 0;
      }
      break;

    case SENTRY_DYING_AIR:
      if (s->subStateTimer >= SENTRY_DEATHAIR_FRAMES * 20) {
        s->active = 0;
      }
      break;

    default:
      break;
    }

    if ((s->state == SENTRY_WALK_LEFT || s->state == SENTRY_WALK_RIGHT ||
         s->state == SENTRY_IDLE_STATE || s->state == SENTRY_RUN_LEFT ||
         s->state == SENTRY_RUN_RIGHT) &&
        distance <= SENTRY_ATTACK_RANGE && s->attackCooldown <= 0) {

      s->facingRight = (dx > 0) ? 1 : 0;
      if (rand() % 100 < SENTRY_ATTACK_JUMP_CHANCE) {
        s->state = SENTRY_JUMP_ATTACK;
        s->vy = SENTRY_JUMP_V;
        s->vx = s->facingRight ? 4 : -4;
        s->subStateTimer = 0;
      } else {
        s->state = s->facingRight ? SENTRY_ATTACK_RIGHT : SENTRY_ATTACK_LEFT;
        s->subStateTimer = 0;
      }
    }

    if (s->animationTimer >= 6) {
      s->animationTimer = 0;
      switch (s->state) {
      case SENTRY_WAKING:
        s->frame = (s->frame + 1) % SENTRY_WAKE_FRAMES;
        break;
      case SENTRY_IDLE_STATE:
        s->frame = (s->frame + 1) % SENTRY_IDLE_FRAMES;
        break;
      case SENTRY_WALK_LEFT:
        s->frame = (s->frame + 1) % SENTRY_WALKL_FRAMES;
        break;
      case SENTRY_WALK_RIGHT:
        s->frame = (s->frame + 1) % SENTRY_WALKR_FRAMES;
        break;
      case SENTRY_RUN_LEFT:
        s->frame = (s->frame + 1) % SENTRY_RUNL_FRAMES;
        break;
      case SENTRY_RUN_RIGHT:
        s->frame = (s->frame + 1) % SENTRY_RUNR_FRAMES;
        break;
      case SENTRY_ATTACK_LEFT:
        s->frame = (s->frame + 1) % SENTRY_ATTACKL_FRAMES;
        break;
      case SENTRY_ATTACK_RIGHT:
        s->frame = (s->frame + 1) % SENTRY_ATTACKR_FRAMES;
        break;
      case SENTRY_JUMP_ATTACK:
        s->frame = (s->frame + 1) % SENTRY_SLASHL_FRAMES;
        break;
      case SENTRY_SLASH_LEFT:
        s->frame = (s->frame + 1) % SENTRY_SLASHL_FRAMES;
        break;
      case SENTRY_SLASH_RIGHT:
        s->frame = (s->frame + 1) % SENTRY_SLASHR_FRAMES;
        break;
      case SENTRY_TURNING_STATE:
        s->frame = (s->frame + 1) % SENTRY_TURNL_FRAMES;
        break;
      case SENTRY_DYING:
        s->frame = (s->frame + 1) % SENTRY_DEATH_FRAMES;
        break;
      case SENTRY_DYING_AIR:
        s->frame = (s->frame + 1) % SENTRY_DEATHAIR_FRAMES;
        break;
      default:
        break;
      }
    }

    if (s->state != SENTRY_DYING && s->state != SENTRY_DYING_AIR &&
        s->state != SENTRY_WAKING) {
      int sY = s->y;
      int sH = SENTRY_SIZE;
      if (s->state == SENTRY_JUMP_ATTACK) {
        sY -= 80;
        sH += 80;
      }

      if (checkCollisionSentry(player->x + 24, player->y + 24, 80, 80, s->x, sY,
                               SENTRY_SIZE, sH)) {
        bool isSwinging =
            (s->state == SENTRY_ATTACK_LEFT || s->state == SENTRY_ATTACK_RIGHT ||
             s->state == SENTRY_SLASH_LEFT || s->state == SENTRY_SLASH_RIGHT ||
             s->state == SENTRY_JUMP_ATTACK);

        int damage = isSwinging ? DAMAGE_PLAYER_TAKES : DAMAGE_PLAYER_TAKES / 2;
        damagePlayer(*player, damage);
      }
    }

   
    if (s->state != SENTRY_DYING && s->state != SENTRY_DYING_AIR &&
        checkPlayerAttackCollision(*player, s->x, s->y, SENTRY_SIZE, SENTRY_SIZE)) {

      if (s->invincibilityTimer == 0) {
        int damage = getPlayerAttackDamage();
        s->currentHealth -= damage;
        s->invincibilityTimer = 90;

        if (s->currentHealth <= 0) {
          s->currentHealth = 0;
          s->state = (s->y <= GROUND_Y) ? SENTRY_DYING : SENTRY_DYING_AIR;
          s->subStateTimer = 0;
          s->animationTimer = 0;
          s->frame = 0;
          s->vx = 0;
          s->vy = 0;

          playEnemyKillSound();
        } else {
          s->damageAnimTimer = SENTRY_DAMAGE_FRAMES * 4;
          s->damageFrame = 0;
        }
      }
    }

    
    if (s->state != SENTRY_DYING && s->state != SENTRY_DYING_AIR &&
        s->invincibilityTimer == 0) {
      for (int a = 0; a < MAX_ARROWS; a++) {
        if (checkArrowCollision(arrows[a], s->x, s->y, SENTRY_SIZE, SENTRY_SIZE)) {
          int damage = getArrowDamage();
          s->currentHealth -= damage;
          s->invincibilityTimer = 90;
          arrows[a].active = false;

          if (s->currentHealth <= 0) {
            s->currentHealth = 0;
            s->state = (s->y <= GROUND_Y) ? SENTRY_DYING : SENTRY_DYING_AIR;
            s->subStateTimer = 0;
            s->animationTimer = 0;
            s->frame = 0;
            s->vx = 0;
            s->vy = 0;

            playEnemyKillSound();
          } else {
            s->damageAnimTimer = SENTRY_DAMAGE_FRAMES * 4;
            s->damageFrame = 0;
          }
          break;
        }
      }
    }

    if (s->invincibilityTimer > 0) {
      s->invincibilityTimer--;
    }

    if (s->damageAnimTimer > 0) {
      s->damageAnimTimer--;
      if (s->damageAnimTimer % 4 == 0 &&
          s->damageFrame < SENTRY_DAMAGE_FRAMES - 1) {
        s->damageFrame++;
      }
    }
  }
}

void renderSentries(struct Sentry sentries[], struct Camera *camera) {
  for (int i = 0; i < MAX_SENTRIES; i++) {
    if (!sentries[i].active)
      continue;

    struct Sentry *s = &sentries[i];
    unsigned int tex;

    switch (s->state) {
    case SENTRY_WAKING:
      tex = sentryWake[s->frame % SENTRY_WAKE_FRAMES];
      break;
    case SENTRY_IDLE_STATE:
      tex = sentryIdle[s->frame % SENTRY_IDLE_FRAMES];
      break;
    case SENTRY_WALK_LEFT:
      tex = sentryWalkL[s->frame % SENTRY_WALKL_FRAMES];
      break;
    case SENTRY_WALK_RIGHT:
      tex = sentryWalkR[s->frame % SENTRY_WALKR_FRAMES];
      break;
    case SENTRY_RUN_LEFT:
      tex = sentryRunL[s->frame % SENTRY_RUNL_FRAMES];
      break;
    case SENTRY_RUN_RIGHT:
      tex = sentryRunR[s->frame % SENTRY_RUNR_FRAMES];
      break;
    case SENTRY_ATTACK_LEFT:
      tex = sentryAttackL[s->frame % SENTRY_ATTACKL_FRAMES];
      break;
    case SENTRY_ATTACK_RIGHT:
      tex = sentryAttackR[s->frame % SENTRY_ATTACKR_FRAMES];
      break;
    case SENTRY_SLASH_LEFT:
      tex = sentrySlashL[s->frame % SENTRY_SLASHL_FRAMES];
      break;
    case SENTRY_SLASH_RIGHT:
      tex = sentrySlashR[s->frame % SENTRY_SLASHR_FRAMES];
      break;
    case SENTRY_TURNING_STATE:
      tex = s->facingRight ? sentryTurnR[s->frame % SENTRY_TURNR_FRAMES]
                           : sentryTurnL[s->frame % SENTRY_TURNL_FRAMES];
      break;
    case SENTRY_DYING:
      tex = sentryDeath[s->frame % SENTRY_DEATH_FRAMES];
      break;
    case SENTRY_DYING_AIR:
      tex = sentryDeathAir[s->frame % SENTRY_DEATHAIR_FRAMES];
      break;
    default:
      tex = sentryIdle[0];
      break;
    }

    float screenX = getScreenX(s->x, camera);
    float screenY = getScreenY(s->y, camera);
    iShowImage(screenX, screenY, SENTRY_SIZE, SENTRY_SIZE, tex);

    if (s->damageAnimTimer > 0) {
      unsigned int dmgTex = sentryDamage[s->damageFrame % SENTRY_DAMAGE_FRAMES];
      if (dmgTex != 0) {
        iShowImage(screenX, screenY, SENTRY_SIZE, SENTRY_SIZE, dmgTex);
      }
    }

    if (s->state != SENTRY_DYING && s->state != SENTRY_DYING_AIR &&
        s->state != SENTRY_INACTIVE && s->maxHealth > 0) {
      int barW = 50;
      int barH = 5;
      int barX = screenX + (SENTRY_SIZE - barW) / 2;
      int barY = screenY + SENTRY_SIZE + 5;
      int currHealth = s->currentHealth;
      if (currHealth < 0)
        currHealth = 0;
      int filled = (currHealth * barW) / s->maxHealth;

      iSetColor(80, 0, 0);
      iFilledRectangle(barX, barY, barW, barH);
      iSetColor(220, 30, 30);
      iFilledRectangle(barX, barY, filled, barH);
      iSetColor(255, 255, 255);
      iRectangle(barX, barY, barW, barH);
    }
  }
}

#endif
