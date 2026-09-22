#ifndef PLAYER_HPP
#define PLAYER_HPP


#include "iGraphics.h"
#include "config.hpp"
#include "structs.hpp"
#include "midground.hpp"
#include "camera.hpp"
#include <windows.h>
#include <cstdio>


#define PLAYER_SPEED 6
#define JUMP_POWER 17

#define PLAYER_DASH_SPEED 18
#define DASH_DURATION 8
#define DASH_STAMINA_COST 25

#define ATTACK_DURATION 12
#define ATTACK_STAMINA_COST 5
#define ATTACK_DAMAGE 20
#define PLAYER_ATTACK_RANGE 70


#define PLAYER_ATTACK_HITBOX_OVERLAP 30


#define ARROW_ATTACK_DURATION 12
#define ARROW_ATTACK_STAMINA_COST 5
#define ARROW_DAMAGE 20
#define ARROW_SPEED 25
#define ARROW_WIDTH 50
#define ARROW_HEIGHT 18
#define ARROW_ANIM_FRAME_DELAY 4

#define MAX_HEALTH PLAYER_MAX_HEALTH
#define MAX_STAMINA PLAYER_MAX_STAMINA

#define STAMINA_REGEN_SPEED 1

#define INVINCIBILITY_TIME 60


unsigned int arrowFlyTextures[ARROW_FLY_FRAMES];




inline void initPlayer(Player &p)
{
	p.x = 200;
	p.y = GROUND_Y;

	p.vx = 0;
	p.vy = 0;

	p.health = MAX_HEALTH;
	p.maxHealth = MAX_HEALTH;

	p.stamina = MAX_STAMINA;
	p.maxStamina = MAX_STAMINA;

	p.onGround = true;
	p.facingRight = true;

	p.attacking = false;
	p.dashing = false;
	p.shootingArrow = false;

	p.attackTimer = 0;
	p.dashTimer = 0;
	p.arrowAttackTimer = 0;

	p.invincibilityTimer = 0;

	p.moving = false;

	p.animState = PLAYER_ANIM_IDLE;
	p.animFrame = 0;
	p.animTimer = 0;

	// Level 3 progression starts fresh while keeping the same player.
	p.fragments = 0;
	p.hasSwiftness = 0;
	p.hasSoul = 0;
	p.hasKeyItem = 0;
	p.swiftnessUsed = 0;
	p.soulUsed = 0;
	p.keyUsed = 0;
	p.swiftnessActive = 0;
	p.soulActive = 0;
	p.speedMultiplier = 1.0f;
	p.isTrapped = 0;
	p.bossEntryFrame = 0;
	p.bossEntryTimer = 0;

	
	char path[128];

	for (int i = 0; i < PLAYER_WALK_FRAMES; i++)
	{
		sprintf_s(path, sizeof(path), "Assets/mc/walk/walk R/%d.png", i + 1);
		p.walkRight[i] = iLoadImage(path);

		sprintf_s(path, sizeof(path), "Assets/mc/walk/walk L/%d.png", i + 1);
		p.walkLeft[i] = iLoadImage(path);
	}

	for (int i = 0; i < PLAYER_IDLE_FRAMES; i++)
	{
		sprintf_s(path, sizeof(path), "Assets/mc/idle/%d.png", i + 1);
		p.idleImages[i] = iLoadImage(path);
	}

	for (int i = 0; i < PLAYER_SLASH_FRAMES; i++)
	{
		sprintf_s(path, sizeof(path), "Assets/mc/slash/slash R/slashing/%d.png", i + 1);
		p.slashRight[i] = iLoadImage(path);

		sprintf_s(path, sizeof(path), "Assets/mc/slash/slash L/slashing/%d.png", i + 1);
		p.slashLeft[i] = iLoadImage(path);
	}

	for (int i = 0; i < PLAYER_ARROW_FRAMES; i++)
	{
		sprintf_s(path, sizeof(path), "Assets/mc/arrow/arrow R/%d.png", i + 1);
		p.arrowRight[i] = iLoadImage(path);

		sprintf_s(path, sizeof(path), "Assets/mc/arrow/arrow L/%d.png", i + 1);
		p.arrowLeft[i] = iLoadImage(path);
	}

	for (int i = 0; i < ARROW_FLY_FRAMES; i++)
	{
		sprintf_s(path, sizeof(path), "Assets/mc/arrow/fly/%d.png", i + 1);
		arrowFlyTextures[i] = iLoadImage(path);
	}
}



inline void updatePlayerMovement(Player &p)
{
	p.moving = false;

	if (p.dashing)
		return;

	int moveSpeed = (int)(PLAYER_SPEED * p.speedMultiplier);
	if (moveSpeed < 1) moveSpeed = PLAYER_SPEED;

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		p.x += moveSpeed;
		p.facingRight = true;
		p.moving = true;
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		p.x -= moveSpeed;
		p.facingRight = false;
		p.moving = true;
	}

	 
	if (p.x < 0)
		p.x = 0;

	if (p.x > TOTAL_BG_WIDTH - PLAYER_WIDTH)
		p.x = TOTAL_BG_WIDTH - PLAYER_WIDTH;
}



inline void playerJump(Player &p)
{
	static bool previousSpace = false;

	bool currentSpace =
		(GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

	if (currentSpace && !previousSpace)
	{
		if (p.onGround)
		{
			p.vy = JUMP_POWER;
			p.onGround = false;
		}
	}

	previousSpace = currentSpace;
}




inline void updatePlayerGravity(Player &p, struct Midground *mg, int groundY = GROUND_Y)
{
	if (!p.onGround)
	{
		p.y += p.vy;
		p.vy -= GRAVITY;
	}

	int platformY = 0;
	int onPlatform = (mg != 0) &&
		checkMidgroundCollision((struct Player*)&p, mg, &platformY, 0);

	if (onPlatform && p.vy <= 0)
	{
		p.y = platformY;
		p.vy = 0;
		p.onGround = true;
	}
	else if (!onPlatform && p.y <= groundY && p.vy <= 0)
	{
		p.y = groundY;
		p.vy = 0;
		p.onGround = true;
	}
	else if (!onPlatform && p.y > groundY)
	{
		
		p.onGround = false;
	}
}


inline void playerDash(Player &p)
{
	static bool previousZ = false;

	bool currentZ =
		(GetAsyncKeyState('Z') & 0x8000) != 0;

	if (currentZ && !previousZ)
	{
		if (!p.dashing &&
			p.stamina >= DASH_STAMINA_COST)
		{
			p.dashing = true;

			p.dashTimer = DASH_DURATION;

			p.stamina -= DASH_STAMINA_COST;
		}
	}

	previousZ = currentZ;


	if (p.dashing)
	{
		if (p.facingRight)
			p.x += PLAYER_DASH_SPEED;
		else
			p.x -= PLAYER_DASH_SPEED;

		p.dashTimer--;

		if (p.dashTimer <= 0)
		{
			p.dashing = false;
		}
	}


	if (p.x < 0)
		p.x = 0;

	if (p.x > TOTAL_BG_WIDTH - PLAYER_WIDTH)
		p.x = TOTAL_BG_WIDTH - PLAYER_WIDTH;
}



inline void playerAttack(Player &p)
{
	static bool previousX = false;

	bool currentX =
		(GetAsyncKeyState('X') & 0x8000) != 0;

	if (currentX && !previousX)
	{
		if (!p.attacking &&
			p.stamina >= ATTACK_STAMINA_COST)
		{
			p.attacking = true;

			p.attackTimer = ATTACK_DURATION;

			p.stamina -= ATTACK_STAMINA_COST;

			p.animState = PLAYER_ANIM_ATTACK;
			p.animFrame = 0;
			p.animTimer = 0;
		}
	}

	previousX = currentX;


	if (p.attacking)
	{
		p.attackTimer--;

		if (p.attackTimer <= 0)
		{
			p.attacking = false;
		}
	}
}




inline void initArrows(struct Arrow arrows[])
{
	for (int i = 0; i < MAX_ARROWS; i++)
	{
		arrows[i].active = false;
		arrows[i].x = 0;
		arrows[i].y = 0;
		arrows[i].facingRight = true;
		arrows[i].shootingUp = false;
		arrows[i].frame = 0;
		arrows[i].animTimer = 0;
	}
}

inline void playerArrowAttack(Player &p, struct Arrow arrows[])
{
	static bool previousC = false;

	bool currentC =
		(GetAsyncKeyState('C') & 0x8000) != 0;

	if (currentC && !previousC)
	{
		if (!p.attacking && !p.shootingArrow &&
			p.stamina >= ARROW_ATTACK_STAMINA_COST)
		{
			p.shootingArrow = true;

			p.arrowAttackTimer = ARROW_ATTACK_DURATION;

			p.stamina -= ARROW_ATTACK_STAMINA_COST;

			p.animState = PLAYER_ANIM_ARROW;
			p.animFrame = 0;
			p.animTimer = 0;

			for (int i = 0; i < MAX_ARROWS; i++)
			{
				if (!arrows[i].active)
				{
					arrows[i].active = true;
					arrows[i].facingRight = p.facingRight;
					arrows[i].frame = 0;
					arrows[i].animTimer = 0;

					bool aimingUp = (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
					arrows[i].shootingUp = aimingUp;

					if (aimingUp)
					{
						
						arrows[i].x = p.x + PLAYER_WIDTH / 2 - ARROW_HEIGHT / 2;
						arrows[i].y = p.y + PLAYER_HEIGHT;
					}
					else
					{
						arrows[i].y = p.y + 25;

						if (p.facingRight)
							arrows[i].x = p.x + PLAYER_WIDTH;
						else
							arrows[i].x = p.x - ARROW_WIDTH;
					}

					break;
				}
			}
		}
	}

	previousC = currentC;


	if (p.shootingArrow)
	{
		p.arrowAttackTimer--;

		if (p.arrowAttackTimer <= 0)
		{
			p.shootingArrow = false;
		}
	}
}

inline void updateArrows(struct Arrow arrows[])
{
	for (int i = 0; i < MAX_ARROWS; i++)
	{
		if (!arrows[i].active)
			continue;

		if (arrows[i].shootingUp)
		{
			arrows[i].y += ARROW_SPEED;

			if (arrows[i].y > GROUND_Y + ARROW_VERTICAL_RANGE)
			{
				arrows[i].active = false;
				continue;
			}
		}
		else
		{
			if (arrows[i].facingRight)
				arrows[i].x += ARROW_SPEED;
			else
				arrows[i].x -= ARROW_SPEED;

			if (arrows[i].x < -ARROW_WIDTH || arrows[i].x > TOTAL_BG_WIDTH)
			{
				arrows[i].active = false;
				continue;
			}
		}

		arrows[i].animTimer++;

		if (arrows[i].animTimer >= ARROW_ANIM_FRAME_DELAY)
		{
			arrows[i].animTimer = 0;
			arrows[i].frame = (arrows[i].frame + 1) % ARROW_FLY_FRAMES;
		}
	}
}

inline void renderArrows(struct Arrow arrows[], struct Camera *camera)
{
	for (int i = 0; i < MAX_ARROWS; i++)
	{
		if (!arrows[i].active)
			continue;

		unsigned int tex = arrowFlyTextures[arrows[i].frame % ARROW_FLY_FRAMES];

		if (tex != 0)
		{
			float screenX = getScreenX((float)arrows[i].x, camera);
			float screenY = getScreenY((float)arrows[i].y, camera);

			int w = arrows[i].shootingUp ? ARROW_HEIGHT : ARROW_WIDTH;
			int h = arrows[i].shootingUp ? ARROW_WIDTH : ARROW_HEIGHT;

			iShowImage(screenX, screenY, w, h, tex);
		}
	}
}


inline void renderArrowsScreenSpace(struct Arrow arrows[])
{
	for (int i = 0; i < MAX_ARROWS; i++)
	{
		if (!arrows[i].active)
			continue;

		unsigned int tex = arrowFlyTextures[arrows[i].frame % ARROW_FLY_FRAMES];

		if (tex != 0)
		{
			int w = arrows[i].shootingUp ? ARROW_HEIGHT : ARROW_WIDTH;
			int h = arrows[i].shootingUp ? ARROW_WIDTH : ARROW_HEIGHT;

			iShowImage((float)arrows[i].x, (float)arrows[i].y, w, h, tex);
		}
	}
}




inline void updatePlayerAnimation(Player &p)
{
	int desiredState;

	if (p.attacking)
		desiredState = PLAYER_ANIM_ATTACK;
	else if (p.shootingArrow)
		desiredState = PLAYER_ANIM_ARROW;
	else if (p.moving)
		desiredState = PLAYER_ANIM_WALK;
	else
		desiredState = PLAYER_ANIM_IDLE;

	if (desiredState != p.animState)
	{
		p.animState = desiredState;
		p.animFrame = 0;
		p.animTimer = 0;
		return;
	}

	p.animTimer++;

	if (p.animTimer < PLAYER_ANIM_FRAME_DELAY)
		return;

	p.animTimer = 0;

	switch (p.animState)
	{
	case PLAYER_ANIM_WALK:
		p.animFrame = (p.animFrame + 1) % PLAYER_WALK_FRAMES;
		break;

	case PLAYER_ANIM_ATTACK:
		if (p.animFrame < PLAYER_SLASH_FRAMES - 1)
			p.animFrame++;
		break;

	case PLAYER_ANIM_ARROW:
		if (p.animFrame < PLAYER_ARROW_FRAMES - 1)
			p.animFrame++;
		break;

	case PLAYER_ANIM_IDLE:
	default:
		p.animFrame = (p.animFrame + 1) % PLAYER_IDLE_FRAMES;
		break;
	}
}



inline void regenerateStamina(Player &p)
{
	static int timer = 0;

	if (!p.attacking && !p.dashing && !p.shootingArrow)
	{
		timer++;

		if (timer >= 4)
		{
			if (p.stamina < p.maxStamina)
			{
				p.stamina += STAMINA_REGEN_SPEED;

				if (p.stamina > p.maxStamina)
					p.stamina = p.maxStamina;
			}

			timer = 0;
		}
	}
	else
	{
		timer = 0;
	}
}




inline void damagePlayer(Player &p, int damage)
{
	if (p.invincibilityTimer > 0)
		return;

	p.health -= damage;

	if (p.health < 0)
		p.health = 0;

	p.invincibilityTimer = INVINCIBILITY_TIME;
}


inline void updatePlayerInvincibility(Player &p)
{
	if (p.invincibilityTimer > 0)
		p.invincibilityTimer--;
}


inline bool isPlayerDead(Player &p)
{
	return p.health <= 0;
}



inline bool isPlayerAttacking(Player &p)
{
	return p.attacking;
}

inline int getPlayerAttackDamage()
{
	return ATTACK_DAMAGE;
}

inline void getPlayerAttackHitbox(Player &p, int &hitX, int &hitY, int &hitWidth, int &hitHeight)
{
	hitWidth = PLAYER_ATTACK_RANGE + PLAYER_ATTACK_HITBOX_OVERLAP;
	hitHeight = 50;

	hitY = p.y + 25;

	if (p.facingRight)
	{
		hitX = p.x + PLAYER_WIDTH - PLAYER_ATTACK_HITBOX_OVERLAP;
	}
	else
	{
		hitX = p.x - PLAYER_ATTACK_RANGE;
	}
}

inline bool checkPlayerAttackCollision(Player &p, int enemyX, int enemyY, int enemyWidth, int enemyHeight)
{
	if (!p.attacking)
		return false;

	int hitX, hitY, hitWidth, hitHeight;
	getPlayerAttackHitbox(p, hitX, hitY, hitWidth, hitHeight);

	return (hitX < enemyX + enemyWidth &&
		hitX + hitWidth > enemyX &&
		hitY < enemyY + enemyHeight &&
		hitY + hitHeight > enemyY);
}


inline int getArrowDamage()
{
	return ARROW_DAMAGE;
}

inline bool checkArrowCollision(struct Arrow &arrow, int enemyX, int enemyY, int enemyWidth, int enemyHeight)
{
	
	{
		if (!arrow.active)
			return false;

		int aw = arrow.shootingUp ? ARROW_HEIGHT : ARROW_WIDTH;
		int ah = arrow.shootingUp ? ARROW_WIDTH : ARROW_HEIGHT;

		return (arrow.x < enemyX + enemyWidth &&
			arrow.x + aw > enemyX &&
			arrow.y < enemyY + enemyHeight &&
			arrow.y + ah > enemyY);
	}
}




inline void updatePlayer(Player &p, struct Midground *mg, struct Arrow arrows[], int groundY = GROUND_Y)
{
	updatePlayerMovement(p);

	playerJump(p);

	updatePlayerGravity(p, mg, groundY);

	playerDash(p);

	playerAttack(p);

	playerArrowAttack(p, arrows);

	updateArrows(arrows);

	updatePlayerAnimation(p);

	regenerateStamina(p);

	updatePlayerInvincibility(p);
}




inline void drawPlayer(Player &p, float screenX, float screenY)
{
	if (p.invincibilityTimer > 0)
	{
		if ((p.invincibilityTimer / 3) % 2 == 0)
			return;
	}

	unsigned int currentImage;

	if (p.animState == PLAYER_ANIM_ATTACK)
	{
		currentImage = p.facingRight
			? p.slashRight[p.animFrame]
			: p.slashLeft[p.animFrame];
	}
	else if (p.animState == PLAYER_ANIM_ARROW)
	{
		currentImage = p.facingRight
			? p.arrowRight[p.animFrame]
			: p.arrowLeft[p.animFrame];
	}
	else if (p.animState == PLAYER_ANIM_WALK)
	{
		currentImage = p.facingRight
			? p.walkRight[p.animFrame]
			: p.walkLeft[p.animFrame];
	}
	else
	{
		currentImage = p.idleImages[p.animFrame];
	}

	iShowImage(screenX, screenY, PLAYER_WIDTH, PLAYER_HEIGHT, currentImage);
}

#endif
