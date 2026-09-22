#ifndef LIGHTNING_THROWER_HPP
#define LIGHTNING_THROWER_HPP

#include "iGraphics.h"
#include "config.hpp"   // SCREEN_W / SCREEN_H -- real cave/game canvas size.
#include "player.hpp"   // Player struct + PLAYER_WIDTH / PLAYER_HEIGHT.
#include <cstdio>
#include <cstdlib>

class LightningThrower {
public:
	enum State {
		STATE_IDLE,
		STATE_WALK_RIGHT,
		STATE_WALK_LEFT,
		STATE_PREPARE,
		STATE_ATTACK,
		STATE_DAMAGE,
		STATE_DYING
	};

private:
	static const int TOTAL_FRAMES = 7;
	

	State currentState;
	int frameIndex;
	int xPos;
	int yPos;
	int speed;
	int spriteWidth;
	int spriteHeight;

	// Lightning Projectile
	bool isLightningActive;
	int lightningX;
	int lightningY;
	int lightningSpeed;
	int lightningWidth;
	int lightningHeight;
	int facingDirection; 

	// --- Long-range autonomous attack AI ---
	int detectionRange;     
	int approachRange;      
	int attackCooldown;    
	int cooldownTimer;      
	bool hasHitPlayerThisBolt; 

	// --- Health / removal bookkeeping ---
	int hp;
	int maxHp;
	bool isDead;
	int deathHoldTimer;      
	static const int DEATH_HOLD_FRAMES = 45;

	// Asset paths
	char idleImages[TOTAL_FRAMES][60];
	char walkRightImages[TOTAL_FRAMES][60];
	char walkLeftImages[TOTAL_FRAMES][60];
	char prepImages[TOTAL_FRAMES][60];
	char throwImages[TOTAL_FRAMES][60];
	char damageImages[TOTAL_FRAMES][60];
	char dyingImages[TOTAL_FRAMES][60];
	char lightningImage[60];

	 
	unsigned int idleTex[TOTAL_FRAMES];
	unsigned int walkRightTex[TOTAL_FRAMES];
	unsigned int walkLeftTex[TOTAL_FRAMES];
	unsigned int prepTex[TOTAL_FRAMES];
	unsigned int throwTex[TOTAL_FRAMES];
	unsigned int damageTex[TOTAL_FRAMES];
	unsigned int dyingTex[TOTAL_FRAMES];
	unsigned int lightningTex;

public:
	LightningThrower(int startX = 200, int startY = 200, int startHp = 4)
		: currentState(STATE_IDLE), frameIndex(0), xPos(startX), yPos(startY),
		speed(4), spriteWidth(150), spriteHeight(150), isLightningActive(false),
		lightningX(0), lightningY(0), lightningSpeed(15), lightningWidth(300),
		lightningHeight(80), facingDirection(1),
		detectionRange(SCREEN_W * 2), 
		approachRange(450),           
		attackCooldown(90),              
		cooldownTimer(0),
		hasHitPlayerThisBolt(false),
		hp(startHp), maxHp(startHp), isDead(false), deathHoldTimer(0)
	{
		// lightning.png lives at Assets/Cave/Lightner/lightning.png
		sprintf_s(lightningImage, sizeof(lightningImage), "Assets/Cave/Lightner/lightning.png");
		initImagePaths();
		loadTextures();
	}

	void initImagePaths() {
		// Base folder matches the actual project layout: Assets/Cave/Lightner.
		const char* base = "Assets/Cave/Lightner";

		// Damage/1.png ... 7.png -> complete set, regular naming
		for (int i = 0; i < TOTAL_FRAMES; i++) {
			sprintf_s(damageImages[i], sizeof(damageImages[i]), "%s/Damage/%d.png", base, i + 1);
		}

		// idle/1.png ... 6.png -> frame 7 is MISSING from the zip.
		sprintf_s(idleImages[0], sizeof(idleImages[0]), "%s/idle/1.png", base);
		sprintf_s(idleImages[1], sizeof(idleImages[1]), "%s/idle/2.png", base);
		sprintf_s(idleImages[2], sizeof(idleImages[2]), "%s/idle/3.png", base);
		sprintf_s(idleImages[3], sizeof(idleImages[3]), "%s/idle/4.png", base);
		sprintf_s(idleImages[4], sizeof(idleImages[4]), "%s/idle/5.png", base);
		sprintf_s(idleImages[5], sizeof(idleImages[5]), "%s/idle/6.png", base);
		sprintf_s(idleImages[6], sizeof(idleImages[6]), "%s/idle/6.png", base); // fallback: 7.png missing

		// walk_right/1.png ... 6.png -> frame 7 is MISSING from the zip.
		sprintf_s(walkRightImages[0], sizeof(walkRightImages[0]), "%s/walk_right/1.png", base);
		sprintf_s(walkRightImages[1], sizeof(walkRightImages[1]), "%s/walk_right/2.png", base);
		sprintf_s(walkRightImages[2], sizeof(walkRightImages[2]), "%s/walk_right/3.png", base);
		sprintf_s(walkRightImages[3], sizeof(walkRightImages[3]), "%s/walk_right/4.png", base);
		sprintf_s(walkRightImages[4], sizeof(walkRightImages[4]), "%s/walk_right/5.png", base);
		sprintf_s(walkRightImages[5], sizeof(walkRightImages[5]), "%s/walk_right/6.png", base);
		sprintf_s(walkRightImages[6], sizeof(walkRightImages[6]), "%s/walk_right/6.png", base); // fallback: 7.png missing

		// walk_left/walk_left1.png ... walk_left7.png -> complete, but
		// frame 4 is saved as "walk_left4 (1).png" (stray " (1)").
		sprintf_s(walkLeftImages[0], sizeof(walkLeftImages[0]), "%s/walk_left/walk_left1.png", base);
		sprintf_s(walkLeftImages[1], sizeof(walkLeftImages[1]), "%s/walk_left/walk_left2.png", base);
		sprintf_s(walkLeftImages[2], sizeof(walkLeftImages[2]), "%s/walk_left/walk_left3.png", base);
		sprintf_s(walkLeftImages[3], sizeof(walkLeftImages[3]), "%s/walk_left/walk_left4 (1).png", base);
		sprintf_s(walkLeftImages[4], sizeof(walkLeftImages[4]), "%s/walk_left/walk_left5.png", base);
		sprintf_s(walkLeftImages[5], sizeof(walkLeftImages[5]), "%s/walk_left/walk_left6.png", base);
		sprintf_s(walkLeftImages[6], sizeof(walkLeftImages[6]), "%s/walk_left/walk_left7.png", base);

		// prep/1 (1).png, 2.png ... 6.png -> frame 1 is saved as "1 (1).png",
		// and frame 7 is MISSING from the zip (falls back to frame 6).
		sprintf_s(prepImages[0], sizeof(prepImages[0]), "%s/prep/1 (1).png", base);
		sprintf_s(prepImages[1], sizeof(prepImages[1]), "%s/prep/2.png", base);
		sprintf_s(prepImages[2], sizeof(prepImages[2]), "%s/prep/3.png", base);
		sprintf_s(prepImages[3], sizeof(prepImages[3]), "%s/prep/4.png", base);
		sprintf_s(prepImages[4], sizeof(prepImages[4]), "%s/prep/5.png", base);
		sprintf_s(prepImages[5], sizeof(prepImages[5]), "%s/prep/6.png", base);
		sprintf_s(prepImages[6], sizeof(prepImages[6]), "%s/prep/6.png", base); // fallback: 7.png missing

		// throw/1 (1).png, 2.png ... 5.png -> frame 1 is saved as "1 (1).png",
		// and frames 6 & 7 are MISSING from the zip (only 5 frames exist).
		sprintf_s(throwImages[0], sizeof(throwImages[0]), "%s/throw/1 (1).png", base);
		sprintf_s(throwImages[1], sizeof(throwImages[1]), "%s/throw/2.png", base);
		sprintf_s(throwImages[2], sizeof(throwImages[2]), "%s/throw/3.png", base);
		sprintf_s(throwImages[3], sizeof(throwImages[3]), "%s/throw/4.png", base);
		sprintf_s(throwImages[4], sizeof(throwImages[4]), "%s/throw/5.png", base);
		sprintf_s(throwImages[5], sizeof(throwImages[5]), "%s/throw/5.png", base); // fallback: 6.png missing
		sprintf_s(throwImages[6], sizeof(throwImages[6]), "%s/throw/5.png", base); // fallback: 7.png missing

		// Death/1.png, 2.png, 4.png ... 7.png -> frame 3 is MISSING from
		// the zip. Frame index 2 falls back to frame 2's image.
		sprintf_s(dyingImages[0], sizeof(dyingImages[0]), "%s/Death/1.png", base);
		sprintf_s(dyingImages[1], sizeof(dyingImages[1]), "%s/Death/2.png", base);
		sprintf_s(dyingImages[2], sizeof(dyingImages[2]), "%s/Death/2.png", base); // fallback: 3.png missing
		sprintf_s(dyingImages[3], sizeof(dyingImages[3]), "%s/Death/4.png", base);
		sprintf_s(dyingImages[4], sizeof(dyingImages[4]), "%s/Death/5.png", base);
		sprintf_s(dyingImages[5], sizeof(dyingImages[5]), "%s/Death/6.png", base);
		sprintf_s(dyingImages[6], sizeof(dyingImages[6]), "%s/Death/7.png", base);
	}

	
	void loadTextures() {
		for (int i = 0; i < TOTAL_FRAMES; i++) {
			idleTex[i]      = iLoadImage(idleImages[i]);
			walkRightTex[i] = iLoadImage(walkRightImages[i]);
			walkLeftTex[i]  = iLoadImage(walkLeftImages[i]);
			prepTex[i]      = iLoadImage(prepImages[i]);
			throwTex[i]     = iLoadImage(throwImages[i]);
			damageTex[i]    = iLoadImage(damageImages[i]);
			dyingTex[i]     = iLoadImage(dyingImages[i]);
		}
		lightningTex = iLoadImage(lightningImage);
	}

	void updateAI(Player &player) {
		// Don't think/attack while dead, taking damage, or already mid-attack
		if (isDead || currentState == STATE_DYING || currentState == STATE_DAMAGE) {
			return;
		}

		if (cooldownTimer > 0) {
			cooldownTimer--;
		}

		int distance = player.x - xPos;
		int absDistance = (distance < 0) ? -distance : distance;

		bool playerInRange = (absDistance <= detectionRange);
		bool readyToAttack = (cooldownTimer == 0);
		bool idleEnough = (currentState == STATE_IDLE ||
			currentState == STATE_WALK_RIGHT ||
			currentState == STATE_WALK_LEFT);

		if (!playerInRange || !idleEnough) {
			return;
		}

		// Face the player before doing anything else.
		facingDirection = (distance >= 0) ? 1 : -1;

		if (absDistance > approachRange) {
			// Too far to throw -- charge toward the player instead.
			setState((distance >= 0) ? STATE_WALK_RIGHT : STATE_WALK_LEFT);
			return;
		}

		// Close enough: stop and throw once the cooldown allows it.
		if (readyToAttack) {
			setState(STATE_PREPARE);
			cooldownTimer = attackCooldown;
		}
		else if (currentState != STATE_IDLE) {
			setState(STATE_IDLE);
		}
	}

	void updateAnimation() {
		frameIndex = (frameIndex + 1) % TOTAL_FRAMES;

		if (currentState == STATE_WALK_RIGHT) {
			xPos += speed;
			if (xPos > SCREEN_W - spriteWidth) xPos = SCREEN_W - spriteWidth;
		}
		else if (currentState == STATE_WALK_LEFT) {
			xPos -= speed;
			if (xPos < 0) xPos = 0;
		}

		if (currentState == STATE_PREPARE && frameIndex == TOTAL_FRAMES - 1) {
			currentState = STATE_ATTACK;
			frameIndex = 0;

			isLightningActive = true;
			hasHitPlayerThisBolt = false;
			lightningY = yPos + 35;
			lightningX = (facingDirection == 1) ? (xPos + spriteWidth - 20) : (xPos - lightningWidth + 20);
		}

		// After the throw animation finishes, go back to idle so the AI
		// can walk/attack again once the cooldown expires.
		if (currentState == STATE_ATTACK && frameIndex == TOTAL_FRAMES - 1) {
			setState(STATE_IDLE);
		}

		// After a hit reaction finishes, go back to idle.
		if (currentState == STATE_DAMAGE && frameIndex == TOTAL_FRAMES - 1) {
			setState(STATE_IDLE);
		}

		if (currentState == STATE_DYING && frameIndex == TOTAL_FRAMES - 1) {
			frameIndex = TOTAL_FRAMES - 1;
			if (deathHoldTimer > 0) {
				deathHoldTimer--;
			}
		}
	}

	
	void takeDamage(int amount) {
		if (isDead) return;

		hp -= amount;
		if (hp <= 0) {
			hp = 0;
			isDead = true;
			deathHoldTimer = DEATH_HOLD_FRAMES;
			isLightningActive = false;
			setState(STATE_DYING);
		}
		else {
			setState(STATE_DAMAGE);
		}
	}

	bool isReadyToRemove() const {
		return isDead && currentState == STATE_DYING && deathHoldTimer <= 0;
	}

	int getHp() const { return hp; }
	int getMaxHp() const { return maxHp; }
	bool getIsDead() const { return isDead; }
	int getX() const { return xPos; }
	int getY() const { return yPos; }
	int getWidth() const { return spriteWidth; }
	int getHeight() const { return spriteHeight; }

	void updateProjectile() {
		if (isLightningActive) {
			lightningX += lightningSpeed * facingDirection;

			if (lightningX > SCREEN_W || lightningX + lightningWidth < 0) {
				isLightningActive = false;
			}
		}
	}


	bool checkLightningHitsPlayer(Player &player) {
		if (!isLightningActive || hasHitPlayerThisBolt) {
			return false;
		}

		bool overlapX = (lightningX < player.x + PLAYER_WIDTH) && (lightningX + lightningWidth > player.x);
		bool overlapY = (lightningY < player.y + PLAYER_HEIGHT) && (lightningY + lightningHeight > player.y);

		if (overlapX && overlapY) {
			hasHitPlayerThisBolt = true;
			isLightningActive = false; // bolt is consumed on hit
			return true;
		}

		return false;
	}

	void draw() {
		unsigned int currentTex;

		switch (currentState) {
		case STATE_IDLE:       currentTex = idleTex[frameIndex]; break;
		case STATE_WALK_RIGHT: currentTex = walkRightTex[frameIndex]; break;
		case STATE_WALK_LEFT:  currentTex = walkLeftTex[frameIndex]; break;
		case STATE_PREPARE:    currentTex = prepTex[frameIndex]; break;
		case STATE_ATTACK:     currentTex = throwTex[frameIndex]; break;
		case STATE_DAMAGE:     currentTex = damageTex[frameIndex]; break;
		case STATE_DYING:      currentTex = dyingTex[frameIndex]; break;
		default:               currentTex = idleTex[frameIndex]; break;
		}

		iShowImage(xPos, yPos, spriteWidth, spriteHeight, currentTex);

		if (isLightningActive) {
			iShowImage(lightningX, lightningY, lightningWidth, lightningHeight, lightningTex);
		}
	}

	void handleKeyboard(unsigned char key) {
		if (key == 'i' || key == 'I') {
			setState(STATE_IDLE);
		}
		else if (key == 'a' || key == 'A') {
			setState(STATE_PREPARE);
		}
		else if (key == 'd' || key == 'D') {
			setState(STATE_DAMAGE);
		}
		else if (key == 'x' || key == 'X') {
			setState(STATE_DYING);
		}
	}

	void handleSpecialKeyboard(unsigned char key) {
		if (key == GLUT_KEY_RIGHT) {
			setState(STATE_WALK_RIGHT);
			facingDirection = 1;
		}
		else if (key == GLUT_KEY_LEFT) {
			setState(STATE_WALK_LEFT);
			facingDirection = -1;
		}
	}

	void setState(State newState) {
		currentState = newState;
		frameIndex = 0;
	}

	// Optional tuning hooks if you want to expose these from outside
	void setDetectionRange(int range) { detectionRange = range; }
	void setApproachRange(int range) { approachRange = range; }
	void setAttackCooldown(int frames) { attackCooldown = frames; }
	State getState() const { return currentState; }
};

#endif