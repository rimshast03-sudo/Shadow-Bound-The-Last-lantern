// ============================================================
// caveboss.hpp — Cave boss (final wave)
// ============================================================
#ifndef CAVEBOSS_HPP
#define CAVEBOSS_HPP

#include "iGraphics.h"
#include "config.hpp"
#include "player.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum CaveBossState {
	CAVEBOSS_FLIGHT,
	CAVEBOSS_ATTACK_RAP,
	CAVEBOSS_TENTACLE_HARPOON,
	CAVEBOSS_FLINCH,
	CAVEBOSS_DEATH
};

struct CaveBoss
{
	int x;
	int y;
	int cruiseY;  // High cruising altitude
	int attackY;  // Lower altitude when attacking
	bool active;
	int hp;
	CaveBossState state;
	int currentFrame;
	int frameTimer;
	int attackCooldown;

	bool hitRegisteredThisSwing;
	bool attackHitRegistered;
};

CaveBoss caveBoss;

unsigned int caveBossAttackRapImg[CAVE_GRIMMASTER_ATTACK_RAP_FRAMES];
unsigned int caveBossTentacleHarpoonImg[CAVE_GRIMMASTER_TENTACLE_HARPOON_FRAMES];
unsigned int caveBossFlightImg[CAVE_GRIMMASTER_FLIGHT_FRAMES];
unsigned int caveBossFlinchImg[CAVE_GRIMMASTER_FLINCH_FRAMES];
unsigned int caveBossDeathImg[CAVE_GRIMMASTER_DEATH_FRAMES];
bool caveBossTexturesLoaded = false;

inline unsigned int caveBossLoadImg(const char* filename)
{
	char buf[200];
	strcpy_s(buf, filename);
	return iLoadImage(buf);
}

inline void loadCaveBossTextures()
{
	if (caveBossTexturesLoaded) return;

	char path[150];
	for (int i = 0; i < CAVE_GRIMMASTER_TENTACLE_HARPOON_FRAMES; i++)
	{
		sprintf_s(path, "Assets/Cave/GrimMaster/attacktent/attacktent%d.png", i + 1);
		caveBossTentacleHarpoonImg[i] = caveBossLoadImg(path);
	}

	for (int i = 0; i < CAVE_GRIMMASTER_FLINCH_FRAMES; i++)
	{
		sprintf_s(path, "Assets/Cave/GrimMaster/flinch/flinch%d.png", i + 1);
		caveBossFlinchImg[i] = caveBossLoadImg(path);
	}

	for (int i = 0; i < CAVE_GRIMMASTER_DEATH_FRAMES; i++)
	{
		sprintf_s(path, "Assets/Cave/GrimMaster/deathA1/deathA%d.png", i + 1);
		caveBossDeathImg[i] = caveBossLoadImg(path);
	}

	// attackrap frame 5 is missing on disk (goes 1,2,3,4,6), so it is listed explicitly.
	const char* rapFiles[CAVE_GRIMMASTER_ATTACK_RAP_FRAMES] = {
		"Assets/Cave/GrimMaster/attackrap/attackrap1.png",
		"Assets/Cave/GrimMaster/attackrap/attackrap2.png",
		"Assets/Cave/GrimMaster/attackrap/attackrap3.png",
		"Assets/Cave/GrimMaster/attackrap/attackrap4png.png",
		"Assets/Cave/GrimMaster/attackrap/attackrap6.png"
	};
	for (int i = 0; i < CAVE_GRIMMASTER_ATTACK_RAP_FRAMES; i++)
		caveBossAttackRapImg[i] = caveBossLoadImg(rapFiles[i]);

	// flight frame 4 is misnamed "flighr4.png" on disk.
	const char* flightFiles[CAVE_GRIMMASTER_FLIGHT_FRAMES] = {
		"Assets/Cave/GrimMaster/flight/flight1.png",
		"Assets/Cave/GrimMaster/flight/flight2.png",
		"Assets/Cave/GrimMaster/flight/flight3.png",
		"Assets/Cave/GrimMaster/flight/flighr4.png",
		"Assets/Cave/GrimMaster/flight/flight5.png"
	};
	for (int i = 0; i < CAVE_GRIMMASTER_FLIGHT_FRAMES; i++)
		caveBossFlightImg[i] = caveBossLoadImg(flightFiles[i]);

	caveBossTexturesLoaded = true;
}

// Option B: Preserves (int spawnX, int spawnY) signature; calculates attackY via offset.
inline void initCaveBoss(int spawnX, int spawnY)
{
	loadCaveBossTextures();

	caveBoss.x = spawnX;
	caveBoss.y = spawnY;
	caveBoss.cruiseY = spawnY;
	caveBoss.attackY = spawnY - 150; // Adjust this offset value if you want it higher/lower
	caveBoss.active = true;
	caveBoss.hp = CAVE_GRIMMASTER_MAX_HP;
	caveBoss.state = CAVEBOSS_FLIGHT;
	caveBoss.currentFrame = 0;
	caveBoss.frameTimer = 0;
	caveBoss.attackCooldown = 0;
	caveBoss.hitRegisteredThisSwing = false;
	caveBoss.attackHitRegistered = false;
}

inline void damageCaveBoss(int amount)
{
	if (!caveBoss.active || caveBoss.state == CAVEBOSS_DEATH) return;

	caveBoss.hp -= amount;
	caveBoss.currentFrame = 0;
	caveBoss.frameTimer = 0;

	if (caveBoss.hp <= 0)
	{
		caveBoss.hp = 0;
		caveBoss.state = CAVEBOSS_DEATH;
	}
	else
	{
		caveBoss.state = CAVEBOSS_FLINCH;
	}
}

inline void updateCaveBoss(Player &player, struct Arrow arrows[])
{
	if (!caveBoss.active) return;

	// Let the player's sword damage the boss (once per swing).
	if (checkPlayerAttackCollision(player, caveBoss.x, caveBoss.y,
		CAVE_GRIMMASTER_WIDTH, CAVE_GRIMMASTER_HEIGHT))
	{
		if (!caveBoss.hitRegisteredThisSwing && caveBoss.state != CAVEBOSS_DEATH)
		{
			damageCaveBoss(getPlayerAttackDamage());
			caveBoss.hitRegisteredThisSwing = true;
		}
	}
	else if (!player.attacking)
	{
		caveBoss.hitRegisteredThisSwing = false;
	}

	// Let the player's arrow damage the boss.
	if (caveBoss.state != CAVEBOSS_DEATH)
	{
		for (int a = 0; a < MAX_ARROWS; a++)
		{
			if (checkArrowCollision(arrows[a], caveBoss.x, caveBoss.y,
				CAVE_GRIMMASTER_WIDTH, CAVE_GRIMMASTER_HEIGHT))
			{
				damageCaveBoss(getArrowDamage());
				arrows[a].active = false;
				break;
			}
		}
	}

	caveBoss.frameTimer++;
	if (caveBoss.frameTimer < CAVE_GRIMMASTER_ANIM_SPEED) return;
	caveBoss.frameTimer = 0;

	switch (caveBoss.state)
	{
	case CAVEBOSS_FLIGHT:
		// Ascend back to high cruising altitude
		if (caveBoss.y < caveBoss.cruiseY)
		{
			caveBoss.y += 10;
			if (caveBoss.y > caveBoss.cruiseY) caveBoss.y = caveBoss.cruiseY;
		}

		caveBoss.currentFrame = (caveBoss.currentFrame + 1) % CAVE_GRIMMASTER_FLIGHT_FRAMES;

		if (caveBoss.attackCooldown > 0)
		{
			caveBoss.attackCooldown--;
		}
		else if (abs(caveBoss.x - player.x) < CAVE_GRIMMASTER_ATTACK_RANGE)
		{
			// Alternate between the tentacle harpoon and rapier-tentacle strike.
			caveBoss.state = (rand() % 2 == 0) ? CAVEBOSS_TENTACLE_HARPOON : CAVEBOSS_ATTACK_RAP;
			caveBoss.currentFrame = 0;
			caveBoss.attackHitRegistered = false;
		}
		break;

	case CAVEBOSS_ATTACK_RAP:
		// Descend to attack altitude
		if (caveBoss.y > caveBoss.attackY)
		{
			caveBoss.y -= 15;
			if (caveBoss.y < caveBoss.attackY) caveBoss.y = caveBoss.attackY;
		}

		if (!caveBoss.attackHitRegistered && caveBoss.currentFrame >= CAVE_GRIMMASTER_ATTACK_RAP_FRAMES / 2
			&& abs(caveBoss.x - player.x) < CAVE_GRIMMASTER_ATTACK_RANGE)
		{
			damagePlayer(player, CAVE_GRIMMASTER_DAMAGE_TO_PLAYER);
			caveBoss.attackHitRegistered = true;
		}

		caveBoss.currentFrame++;
		if (caveBoss.currentFrame >= CAVE_GRIMMASTER_ATTACK_RAP_FRAMES)
		{
			caveBoss.currentFrame = 0;
			caveBoss.state = CAVEBOSS_FLIGHT;
			caveBoss.attackCooldown = 30;
		}
		break;

	case CAVEBOSS_TENTACLE_HARPOON:
		// Descend to attack altitude
		if (caveBoss.y > caveBoss.attackY)
		{
			caveBoss.y -= 15;
			if (caveBoss.y < caveBoss.attackY) caveBoss.y = caveBoss.attackY;
		}

		if (!caveBoss.attackHitRegistered && caveBoss.currentFrame >= CAVE_GRIMMASTER_TENTACLE_HARPOON_FRAMES / 2
			&& abs(caveBoss.x - player.x) < CAVE_GRIMMASTER_ATTACK_RANGE)
		{
			damagePlayer(player, CAVE_GRIMMASTER_DAMAGE_TO_PLAYER);
			caveBoss.attackHitRegistered = true;
		}

		caveBoss.currentFrame++;
		if (caveBoss.currentFrame >= CAVE_GRIMMASTER_TENTACLE_HARPOON_FRAMES)
		{
			caveBoss.currentFrame = 0;
			caveBoss.state = CAVEBOSS_FLIGHT;
			caveBoss.attackCooldown = 30;
		}
		break;

	case CAVEBOSS_FLINCH:
		caveBoss.currentFrame++;
		if (caveBoss.currentFrame >= CAVE_GRIMMASTER_FLINCH_FRAMES)
		{
			caveBoss.currentFrame = 0;
			caveBoss.state = CAVEBOSS_FLIGHT;
		}
		break;

	case CAVEBOSS_DEATH:
		caveBoss.currentFrame++;
		if (caveBoss.currentFrame >= CAVE_GRIMMASTER_DEATH_FRAMES)
		{
			caveBoss.active = false;
		}
		break;
	}
}

inline bool caveBossAlive()
{
	return caveBoss.active;
}

inline void drawCaveBoss()
{
	if (!caveBoss.active) return;

	unsigned int img = 0;
	switch (caveBoss.state)
	{
	case CAVEBOSS_FLIGHT:
		img = caveBossFlightImg[caveBoss.currentFrame % CAVE_GRIMMASTER_FLIGHT_FRAMES];
		break;
	case CAVEBOSS_ATTACK_RAP:
		img = caveBossAttackRapImg[caveBoss.currentFrame % CAVE_GRIMMASTER_ATTACK_RAP_FRAMES];
		break;
	case CAVEBOSS_TENTACLE_HARPOON:
		img = caveBossTentacleHarpoonImg[caveBoss.currentFrame % CAVE_GRIMMASTER_TENTACLE_HARPOON_FRAMES];
		break;
	case CAVEBOSS_FLINCH:
		img = caveBossFlinchImg[caveBoss.currentFrame % CAVE_GRIMMASTER_FLINCH_FRAMES];
		break;
	case CAVEBOSS_DEATH:
		img = caveBossDeathImg[caveBoss.currentFrame % CAVE_GRIMMASTER_DEATH_FRAMES];
		break;
	}

	iShowImage(caveBoss.x, caveBoss.y, CAVE_GRIMMASTER_WIDTH, CAVE_GRIMMASTER_HEIGHT, img);

	// Boss HP bar
	iSetColor(200, 0, 0);
	iFilledRectangle(SCREEN_W / 2 - 300, SCREEN_H - 20, 600, 12);
	iSetColor(0, 200, 0);
	iFilledRectangle(SCREEN_W / 2 - 300, SCREEN_H - 20, 600 * caveBoss.hp / CAVE_GRIMMASTER_MAX_HP, 12);
	iSetColor(255, 255, 255);
	iText(SCREEN_W / 2 - 320, SCREEN_H - 4, "GRIM MASTER");
}

#endif