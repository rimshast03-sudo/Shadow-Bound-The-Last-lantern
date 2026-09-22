#ifndef TEXTURES_HPP
#define TEXTURES_HPP

#include "config.hpp"
#include "iGraphics.h"
#include <cstdio>

unsigned int idle[IDLE_FRAMES];
unsigned int walkLeft[WALK_FRAMES];
unsigned int walkRight[WALK_FRAMES];
unsigned int turnLeft[TURN_FRAMES];
unsigned int turnRight[TURN_FRAMES];
unsigned int jumpLeft[JUMP_LEFT_FRAMES];
unsigned int jumpRight[JUMP_RIGHT_FRAMES];
unsigned int fallLeft[FALL_LEFT_FRAMES];
unsigned int fallRight[FALL_RIGHT_FRAMES];
unsigned int landLeft[LAND_FRAMES];
unsigned int landRight[LAND_FRAMES];
unsigned int inventoryBaseTex = 0;
unsigned int inventoryHeartTex = 0;
unsigned int inventoryPotionTex = 0;
unsigned int inventoryPowerTex = 0;
unsigned int inventoryGainTex = 0;
unsigned int pickableHeartTex = 0;
unsigned int pickablePotionTex = 0;
unsigned int pickablePowerTex = 0;
unsigned int pickableGainTex = 0;
unsigned int glowTextures[GLOW_FRAMES];


unsigned int dashLeft[DASH_LEFT_FRAMES];
unsigned int dashRight[DASH_RIGHT_FRAMES];

unsigned int downstabAnticipate[DOWNSTAB_ANTICIPATE_FRAMES];
unsigned int downstabSlam[DOWNSTAB_SLAM_FRAMES];
unsigned int downstabLand[DOWNSTAB_LAND_FRAMES];

unsigned int overheadAttackRecoverLeft[OVERHEAD_ATTACK_RECOVER_FRAMES];
unsigned int overheadAttackRecoverRight[OVERHEAD_ATTACK_RECOVER_FRAMES];
unsigned int overheadAttackSlashingLeft[OVERHEAD_ATTACK_SLASHING_FRAMES];
unsigned int overheadAttackSlashingRight[OVERHEAD_ATTACK_SLASHING_FRAMES];

unsigned int evadeLeftPre[EVADE_LEFT_PRE_FRAMES];
unsigned int evadeLeftActive[EVADE_LEFT_ACTIVE_FRAMES];
unsigned int evadeLeftRecover[EVADE_LEFT_RECOVER_FRAMES];
unsigned int evadeRightPre[EVADE_RIGHT_PRE_FRAMES];
unsigned int evadeRightActive[EVADE_RIGHT_ACTIVE_FRAMES];
unsigned int evadeRightRecover[EVADE_RIGHT_RECOVER_FRAMES];

unsigned int death[DEATH_FRAMES];
unsigned int gameOverFrames[GAME_OVER_FRAMES];
unsigned int bossEntryFrames[BOSS_ENTRY_FRAMES];
unsigned int gameEndingFrames[GAME_ENDING_FRAMES];

unsigned int wakePart1[WAKE_FRAMES_PART1];
unsigned int wakePart2[WAKE_FRAMES_PART2];
unsigned int wakePart3[WAKE_FRAMES_PART3];
unsigned int roar[ROAR_FRAMES];
unsigned int roarActive[ROAR_ACTIVE_FRAMES];
unsigned int roarEnd[ROAR_END_FRAMES];

unsigned int overheadAttackSlashwaveLeft[OVERHEAD_ATTACK_SLASHWAVE_FRAMES];
unsigned int overheadAttackSlashwaveRight[OVERHEAD_ATTACK_SLASHWAVE_FRAMES];

unsigned int backgroundTextures[BG_SEGMENTS];
unsigned int level2BackgroundTextures[BG_SEGMENTS];
unsigned int level3BackgroundTextures[BG_SEGMENTS];
unsigned int bossBackgroundTexture = 0;
unsigned int tunnelTextures[2];

unsigned int creatureNormal[CREATURE_NORMAL_FRAMES];
unsigned int creatureFlyL[CREATURE_FLYL_FRAMES];
unsigned int creatureFlyR[CREATURE_FLYR_FRAMES];
unsigned int creatureTurn[CREATURE_TURN_FRAMES];
unsigned int creatureAttack[CREATURE_ATTACK_FRAMES];
unsigned int creatureBurst[CREATURE_BURST_FRAMES];
unsigned int creatureDamage[CREATURE_DAMAGE_FRAMES];

unsigned int sentryIdle[SENTRY_IDLE_FRAMES];
unsigned int sentryWalkL[SENTRY_WALKL_FRAMES];
unsigned int sentryWalkR[SENTRY_WALKR_FRAMES];
unsigned int sentryRunL[SENTRY_RUNL_FRAMES];
unsigned int sentryRunR[SENTRY_RUNR_FRAMES];
unsigned int sentryAttackL[SENTRY_ATTACKL_FRAMES];
unsigned int sentryAttackR[SENTRY_ATTACKR_FRAMES];
unsigned int sentrySlashL[SENTRY_SLASHL_FRAMES];
unsigned int sentrySlashR[SENTRY_SLASHR_FRAMES];
unsigned int sentryTurnL[SENTRY_TURNL_FRAMES];
unsigned int sentryTurnR[SENTRY_TURNR_FRAMES];
unsigned int sentryWake[SENTRY_WAKE_FRAMES];
unsigned int sentryDeath[SENTRY_DEATH_FRAMES];
unsigned int sentryDeathAir[SENTRY_DEATHAIR_FRAMES];
unsigned int sentryDamage[SENTRY_DAMAGE_FRAMES];

unsigned int sparkleTextures[SPARKLE_FRAMES];

unsigned int level2TileFlat = 0;
unsigned int level2Tile1 = 0;

unsigned int caveTexture = 0;
unsigned int crestInventoryTex = 0;

// Greeting NPC textures (end of Level 2)
unsigned int greetIdleTex = 0;
unsigned int greetAnimTex[GREET_ANIM_FRAMES];

unsigned int level3Tile1 = 0;
unsigned int level3Tile2 = 0;

// Trade item textures
unsigned int tradeListTex = 0;
unsigned int tradeFragmentTex = 0;
unsigned int tradeSwiftnessTex = 0;
unsigned int tradeSoulTex = 0;
unsigned int tradeKeyImageTex = 0;
unsigned int tradeDesc1Tex = 0;
unsigned int tradeDesc2Tex = 0;
unsigned int tradeDesc3Tex = 0;
unsigned int swiftnessIconTex = 0;
unsigned int soulIconTex = 0;
unsigned int keyIconTex = 0;

// Door textures
unsigned int doorLockedTex[DOOR_LOCKED_FRAMES];
unsigned int doorOpenTex[DOOR_OPEN_FRAMES];

// Quit game menu textures
unsigned int quitBgTex = 0;
unsigned int quitYesTex = 0;
unsigned int quitNoTex = 0;
unsigned int quitArrow1Tex = 0;
unsigned int quitArrow2Tex = 0;
unsigned int congratsTex = 0;

#define NPC_APPROACH_FRAMES 12
#define NPC_IDLE_LEFT_FRAMES 8
#define NPC_IDLE_RIGHT_FRAMES 6
#define NPC_TURN_FRAMES 2
#define NPC_SUMMON_START_FRAMES 6
#define NPC_SUMMONING_FRAMES 7
#define NPC_RETREAT_FRAMES 13
#define DIALOGUE_COUNT 3

unsigned int npcApproach[NPC_APPROACH_FRAMES];
unsigned int npcIdleLeft[NPC_IDLE_LEFT_FRAMES];
unsigned int npcIdleRight[NPC_IDLE_RIGHT_FRAMES];
unsigned int npcTurn[NPC_TURN_FRAMES];
unsigned int npcSummonStart[NPC_SUMMON_START_FRAMES];
unsigned int npcSummoning[NPC_SUMMONING_FRAMES];
unsigned int npcRetreat[NPC_RETREAT_FRAMES];
unsigned int dialogues[DIALOGUE_COUNT];

void loadBossEntityTextures(); 
void loadSet(unsigned int *arr, int n, const char *fmt) {
  char name[128];
  for (int i = 0; i < n; i++) {
    sprintf_s(name, sizeof(name), fmt, i + 1);
    arr[i] = iLoadImage(name);
  }
}

void loadBackgroundTexture() {
  loadSet(backgroundTextures, BG_SEGMENTS, "Assets/Background/bg (%d).png");
}

void loadCreatureTextures() {
  loadSet(creatureNormal, CREATURE_NORMAL_FRAMES, "Assets/Bug/Normal/%d.png");
  loadSet(creatureFlyL, CREATURE_FLYL_FRAMES, "Assets/Bug/FlyL/%d.png");
  loadSet(creatureFlyR, CREATURE_FLYR_FRAMES, "Assets/Bug/FlyR/%d.png");
  loadSet(creatureTurn, CREATURE_TURN_FRAMES, "Assets/Bug/Turn/%d.png");
  loadSet(creatureAttack, CREATURE_ATTACK_FRAMES, "Assets/Bug/Attack/%d.png");
  loadSet(creatureBurst, CREATURE_BURST_FRAMES, "Assets/Bug/Burst/%d.png");
  loadSet(creatureDamage, CREATURE_DAMAGE_FRAMES, "Assets/Bug/Damage/%d.png");
}

void loadSentryTextures() {
  loadSet(sentryIdle, SENTRY_IDLE_FRAMES,
          "Assets/Level 2/Sentry/Idle/idle (%d).png");
  loadSet(sentryWalkL, SENTRY_WALKL_FRAMES,
          "Assets/Level 2/Sentry/WalkL/walk (%d).png");
  loadSet(sentryWalkR, SENTRY_WALKR_FRAMES,
          "Assets/Level 2/Sentry/WalkR/walk (%d).png");
  loadSet(sentryRunL, SENTRY_RUNL_FRAMES,
          "Assets/Level 2/Sentry/RunL/run (%d).png");
  loadSet(sentryRunR, SENTRY_RUNR_FRAMES,
          "Assets/Level 2/Sentry/RunR/run (%d).png");
  loadSet(sentryAttackL, SENTRY_ATTACKL_FRAMES,
          "Assets/Level 2/Sentry/AttackL/attack (%d).png");
  loadSet(sentryAttackR, SENTRY_ATTACKR_FRAMES,
          "Assets/Level 2/Sentry/AttackR/attack (%d).png");
  loadSet(sentrySlashL, SENTRY_SLASHL_FRAMES,
          "Assets/Level 2/Sentry/SlashL/slash (%d).png");
  loadSet(sentrySlashR, SENTRY_SLASHR_FRAMES,
          "Assets/Level 2/Sentry/SlashR/slash (%d).png");
  loadSet(sentryTurnL, SENTRY_TURNL_FRAMES,
          "Assets/Level 2/Sentry/TurnL/turn (%d).png");
  loadSet(sentryTurnR, SENTRY_TURNR_FRAMES,
          "Assets/Level 2/Sentry/TurnR/turn (%d).png");
  loadSet(sentryWake, SENTRY_WAKE_FRAMES,
          "Assets/Level 2/Sentry/Wake/wake (%d).png");
  loadSet(sentryDeath, SENTRY_DEATH_FRAMES,
          "Assets/Level 2/Sentry/Death/death (%d).png");
  loadSet(sentryDeathAir, SENTRY_DEATHAIR_FRAMES,
          "Assets/Level 2/Sentry/DeathAir/death (%d).png");
  loadSet(sentryDamage, SENTRY_DAMAGE_FRAMES,
          "Assets/Level 2/Sentry/Damage/%d.png");
}

void loadSparkleTextures() {
  for (int i = 0; i < SPARKLE_FRAMES; i++) {
    char name[128];
    sprintf_s(name, "Assets/Level 2/Sparkle/sparkle (%d).png", i + 1);
    sparkleTextures[i] = iLoadImage(name);
  }
}

void loadLevel2TileTextures() {
  level2TileFlat = iLoadImage("Assets/Level 2/bg/flattile.png");
  level2Tile1 = iLoadImage("Assets/Level 2/bg/tile (1).png");
}

void loadGreetTextures() {
  greetIdleTex = iLoadImage("Assets/Level 2/greet/idle.png");
  loadSet(greetAnimTex, GREET_ANIM_FRAMES, "Assets/Level 2/greet/greeting/%d.png");
}

void loadLevel3Textures() {
  loadSet(level3BackgroundTextures, BG_SEGMENTS, "Assets/Level 3/bg/%d.png");
}

void loadBossTextures() {
  bossBackgroundTexture = iLoadImage("Assets/Level 3/bossbg/bosslevelbg.png");
}

void loadBossEntryTextures() {
  loadSet(bossEntryFrames, BOSS_ENTRY_FRAMES,
          "Assets/BossEntry/ezgif-frame-%03d.png");
}

void loadInventoryTextures() {
  inventoryBaseTex = iLoadImage("Assets/UI/Inventory/inventory.png");
  inventoryHeartTex = iLoadImage("Assets/UI/Inventory/heart.png");
  inventoryPotionTex = iLoadImage("Assets/UI/Inventory/potion.png");
  inventoryPowerTex = iLoadImage("Assets/UI/Inventory/power.png");
  pickableHeartTex = iLoadImage("Assets/UI/Pickables/heart.png");
  pickablePotionTex = iLoadImage("Assets/UI/Pickables/potion.png");
  pickablePowerTex = iLoadImage("Assets/UI/Pickables/power.png");
  inventoryGainTex = iLoadImage("Assets/UI/Inventory/gain.png");
  pickableGainTex = iLoadImage("Assets/UI/Pickables/gain.png");
  loadSet(glowTextures, GLOW_FRAMES, "Assets/UI/Glow/glow (%d).png");
  crestInventoryTex = iLoadImage("Assets/UI/Crests/crest inventory.png");
}

void loadLevel3TileTextures() {
  level3Tile1 = iLoadImage("Assets/Level 3/tiles/tile (1).png");
  level3Tile2 = iLoadImage("Assets/Level 3/tiles/tile (2).png");
}

void loadNPCTextures() {
  loadSet(npcApproach, NPC_APPROACH_FRAMES,
          "Assets/Level 2/Npc/Approach/ (%d).png");
  loadSet(npcIdleLeft, NPC_IDLE_LEFT_FRAMES,
          "Assets/Level 2/Npc/Idle/IdleLeft/ (%d).png");
  loadSet(npcIdleRight, NPC_IDLE_RIGHT_FRAMES,
          "Assets/Level 2/Npc/Idle/IdleRight/ (%d).png");
  loadSet(npcTurn, NPC_TURN_FRAMES, "Assets/Level 2/Npc/Turn/ (%d).png");
  loadSet(npcSummonStart, NPC_SUMMON_START_FRAMES,
          "Assets/Level 2/Npc/Summon/SummonStart/ (%d).png");
  loadSet(npcSummoning, NPC_SUMMONING_FRAMES,
          "Assets/Level 2/Npc/Summon/Summoning/ (%d).png");
  loadSet(npcRetreat, NPC_RETREAT_FRAMES,
          "Assets/Level 2/Npc/Retreat/ (%d).png");
  loadSet(dialogues, DIALOGUE_COUNT, "Assets/Level 2/Dialogue/dialogue%d.png");
}



unsigned int bossIdle[BOSS_IDLE_FRAMES];
unsigned int bossWalkL[BOSS_WALK_FRAMES];
unsigned int bossWalkR[BOSS_WALK_FRAMES];
unsigned int bossSlashL[BOSS_SLASH_FRAMES];
unsigned int bossSlashR[BOSS_SLASH_FRAMES];
unsigned int bossDashL[BOSS_DASH_FRAMES];
unsigned int bossDashR[BOSS_DASH_FRAMES];
unsigned int bossCastL[BOSS_CAST_FRAMES];
unsigned int bossCastR[BOSS_CAST_FRAMES];
unsigned int bossTeleportOut[BOSS_TELEPORT_FRAMES];
unsigned int bossTeleportIn[BOSS_TELEPORT_FRAMES];
unsigned int bossSpike[BOSS_SPIKE_FRAMES];
unsigned int bossTrapIn[BOSS_TRAPIN_FRAMES];
unsigned int bossTrapOut[BOSS_TRAPOUT_FRAMES];
unsigned int bossDeath[BOSS_DEATH_FRAMES];

unsigned int fireBatL[FIREBAT_FRAMES];
unsigned int fireBatR[FIREBAT_FRAMES];
unsigned int batL[BAT_FRAMES];
unsigned int batR[BAT_FRAMES];
unsigned int batDeath[BAT_DEATH_FRAMES];

void loadBossEntityTextures() {
  loadSet(bossIdle, BOSS_IDLE_FRAMES, "Assets/Boss/Idle/%d.png");
  loadSet(bossWalkL, BOSS_WALK_FRAMES, "Assets/Boss/WalkL/%d.png");
  loadSet(bossWalkR, BOSS_WALK_FRAMES, "Assets/Boss/WalkR/%d.png");
  loadSet(bossSlashL, BOSS_SLASH_FRAMES, "Assets/Boss/SlashL/%d.png");
  loadSet(bossSlashR, BOSS_SLASH_FRAMES, "Assets/Boss/SlashR/%d.png");
  loadSet(bossDashL, BOSS_DASH_FRAMES, "Assets/Boss/DashL/%d.png");
  loadSet(bossDashR, BOSS_DASH_FRAMES, "Assets/Boss/DashR/%d.png");
  loadSet(bossCastL, BOSS_CAST_FRAMES, "Assets/Boss/CastL/%d.png");
  loadSet(bossCastR, BOSS_CAST_FRAMES, "Assets/Boss/CastR/%d.png");
  loadSet(bossTeleportOut, BOSS_TELEPORT_FRAMES,
          "Assets/Boss/TeleportOut/%d.png");
  loadSet(bossTeleportIn, BOSS_TELEPORT_FRAMES,
          "Assets/Boss/TeleportIn/%d.png");
  loadSet(bossSpike, BOSS_SPIKE_FRAMES, "Assets/Boss/Spike/%d.png");
  loadSet(bossTrapIn, BOSS_TRAPIN_FRAMES, "Assets/Boss/TrapIn/%d.png");
  loadSet(bossTrapOut, BOSS_TRAPOUT_FRAMES, "Assets/Boss/TrapOut/%d.png");
  loadSet(bossDeath, BOSS_DEATH_FRAMES, "Assets/Boss/Death/%d.png");
  loadSet(fireBatL, FIREBAT_FRAMES, "Assets/Boss/FireBatL/%d.png");
  loadSet(fireBatR, FIREBAT_FRAMES, "Assets/Boss/FireBatR/%d.png");
  loadSet(batL, BAT_FRAMES, "Assets/Boss/BatL/%d.png");
  loadSet(batR, BAT_FRAMES, "Assets/Boss/BatR/%d.png");
  loadSet(batDeath, BAT_DEATH_FRAMES, "Assets/Boss/BatDeath/%d.png");
}

unsigned int traderIdle[TRADER_IDLE_FRAMES];
unsigned int traderWalkL[TRADER_WALK_L_FRAMES];
unsigned int traderWalkR[TRADER_WALK_R_FRAMES];
unsigned int traderTurn[TRADER_TURN_FRAMES];
unsigned int traderTrade[TRADER_TRADE_FRAMES];
unsigned int traderKeyTex = 0;

void loadTraderNPCTextures() {
  loadSet(traderIdle, TRADER_IDLE_FRAMES,
          "Assets/Level 3/trader npc/idle/%d.png");
  loadSet(traderWalkL, TRADER_WALK_L_FRAMES,
          "Assets/Level 3/trader npc/walk/walk L/%d.png");
  loadSet(traderWalkR, TRADER_WALK_R_FRAMES,
          "Assets/Level 3/trader npc/walk/walk R/%d.png");
  loadSet(traderTurn, TRADER_TURN_FRAMES,
          "Assets/Level 3/trader npc/turn/%d.png");
  loadSet(traderTrade, TRADER_TRADE_FRAMES,
          "Assets/Level 3/trader npc/trade/%d.png");
  traderKeyTex = iLoadImage("Assets/Level 3/trade item/key.png");
}

void loadTradeItemTextures() {
  tradeListTex = iLoadImage("Assets/Level 3/trade item/list.png");
  tradeFragmentTex = iLoadImage("Assets/Level 3/trade item/fragment.png");
  tradeSwiftnessTex = iLoadImage("Assets/Level 3/trade item/swiftness.png");
  tradeSoulTex = iLoadImage("Assets/Level 3/trade item/soul.png");
  tradeKeyImageTex = iLoadImage("Assets/Level 3/trade item/key.png");
  tradeDesc1Tex = iLoadImage("Assets/Level 3/trade item/description 1.png");
  tradeDesc2Tex = iLoadImage("Assets/Level 3/trade item/description 2.png");
  tradeDesc3Tex = iLoadImage("Assets/Level 3/trade item/description 3.png");
  swiftnessIconTex = iLoadImage("Assets/Level 3/trade item/swiftness icon.png");
  soulIconTex = iLoadImage("Assets/Level 3/trade item/soul icon.png");
  keyIconTex = iLoadImage("Assets/Level 3/trade item/key icon.png");
}

void loadDoorTextures() {
  loadSet(doorLockedTex, DOOR_LOCKED_FRAMES, "Assets/Level 3/door/locked/%d.png");
  loadSet(doorOpenTex, DOOR_OPEN_FRAMES, "Assets/Level 3/door/open/%d.png");
}

unsigned int grimIdle[GRIM_IDLE_FRAMES];
unsigned int grimTurnL[GRIM_TURN_L_FRAMES];
unsigned int grimTurnR[GRIM_TURN_R_FRAMES];
unsigned int grimDashAnticL[GRIM_DASH_ANTIC_L_FRAMES];
unsigned int grimDashAnticR[GRIM_DASH_ANTIC_R_FRAMES];
unsigned int grimDashL[GRIM_DASH_L_FRAMES];
unsigned int grimDashR[GRIM_DASH_R_FRAMES];
unsigned int grimDeath[GRIM_DEATH_FRAMES];
unsigned int grimThrowAnticL[GRIM_THROW_ANTIC_L_FRAMES];
unsigned int grimThrowAnticR[GRIM_THROW_ANTIC_R_FRAMES];
unsigned int grimThrowL[GRIM_THROW_L_FRAMES];
unsigned int grimThrowR[GRIM_THROW_R_FRAMES];
unsigned int grimFireball[GRIM_FIREBALL_FRAMES];
unsigned int grimFireballExplode[GRIM_FIREBALL_EXPLODE_FRAMES];
unsigned int grimTeleInPillar[GRIM_TELEPORT_IN_PILLAR_FRAMES];
unsigned int grimTeleIn[GRIM_TELEPORT_IN_FRAMES];
unsigned int grimTeleOut[GRIM_TELEPORT_OUT_FRAMES];
unsigned int grimTeleOutPillar[GRIM_TELEPORT_OUT_PILLAR_FRAMES];

void loadGrimMasterTextures() {
  loadSet(grimIdle, GRIM_IDLE_FRAMES,
          "Assets/Level 3/grim master/idle/%d.png");
  loadSet(grimTurnL, GRIM_TURN_L_FRAMES,
          "Assets/Level 3/grim master/turn/turn L/%d.png");
  loadSet(grimTurnR, GRIM_TURN_R_FRAMES,
          "Assets/Level 3/grim master/turn/turn R/%d.png");
  loadSet(grimDashAnticL, GRIM_DASH_ANTIC_L_FRAMES,
          "Assets/Level 3/grim master/dash/anticipate/anticipate L/%d.png");
  loadSet(grimDashAnticR, GRIM_DASH_ANTIC_R_FRAMES,
          "Assets/Level 3/grim master/dash/anticipate/anticipate R/%d.png");
  loadSet(grimDashL, GRIM_DASH_L_FRAMES,
          "Assets/Level 3/grim master/dash/dash/dash L/%d.png");
  loadSet(grimDashR, GRIM_DASH_R_FRAMES,
          "Assets/Level 3/grim master/dash/dash/dash R/%d.png");
  loadSet(grimDeath, GRIM_DEATH_FRAMES,
          "Assets/Level 3/grim master/death/%d.png");
  loadSet(grimThrowAnticL, GRIM_THROW_ANTIC_L_FRAMES,
          "Assets/Level 3/grim master/throw/anticipate/anticipate L/%d.png");
  loadSet(grimThrowAnticR, GRIM_THROW_ANTIC_R_FRAMES,
          "Assets/Level 3/grim master/throw/anticipate/anticipate R/%d.png");
  loadSet(grimThrowL, GRIM_THROW_L_FRAMES,
          "Assets/Level 3/grim master/throw/throws/throw L/%d.png");
  loadSet(grimThrowR, GRIM_THROW_R_FRAMES,
          "Assets/Level 3/grim master/throw/throws/throw R/%d.png");
  loadSet(grimFireball, GRIM_FIREBALL_FRAMES,
          "Assets/Level 3/grim master/fireball/fire balls/%d.png");
  loadSet(grimFireballExplode, GRIM_FIREBALL_EXPLODE_FRAMES,
          "Assets/Level 3/grim master/fireball/fire ball explode/%d.png");
  loadSet(grimTeleInPillar, GRIM_TELEPORT_IN_PILLAR_FRAMES,
          "Assets/Level 3/grim master/teleport/teleport in/pillar/%d.png");
  loadSet(grimTeleIn, GRIM_TELEPORT_IN_FRAMES,
          "Assets/Level 3/grim master/teleport/teleport in/%d.png");
  loadSet(grimTeleOut, GRIM_TELEPORT_OUT_FRAMES,
          "Assets/Level 3/grim master/teleport/teleport out/%d.png");
  loadSet(grimTeleOutPillar, GRIM_TELEPORT_OUT_PILLAR_FRAMES,
          "Assets/Level 3/grim master/teleport/teleport out/pillar/%d.png");
}

void loadImages() {
  loadBackgroundTexture();
  loadSet(level2BackgroundTextures, BG_SEGMENTS,
          "Assets/Level 2/bg/bg (%d).png");
  loadCreatureTextures();
  loadSentryTextures();
  loadLevel3Textures();
  loadBossTextures();
  loadBossEntryTextures();
  loadLevel3TileTextures();
  loadNPCTextures();
  loadGreetTextures();
  loadTraderNPCTextures();
  loadTradeItemTextures();
  loadDoorTextures();
  loadBossEntityTextures();
}

#endif
