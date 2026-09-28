
#ifndef CAVE_HPP
#define CAVE_HPP
#include "iGraphics.h"
#include "config.hpp"
#include "player.hpp"
#include "bug.hpp"
#include "goblin.hpp"
#include "boss.hpp"
#include "FlyingCreature.hpp"
#include "LightningThrower.hpp"
bool caveStarted = false;

// --- Wave 4: Flyers + Lightning Throwers (final wave, after the boss) ---
// No separate wave file -- these live right here in cave.hpp since
// FlyingCreature.hpp / LightningThrower.hpp only provide the enemy
// classes themselves, not a spawn queue (unlike bug.hpp / goblin.hpp,
// which already manage their own).

#ifndef CAVE_FLYER_MAX
#define CAVE_FLYER_MAX 3
#endif
#ifndef CAVE_THROWER_MAX
#define CAVE_THROWER_MAX 2
#endif
#ifndef CAVE_FLYER_SPAWN_X
#define CAVE_FLYER_SPAWN_X 1100
#endif
#ifndef CAVE_FLYER_SPAWN_Y
#define CAVE_FLYER_SPAWN_Y 400
#endif
#ifndef CAVE_THROWER_SPAWN_X
#define CAVE_THROWER_SPAWN_X 1100
#endif
#ifndef CAVE_THROWER_SPAWN_Y
#define CAVE_THROWER_SPAWN_Y 300
#endif
#ifndef CAVE_FLYER_HP
#define CAVE_FLYER_HP 3
#endif
#ifndef CAVE_THROWER_HP
#define CAVE_THROWER_HP 4
#endif
#ifndef CAVE_WAVE4_PLAYER_DAMAGE
#define CAVE_WAVE4_PLAYER_DAMAGE 1
#endif

FlyingCreature*   activeFlyer = nullptr;
LightningThrower* activeThrower = nullptr;

int flyersSpawned = 0;
int flyersDefeated = 0;
int throwersSpawned = 0;
int throwersDefeated = 0;

// Mirrors the "hitRegisteredThisSwing" pattern used in bug.hpp / goblin.hpp:
// makes sure one sword swing only damages the active enemy once, even
// though checkPlayerAttackCollision() stays true for the whole swing.
bool finalWaveHitRegisteredThisSwing = false;

inline void initFinalWave() {
	if (activeFlyer)   { delete activeFlyer;   activeFlyer = nullptr; }
	if (activeThrower) { delete activeThrower; activeThrower = nullptr; }
	flyersSpawned = 0;
	flyersDefeated = 0;
	throwersSpawned = 0;
	throwersDefeated = 0;
	finalWaveHitRegisteredThisSwing = false;
}

inline bool allFlyersDefeated()   { return flyersDefeated >= CAVE_FLYER_MAX; }
inline bool allThrowersDefeated() { return throwersDefeated >= CAVE_THROWER_MAX; }
inline bool isFinalWaveDefeated() { return allFlyersDefeated() && allThrowersDefeated(); }

// Spawns the next enemy in the queue if there's an empty slot. Flyers go
// first (CAVE_FLYER_MAX of them), then throwers (CAVE_THROWER_MAX). Safe
// to call every frame -- it only actually spawns something when needed.
inline void spawnFinalWave() {
	if (!allFlyersDefeated()) {
		if (activeFlyer == nullptr && flyersSpawned < CAVE_FLYER_MAX) {
			activeFlyer = new FlyingCreature(CAVE_FLYER_SPAWN_X, CAVE_FLYER_SPAWN_Y, CAVE_FLYER_HP);
			flyersSpawned++;
		}
		return;
	}
	if (!allThrowersDefeated()) {
		if (activeThrower == nullptr && throwersSpawned < CAVE_THROWER_MAX) {
			activeThrower = new LightningThrower(CAVE_THROWER_SPAWN_X, CAVE_THROWER_SPAWN_Y, CAVE_THROWER_HP);
			throwersSpawned++;
		}
	}
}

inline void updateFinalWave(Player &player) {
	spawnFinalWave();

	if (activeFlyer) {
		activeFlyer->updateAI(player);
		activeFlyer->updateAnimation();

		if (activeFlyer->checkSlashHitsPlayer(player)) {
			damagePlayer(player, CAVE_WAVE4_PLAYER_DAMAGE);
		}
		if (activeFlyer->checkSplatHitsPlayer(player)) {
			damagePlayer(player, CAVE_WAVE4_PLAYER_DAMAGE);
		}

		// Let the player's sword damage the flyer (once per swing).
		if (checkPlayerAttackCollision(player, activeFlyer->getX(), activeFlyer->getY(),
			activeFlyer->getWidth(), activeFlyer->getHeight())) {
			if (!finalWaveHitRegisteredThisSwing && !activeFlyer->getIsDead()) {
				activeFlyer->takeDamage(getPlayerAttackDamage());
				finalWaveHitRegisteredThisSwing = true;
			}
		}
		else if (!player.attacking) {
			finalWaveHitRegisteredThisSwing = false;
		}

		if (activeFlyer->isReadyToRemove()) {
			delete activeFlyer;
			activeFlyer = nullptr;
			flyersDefeated++;
			finalWaveHitRegisteredThisSwing = false;
		}
	}
	else if (activeThrower) {
		activeThrower->updateAI(player);
		activeThrower->updateAnimation();
		activeThrower->updateProjectile();

		if (activeThrower->checkLightningHitsPlayer(player)) {
			damagePlayer(player, CAVE_WAVE4_PLAYER_DAMAGE);
		}

		// Let the player's sword damage the thrower (once per swing).
		if (checkPlayerAttackCollision(player, activeThrower->getX(), activeThrower->getY(),
			activeThrower->getWidth(), activeThrower->getHeight())) {
			if (!finalWaveHitRegisteredThisSwing && !activeThrower->getIsDead()) {
				activeThrower->takeDamage(getPlayerAttackDamage());
				finalWaveHitRegisteredThisSwing = true;
			}
		}
		else if (!player.attacking) {
			finalWaveHitRegisteredThisSwing = false;
		}

		if (activeThrower->isReadyToRemove()) {
			delete activeThrower;
			activeThrower = nullptr;
			throwersDefeated++;
			finalWaveHitRegisteredThisSwing = false;
		}
	}
}

inline void drawFinalWave() {
	if (activeFlyer)   activeFlyer->draw();
	if (activeThrower) activeThrower->draw();
}

// -----------------------------------------------------------------
// The player's sword now damages the active flyer/thrower directly
// inside updateFinalWave() (same checkPlayerAttackCollision() +
// getPlayerAttackDamage() pattern bug.hpp / goblin.hpp use), so this
// helper isn't called from here anymore. Left in place in case you
// want to damage the active enemy from somewhere else too.
// -----------------------------------------------------------------
inline void damageActiveFinalWaveEnemy(int amount) {
	if (activeFlyer)        activeFlyer->takeDamage(amount);
	else if (activeThrower) activeThrower->takeDamage(amount);
}

inline void initCave() {
	initBugs();
	initGoblins();
	initBoss();
	initFinalWave();
	caveStarted = false; // spawnBugWave() below flips this on
}
// Call once, right when the player drops into CAVE_STATE.
inline void startCaveEncounter(int playerX) {
	spawnBugWave(playerX);
	caveStarted = true;
}
// LEVEL 1 -- bugs, goblins, Grim Master boss. Ends the moment the boss falls;
// iMain.cpp is what actually switches gameState to LEVEL2_STATE once
// isBossDefeated() is true (see the CAVE_STATE branch of animate()).
inline void updateCave(Player &player, struct Arrow arrows[]) {
	if (!caveStarted) return;
	// Step 1: Bug wave (one bug active on screen at a time). Untouched.
	if (!areAllBugsDefeated()) {
		updateBugs(player, arrows);
		return;
	}
	// Step 2: Once bugs are cleared, goblin wave starts (also one at a time). Untouched.
	if (!areAllGoblinsDefeated()) {
		spawnGoblinWave(player.x);
		updateGoblins(player, arrows);
		return;
	}
	// Step 3: Once goblins are cleared, unlock the boss further into the cave.
	// This is the last step of Level 1 -- once isBossDefeated() is true,
	// iMain.cpp moves the player into LEVEL2_STATE instead of this function
	// falling through to the flyer/thrower wave itself.
	if (!isBossDefeated()) {
		if (player.x >= CAVE_BOSS_UNLOCK_X && !bossActive) {
			startBoss();
		}
		updateBoss(player, arrows);
	}
}
inline void drawCave() {
	if (!caveStarted) return;
	// The real scrolling background is drawn by iMain.cpp before this function.
	// Do NOT paint a full-screen rectangle here; doing so hides the background.
	// Wave progress UI.
	iSetColor(255, 255, 255);
	if (!areAllBugsDefeated()) {
		iText(SCREEN_W / 2 - 110, SCREEN_H - 30, "WAVE 1: Defeat the Bugs!");
	}
	else if (!areAllGoblinsDefeated()) {
		iText(SCREEN_W / 2 - 110, SCREEN_H - 30, "WAVE 2: Defeat the Goblins!");
	}
	else {
		// isBossDefeated() firing switches gameState to LEVEL1_CLEARED_STATE
		// the same tick (see iMain.cpp's animate()), so this branch only
		// covers the moments before that: still fighting/approaching the boss.
		iText(SCREEN_W / 2 - 190, SCREEN_H - 30, "WAVE 3: Push forward to face the Grim Master!");
	}
	drawBugs();
	drawGoblins();
	drawBoss();
}

// LEVEL 2 -- flyers + lightning throwers only (FlyingCreature.hpp /
// LightningThrower.hpp). Entered once Level 1's boss falls; ends once
// isFinalWaveDefeated() is true (checked in iMain.cpp's animate()).
inline void updateLevel2(Player &player) {
	updateFinalWave(player);
}
inline void drawLevel2() {
	iSetColor(255, 255, 255);
	if (!isFinalWaveDefeated()) {
		iText(SCREEN_W / 2 - 170, SCREEN_H - 30, "LEVEL 2: Defeat the Flyers and Lightning Throwers!");
	}
	else {
		iSetColor(255, 215, 0);
		iText(SCREEN_W / 2 - 90, SCREEN_H / 2, "CAVE CLEARED!", GLUT_BITMAP_TIMES_ROMAN_24);
	}
	drawFinalWave();
}
#endif