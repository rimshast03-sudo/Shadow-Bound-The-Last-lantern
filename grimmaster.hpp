#ifndef GRIMMASTER_HPP
#define GRIMMASTER_HPP

// Level 3 "Grim" enemy (Assets/Level 3/grim master).
//
// A roaming caster, distinct from both the Level 1 cave boss (see
// caveboss.hpp, which reuses the older Assets/Cave/GrimMaster sprites)
// and the Level 3 final boss (see level3boss.hpp, which uses the shared
// Assets/Boss sprites). This one patrols the Level 3 corridor and, once
// it notices the player, either dashes in for a melee ram, throws a
// fireball, or teleports to reposition -- picked semi-randomly using the
// GRIM_* chance/range constants in config.hpp.
//
// Written in the same plain-struct / switch-on-state style as Sentry.hpp.

#include "camera.hpp"
#include "config.hpp"
#include "iGraphics.h"
#include "player.hpp"
#include "sounds.hpp"
#include "structs.hpp"
#include "textures.hpp"
#include <math.h>
#include <stdlib.h>

inline int checkCollisionGrim(int x1, int y1, int w1, int h1, int x2, int y2,
                               int w2, int h2) {
  return !(x1 + w1 < x2 || x2 + w2 < x1 || y1 + h1 < y2 || y2 + h2 < y1);
}

// ---------------------------------------------------------------------
// Init / spawn
// ---------------------------------------------------------------------

void initGrims(struct Grim grims[]) {
  for (int i = 0; i < MAX_GRIMS; i++) {
    grims[i].active = 0;
    grims[i].state = GRIM_STATE_INACTIVE;
    grims[i].frame = 0;
    grims[i].facingRight = 0;
    grims[i].subStateTimer = 0;
    grims[i].animationTimer = 0;
    grims[i].invincibilityTimer = 0;
    grims[i].damageAnimTimer = 0;
    grims[i].damageFrame = 0;
    grims[i].attackCooldown = 0;
    grims[i].contactHitCooldown = 0;
    grims[i].teleportTargetX = 0;
  }
}

void spawnGrim(struct Grim *grim, int x, int y) {
  grim->x = x;
  grim->y = y;
  grim->frame = 0;
  grim->active = 1;
  grim->state = GRIM_STATE_IDLE;
  grim->facingRight = 0;
  grim->subStateTimer = 0;
  grim->animationTimer = 0;
  grim->maxHealth = GRIM_MAX_HEALTH;
  grim->currentHealth = GRIM_MAX_HEALTH;
  grim->invincibilityTimer = 0;
  grim->damageAnimTimer = 0;
  grim->damageFrame = 0;
  grim->attackCooldown = 30;
  grim->contactHitCooldown = 0;
  grim->teleportTargetX = x;
}

void initGrimFireballs(struct GrimFireball fireballs[]) {
  for (int i = 0; i < MAX_GRIM_FIREBALLS; i++) {
    fireballs[i].active = 0;
    fireballs[i].exploding = 0;
    fireballs[i].frame = 0;
    fireballs[i].animTimer = 0;
  }
}

void spawnGrimFireball(struct GrimFireball fireballs[], struct Grim *grim,
                        Player *player) {
  for (int i = 0; i < MAX_GRIM_FIREBALLS; i++) {
    if (fireballs[i].active)
      continue;

    struct GrimFireball *f = &fireballs[i];
    f->active = 1;
    f->exploding = 0;
    f->frame = 0;
    f->animTimer = 0;
    f->facingRight = grim->facingRight;

    f->x = grim->x + (grim->facingRight ? GRIM_FB_SPAWN_OFFSET_X_RIGHT
                                         : -GRIM_FB_SPAWN_OFFSET_X_LEFT);
    f->y = grim->y + GRIM_FB_SPAWN_OFFSET_Y;

    f->vx = grim->facingRight ? GRIM_FIREBALL_SPEED : -GRIM_FIREBALL_SPEED;

    // Aim roughly at the player's ground level -- gives the shot a slight
    // downward drift over its flight instead of flying dead level.
    int targetY = LEVEL3_GROUND_Y + GRIM_FB_TARGET_GROUND_OFFSET;
    int framesToClose = GRIM_FB_TARGET_PLAYER_OFFSET_X / GRIM_FIREBALL_SPEED;
    if (framesToClose < 1)
      framesToClose = 1;
    f->vy = (targetY - f->y) / framesToClose;
    (void)player;
    break;
  }
}

// ---------------------------------------------------------------------
// Update
// ---------------------------------------------------------------------

void updateGrimFireballs(struct GrimFireball fireballs[], Player *player) {
  for (int i = 0; i < MAX_GRIM_FIREBALLS; i++) {
    struct GrimFireball *f = &fireballs[i];
    if (!f->active)
      continue;

    if (!f->exploding) {
      f->x += f->vx;
      f->y += f->vy;

      f->animTimer++;
      if (f->animTimer >= GRIM_ANIM_SPEED) {
        f->animTimer = 0;
        f->frame = (f->frame + 1) % GRIM_FIREBALL_FRAMES;
      }

      bool hitPlayer = checkCollisionGrim(
          player->x, player->y, PLAYER_WIDTH, PLAYER_HEIGHT,
          f->x - GRIM_FIREBALL_HIT_W / 2, f->y - GRIM_FIREBALL_HIT_H / 2,
          GRIM_FIREBALL_HIT_W, GRIM_FIREBALL_HIT_H);

      bool hitGround = (f->y <= LEVEL3_GROUND_Y);

      if (hitPlayer || hitGround) {
        if (hitPlayer)
          damagePlayer(*player, GRIM_FIREBALL_DAMAGE);
        f->exploding = 1;
        f->frame = 0;
        f->animTimer = 0;
      }
    } else {
      f->animTimer++;
      if (f->animTimer >= GRIM_ANIM_SPEED) {
        f->animTimer = 0;
        f->frame++;
        if (f->frame >= GRIM_FIREBALL_EXPLODE_FRAMES) {
          f->active = 0;
        }
      }
    }
  }
}

void updateGrims(struct Grim grims[], struct GrimFireball fireballs[],
                  Player *player, struct Arrow arrows[], int gameState) {
  if (gameState != LEVEL3_STATE)
    return;

  for (int i = 0; i < MAX_GRIMS; i++) {
    if (!grims[i].active && grims[i].state == GRIM_STATE_INACTIVE) {
      int spawnX = (i == 0) ? GRIM_SPAWN_1_X : GRIM_SPAWN_2_X;
      int spawnY = LEVEL3_GROUND_Y + GRIM_SPAWN_Y_OFFSET;

      int dx = player->x - spawnX;
      int dy = player->y - spawnY;
      float spawnDistance = sqrt((double)(dx * dx + dy * dy));

      if (spawnDistance <= CREATURE_SPAWN_TRIGGER) {
        spawnGrim(&grims[i], spawnX, spawnY);
      }
    }
  }

  for (int i = 0; i < MAX_GRIMS; i++) {
    if (!grims[i].active)
      continue;

    struct Grim *g = &grims[i];
    int dx = player->x - g->x;
    int dy = player->y - g->y;
    float distance = sqrt((double)(dx * dx + dy * dy));

    g->subStateTimer++;
    g->animationTimer++;
    if (g->attackCooldown > 0)
      g->attackCooldown--;
    if (g->invincibilityTimer > 0)
      g->invincibilityTimer--;

    switch (g->state) {
    case GRIM_STATE_IDLE: {
      int desiredFacing = (dx >= 0) ? 1 : 0;

      if (distance <= GRIM_DETECTION_RANGE) {
        if (g->facingRight != desiredFacing) {
          g->facingRight = desiredFacing;
          g->state = GRIM_STATE_TURN;
          g->subStateTimer = 0;
        } else if (g->attackCooldown <= 0) {
          if (rand() % GRIM_TELEPORT_CHANCE == 0) {
            g->state = GRIM_STATE_TELEPORT_OUT;
            g->subStateTimer = 0;
            g->frame = 0;
          } else if (distance <= GRIM_DASH_RANGE) {
            g->state = GRIM_STATE_DASH_ANTIC;
            g->subStateTimer = 0;
            g->frame = 0;
          } else if (distance <= GRIM_ATTACK_RANGE &&
                     rand() % GRIM_THROW_CHANCE == 0) {
            g->state = GRIM_STATE_THROW_ANTIC;
            g->subStateTimer = 0;
            g->frame = 0;
          }
        }
      }
      break;
    }

    case GRIM_STATE_TURN: {
      int turnFrames = g->facingRight ? GRIM_TURN_R_FRAMES : GRIM_TURN_L_FRAMES;
      if (g->subStateTimer >= turnFrames * GRIM_ANIM_SPEED) {
        g->state = GRIM_STATE_IDLE;
        g->subStateTimer = 0;
      }
      break;
    }

    case GRIM_STATE_DASH_ANTIC: {
      int frames =
          g->facingRight ? GRIM_DASH_ANTIC_R_FRAMES : GRIM_DASH_ANTIC_L_FRAMES;
      if (g->subStateTimer >= frames * GRIM_ANIM_SPEED) {
        g->state = GRIM_STATE_DASHING;
        g->subStateTimer = 0;
        g->frame = 0;
        g->contactHitCooldown = 0;
      }
      break;
    }

    case GRIM_STATE_DASHING: {
      int frames = g->facingRight ? GRIM_DASH_R_FRAMES : GRIM_DASH_L_FRAMES;
      g->x += g->facingRight ? GRIM_DASH_SPEED : -GRIM_DASH_SPEED;

      if (g->contactHitCooldown == 0 &&
          checkCollisionGrim(player->x, player->y, PLAYER_WIDTH,
                              PLAYER_HEIGHT, g->x, g->y, GRIM_SIZE,
                              GRIM_SIZE)) {
        damagePlayer(*player, GRIM_CONTACT_DAMAGE);
        g->contactHitCooldown = 1;
      }

      if (g->subStateTimer >= frames * GRIM_ANIM_SPEED) {
        g->state = GRIM_STATE_IDLE;
        g->subStateTimer = 0;
        g->attackCooldown = 60;
      }
      break;
    }

    case GRIM_STATE_THROW_ANTIC: {
      int frames = g->facingRight ? GRIM_THROW_ANTIC_R_FRAMES
                                   : GRIM_THROW_ANTIC_L_FRAMES;
      if (g->subStateTimer >= frames * GRIM_ANIM_SPEED) {
        g->state = GRIM_STATE_THROWING;
        g->subStateTimer = 0;
        g->frame = 0;
        spawnGrimFireball(fireballs, g, player);
      }
      break;
    }

    case GRIM_STATE_THROWING: {
      int frames = g->facingRight ? GRIM_THROW_R_FRAMES : GRIM_THROW_L_FRAMES;
      if (g->subStateTimer >= frames * GRIM_ANIM_SPEED) {
        g->state = GRIM_STATE_IDLE;
        g->subStateTimer = 0;
        g->attackCooldown = 60;
      }
      break;
    }

    case GRIM_STATE_TELEPORT_OUT: {
      if (g->subStateTimer >= GRIM_TELEPORT_OUT_FRAMES * GRIM_ANIM_SPEED) {
        int offset = (rand() % (2 * GRIM_TELEPORT_RADIUS)) - GRIM_TELEPORT_RADIUS;
        g->x = player->x + offset;
        g->state = GRIM_STATE_TELEPORT_IN;
        g->subStateTimer = 0;
        g->frame = 0;
        g->facingRight = (player->x >= g->x) ? 1 : 0;
      }
      break;
    }

    case GRIM_STATE_TELEPORT_IN: {
      if (g->subStateTimer >= GRIM_TELEPORT_IN_FRAMES * GRIM_ANIM_SPEED) {
        g->state = GRIM_STATE_IDLE;
        g->subStateTimer = 0;
        g->attackCooldown = 30;
      }
      break;
    }

    case GRIM_STATE_DYING: {
      if (g->subStateTimer >= GRIM_DEATH_FRAMES * GRIM_ANIM_SPEED) {
        g->active = 0;
      }
      break;
    }

    default:
      break;
    }

    // Animation frame stepping.
    if (g->animationTimer >= GRIM_ANIM_SPEED) {
      g->animationTimer = 0;
      switch (g->state) {
      case GRIM_STATE_IDLE:
        g->frame = (g->frame + 1) % GRIM_IDLE_FRAMES;
        break;
      case GRIM_STATE_TURN:
        g->frame = (g->frame + 1) %
                   (g->facingRight ? GRIM_TURN_R_FRAMES : GRIM_TURN_L_FRAMES);
        break;
      case GRIM_STATE_DASH_ANTIC:
        g->frame = (g->frame + 1) % (g->facingRight ? GRIM_DASH_ANTIC_R_FRAMES
                                                     : GRIM_DASH_ANTIC_L_FRAMES);
        break;
      case GRIM_STATE_DASHING:
        g->frame = (g->frame + 1) %
                   (g->facingRight ? GRIM_DASH_R_FRAMES : GRIM_DASH_L_FRAMES);
        break;
      case GRIM_STATE_THROW_ANTIC:
        g->frame = (g->frame + 1) % (g->facingRight ? GRIM_THROW_ANTIC_R_FRAMES
                                                     : GRIM_THROW_ANTIC_L_FRAMES);
        break;
      case GRIM_STATE_THROWING:
        g->frame = (g->frame + 1) %
                   (g->facingRight ? GRIM_THROW_R_FRAMES : GRIM_THROW_L_FRAMES);
        break;
      case GRIM_STATE_TELEPORT_OUT:
        g->frame = (g->frame + 1) % GRIM_TELEPORT_OUT_FRAMES;
        break;
      case GRIM_STATE_TELEPORT_IN:
        g->frame = (g->frame + 1) % GRIM_TELEPORT_IN_FRAMES;
        break;
      case GRIM_STATE_DYING:
        g->frame = (g->frame + 1) % GRIM_DEATH_FRAMES;
        break;
      default:
        break;
      }
    }

    // Player melee vs grim.
    if (g->state != GRIM_STATE_DYING && g->invincibilityTimer == 0 &&
        checkPlayerAttackCollision(*player, g->x, g->y, GRIM_SIZE, GRIM_SIZE)) {
      int damage = getPlayerAttackDamage();
      g->currentHealth -= damage;
      g->invincibilityTimer = GRIM_INVINCIBILITY_FRAMES;
      g->damageAnimTimer = GRIM_DAMAGE_FRAMES * 4;
      g->damageFrame = 0;

      if (g->currentHealth <= 0) {
        g->currentHealth = 0;
        g->state = GRIM_STATE_DYING;
        g->subStateTimer = 0;
        g->animationTimer = 0;
        g->frame = 0;
        playEnemyKillSound();
      }
    }

    // Player arrows vs grim.
    if (g->state != GRIM_STATE_DYING && g->invincibilityTimer == 0) {
      for (int a = 0; a < MAX_ARROWS; a++) {
        if (checkArrowCollision(arrows[a], g->x, g->y, GRIM_SIZE, GRIM_SIZE)) {
          int damage = getArrowDamage();
          g->currentHealth -= damage;
          g->invincibilityTimer = GRIM_INVINCIBILITY_FRAMES;
          g->damageAnimTimer = GRIM_DAMAGE_FRAMES * 4;
          g->damageFrame = 0;
          arrows[a].active = false;

          if (g->currentHealth <= 0) {
            g->currentHealth = 0;
            g->state = GRIM_STATE_DYING;
            g->subStateTimer = 0;
            g->animationTimer = 0;
            g->frame = 0;
            playEnemyKillSound();
          }
          break;
        }
      }
    }

    if (g->damageAnimTimer > 0)
      g->damageAnimTimer--;
  }
}

// ---------------------------------------------------------------------
// Render
// ---------------------------------------------------------------------

void renderGrims(struct Grim grims[], struct Camera *camera) {
  for (int i = 0; i < MAX_GRIMS; i++) {
    if (!grims[i].active)
      continue;

    struct Grim *g = &grims[i];
    unsigned int tex;
    unsigned int pillarTex = 0;

    switch (g->state) {
    case GRIM_STATE_IDLE:
      tex = grimIdle[g->frame % GRIM_IDLE_FRAMES];
      break;
    case GRIM_STATE_TURN:
      tex = g->facingRight ? grimTurnR[g->frame % GRIM_TURN_R_FRAMES]
                            : grimTurnL[g->frame % GRIM_TURN_L_FRAMES];
      break;
    case GRIM_STATE_DASH_ANTIC:
      tex = g->facingRight
                ? grimDashAnticR[g->frame % GRIM_DASH_ANTIC_R_FRAMES]
                : grimDashAnticL[g->frame % GRIM_DASH_ANTIC_L_FRAMES];
      break;
    case GRIM_STATE_DASHING:
      tex = g->facingRight ? grimDashR[g->frame % GRIM_DASH_R_FRAMES]
                            : grimDashL[g->frame % GRIM_DASH_L_FRAMES];
      break;
    case GRIM_STATE_THROW_ANTIC:
      tex = g->facingRight
                ? grimThrowAnticR[g->frame % GRIM_THROW_ANTIC_R_FRAMES]
                : grimThrowAnticL[g->frame % GRIM_THROW_ANTIC_L_FRAMES];
      break;
    case GRIM_STATE_THROWING:
      tex = g->facingRight ? grimThrowR[g->frame % GRIM_THROW_R_FRAMES]
                            : grimThrowL[g->frame % GRIM_THROW_L_FRAMES];
      break;
    case GRIM_STATE_TELEPORT_OUT:
      tex = grimTeleOut[g->frame % GRIM_TELEPORT_OUT_FRAMES];
      pillarTex =
          grimTeleOutPillar[g->frame % GRIM_TELEPORT_OUT_PILLAR_FRAMES];
      break;
    case GRIM_STATE_TELEPORT_IN:
      tex = grimTeleIn[g->frame % GRIM_TELEPORT_IN_FRAMES];
      pillarTex = grimTeleInPillar[g->frame % GRIM_TELEPORT_IN_PILLAR_FRAMES];
      break;
    case GRIM_STATE_DYING:
      tex = grimDeath[g->frame % GRIM_DEATH_FRAMES];
      break;
    default:
      tex = grimIdle[0];
      break;
    }

    float screenX = getScreenX((float)g->x, camera);
    float screenY = getScreenY((float)g->y, camera);

    // Flicker while invincible from a recent hit, same idea as the
    // Sentry's damage flash but without a dedicated damage sprite.
    bool visible = (g->invincibilityTimer == 0) ||
                   ((g->invincibilityTimer / 4) % 2 == 0);

    if (pillarTex != 0)
      iShowImage((int)screenX, (int)screenY, GRIM_SIZE, GRIM_SIZE, pillarTex);

    if (visible && tex != 0)
      iShowImage((int)screenX, (int)screenY, GRIM_SIZE, GRIM_SIZE, tex);

    if (g->state != GRIM_STATE_DYING && g->maxHealth > 0) {
      int barW = 60;
      int barH = 5;
      int barX = (int)screenX + (GRIM_SIZE - barW) / 2;
      int barY = (int)screenY + GRIM_SIZE + 5;
      int currHealth = g->currentHealth;
      if (currHealth < 0)
        currHealth = 0;
      int filled = (currHealth * barW) / g->maxHealth;

      iSetColor(80, 0, 0);
      iFilledRectangle(barX, barY, barW, barH);
      iSetColor(180, 40, 220);
      iFilledRectangle(barX, barY, filled, barH);
      iSetColor(255, 255, 255);
      iRectangle(barX, barY, barW, barH);
    }
  }
}

void renderGrimFireballs(struct GrimFireball fireballs[], struct Camera *camera) {
  for (int i = 0; i < MAX_GRIM_FIREBALLS; i++) {
    struct GrimFireball *f = &fireballs[i];
    if (!f->active)
      continue;

    float screenX = getScreenX((float)f->x, camera);
    float screenY = getScreenY((float)f->y, camera);

    if (!f->exploding) {
      unsigned int tex = grimFireball[f->frame % GRIM_FIREBALL_FRAMES];
      if (tex != 0)
        iShowImage((int)screenX - GRIM_FIREBALL_DISPLAY_SIZE / 2,
                   (int)screenY - GRIM_FIREBALL_DISPLAY_SIZE / 2,
                   GRIM_FIREBALL_DISPLAY_SIZE, GRIM_FIREBALL_DISPLAY_SIZE,
                   tex);
    } else {
      unsigned int tex =
          grimFireballExplode[f->frame % GRIM_FIREBALL_EXPLODE_FRAMES];
      if (tex != 0)
        iShowImage((int)screenX - GRIM_FIREBALL_EXPLODE_DISPLAY_SIZE / 2,
                   (int)screenY - GRIM_FIREBALL_EXPLODE_DISPLAY_SIZE / 2,
                   GRIM_FIREBALL_EXPLODE_DISPLAY_SIZE,
                   GRIM_FIREBALL_EXPLODE_DISPLAY_SIZE, tex);
    }
  }
}

#endif
