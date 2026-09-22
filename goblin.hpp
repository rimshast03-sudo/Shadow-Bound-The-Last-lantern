// ============================================================
// goblin.hpp — Cave enemy, Wave 2
//
// Ported from "The Last Lantern" enemy-system demo, following
// the same conventions as bug.hpp: constants in config.hpp
// (CAVE_GOBLIN_ prefix), real combat via player.hpp's hooks,
// and goblins spawned ONE AT A TIME (see updateGoblins()).
// ============================================================
#ifndef GOBLIN_HPP
#define GOBLIN_HPP

#include "iGraphics.h"
#include "config.hpp"
#include "player.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum GoblinState {
	GOBLIN_WALK,
	GOBLIN_SPIN_ATTACK,
	GOBLIN_TAKEDAMAGE,
	GOBLIN_DEATH
};

struct Goblin
{
	int x;
	int y;
	bool active;
	int hp;
	int maxHp;
	GoblinState state;
	int currentFrame;
	int frameTimer;
	bool movingRight;

	bool hitRegisteredThisSwing;
	bool attackHitRegistered;
};

Goblin goblins[CAVE_GOBLIN_MAX];

bool goblinWaveStarted = false;
int goblinCurrentIndex = 0;
int goblinSpawnDelayTimer = 0;

// walkleft* frames double as the "moving left" set; goblinright* as the "moving right" set.
unsigned int goblinWalkLeftImg[CAVE_GOBLIN_WALK_FRAMES];
unsigned int goblinWalkRightImg[CAVE_GOBLIN_WALK_FRAMES];
unsigned int goblinSpinAttackImg[CAVE_GOBLIN_SPIN_ATTACK_FRAMES];
unsigned int goblinTakeDamageImg[CAVE_GOBLIN_TAKEDAMAGE_FRAMES];
unsigned int goblinDeathImg[CAVE_GOBLIN_DEATH_FRAMES];
bool goblinTexturesLoaded = false;

inline unsigned int goblinLoadImg(const char* filename)
{
	char buf[200];
	strcpy_s(buf, filename);
	return iLoadImage(buf);
}

inline void loadGoblinTextures()
{
	if (goblinTexturesLoaded) return;

	char path[150];
	for (int i = 0; i < CAVE_GOBLIN_WALK_FRAMES; i++)
	{
		sprintf_s(path, "Assets/Cave/Goblin/walkleft%d.png", i + 1);
		goblinWalkLeftImg[i] = goblinLoadImg(path);
	}

	for (int i = 0; i < CAVE_GOBLIN_DEATH_FRAMES; i++)
	{
		sprintf_s(path, "Assets/Cave/Goblin/deathgoblin%d.png", i + 1);
		goblinDeathImg[i] = goblinLoadImg(path);
	}

	// goblinright frame 5 is missing on disk, so it is listed explicitly.
	const char* rightFiles[CAVE_GOBLIN_WALK_FRAMES] = {
		"Assets/Cave/Goblin/goblinright1.png",
		"Assets/Cave/Goblin/goblinright2.png",
		"Assets/Cave/Goblin/goblinright3.png",
		"Assets/Cave/Goblin/goblinright4.png",
		"Assets/Cave/Goblin/goblinright6.png"
	};
	for (int i = 0; i < CAVE_GOBLIN_WALK_FRAMES; i++)
		goblinWalkRightImg[i] = goblinLoadImg(rightFiles[i]);

	const char* spinFiles[CAVE_GOBLIN_SPIN_ATTACK_FRAMES] = {
		"Assets/Cave/Goblin/spinAttackgoblin1.png",
		"Assets/Cave/Goblin/spinAttackgoblin2(1).png",
		"Assets/Cave/Goblin/spinAttackgoblin3 (1).png",
		"Assets/Cave/Goblin/spinAttackgoblin4 (1).png",
		"Assets/Cave/Goblin/spinAttackgoblin5 (1).png"
	};
	for (int i = 0; i < CAVE_GOBLIN_SPIN_ATTACK_FRAMES; i++)
		goblinSpinAttackImg[i] = goblinLoadImg(spinFiles[i]);

	// takedamage frame 1 is missing on disk, frames 2-6 are used.
	const char* damageFiles[CAVE_GOBLIN_TAKEDAMAGE_FRAMES] = {
		"Assets/Cave/Goblin/takedamage2.png",
		"Assets/Cave/Goblin/takedamage3.png",
		"Assets/Cave/Goblin/takedamage4.png",
		"Assets/Cave/Goblin/takedamage5.png",
		"Assets/Cave/Goblin/takedamage6.png"
	};
	for (int i = 0; i < CAVE_GOBLIN_TAKEDAMAGE_FRAMES; i++)
		goblinTakeDamageImg[i] = goblinLoadImg(damageFiles[i]);

	goblinTexturesLoaded = true;
}

inline void initGoblins()
{
	loadGoblinTextures();

	for (int i = 0; i < CAVE_GOBLIN_MAX; i++)
	{
		goblins[i].active = false;
		goblins[i].hp = 0;
		goblins[i].maxHp = CAVE_GOBLIN_MAX_HP;
		goblins[i].state = GOBLIN_WALK;
		goblins[i].currentFrame = 0;
		goblins[i].frameTimer = 0;
		goblins[i].movingRight = true;
		goblins[i].hitRegisteredThisSwing = false;
		goblins[i].attackHitRegistered = false;
	}

	goblinWaveStarted = false;
	goblinCurrentIndex = 0;
	goblinSpawnDelayTimer = 0;
}

inline int getRandomGoblinSpawnX(int playerX)
{
	if (playerX < CAVE_RANDOM_SPAWN_TRIGGER_X)
		return CAVE_GOBLIN_SPAWN_X;

	int spawnPoints[CAVE_RANDOM_SPAWN_POINT_COUNT] = {
		CAVE_RANDOM_SPAWN_X1, CAVE_RANDOM_SPAWN_X2, CAVE_RANDOM_SPAWN_X3,
		CAVE_RANDOM_SPAWN_X4, CAVE_RANDOM_SPAWN_X5
	};

	for (int attempt = 0; attempt < CAVE_RANDOM_SPAWN_POINT_COUNT * 2; attempt++)
	{
		int candidate = spawnPoints[rand() % CAVE_RANDOM_SPAWN_POINT_COUNT];
		if (abs(candidate - playerX) >= CAVE_RANDOM_SPAWN_MIN_DISTANCE)
			return candidate;
	}

	return spawnPoints[rand() % CAVE_RANDOM_SPAWN_POINT_COUNT];
}

inline void spawnSingleGoblin(int index, int playerX)
{
	if (index < 0 || index >= CAVE_GOBLIN_MAX) return;

	goblins[index].x = getRandomGoblinSpawnX(playerX);
	goblins[index].y = CAVE_GOBLIN_SPAWN_Y;
	goblins[index].active = true;
	goblins[index].hp = CAVE_GOBLIN_MAX_HP;
	goblins[index].maxHp = CAVE_GOBLIN_MAX_HP;
	goblins[index].state = GOBLIN_WALK;
	goblins[index].currentFrame = 0;
	goblins[index].frameTimer = 0;
	goblins[index].movingRight = false; // enters from the right, faces left
	goblins[index].hitRegisteredThisSwing = false;
	goblins[index].attackHitRegistered = false;
}

// Starts the goblin wave: only the FIRST goblin appears immediately.
// The rest spawn one at a time as earlier ones are defeated
// (see updateGoblins()).
inline void spawnGoblinWave(int playerX)
{
	if (goblinWaveStarted) return;

	goblinWaveStarted = true;
	goblinCurrentIndex = 0;
	goblinSpawnDelayTimer = 0;
	spawnSingleGoblin(goblinCurrentIndex, playerX);
}

inline void damageGoblin(int index, int amount)
{
	if (index < 0 || index >= CAVE_GOBLIN_MAX) return;
	if (!goblins[index].active || goblins[index].state == GOBLIN_DEATH) return;

	goblins[index].hp -= amount;
	if (goblins[index].hp <= 0)
	{
		goblins[index].hp = 0;
		goblins[index].state = GOBLIN_DEATH;
	}
	else
	{
		goblins[index].state = GOBLIN_TAKEDAMAGE;
	}
	goblins[index].currentFrame = 0;
	goblins[index].frameTimer = 0;
}

inline void updateGoblins(Player &player, struct Arrow arrows[])
{
	if (!goblinWaveStarted) return;

	if (goblinCurrentIndex >= CAVE_GOBLIN_MAX) return;

	Goblin &g = goblins[goblinCurrentIndex];

	if (!g.active)
	{
		if (goblinSpawnDelayTimer > 0)
		{
			goblinSpawnDelayTimer--;
			return;
		}

		goblinCurrentIndex++;
		if (goblinCurrentIndex < CAVE_GOBLIN_MAX)
			spawnSingleGoblin(goblinCurrentIndex, player.x);

		return;
	}

	if (checkPlayerAttackCollision(player, g.x, g.y, CAVE_GOBLIN_WIDTH, CAVE_GOBLIN_HEIGHT))
	{
		if (!g.hitRegisteredThisSwing && g.state != GOBLIN_DEATH)
		{
			damageGoblin(goblinCurrentIndex, getPlayerAttackDamage());
			g.hitRegisteredThisSwing = true;
		}
	}
	else if (!player.attacking)
	{
		g.hitRegisteredThisSwing = false;
	}

	if (g.state != GOBLIN_DEATH)
	{
		for (int a = 0; a < MAX_ARROWS; a++)
		{
			if (checkArrowCollision(arrows[a], g.x, g.y, CAVE_GOBLIN_WIDTH, CAVE_GOBLIN_HEIGHT))
			{
				damageGoblin(goblinCurrentIndex, getArrowDamage());
				arrows[a].active = false;
				break;
			}
		}
	}

	g.frameTimer++;
	if (g.frameTimer < CAVE_GOBLIN_ANIM_SPEED) return;
	g.frameTimer = 0;

	switch (g.state)
	{
	case GOBLIN_WALK:
		if (g.x < player.x) { g.x += CAVE_GOBLIN_MOVE_SPEED; g.movingRight = true; }
		else if (g.x > player.x) { g.x -= CAVE_GOBLIN_MOVE_SPEED; g.movingRight = false; }

		g.currentFrame = (g.currentFrame + 1) % CAVE_GOBLIN_WALK_FRAMES;

		if (abs(g.x - player.x) < CAVE_GOBLIN_ATTACK_RANGE)
		{
			g.state = GOBLIN_SPIN_ATTACK;
			g.currentFrame = 0;
			g.attackHitRegistered = false;
		}
		break;

	case GOBLIN_SPIN_ATTACK:
		if (!g.attackHitRegistered && g.currentFrame >= CAVE_GOBLIN_SPIN_ATTACK_FRAMES / 2
			&& abs(g.x - player.x) < CAVE_GOBLIN_ATTACK_RANGE)
		{
			damagePlayer(player, CAVE_GOBLIN_DAMAGE_TO_PLAYER);
			g.attackHitRegistered = true;
		}

		g.currentFrame++;
		if (g.currentFrame >= CAVE_GOBLIN_SPIN_ATTACK_FRAMES)
		{
			g.currentFrame = 0;
			g.state = GOBLIN_WALK;
		}
		break;

	case GOBLIN_TAKEDAMAGE:
		g.currentFrame++;
		if (g.currentFrame >= CAVE_GOBLIN_TAKEDAMAGE_FRAMES)
		{
			g.currentFrame = 0;
			g.state = GOBLIN_WALK;
		}
		break;

	case GOBLIN_DEATH:
		g.currentFrame++;
		if (g.currentFrame >= CAVE_GOBLIN_DEATH_FRAMES)
		{
			g.active = false;
			goblinSpawnDelayTimer = CAVE_GOBLIN_SPAWN_DELAY;
		}
		break;
	}
}

// True once every goblin in the wave has been spawned AND defeated.
inline bool areAllGoblinsDefeated()
{
	if (!goblinWaveStarted) return false;
	return goblinCurrentIndex >= CAVE_GOBLIN_MAX;
}

inline void drawGoblins()
{
	if (!goblinWaveStarted || goblinCurrentIndex >= CAVE_GOBLIN_MAX) return;

	Goblin &g = goblins[goblinCurrentIndex];
	if (!g.active) return;

	unsigned int img = 0;
	switch (g.state)
	{
	case GOBLIN_WALK:
		img = g.movingRight
			? goblinWalkRightImg[g.currentFrame % CAVE_GOBLIN_WALK_FRAMES]
			: goblinWalkLeftImg[g.currentFrame % CAVE_GOBLIN_WALK_FRAMES];
		break;
	case GOBLIN_SPIN_ATTACK:
		img = goblinSpinAttackImg[g.currentFrame % CAVE_GOBLIN_SPIN_ATTACK_FRAMES];
		break;
	case GOBLIN_TAKEDAMAGE:
		img = goblinTakeDamageImg[g.currentFrame % CAVE_GOBLIN_TAKEDAMAGE_FRAMES];
		break;
	case GOBLIN_DEATH:
		img = goblinDeathImg[g.currentFrame % CAVE_GOBLIN_DEATH_FRAMES];
		break;
	}

	iShowImage(g.x, g.y, CAVE_GOBLIN_WIDTH, CAVE_GOBLIN_HEIGHT, img);

	if (g.state != GOBLIN_DEATH)
	{
		iSetColor(200, 0, 0);
		iFilledRectangle(g.x, g.y + CAVE_GOBLIN_HEIGHT + 8, CAVE_GOBLIN_WIDTH, 6);
		iSetColor(0, 200, 0);
		iFilledRectangle(g.x, g.y + CAVE_GOBLIN_HEIGHT + 8, CAVE_GOBLIN_WIDTH * g.hp / g.maxHp, 6);
	}
}

#endif
