#ifndef BUG_HPP
#define BUG_HPP


#include "camera.hpp"
#include "config.hpp"
#include "iGraphics.h"
#include "player.hpp"
#include "sounds.hpp"
#include "structs.hpp"
#include "textures.hpp"
#include <math.h>
#include "inventory.hpp"

void initCreatures(struct Creature creatures[]) {
  for (int i = 0; i < MAX_CREATURES; i++) {
    creatures[i].active = 0;
    creatures[i].state = CREATURE_INACTIVE;
    creatures[i].isAggro = 0;
  }
}

void spawnCreature(struct Creature *creature, int x, int y) {
  creature->x = x;
  creature->y = y;
  creature->vx = 0;
  creature->vy = 0;
  creature->frame = 0;
  creature->active = 1;
  creature->state = CREATURE_LOADING;
  creature->patrolStartX = x;
  creature->subStateTimer = 0;
  creature->animationTimer = 0;
  creature->facingRight = 1;
  creature->maxHealth = BUG_MAX_HEALTH;
  creature->currentHealth = BUG_MAX_HEALTH;
  creature->invincibilityTimer = 0;
  creature->damageAnimTimer = 0;
  creature->damageFrame = 0;
  creature->isAggro = 0;
  creature->attackCooldown = 0;
}

int checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2,
                   int h2) {
  return !(x1 + w1 < x2 || x2 + w2 < x1 || y1 + h1 < y2 || y2 + h2 < y1);
}

void updateCreatures(struct Creature creatures[], Player *player, int gameState, struct Arrow arrows[]) {

  for (int i = 0; i < MAX_CREATURES; i++) {
    if (!creatures[i].active && creatures[i].state == CREATURE_INACTIVE) {
      int spawnX = -1;
      int spawnY = SPAWN_POINT_Y;

      if (gameState == PLAYING_STATE) {
        if (i == 0)
          spawnX = SPAWN_POINT_1_X;
        else if (i == 1)
          spawnX = SPAWN_POINT_2_X;
        else if (i == 2)
          spawnX = SPAWN_POINT_3_X;
      } else if (gameState == LEVEL2_STATE) {
        if (i == 0) {
          spawnX = 2000;
          spawnY = GROUND_Y + 100;
        } else if (i == 1) {
          spawnX = 4325;
          spawnY = GROUND_Y + 250;
        }
      } else if (gameState == LEVEL3_STATE) {
        if (i == 0) {
          spawnX = SPAWN_POINT_L3_1_X;
          spawnY = GROUND_Y + 100;
        } else if (i == 1) {
          spawnX = SPAWN_POINT_L3_2_X;
          spawnY = GROUND_Y + 150;
        } else if (i == 2) {
          spawnX = SPAWN_POINT_L3_3_X;
          spawnY = SPAWN_POINT_Y;
        }
      }

      if (spawnX != -1) {
        int dx = player->x - spawnX;
        int dy = player->y - spawnY;
        float spawnDistance = sqrt((double)(dx * dx + dy * dy));

        if (spawnDistance <= CREATURE_SPAWN_TRIGGER) {
          spawnCreature(&creatures[i], spawnX, spawnY);
        }
      }
    }
  }

  for (int i = 0; i < MAX_CREATURES; i++) {
    if (!creatures[i].active)
      continue;

    struct Creature *enemy = &creatures[i];
    int dx = player->x - enemy->x;
    int dy = player->y - enemy->y;
    float distance = sqrt((double)(dx * dx + dy * dy));
    enemy->subStateTimer++;
    enemy->animationTimer++;
    if (enemy->attackCooldown > 0)
      enemy->attackCooldown--;

  
    if (!enemy->isAggro && distance <= CREATURE_DETECTION_RANGE) {
      enemy->isAggro = 1;
    } else if (enemy->isAggro && distance > CREATURE_DETECTION_RANGE + AGGRO_LOSE_BUFFER) {
      enemy->isAggro = 0;
    }

    switch (enemy->state) {
    case CREATURE_LOADING:
      if (enemy->subStateTimer >= CREATURE_NORMAL_FRAMES * 20) {
        enemy->state = CREATURE_RISING;
        enemy->subStateTimer = 0;
        enemy->vy = -CREATURE_SPEED;
      }
      break;

    case CREATURE_RISING:
      enemy->y += enemy->vy;
      if (enemy->y <= 150) {
        enemy->state = CREATURE_PATROL_RIGHT;
        enemy->subStateTimer = 0;
        enemy->vx = CREATURE_SPEED;
        enemy->vy = 0;
        enemy->facingRight = 1;
      }
      break;

    case CREATURE_PATROL_RIGHT:
      enemy->x += enemy->vx;
      enemy->y = 150;
      if (enemy->x >= enemy->patrolStartX + CREATURE_PATROL_DISTANCE) {
        enemy->state = CREATURE_TURNING;
        enemy->subStateTimer = 0;
      }
      break;

    case CREATURE_PATROL_LEFT:
      enemy->x += enemy->vx;
      enemy->y = 150;
      if (enemy->x <= enemy->patrolStartX - CREATURE_PATROL_DISTANCE) {
        enemy->state = CREATURE_TURNING;
        enemy->subStateTimer = 0;
      }
      break;

    case CREATURE_TURNING:
      enemy->vx = 0;
      enemy->vy = 0;

      if (enemy->subStateTimer >= CREATURE_TURN_FRAMES * 15) {
        if (enemy->facingRight) {
          enemy->state = CREATURE_PATROL_LEFT;
          enemy->vx = -CREATURE_SPEED;
          enemy->facingRight = 0;
        } else {
          enemy->state = CREATURE_PATROL_RIGHT;
          enemy->vx = CREATURE_SPEED;
          enemy->facingRight = 1;
        }
        enemy->subStateTimer = 0;
      }
      break;

    case CREATURE_CHASING:
      
      if (dx > 5) { enemy->vx = CREATURE_SPEED + 1; enemy->facingRight = 1; }
      else if (dx < -5) { enemy->vx = -(CREATURE_SPEED + 1); enemy->facingRight = 0; }
      else enemy->vx = 0;

      if (dy > 5) enemy->vy = (CREATURE_SPEED + 1) / 2;
      else if (dy < -5) enemy->vy = -(CREATURE_SPEED + 1) / 2;
      else enemy->vy = 0;

      enemy->x += enemy->vx;
      enemy->y += enemy->vy;

      if (!enemy->isAggro) {
       
        enemy->state = (enemy->x >= enemy->patrolStartX) ? CREATURE_PATROL_LEFT : CREATURE_PATROL_RIGHT;
        enemy->subStateTimer = 0;
      }
      break;

    case CREATURE_ATTACKING:
     
      if (dx > 0) enemy->vx = CREATURE_ATTACK_SPEED;
      else if (dx < 0) enemy->vx = -CREATURE_ATTACK_SPEED;
      else enemy->vx = 0;
      if (dy > 0) enemy->vy = CREATURE_ATTACK_SPEED / 2;
      else if (dy < 0) enemy->vy = -CREATURE_ATTACK_SPEED / 2;
      else enemy->vy = 0;

      enemy->x += enemy->vx;
      enemy->y += enemy->vy;

      if (enemy->subStateTimer >= 30) {
        enemy->state = enemy->isAggro ? CREATURE_CHASING : (enemy->facingRight ? CREATURE_PATROL_RIGHT : CREATURE_PATROL_LEFT);
        enemy->subStateTimer = 0;
        enemy->attackCooldown = CREATURE_ATTACK_COOLDOWN;
      }
      break;

    case CREATURE_DEAD:
      if (enemy->subStateTimer >= CREATURE_BURST_FRAMES * 20) {
        enemy->active = 0;
      }
      break;

    default:
      break;
    }

   
    if (enemy->isAggro &&
        (enemy->state == CREATURE_PATROL_LEFT || enemy->state == CREATURE_PATROL_RIGHT ||
         enemy->state == CREATURE_TURNING)) {
      enemy->state = CREATURE_CHASING;
      enemy->subStateTimer = 0;
    }

    if (enemy->state == CREATURE_CHASING && distance <= CREATURE_ATTACK_RANGE &&
        enemy->attackCooldown <= 0) {
      enemy->state = CREATURE_ATTACKING;
      enemy->subStateTimer = 0;
    }

    if (enemy->animationTimer >= 8) {
      enemy->animationTimer = 0;

      switch (enemy->state) {
      case CREATURE_LOADING:
        enemy->frame = (enemy->frame + 1) % CREATURE_NORMAL_FRAMES;
        break;
      case CREATURE_RISING:
        enemy->frame = (enemy->frame + 1) % CREATURE_FLYL_FRAMES;
        break;
      case CREATURE_PATROL_LEFT:
      case CREATURE_CHASING:
        enemy->frame = (enemy->frame + 1) % (enemy->facingRight ? CREATURE_FLYR_FRAMES : CREATURE_FLYL_FRAMES);
        break;
      case CREATURE_PATROL_RIGHT:
        enemy->frame = (enemy->frame + 1) % CREATURE_FLYR_FRAMES;
        break;
      case CREATURE_TURNING:
        enemy->frame = (enemy->frame + 1) % CREATURE_TURN_FRAMES;
        break;
      case CREATURE_ATTACKING:
        enemy->frame = (enemy->frame + 1) % CREATURE_ATTACK_FRAMES;
        break;
      case CREATURE_DEAD:
        enemy->frame = (enemy->frame + 1) % CREATURE_BURST_FRAMES;
        break;
      default:
        break;
      }
    }

    // Enemy touches player 
    if (enemy->state != CREATURE_DEAD &&
        checkCollision(player->x + 15, player->y + 15, 80, 80, enemy->x + 16,
                       enemy->y + 16, 32, 32)) {

      int damage = (enemy->state == CREATURE_ATTACKING) ? DAMAGE_PLAYER_TAKES : DAMAGE_PLAYER_TAKES / 2;
      damagePlayer(*player, damage);
    }

    //  Player's sword hits enemy 
    if (checkPlayerAttackCollision(*player, enemy->x, enemy->y, CREATURE_SIZE, CREATURE_SIZE)
        && enemy->state != CREATURE_DEAD) {

      if (enemy->invincibilityTimer == 0) {
        int damage = getPlayerAttackDamage();
        enemy->currentHealth -= damage;
        enemy->invincibilityTimer = 90;

        if (enemy->currentHealth <= 0) {
          enemy->currentHealth = 0;
          enemy->state = CREATURE_DEAD;
          enemy->subStateTimer = 0;
          enemy->animationTimer = 0;
          enemy->frame = 0;
          enemy->vx = 0;
          enemy->vy = 0;
          playEnemyKillSound();
        } else {
          enemy->damageAnimTimer = CREATURE_DAMAGE_FRAMES * 4;
          enemy->damageFrame = 0;
        }
      }
    }

    // Player's arrow hits enemy
    if (enemy->state != CREATURE_DEAD && enemy->invincibilityTimer == 0) {
      for (int a = 0; a < MAX_ARROWS; a++) {
        if (checkArrowCollision(arrows[a], enemy->x, enemy->y, CREATURE_SIZE, CREATURE_SIZE)) {
          int damage = getArrowDamage();
          enemy->currentHealth -= damage;
          enemy->invincibilityTimer = 90;
          arrows[a].active = false;

          if (enemy->currentHealth <= 0) {
            enemy->currentHealth = 0;
            enemy->state = CREATURE_DEAD;
            enemy->subStateTimer = 0;
            enemy->animationTimer = 0;
            enemy->frame = 0;
            enemy->vx = 0;
            enemy->vy = 0;
            playEnemyKillSound();
          } else {
            enemy->damageAnimTimer = CREATURE_DAMAGE_FRAMES * 4;
            enemy->damageFrame = 0;
          }
          break;
        }
      }
    }

    if (enemy->invincibilityTimer > 0) {
      enemy->invincibilityTimer--;
    }

    if (enemy->damageAnimTimer > 0) {
      enemy->damageAnimTimer--;
      if (enemy->damageAnimTimer % 4 == 0 &&
          enemy->damageFrame < CREATURE_DAMAGE_FRAMES - 1) {
        enemy->damageFrame++;
      }
    }
  }
}

void renderCreatures(struct Creature creatures[], struct Camera *camera) {
  for (int i = 0; i < MAX_CREATURES; i++) {
    if (!creatures[i].active)
      continue;

    struct Creature *c = &creatures[i];
    unsigned int tex;

    switch (c->state) {
    case CREATURE_LOADING:
      tex = creatureNormal[c->frame % CREATURE_NORMAL_FRAMES];
      break;
    case CREATURE_RISING:
      tex = creatureFlyL[c->frame % CREATURE_FLYL_FRAMES];
      break;
    case CREATURE_PATROL_LEFT:
    case CREATURE_CHASING:
      tex = c->facingRight
        ? creatureFlyR[c->frame % CREATURE_FLYR_FRAMES]
        : creatureFlyL[c->frame % CREATURE_FLYL_FRAMES];
      break;
    case CREATURE_PATROL_RIGHT:
      tex = creatureFlyR[c->frame % CREATURE_FLYR_FRAMES];
      break;
    case CREATURE_TURNING:
      tex = creatureTurn[c->frame % CREATURE_TURN_FRAMES];
      break;
    case CREATURE_ATTACKING:
      tex = creatureAttack[c->frame % CREATURE_ATTACK_FRAMES];
      break;
    case CREATURE_DEAD:
      tex = creatureBurst[c->frame % CREATURE_BURST_FRAMES];
      break;
    default:
      tex = creatureNormal[0];
      break;
    }

    float screenX = getScreenX(c->x, camera);
    float screenY = getScreenY(c->y, camera);
    iShowImage(screenX, screenY, CREATURE_SIZE, CREATURE_SIZE, tex);

    if (c->damageAnimTimer > 0) {
      unsigned int dmgTex =
          creatureDamage[c->damageFrame % CREATURE_DAMAGE_FRAMES];
      if (dmgTex != 0) {
        iShowImage(screenX, screenY, CREATURE_SIZE, CREATURE_SIZE, dmgTex);
      }
    }

    if (c->state != CREATURE_DEAD && c->state != CREATURE_INACTIVE &&
        c->maxHealth > 0) {
      int barW = 40;
      int barH = 5;
      int barX = screenX + (CREATURE_SIZE - barW) / 2;
      int barY = screenY + CREATURE_SIZE + 5;
      int currHealth = c->currentHealth;
      if (currHealth < 0)
        currHealth = 0;
      int filled = (currHealth * barW) / c->maxHealth;

      iSetColor(80, 0, 0);
      iFilledRectangle(barX, barY, barW, barH);
      iSetColor(220, 30, 30);
      iFilledRectangle(barX, barY, filled, barH);
      iSetColor(255, 255, 255);
      iRectangle(barX, barY, barW, barH);
    }
  }
}

// ============================================================
// Cave bug wave integration
// ============================================================
enum BugState {
	BUG_EMERGE,
	BUG_RUN_CHARGE,
	BUG_SMASH_ATTACK,
	BUG_DEATH
};

struct Bug
{
	int x;
	int y;
	bool active;
	int hp;
	int maxHp;
	BugState state;
	int currentFrame;
	int frameTimer;

	// Prevents the player's single sword swing from registering as
	// several hits while it stays in the collision box.
	bool hitRegisteredThisSwing;

	// Prevents one smash attack from damaging the player on every frame
	// it's active.
	bool attackHitRegistered;
};

Bug bugs[CAVE_BUG_MAX];

// Wave-level bookkeeping for the "one at a time" spawn behaviour.
bool bugWaveStarted = false;
int bugCurrentIndex = 0;
int bugSpawnDelayTimer = 0;

unsigned int bugEmergeImg[CAVE_BUG_EMERGE_FRAMES];
unsigned int bugRunChargeImg[CAVE_BUG_RUN_CHARGE_FRAMES];
unsigned int bugSmashImg[CAVE_BUG_SMASH_FRAMES];
unsigned int bugDeathImg[CAVE_BUG_DEATH_FRAMES];
bool bugTexturesLoaded = false;

inline unsigned int bugLoadImg(const char* filename)
{
	char buf[200];
	strcpy_s(buf, filename);
	return iLoadImage(buf);
}

inline void loadBugTextures()
{
	if (bugTexturesLoaded) return;

	char path[150];
	for (int i = 0; i < CAVE_BUG_EMERGE_FRAMES; i++)
	{
		sprintf_s(path, "Assets/Cave/Bug/emerge/frame_%02d.png", i + 1);
		bugEmergeImg[i] = bugLoadImg(path);
	}

	for (int i = 0; i < CAVE_BUG_DEATH_FRAMES; i++)
	{
		sprintf_s(path, "Assets/Cave/Bug/death/frame%02d.png", i + 1);
		bugDeathImg[i] = bugLoadImg(path);
	}

	// Filenames in run_charge / smashAttack are irregular on disk,
	// so they are listed explicitly (unchanged from the source demo).
	const char* runChargeFiles[CAVE_BUG_RUN_CHARGE_FRAMES] = {
		"Assets/Cave/Bug/run_charge/runcharge1.png",
		"Assets/Cave/Bug/run_charge/run3.png",
		"Assets/Cave/Bug/run_charge/run4.png",
		"Assets/Cave/Bug/run_charge/run5.png",
		"Assets/Cave/Bug/run_charge/run6(1).png",
		"Assets/Cave/Bug/run_charge/run_charge7.png",
		"Assets/Cave/Bug/run_charge/run_charge8.png",
		"Assets/Cave/Bug/run_charge/run_charge9.png",
		"Assets/Cave/Bug/run_charge/run_charge10.png",
		"Assets/Cave/Bug/run_charge/run_charge11.png",
		"Assets/Cave/Bug/run_charge/run_charge12.png",
		"Assets/Cave/Bug/run_charge/run_charge13.png",
		"Assets/Cave/Bug/run_charge/run_charge14.png",
		"Assets/Cave/Bug/run_charge/run_charge15.png",
		"Assets/Cave/Bug/run_charge/run_charge16.png",
		"Assets/Cave/Bug/run_charge/run_charge17.png",
		"Assets/Cave/Bug/run_charge/run_charge18.png"
	};
	for (int i = 0; i < CAVE_BUG_RUN_CHARGE_FRAMES; i++)
		bugRunChargeImg[i] = bugLoadImg(runChargeFiles[i]);

	const char* smashFiles[CAVE_BUG_SMASH_FRAMES] = {
		"Assets/Cave/Bug/smashAttack/smash1.png",
		"Assets/Cave/Bug/smashAttack/shmash2.png",
		"Assets/Cave/Bug/smashAttack/shamsh3.png",
		"Assets/Cave/Bug/smashAttack/smash4.png",
		"Assets/Cave/Bug/smashAttack/shamsh5.png",
		"Assets/Cave/Bug/smashAttack/shmash6.png",
		"Assets/Cave/Bug/smashAttack/smash7.png",
		"Assets/Cave/Bug/smashAttack/smash8.png",
		"Assets/Cave/Bug/smashAttack/smash9.png"
	};
	for (int i = 0; i < CAVE_BUG_SMASH_FRAMES; i++)
		bugSmashImg[i] = bugLoadImg(smashFiles[i]);

	bugTexturesLoaded = true;
}

inline void initBugs()
{
	loadBugTextures();

	for (int i = 0; i < CAVE_BUG_MAX; i++)
	{
		bugs[i].active = false;
		bugs[i].hp = 0;
		bugs[i].maxHp = CAVE_BUG_MAX_HP;
		bugs[i].state = BUG_EMERGE;
		bugs[i].currentFrame = 0;
		bugs[i].frameTimer = 0;
		bugs[i].hitRegisteredThisSwing = false;
		bugs[i].attackHitRegistered = false;
	}

	bugWaveStarted = false;
	bugCurrentIndex = 0;
	bugSpawnDelayTimer = 0;
}

// Pick an X-only spawn point. This intentionally uses rand() rather than
// newer C++ random libraries so it remains compatible with Visual Studio 2013.
//
// Every call now returns a randomized point from CAVE_RANDOM_SPAWN_X1..X5
// (previously this only kicked in once playerX >= CAVE_RANDOM_SPAWN_TRIGGER_X,
// which meant the very first bug of a wave always spawned at the same fixed
// CAVE_BUG_SPAWN_X). If playerX is unknown/not yet meaningful (<= 0, e.g. the
// player hasn't entered the cave yet), we still randomize but skip the
// min-distance check since there's no real player position to avoid.
inline int getRandomCaveSpawnX(int playerX, int defaultX)
{
	int spawnPoints[CAVE_RANDOM_SPAWN_POINT_COUNT] = {
		CAVE_RANDOM_SPAWN_X1, CAVE_RANDOM_SPAWN_X2, CAVE_RANDOM_SPAWN_X3,
		CAVE_RANDOM_SPAWN_X4, CAVE_RANDOM_SPAWN_X5
	};

	if (playerX <= 0)
		return spawnPoints[rand() % CAVE_RANDOM_SPAWN_POINT_COUNT];

	// Try several times not to place an enemy directly on the player.
	for (int attempt = 0; attempt < CAVE_RANDOM_SPAWN_POINT_COUNT * 2; attempt++)
	{
		int candidate = spawnPoints[rand() % CAVE_RANDOM_SPAWN_POINT_COUNT];
		if (abs(candidate - playerX) >= CAVE_RANDOM_SPAWN_MIN_DISTANCE)
			return candidate;
	}

	return spawnPoints[rand() % CAVE_RANDOM_SPAWN_POINT_COUNT];
}

// Activates a single bug (bugs[index]) and resets its state.
// playerX is used to keep the spawn point away from wherever the player
// currently stands.
inline void spawnSingleBug(int index, int playerX)
{
	if (index < 0 || index >= CAVE_BUG_MAX) return;

	bugs[index].x = getRandomCaveSpawnX(playerX, CAVE_BUG_SPAWN_X);
	bugs[index].y = CAVE_BUG_SPAWN_Y;
	bugs[index].active = true;
	bugs[index].hp = CAVE_BUG_MAX_HP;
	bugs[index].maxHp = CAVE_BUG_MAX_HP;
	bugs[index].state = BUG_EMERGE;
	bugs[index].currentFrame = 0;
	bugs[index].frameTimer = 0;
	bugs[index].hitRegisteredThisSwing = false;
	bugs[index].attackHitRegistered = false;
}

// Starts the bug wave: only the FIRST bug appears immediately.
// The rest spawn one at a time as earlier ones are defeated
// (see updateBugs()). playerX is threaded through so even bug #1
// spawns at a randomized point relative to the player, instead of
// always at the same fixed CAVE_BUG_SPAWN_X.
inline void spawnBugWave(int playerX)
{
	if (bugWaveStarted) return;

	bugWaveStarted = true;
	bugCurrentIndex = 0;
	bugSpawnDelayTimer = 0;
	spawnSingleBug(bugCurrentIndex, playerX);
}

inline void damageBug(int index, int amount)
{
	if (index < 0 || index >= CAVE_BUG_MAX) return;
	if (!bugs[index].active || bugs[index].state == BUG_DEATH) return;

	bugs[index].hp -= amount;
	if (bugs[index].hp <= 0)
	{
		bugs[index].hp = 0;
		bugs[index].state = BUG_DEATH;
		bugs[index].currentFrame = 0;
		bugs[index].frameTimer = 0;
	}
}

inline void updateBugs(Player &player, struct Arrow arrows[])
{
	if (!bugWaveStarted) return;

	// Wave already fully cleared.
	if (bugCurrentIndex >= CAVE_BUG_MAX) return;

	Bug &b = bugs[bugCurrentIndex];

	// Waiting between the previous bug's death and the next spawn.
	if (!b.active)
	{
		if (bugSpawnDelayTimer > 0)
		{
			bugSpawnDelayTimer--;
			return;
		}

		bugCurrentIndex++;
		if (bugCurrentIndex < CAVE_BUG_MAX)
			spawnSingleBug(bugCurrentIndex, player.x);

		return;
	}

	// Let the player's sword damage this bug (once per swing).
	if (checkPlayerAttackCollision(player, b.x, b.y, CAVE_BUG_WIDTH, CAVE_BUG_HEIGHT))
	{
		if (!b.hitRegisteredThisSwing && b.state != BUG_DEATH)
		{
			damageBug(bugCurrentIndex, getPlayerAttackDamage());
			b.hitRegisteredThisSwing = true;
		}
	}
	else if (!player.attacking)
	{
		b.hitRegisteredThisSwing = false;
	}

	// Let the player's arrow damage this bug.
	if (b.state != BUG_DEATH)
	{
		for (int a = 0; a < MAX_ARROWS; a++)
		{
			if (checkArrowCollision(arrows[a], b.x, b.y, CAVE_BUG_WIDTH, CAVE_BUG_HEIGHT))
			{
				damageBug(bugCurrentIndex, getArrowDamage());
				arrows[a].active = false;
				break;
			}
		}
	}

	b.frameTimer++;
	if (b.frameTimer < CAVE_BUG_ANIM_SPEED) return;
	b.frameTimer = 0;

	switch (b.state)
	{
	case BUG_EMERGE:
		b.currentFrame++;
		if (b.currentFrame >= CAVE_BUG_EMERGE_FRAMES)
		{
			b.currentFrame = 0;
			b.state = BUG_RUN_CHARGE;
		}
		break;

	case BUG_RUN_CHARGE:
		if (b.x < player.x) b.x += CAVE_BUG_MOVE_SPEED;
		else if (b.x > player.x) b.x -= CAVE_BUG_MOVE_SPEED;

		b.currentFrame = (b.currentFrame + 1) % CAVE_BUG_RUN_CHARGE_FRAMES;

		if (abs(b.x - player.x) < CAVE_BUG_ATTACK_RANGE)
		{
			b.state = BUG_SMASH_ATTACK;
			b.currentFrame = 0;
			b.attackHitRegistered = false;
		}
		break;

	case BUG_SMASH_ATTACK:
		// Deal contact damage once per attack, partway through the swing.
		if (!b.attackHitRegistered && b.currentFrame >= CAVE_BUG_SMASH_FRAMES / 2
			&& abs(b.x - player.x) < CAVE_BUG_ATTACK_RANGE)
		{
			damagePlayer(player, CAVE_BUG_DAMAGE_TO_PLAYER);
			b.attackHitRegistered = true;
		}

		b.currentFrame++;
		if (b.currentFrame >= CAVE_BUG_SMASH_FRAMES)
		{
			b.currentFrame = 0;
			b.state = BUG_RUN_CHARGE;
		}
		break;

	case BUG_DEATH:
		b.currentFrame++;
		if (b.currentFrame >= CAVE_BUG_DEATH_FRAMES)
		{
			b.active = false;
			bugSpawnDelayTimer = CAVE_BUG_SPAWN_DELAY;
		}
		break;
	}
}

// True once every bug in the wave has been spawned AND defeated.
inline bool areAllBugsDefeated()
{
	if (!bugWaveStarted) return false;
	return bugCurrentIndex >= CAVE_BUG_MAX;
}

inline void drawBugs()
{
	if (!bugWaveStarted || bugCurrentIndex >= CAVE_BUG_MAX) return;

	Bug &b = bugs[bugCurrentIndex];
	if (!b.active) return;

	unsigned int img = 0;
	switch (b.state)
	{
	case BUG_EMERGE:
		img = bugEmergeImg[b.currentFrame % CAVE_BUG_EMERGE_FRAMES];
		break;
	case BUG_RUN_CHARGE:
		img = bugRunChargeImg[b.currentFrame % CAVE_BUG_RUN_CHARGE_FRAMES];
		break;
	case BUG_SMASH_ATTACK:
		img = bugSmashImg[b.currentFrame % CAVE_BUG_SMASH_FRAMES];
		break;
	case BUG_DEATH:
		img = bugDeathImg[b.currentFrame % CAVE_BUG_DEATH_FRAMES];
		break;
	}

	iShowImage(b.x, b.y, CAVE_BUG_WIDTH, CAVE_BUG_HEIGHT, img);

	if (b.state != BUG_DEATH)
	{
		iSetColor(200, 0, 0);
		iFilledRectangle(b.x, b.y + CAVE_BUG_HEIGHT + 8, CAVE_BUG_WIDTH, 6);
		iSetColor(0, 200, 0);
		iFilledRectangle(b.x, b.y + CAVE_BUG_HEIGHT + 8, CAVE_BUG_WIDTH * b.hp / b.maxHp, 6);
	}
}

#endif
