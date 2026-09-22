#ifndef BOSS_HPP
#define BOSS_HPP

#include "iGraphics.h"
#include "config.hpp"
#include "player.hpp"
#include "caveboss.hpp"

bool bossActive = false;
bool bossDefeated = false;

inline void initBoss() {
	bossActive = false;
	bossDefeated = false;
}

inline void startBoss() {
	if (bossActive || bossDefeated) return;

	initCaveBoss(CAVE_GRIMMASTER_SPAWN_X, CAVE_GRIMMASTER_SPAWN_Y);
	bossActive = true;
}

inline void updateBoss(Player &player, struct Arrow arrows[]) {
	if (!bossActive) return;

	updateCaveBoss(player, arrows);

	if (!caveBossAlive()) {
		bossActive = false;
		bossDefeated = true;
	}
}

inline void drawBoss() {
	if (!bossActive) return;
	drawCaveBoss();
}

inline bool isBossDefeated() {
	return bossDefeated;
}

#endif
