#ifndef GAME_HPP
#define GAME_HPP

#include "Sentry.hpp"
#include "background.hpp"
#include "bug.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "midground.hpp"
#include "player.hpp"
#include "sounds.hpp"
#include "structs.hpp"
#include "tradernpc.hpp"
#include "boss.hpp"
#include "level3boss.hpp"

extern struct TraderNPC traderNpc;
extern struct BossDoor bossDoor;
extern struct Boss boss;
extern struct BossMinion bossMinions[MAX_BOSS_MINIONS];
extern struct BossHazard bossHazards[MAX_BOSS_HAZARDS];

void setupLevel2Tiles(struct Midground *mg) {
  mg->tileCount = 0;
  int baseHeight = GROUND_Y + 100;
  addTile(mg, 1300, baseHeight, 1);
  addTile(mg, 1700, baseHeight + 50, 2);
  addTile(mg, 2050, baseHeight + 30, 1);
  addTile(mg, 2500, baseHeight + 60, 1);
  addTile(mg, 2900, baseHeight + 40, 2);
  addTile(mg, 3300, baseHeight + 90, 1);
  addTile(mg, 3700, baseHeight + 50, 2);
}

inline void startLevel2(Player &player, struct Creature creatures[],
                        struct Sentry sentries[], struct Camera *camera,
                        struct Midground *mg) {
  player.x = 200;
  player.y = GROUND_Y;
  player.vy = 0;
  player.onGround = true;
  camera->x = 0;
  camera->targetX = 0;
  initCreatures(creatures);
  initSentries(sentries);
  setupLevel2Tiles(mg);
  stopBGMusic();
  playBG2Music();
}

void setupLevel3Tiles(struct Midground *mg) {
  mg->tileCount = 0;
  mg->tileTexture1 = level3Tile1;
  mg->tileTexture2 = level3Tile2;
  int baseHeight = LEVEL3_GROUND_Y + 125;

  addTile(mg, 700,  baseHeight,       1);
  addTile(mg, 1500, baseHeight + 50,  2);
  addTile(mg, 2500, baseHeight + 30,  1);
  addTile(mg, 3400, baseHeight + 60,  2);
  addTile(mg, 4200, baseHeight + 40,  1);
}

inline void startLevel3(Player &player, struct Creature creatures[],
                        struct Sentry sentries[], struct Camera *camera,
                        struct Midground *mg) {
  player.x = 200;
  player.y = LEVEL3_GROUND_Y;
  player.vy = 0;
  player.onGround = true;
  player.fragments = 10; // gives the Level 3 trader a usable starting currency
  player.hasKeyItem = 0;
  player.keyUsed = 0;
  player.hasSwiftness = 0;
  player.hasSoul = 0;
  player.swiftnessUsed = 0;
  player.soulUsed = 0;
  player.swiftnessActive = 0;
  player.soulActive = 0;
  player.speedMultiplier = 1.0f;

  camera->x = 0;
  camera->targetX = 0;
  initCreatures(creatures);
  initSentries(sentries);
  setupLevel3Tiles(mg);

  initTraderNPC(&traderNpc);
  initBossDoor(&bossDoor);
  initLevel3Boss(&boss);
  initLevel3BossMinions(bossMinions);
  initLevel3BossHazards(bossHazards);

  stopBG2Music();
  playBG2Music();
}

inline void respawnPlayer(Player &player, struct Camera *camera, int gameState) {
  player.x = (gameState == BOSS_STATE) ? 200 : 200;
  player.y = (gameState == BOSS_STATE) ? BOSS_GROUND_Y :
             (gameState == LEVEL3_STATE ? LEVEL3_GROUND_Y : GROUND_Y);
  player.vy = 0;
  player.onGround = true;
  player.health = player.maxHealth;
  player.stamina = player.maxStamina;
  player.invincibilityTimer = 60;
  player.attacking = false;
  player.shootingArrow = false;
  player.dashing = false;
  camera->x = 0;
  camera->targetX = 0;
  playDeathSound();
}

inline void beginBossEntry(Player &player, struct Camera *camera, int *gameState) {
  *gameState = BOSS_ENTRY_STATE;
  player.bossEntryFrame = 0;
  player.bossEntryTimer = 0;
  player.x = 200;
  player.y = BOSS_GROUND_Y;
  player.vy = 0;
  player.onGround = true;
  camera->x = 0;
  camera->targetX = 0;
  stopBG2Music();
  playBossEntryMusic();
}

inline void beginBossFight(Player &player, struct Camera *camera, int *gameState) {
  *gameState = BOSS_STATE;
  player.x = 200;
  player.y = BOSS_GROUND_Y;
  player.vy = 0;
  player.onGround = true;
  player.health = player.maxHealth;
  player.stamina = player.maxStamina;
  player.invincibilityTimer = 30;
  camera->x = 0;
  camera->targetX = 0;
  initLevel3Boss(&boss);
  initLevel3BossMinions(bossMinions);
  initLevel3BossHazards(bossHazards);
  stopBossEntryMusic();
  playBossBGMusic();
}

inline void updateBossEntry(Player &player, struct Camera *camera, int *gameState) {
  (void)player;
  camera->x = 0;
  camera->targetX = 0;
  player.bossEntryTimer++;
  if (player.bossEntryTimer >= 2) {
    player.bossEntryTimer = 0;
    player.bossEntryFrame++;
  }
  if (player.bossEntryFrame >= BOSS_ENTRY_FRAMES) {
    beginBossFight(player, camera, gameState);
  }
}

inline void updateBossFight(Player &player, struct Camera *camera,
                            int *gameState, struct Arrow arrows[]) {
  camera->x = 0;
  camera->targetX = 0;

  updateLevel3Boss(&boss, &player, bossMinions, bossHazards);
  if (player.attacking) {
    handlePlayerAttackBoss(&boss, &player, 0);
    handlePlayerAttackMinion(bossMinions, &player, 0);
  }
  handlePlayerArrowBoss(&boss, &player, arrows);
  handlePlayerArrowMinion(bossMinions, &player, arrows);
  updateMinions(bossMinions, &player);
  updateHazards(bossHazards, &player);

  if (!boss.active) {
    stopBossBGMusic();
    *gameState = CONGRATS_STATE;
  }
}

inline void updateGame(Player &player, struct Creature creatures[],
                       struct Sentry sentries[], struct Camera *camera,
                       struct Midground *mg, int *gameState,
                       struct Arrow arrows[]) {
  if (*gameState == BOSS_ENTRY_STATE) {
    updateBossEntry(player, camera, gameState);
    return;
  }

  if (*gameState == BOSS_STATE) {
    updateBossFight(player, camera, gameState, arrows);
    if (isPlayerDead(player))
      respawnPlayer(player, camera, *gameState);
    return;
  }

  if (*gameState != PLAYING_STATE && *gameState != LEVEL2_STATE &&
      *gameState != LEVEL3_STATE)
    return;

  if (*gameState == LEVEL3_STATE) {
    updateTraderNPC(&traderNpc, &player);

    // The key is the progression item needed for the boss door.  Once the
    // player has bought it, approaching the door automatically uses it.
    if (player.hasKeyItem && !player.keyUsed &&
        player.x >= DOOR_X - 100) {
      player.keyUsed = 1;
    }
    updateBossDoor(&bossDoor, &player);
  }

  updateCreatures(creatures, &player, *gameState, arrows);
  updateSentries(sentries, &player, arrows, *gameState);
  updateCamera(camera, &player);

  if (*gameState == PLAYING_STATE && player.x >= LEVEL1_END_X) {
    *gameState = LEVEL2_STATE;
    startLevel2(player, creatures, sentries, camera, mg);
    return;
  }

  if (*gameState == LEVEL2_STATE && player.x >= LEVEL2_FINISH_X) {
    *gameState = LEVEL3_STATE;
    startLevel3(player, creatures, sentries, camera, mg);
    return;
  }

  if (*gameState == LEVEL3_STATE && player.x >= LEVEL3_END_X) {
    if (bossDoor.opened) {
      beginBossEntry(player, camera, gameState);
      return;
    }
    player.x = LEVEL3_END_X - 10;
  }

  if (isPlayerDead(player))
    respawnPlayer(player, camera, *gameState);
}

#endif
