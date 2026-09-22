#ifndef FLYING_CREATURE_HPP
#define FLYING_CREATURE_HPP

#include "iGraphics.h"
#include "config.hpp"   // SCREEN_W / SCREEN_H -- keeps this creature's movement
                        // bounds in sync with the actual cave/game canvas instead
                        // of a hardcoded guess.
#include "player.hpp"   // Player struct + PLAYER_WIDTH / PLAYER_HEIGHT, so this
                        // class can take the real Player object directly instead
                        // of the caller unpacking x/y/w/h by hand.
#include <cstdio>
#include <cstdlib>

class FlyingCreature {
public:
	enum State {
		STATE_IDLE,
		STATE_FLY_HOVER,
		STATE_MOVE_RIGHT,
		STATE_MOVE_LEFT,
		STATE_ATTACK_SLASH,
		STATE_TAKE_DAMAGE,
		STATE_GROUND_IMPACT,
		STATE_DEATH_FALL,
		STATE_CORRODING_SPLAT
	};

private:
	static const int TOTAL_FRAMES = 7;
	// NOTE: don't name these SCREEN_WIDTH/SCREEN_HEIGHT -- player.hpp
	// #defines those macros (1200/800), and since player.hpp is now
	// included above, that name would get text-replaced right here and
	// break this declaration. Use config.hpp's SCREEN_W/SCREEN_H (the
	// real cave canvas size, 1000x600) directly instead.

	State currentState;
	int frameIndex;
	int xPos;
	int yPos;
	int speed;
	int spriteWidth;
	int spriteHeight;
	int facingDirection; // 1 = Right, -1 = Left

	// --- Melee / splat damage tuning ---
	int slashReach;            // how far the slash hitbox extends beyond the sprite, in facingDirection
	int slashActiveFrameStart; // first frame index where the slash can actually connect
	int slashActiveFrameEnd;   // last frame index where the slash can actually connect
	bool hasDealtSlashDamage;  // makes sure one slash only hits once

	int splatRadius;           // "near it" radius for the corroding splat, measured center-to-center
	int splatDamageFrame;      // the frame at which the splat actually deals damage
	bool hasDealtSplatDamage;  // makes sure one splat only hits once

	// --- Health / removal bookkeeping ---
	int hp;
	int maxHp;
	bool isDead;
	int deathHoldTimer;       // frames to linger on the death pose before isReadyToRemove() fires
	static const int DEATH_HOLD_FRAMES = 45;
	int attackRange;          // how close (center-to-center-ish) the AI needs to be to start a slash

	// Asset paths
	char idleImages[TOTAL_FRAMES][60];
	char flyImages[TOTAL_FRAMES][60];
	char moveRightImages[TOTAL_FRAMES][60];
	char moveLeftImages[TOTAL_FRAMES][60];
	char slashImages[TOTAL_FRAMES][60];
	char damageImages[TOTAL_FRAMES][60];
	char impactImages[TOTAL_FRAMES][60];
	char deathImages[TOTAL_FRAMES][60];
	char splatImages[TOTAL_FRAMES][60];

	// Loaded GL texture handles, one per frame per animation, filled in by
	// loadTextures() below. draw() uses these via iShowImage() -- NOT
	// iShowBMP2() -- because iShowBMP2() calls auxDIBImageLoad(), which
	// only understands BMP files. Every asset here is a PNG, so calling
	// it on one returns a null AUX_RGBImageRec* that iShowBMP2() then
	// dereferences immediately, crashing with an access violation the
	// instant draw() runs (0xC0000005 reading 0x00000000).
	unsigned int idleTex[TOTAL_FRAMES];
	unsigned int flyTex[TOTAL_FRAMES];
	unsigned int moveRightTex[TOTAL_FRAMES];
	unsigned int moveLeftTex[TOTAL_FRAMES];
	unsigned int slashTex[TOTAL_FRAMES];
	unsigned int damageTex[TOTAL_FRAMES];
	unsigned int impactTex[TOTAL_FRAMES];
	unsigned int deathTex[TOTAL_FRAMES];
	unsigned int splatTex[TOTAL_FRAMES];

public:
	FlyingCreature(int startX = 400, int startY = 350, int startHp = 3)
		: currentState(STATE_FLY_HOVER), frameIndex(0), xPos(startX), yPos(startY),
		speed(5), spriteWidth(160), spriteHeight(160), facingDirection(1),
		slashReach(80), slashActiveFrameStart(3), slashActiveFrameEnd(5), hasDealtSlashDamage(false),
		splatRadius(120), splatDamageFrame(TOTAL_FRAMES - 1), hasDealtSplatDamage(false),
		hp(startHp), maxHp(startHp), isDead(false), deathHoldTimer(0), attackRange(100)
	{
		initImagePaths();
		loadTextures();
	}

	void initImagePaths() {
		// Base folder matches the actual project layout: Assets/Cave/flyingCreature.
		const char* base = "Assets/Cave/flyingCreature";

		for (int i = 0; i < TOTAL_FRAMES; i++) {
			// fly_1.png ... fly_7.png -> flat files, regular naming
			sprintf_s(flyImages[i], sizeof(flyImages[i]), "%s/fly_%d.png", base, i + 1);

			// move_left_1.png ... move_left_7.png -> flat files, regular naming
			sprintf_s(moveLeftImages[i], sizeof(moveLeftImages[i]), "%s/move_left_%d.png", base, i + 1);

			// Attack Slash/1.png ... 7.png
			sprintf_s(slashImages[i], sizeof(slashImages[i]), "%s/Attack Slash/%d.png", base, i + 1);

			// Take Damage/1.png ... 7.png
			sprintf_s(damageImages[i], sizeof(damageImages[i]), "%s/Take Damage/%d.png", base, i + 1);

			// Ground Impact/1.png ... 7.png
			sprintf_s(impactImages[i], sizeof(impactImages[i]), "%s/Ground Impact/%d.png", base, i + 1);

			// Death/1.png ... 7.png
			sprintf_s(deathImages[i], sizeof(deathImages[i]), "%s/Death/%d.png", base, i + 1);

			// Corroding Splat/1.png ... 7.png
			sprintf_s(splatImages[i], sizeof(splatImages[i]), "%s/Corroding Splat/%d.png", base, i + 1);
		}

		// idle_*.png -> naming is NOT regular in the zip: frames 1, 4, and 5
		// are saved as "idle_1 .png", "idle_4 .png", "idle_5 .png" (stray
		// space before the extension). Hardcoded to match the real files.
		sprintf_s(idleImages[0], sizeof(idleImages[0]), "%s/idle_1 .png", base);
		sprintf_s(idleImages[1], sizeof(idleImages[1]), "%s/idle_2.png", base);
		sprintf_s(idleImages[2], sizeof(idleImages[2]), "%s/idle_3.png", base);
		sprintf_s(idleImages[3], sizeof(idleImages[3]), "%s/idle_4 .png", base);
		sprintf_s(idleImages[4], sizeof(idleImages[4]), "%s/idle_5 .png", base);
		sprintf_s(idleImages[5], sizeof(idleImages[5]), "%s/idle_6.png", base);
		sprintf_s(idleImages[6], sizeof(idleImages[6]), "%s/idle_7.png", base);

		// Move Right/1.png, 2.png, 3.png, 4.png, 6.png, 7.png -> "5.png" is
		// MISSING from the zip. Frame index 4 (i.e. would-be "5.png") falls
		// back to "4.png" so the animation doesn't break. Replace this once
		// the missing asset is added.
		sprintf_s(moveRightImages[0], sizeof(moveRightImages[0]), "%s/Move Right/1.png", base);
		sprintf_s(moveRightImages[1], sizeof(moveRightImages[1]), "%s/Move Right/2.png", base);
		sprintf_s(moveRightImages[2], sizeof(moveRightImages[2]), "%s/Move Right/3.png", base);
		sprintf_s(moveRightImages[3], sizeof(moveRightImages[3]), "%s/Move Right/4.png", base);
		sprintf_s(moveRightImages[4], sizeof(moveRightImages[4]), "%s/Move Right/4.png", base); // fallback: 5.png missing
		sprintf_s(moveRightImages[5], sizeof(moveRightImages[5]), "%s/Move Right/6.png", base);
		sprintf_s(moveRightImages[6], sizeof(moveRightImages[6]), "%s/Move Right/7.png", base);
	}

	// Loads every path built by initImagePaths() into an actual GL texture
	// via iLoadImage() (stb_image-based, handles PNG fine -- unlike
	// iShowBMP2()/auxDIBImageLoad(), which only reads BMP). Call once,
	// right after initImagePaths(); see the constructor.
	void loadTextures() {
		for (int i = 0; i < TOTAL_FRAMES; i++) {
			idleTex[i]      = iLoadImage(idleImages[i]);
			flyTex[i]       = iLoadImage(flyImages[i]);
			moveRightTex[i] = iLoadImage(moveRightImages[i]);
			moveLeftTex[i]  = iLoadImage(moveLeftImages[i]);
			slashTex[i]     = iLoadImage(slashImages[i]);
			damageTex[i]    = iLoadImage(damageImages[i]);
			impactTex[i]    = iLoadImage(impactImages[i]);
			deathTex[i]     = iLoadImage(deathImages[i]);
			splatTex[i]     = iLoadImage(splatImages[i]);
		}
	}

	void updateAnimation() {
		int previousFrame = frameIndex;
		frameIndex = (frameIndex + 1) % TOTAL_FRAMES;

		if (currentState == STATE_MOVE_RIGHT) {
			xPos += speed;
			if (xPos > SCREEN_W - spriteWidth) xPos = SCREEN_W - spriteWidth;
		}
		else if (currentState == STATE_MOVE_LEFT) {
			xPos -= speed;
			if (xPos < 0) xPos = 0;
		}

		// Terminal poses hold on their last frame instead of looping.
		if ((currentState == STATE_DEATH_FALL || currentState == STATE_CORRODING_SPLAT || currentState == STATE_GROUND_IMPACT)
			&& previousFrame == TOTAL_FRAMES - 1) {
			frameIndex = TOTAL_FRAMES - 1;

			if (currentState == STATE_DEATH_FALL && deathHoldTimer > 0) {
				deathHoldTimer--;
			}
			return;
		}

		// One-shot reaction animations return to hovering once they finish playing.
		if ((currentState == STATE_ATTACK_SLASH || currentState == STATE_TAKE_DAMAGE)
			&& previousFrame == TOTAL_FRAMES - 1) {
			setState(STATE_FLY_HOVER);
		}
	}

	// -----------------------------------------------------------------
	// AI: call once per frame (before updateAnimation) while the creature
	// is alive. Flies toward the player and, once in range, plays the
	// slash attack; steps out of the way of anything already playing an
	// attack/hit/death animation so those aren't interrupted mid-swing.
	// -----------------------------------------------------------------
	void updateAI(Player &player) {
		if (isDead) return;
		if (currentState == STATE_ATTACK_SLASH || currentState == STATE_TAKE_DAMAGE
			|| currentState == STATE_GROUND_IMPACT || currentState == STATE_CORRODING_SPLAT
			|| currentState == STATE_DEATH_FALL) {
			return;
		}

		int dx = player.x - xPos;
		int absDx = (dx < 0) ? -dx : dx;
		int newFacing = (dx >= 0) ? 1 : -1;

		if (absDx <= attackRange) {
			facingDirection = newFacing;
			if (currentState != STATE_ATTACK_SLASH) {
				setState(STATE_ATTACK_SLASH);
			}
		}
		else {
			facingDirection = newFacing;
			State desired = (dx >= 0) ? STATE_MOVE_RIGHT : STATE_MOVE_LEFT;
			if (currentState != desired) {
				setState(desired);
			}
		}
	}

	// -----------------------------------------------------------------
	// Damage / removal bookkeeping. hasDealtSlashDamage / hasDealtSplatDamage
	// above are about the creature's own attacks hitting the player;
	// takeDamage()/isReadyToRemove() are about the PLAYER'S attack hitting
	// this creature, which is what cave.hpp needs to retire it from the wave.
	// -----------------------------------------------------------------
	void takeDamage(int amount) {
		if (isDead) return;

		hp -= amount;
		if (hp <= 0) {
			hp = 0;
			isDead = true;
			deathHoldTimer = DEATH_HOLD_FRAMES;
			setState(STATE_DEATH_FALL);
		}
		else {
			setState(STATE_TAKE_DAMAGE);
		}
	}

	bool isReadyToRemove() const {
		return isDead && currentState == STATE_DEATH_FALL && deathHoldTimer <= 0;
	}

	int getHp() const { return hp; }
	int getMaxHp() const { return maxHp; }
	bool getIsDead() const { return isDead; }
	int getX() const { return xPos; }
	int getY() const { return yPos; }
	int getWidth() const { return spriteWidth; }
	int getHeight() const { return spriteHeight; }

	// -----------------------------------------------------------------
	// Melee damage: call once per frame with the player's hitbox while
	// the creature is in STATE_ATTACK_SLASH. Only connects during the
	// "active" swing frames (slashActiveFrameStart..slashActiveFrameEnd),
	// and only once per attack (hasDealtSlashDamage guards that). The
	// hitbox itself is the creature's sprite box extended by slashReach
	// pixels in the direction it's facing, so it behaves like a short
	// melee swing rather than a full-body collision.
	// -----------------------------------------------------------------
	bool checkSlashHitsPlayer(Player &player) {
		if (currentState != STATE_ATTACK_SLASH || hasDealtSlashDamage) {
			return false;
		}
		if (frameIndex < slashActiveFrameStart || frameIndex > slashActiveFrameEnd) {
			return false;
		}

		int hitboxX = xPos;
		int hitboxWidth = spriteWidth;
		if (facingDirection == 1) {
			hitboxWidth += slashReach;
		}
		else {
			hitboxX -= slashReach;
			hitboxWidth += slashReach;
		}

		bool overlapX = (hitboxX < player.x + PLAYER_WIDTH) && (hitboxX + hitboxWidth > player.x);
		bool overlapY = (yPos < player.y + PLAYER_HEIGHT) && (yPos + spriteHeight > player.y);

		if (overlapX && overlapY) {
			hasDealtSlashDamage = true;
			return true;
		}
		return false;
	}

	// -----------------------------------------------------------------
	// Splat damage: call once per frame with the player's position while
	// the creature is in STATE_CORRODING_SPLAT. Unlike the slash, this is
	// a proximity check ("if it's near it") using center-to-center
	// distance against splatRadius, rather than a tight AABB -- the splat
	// spreads outward, so anything within range when it goes off gets
	// hit. Only fires once, on splatDamageFrame (defaults to the last
	// frame of the animation, i.e. the moment it fully splats).
	// -----------------------------------------------------------------
	bool checkSplatHitsPlayer(Player &player) {
		if (currentState != STATE_CORRODING_SPLAT || hasDealtSplatDamage) {
			return false;
		}
		if (frameIndex != splatDamageFrame) {
			return false;
		}

		int creatureCenterX = xPos + spriteWidth / 2;
		int creatureCenterY = yPos + spriteHeight / 2;
		int playerCenterX = player.x + PLAYER_WIDTH / 2;
		int playerCenterY = player.y + PLAYER_HEIGHT / 2;

		int dx = playerCenterX - creatureCenterX;
		int dy = playerCenterY - creatureCenterY;
		int distSquared = dx * dx + dy * dy;

		if (distSquared <= splatRadius * splatRadius) {
			hasDealtSplatDamage = true;
			return true;
		}
		return false;
	}

	void draw() {
		unsigned int currentTex;

		switch (currentState) {
		case STATE_IDLE:            currentTex = idleTex[frameIndex]; break;
		case STATE_FLY_HOVER:       currentTex = flyTex[frameIndex]; break;
		case STATE_MOVE_RIGHT:      currentTex = moveRightTex[frameIndex]; break;
		case STATE_MOVE_LEFT:       currentTex = moveLeftTex[frameIndex]; break;
		case STATE_ATTACK_SLASH:    currentTex = slashTex[frameIndex]; break;
		case STATE_TAKE_DAMAGE:     currentTex = damageTex[frameIndex]; break;
		case STATE_GROUND_IMPACT:   currentTex = impactTex[frameIndex]; break;
		case STATE_DEATH_FALL:      currentTex = deathTex[frameIndex]; break;
		case STATE_CORRODING_SPLAT: currentTex = splatTex[frameIndex]; break;
		default:                    currentTex = flyTex[frameIndex]; break;
		}

		iShowImage(xPos, yPos, spriteWidth, spriteHeight, currentTex);
	}

	void handleKeyboard(unsigned char key) {
		if (key == 'i' || key == 'I') {
			setState(STATE_FLY_HOVER);
		}
		else if (key == 'a' || key == 'A') {
			setState(STATE_ATTACK_SLASH);
		}
		else if (key == 'd' || key == 'D') {
			setState(STATE_TAKE_DAMAGE);
		}
		else if (key == 'g' || key == 'G') {
			setState(STATE_GROUND_IMPACT);
		}
		else if (key == 'x' || key == 'X') {
			setState(STATE_DEATH_FALL);
		}
		else if (key == 'c' || key == 'C') {
			setState(STATE_CORRODING_SPLAT);
		}
	}

	void handleSpecialKeyboard(unsigned char key) {
		if (key == GLUT_KEY_RIGHT) {
			setState(STATE_MOVE_RIGHT);
			facingDirection = 1;
		}
		else if (key == GLUT_KEY_LEFT) {
			setState(STATE_MOVE_LEFT);
			facingDirection = -1;
		}
		else if (key == GLUT_KEY_UP) {
			yPos += speed;
			if (yPos > SCREEN_H - spriteHeight) yPos = SCREEN_H - spriteHeight;
		}
		else if (key == GLUT_KEY_DOWN) {
			yPos -= speed;
			if (yPos < 0) yPos = 0;
		}
	}

	void setState(State newState) {
		currentState = newState;
		frameIndex = 0;

		// Reset the "already dealt damage" guards whenever we (re-)enter
		// the attack states, so each new slash/splat can hit again.
		if (newState == STATE_ATTACK_SLASH) {
			hasDealtSlashDamage = false;
		}
		if (newState == STATE_CORRODING_SPLAT) {
			hasDealtSplatDamage = false;
		}
	}

	// Optional tuning hooks if you want to expose these from outside
	void setSlashReach(int reach) { slashReach = reach; }
	void setSlashActiveFrames(int startFrame, int endFrame) { slashActiveFrameStart = startFrame; slashActiveFrameEnd = endFrame; }
	void setSplatRadius(int radius) { splatRadius = radius; }
	void setSplatDamageFrame(int frame) { splatDamageFrame = frame; }
	State getState() const { return currentState; }
};

#endif