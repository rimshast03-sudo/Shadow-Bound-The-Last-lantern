// ============================================================
// platforms.hpp
// Person C — floating platforms the player can stand on.
//
// COORDINATE SPACE NOTE:
// drawPlayer()/updatePlayer() in player.hpp use p.x / p.y as plain
// screen coordinates (same as GROUND_Y) — they are NOT run through
// Camera::toScreenX/Y the way the scrolling background is. So these
// platforms are placed and collided against in that same screen
// space, or the player would visually drift off any camera-scrolled
// platform. If the player's draw/collision is ever hooked up to the
// camera later, this file needs the same treatment.
// ============================================================
#ifndef PLATFORMS_HPP
#define PLATFORMS_HPP

#include "iGraphics.h"
#include "player.hpp"   // needs PLAYER_WIDTH, GRAVITY, GROUND_Y, Player

// =====================================================
// PLATFORM
// =====================================================

struct Platform
{
	int x;          // left edge (screen space)
	int y;          // Y of the walkable surface / top of the platform
	int width;
	int thickness;  // purely visual — how tall the block is drawn
};


// =====================================================
// LEVEL 1 PLATFORMS
//
// Placeholder positions — tune x/y/width to line up with the actual
// floating roots/branches in your background art. Heights are kept
// within a single JUMP_POWER's reach (~140px) from the ground or
// from each other so they're all reachable without a double jump.
// =====================================================

#define LEVEL1_PLATFORM_COUNT 4

class Level1Platforms
{
private:
	Platform list[LEVEL1_PLATFORM_COUNT];

public:
	Level1Platforms()
	{
		list[0] = { 250, 140, 150, 20 };
		list[1] = { 480, 230, 150, 20 };
		list[2] = { 700, 150, 150, 20 };
		list[3] = { 860, 260, 120, 20 };
	}

	// No texture yet — platforms are drawn as flat-colored blocks.
	// Kept for symmetry with Level1Background::load() in case art
	// gets added later.
	void load()
	{
	}

	void draw()
	{
		for (int i = 0; i < LEVEL1_PLATFORM_COUNT; i++)
		{
			Platform &plat = list[i];

			// Body
			iSetColor(90, 65, 40);
			iFilledRectangle(plat.x, plat.y - plat.thickness, plat.width, plat.thickness);

			// Lighter top edge so the walkable surface reads clearly
			iSetColor(130, 100, 60);
			iFilledRectangle(plat.x, plat.y - 4, plat.width, 4);
		}
	}

	Platform* getList()
	{
		return list;
	}

	int getCount() const
	{
		return LEVEL1_PLATFORM_COUNT;
	}
};


// =====================================================
// PLAYER <-> PLATFORM COLLISION
//
// Call this once per frame, AFTER updatePlayer(), so it sees this
// frame's already-updated p.x / p.y / p.vy from gravity, movement,
// and jumping.
// =====================================================

inline void updatePlayerPlatforms(Player &p, Platform platforms[], int platformCount)
{
	// Assume airborne this frame unless a platform (or the main
	// ground, handled separately in player.hpp) is found supporting
	// the player below.
	bool supported = false;

	int playerLeft = p.x;
	int playerRight = p.x + PLAYER_WIDTH;

	for (int i = 0; i < platformCount; i++)
	{
		Platform &plat = platforms[i];

		bool horizontallyOverlapping =
			playerRight > plat.x && playerLeft < plat.x + plat.width;

		if (!horizontallyOverlapping)
			continue;

		// Already resting on this platform from a previous frame.
		if (p.onGround && p.y == plat.y)
		{
			supported = true;
			break;
		}

		// Only land while falling (or right at the jump's apex) —
		// never while still moving upward, so the player can jump
		// up through a platform from underneath it.
		if (p.vy > 0)
			continue;

		// How far gravity could have moved the player this single
		// frame — used as a tolerance so a fast fall doesn't tunnel
		// straight through a thin platform without being caught.
		int fallDistance = -p.vy + GRAVITY + 1;

		if (p.y <= plat.y && p.y >= plat.y - fallDistance)
		{
			p.y = plat.y;
			p.vy = 0;
			p.onGround = true;
			supported = true;
			break;
		}
	}

	// Walked off the edge of a platform (and isn't on the main
	// ground either) — let gravity take back over next frame.
	if (!supported && p.onGround && p.y != GROUND_Y)
	{
		p.onGround = false;
	}
}

#endif
