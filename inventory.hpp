#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include "camera.hpp"
#include "config.hpp"
#include "iGraphics.h"
#include "sounds.hpp"
#include "structs.hpp"
#include "textures.hpp"
#include <math.h>
#include <stdlib.h>

inline void initPlayerInventory(struct Player *player) {
  player->hasHeart = 0;
  player->hasPotion = 0;
  player->hasPower = 0;
  player->hasGain = 0;
  player->potionTimer = 0;
  player->damageMultiplier = 1;
  player->heartDuration = 0;
  player->heartRegenTimer = 0;
  player->gainTimer = 0;
  player->gainRegenTimer = 0;
  player->hasKey = 0;
  player->hasUsedKey = 0;
}

inline void initPickups(struct Pickup pickups[]) {
  for (int i = 0; i < MAX_PICKUPS; i++) {
    pickups[i].active = 0;
  }
}

inline void initGlowProjectile(struct GlowProjectile *glow) {
  glow->active = 0;
  glow->frame = 0;
  glow->animTimer = 0;
  glow->targetCreatureIdx = -1;
  glow->targetSentryIdx = -1;
}

inline void tryDropItem(struct Pickup pickups[], int x, int y) {

  if (rand() % 100 >= PICKUP_DROP_CHANCE)
    return;

  for (int i = 0; i < MAX_PICKUPS; i++) {
    if (!pickups[i].active) {
      pickups[i].x = x;
      pickups[i].y = y;
      pickups[i].active = 1;
      pickups[i].type = (enum ItemType)(rand() % 4);
      break;
    }
  }
}

inline void updatePickups(struct Pickup pickups[], struct Player *player) {
  int playerCX = player->x + 64;
  int playerCY = player->y + 64;

  for (int i = 0; i < MAX_PICKUPS; i++) {
    if (!pickups[i].active)
      continue;

    int dx = playerCX - (pickups[i].x + PICKUP_SIZE / 2);
    int dy = playerCY - (pickups[i].y + PICKUP_SIZE / 2);
    float dist = sqrt((double)(dx * dx + dy * dy));

    if (dist < 60) {
      int canPickup = 1;
      switch (pickups[i].type) {
      case ITEM_HEART:
        player->hasHeart = 1;
        break;
      case ITEM_POTION:
        player->hasPotion = 1;
        break;
      case ITEM_POWER:
        player->hasPower = 1;
        break;
      case ITEM_GAIN:
        player->hasGain = 1;
        break;
      case ITEM_KEY:
        player->hasKey = 1;
        break;
      }
      if (canPickup) {
        pickups[i].active = 0;
        playItemCollectSound();
        printf("Item collected: %d\n", pickups[i].type);
      }
    }
  }
}

inline void handleItemInput(struct Player *player, struct GlowProjectile *glow,
                            struct Creature creatures[],
                            struct Sentry sentries[], int gameState) {

  static int key2WasPressed = 0;

  int key2 = GetAsyncKeyState('2') & 0x8000;

  if (key2 && !key2WasPressed && player->hasPotion) {
    player->hasPotion = 0;
    player->potionTimer = POTION_DURATION;
    player->damageMultiplier = POTION_DAMAGE_MULTIPLIER;
  }

  key2WasPressed = key2;

  static int key3WasPressed = 0;

  int key3 = GetAsyncKeyState('3') & 0x8000;

  if (key3 && !key3WasPressed && player->hasPower && !glow->active) {
    player->hasPower = 0;
    float closestDist = 999999.0f;
    int foundTarget = 0;
    int targetX = 0, targetY = 0;
    int targetCreatureIdx = -1;
    int targetSentryIdx = -1;

    if (gameState == LEVEL2_STATE || gameState == PLAYING_STATE) {
      for (int i = 0; i < MAX_CREATURES; i++) {
        if (!creatures[i].active || creatures[i].state == CREATURE_DEAD)
          continue;
        int dx = creatures[i].x - player->x;
        int dy = creatures[i].y - player->y;
        float dist = sqrt((double)(dx * dx + dy * dy));
        if (dist < closestDist) {
          closestDist = dist;
          targetX = creatures[i].x;
          targetY = creatures[i].y;
          targetCreatureIdx = i;
          targetSentryIdx = -1;
          foundTarget = 1;
        }
      }
    }

    if (gameState == LEVEL2_STATE) {
      for (int i = 0; i < MAX_SENTRIES; i++) {
        if (!sentries[i].active || sentries[i].state == SENTRY_DYING ||
            sentries[i].state == SENTRY_DYING_AIR)
          continue;
        int dx = sentries[i].x - player->x;
        int dy = sentries[i].y - player->y;
        float dist = sqrt((double)(dx * dx + dy * dy));
        if (dist < closestDist) {
          closestDist = dist;
          targetX = sentries[i].x;
          targetY = sentries[i].y;
          targetCreatureIdx = -1;
          targetSentryIdx = i;
          foundTarget = 1;
        }
      }
    }

    if (foundTarget) {
      glow->active = 1;
      glow->x = player->x + 64;
      glow->y = player->y + 64;
      glow->targetX = targetX;
      glow->targetY = targetY;
      glow->targetCreatureIdx = targetCreatureIdx;
      glow->targetSentryIdx = targetSentryIdx;
      glow->frame = 0;
      glow->animTimer = 0;
    } else {
      player->hasPower = 1;
    }
  }

  key3WasPressed = key3;

  static int key4WasPressed = 0;
  int key4 = GetAsyncKeyState('4') & 0x8000;
  if (key4 && !key4WasPressed && player->hasHeart) {
    player->hasHeart = 0;
    player->heartDuration = HEART_DURATION;
    player->heartRegenTimer = 0;
  }

  key4WasPressed = key4;

  static int key5WasPressed = 0;
  int key5 = GetAsyncKeyState('5') & 0x8000;
  if (key5 && !key5WasPressed && player->hasGain) {
    player->hasGain = 0;
    player->gainTimer = GAIN_DURATION;
    player->gainRegenTimer = 0;
  }
  key5WasPressed = key5;

  static int key6WasPressed = 0;
  int key6 = GetAsyncKeyState('6') & 0x8000;
  if (key6 && !key6WasPressed && player->hasKey &&
      gameState == LEVEL3_STATE && player->x >= LEVEL3_END_X - 200) {
    player->hasKey = 0;
    player->hasUsedKey = 1;
    printf("Key used! You can now enter the boss area.\n");
  }
  key6WasPressed = key6;
}

inline void updateItemEffects(struct Player *player) {
  if (player->potionTimer > 0) {
    player->potionTimer--;
    if (player->potionTimer <= 0) {
      player->damageMultiplier = 1;
    }
  }

  if (player->heartDuration > 0) {
    player->heartDuration--;
    player->heartRegenTimer++;
    if (player->heartRegenTimer >= HEART_REGEN_INTERVAL) {
      player->heartRegenTimer = 0;
      player->health += HEART_REGEN_AMOUNT;
      if (player->health > player->maxHealth)
        player->health = player->maxHealth;
    }
  }

  if (player->gainTimer > 0) {
    player->gainTimer--;
    player->gainRegenTimer++;
    if (player->gainRegenTimer >= GAIN_REGEN_INTERVAL) {
      player->gainRegenTimer = 0;
      player->stamina += GAIN_REGEN_AMOUNT;
      if (player->stamina > player->maxStamina)
        player->stamina = player->maxStamina;
    }
  }
}

inline void updateGlowProjectile(struct GlowProjectile *glow,
                                 struct Creature creatures[],
                                 struct Sentry sentries[]) {
  if (!glow->active)
    return;

  glow->animTimer++;
  if (glow->animTimer >= 4) {
    glow->animTimer = 0;
    glow->frame = (glow->frame + 1) % GLOW_FRAMES;
  }

  int dx = glow->targetX - glow->x;
  int dy = glow->targetY - glow->y;
  float dist = sqrt((double)(dx * dx + dy * dy));

  if (dist < GLOW_SPEED + 5) {
    if (glow->targetCreatureIdx >= 0) {
      struct Creature *enemy = &creatures[glow->targetCreatureIdx];
      if (enemy->active && enemy->state != CREATURE_DEAD) {
        int damage = enemy->maxHealth * POWER_DAMAGE_PERCENT / 100;
        enemy->currentHealth -= damage;
        enemy->damageAnimTimer = CREATURE_DAMAGE_FRAMES * 4;
        enemy->damageFrame = 0;
        if (enemy->currentHealth <= 0) {
          enemy->currentHealth = 0;
          enemy->state = CREATURE_DEAD;
          enemy->subStateTimer = 0;
          enemy->animationTimer = 0;
          enemy->frame = 0;
          enemy->vx = 0;
          enemy->vy = 0;
        }
      }
    } else if (glow->targetSentryIdx >= 0) {
      struct Sentry *s = &sentries[glow->targetSentryIdx];
      if (s->active && s->state != SENTRY_DYING &&
          s->state != SENTRY_DYING_AIR) {
        int damage = s->maxHealth * POWER_DAMAGE_PERCENT / 100;
        s->currentHealth -= damage;
        s->damageAnimTimer = SENTRY_DAMAGE_FRAMES * 4;
        s->damageFrame = 0;
        if (s->currentHealth <= 0) {
          s->currentHealth = 0;
          s->state = SENTRY_DYING;
          s->subStateTimer = 0;
          s->animationTimer = 0;
          s->frame = 0;
          s->vx = 0;
          s->vy = 0;
        }
      }
    }
    glow->active = 0;
  } else {
    float nx = dx / dist;
    float ny = dy / dist;
    glow->x += (int)(nx * GLOW_SPEED);
    glow->y += (int)(ny * GLOW_SPEED);
  }
}

inline void renderPickups(struct Pickup pickups[], struct Camera *camera) {
  for (int i = 0; i < MAX_PICKUPS; i++) {
    if (!pickups[i].active)
      continue;

    unsigned int tex = 0;
    switch (pickups[i].type) {
    case ITEM_HEART:
      tex = pickableHeartTex;
      break;
    case ITEM_POTION:
      tex = pickablePotionTex;
      break;
    case ITEM_POWER:
      tex = pickablePowerTex;
      break;
    case ITEM_GAIN:
      tex = pickableGainTex;
      break;
    case ITEM_KEY:
      tex = traderKeyTex;
      break;
    }

    if (tex != 0) {
      float screenX = getScreenX(pickups[i].x, camera);
      float screenY = getScreenY(pickups[i].y, camera);
      iShowImage(screenX, screenY, PICKUP_SIZE, PICKUP_SIZE, tex);
    }
  }
}

void renderGlowProjectile(struct GlowProjectile *glow, struct Camera *camera) {

  if (!glow->active)
    return;

  unsigned int tex = glowTextures[glow->frame % GLOW_FRAMES];
  if (tex != 0) {
    float screenX = getScreenX(glow->x, camera);
    float screenY = getScreenY(glow->y, camera);
    iShowImage(screenX, screenY, GLOW_SIZE, GLOW_SIZE, tex);
  }
}

inline void renderInventoryUI(struct Player *player) {

  if (inventoryBaseTex != 0) {
    iShowImage(INVENTORY_X, INVENTORY_Y, INVENTORY_W, INVENTORY_H,
               inventoryBaseTex);
  }

  if (player->hasHeart && inventoryHeartTex != 0) {
    iShowImage(INVENTORY_X, INVENTORY_Y, INVENTORY_W, INVENTORY_H,
               inventoryHeartTex);
  }

  if (player->hasPotion && inventoryPotionTex != 0) {
    iShowImage(INVENTORY_X, INVENTORY_Y, INVENTORY_W, INVENTORY_H,
               inventoryPotionTex);
  }

  if (player->hasPower && inventoryPowerTex != 0) {
    iShowImage(INVENTORY_X, INVENTORY_Y, INVENTORY_W, INVENTORY_H,
               inventoryPowerTex);
  }

  if (player->hasGain && inventoryGainTex != 0) {
	  iShowImage(INVENTORY_X, INVENTORY_Y, INVENTORY_W, INVENTORY_H,
		  inventoryGainTex);
  }


}


#endif
