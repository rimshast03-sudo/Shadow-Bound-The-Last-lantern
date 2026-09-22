#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include "config.hpp"


struct Player;
struct Camera;
struct Midground;
struct Background;
struct Creature;
struct Sentry;
struct TitleScreen;


enum PlayerAnimState
{
	PLAYER_ANIM_IDLE,
	PLAYER_ANIM_WALK,
	PLAYER_ANIM_ATTACK,
	PLAYER_ANIM_ARROW
};

struct Player
{
	int x;
	int y;
	int stamina;
	int maxStamina;
	int staminaRegenTimer;

	int hasHeart;
	int hasPotion;
	int hasPower;
	int potionTimer;
	int damageMultiplier;
	int heartDuration;
	int heartRegenTimer;
	int hasGain;
	int gainTimer;
	int gainRegenTimer;
	int hasKey;
	int hasUsedKey;

	int vx;
	int vy;

	int health;
	int maxHealth;

	bool onGround;
	bool facingRight;

	bool attacking;
	bool dashing;
	bool shootingArrow;

	int attackTimer;
	int dashTimer;
	int arrowAttackTimer;

	int invincibilityTimer;

	bool moving;

	
	unsigned int walkRight[PLAYER_WALK_FRAMES];
	unsigned int walkLeft[PLAYER_WALK_FRAMES];
	unsigned int idleImages[PLAYER_IDLE_FRAMES];
	unsigned int slashRight[PLAYER_SLASH_FRAMES];
	unsigned int slashLeft[PLAYER_SLASH_FRAMES];
	unsigned int arrowRight[PLAYER_ARROW_FRAMES];
	unsigned int arrowLeft[PLAYER_ARROW_FRAMES];

	int animState;
	int animFrame;
	int animTimer;

	// Level 3 trader / boss progression. These fields do not alter
	// the existing two player attacks (slash + arrow).
	int fragments;
	int hasSwiftness;
	int hasSoul;
	int hasKeyItem;
	int swiftnessUsed;
	int soulUsed;
	int keyUsed;
	int swiftnessActive;
	int soulActive;
	float speedMultiplier;
	int isTrapped;

	// Boss entry cinematic state
	int bossEntryFrame;
	int bossEntryTimer;
};

enum ItemType { ITEM_HEART, ITEM_POTION, ITEM_POWER, ITEM_GAIN, ITEM_KEY };

struct Pickup {
	int x, y;
	int active;
	enum ItemType type;
};

struct GlowProjectile {
	int x, y;
	int targetX, targetY;
	int active;
	int frame;
	int animTimer;
	int targetCreatureIdx;
	int targetSentryIdx;
};



// ===== Level 3 trader / boss support =====
enum BossState {
  BOSS_IDLE_STATE,
  BOSS_WALK_STATE,
  BOSS_SLASH_STATE,
  BOSS_DASH_STATE,
  BOSS_CAST_STATE,
  BOSS_TELEPORT_OUT_STATE,
  BOSS_TELEPORT_IN_STATE,
  BOSS_SPIKE_STATE,
  BOSS_TRAP_STATE,
  BOSS_DEATH_RISE_STATE,
  BOSS_DEATH_ANIM_STATE,
  BOSS_WAIT_MINION_STATE
};

struct Boss {
  int x, y, frame, animTimer, active, facingRight;
  enum BossState state;
  int stateTimer, currentHealth, maxHealth, hasUsedTrap;
  int teleportTargetX, phase, riseY, invincibilityTimer;
  enum BossState intendedNextState;
};

enum MinionType { MINION_FIREBAT, MINION_BAT };
struct BossMinion {
  int x, y, active, frame, animTimer, facingRight, health;
  int invincibilityTimer, isDying, deathTimer;
  enum MinionType type;
};

enum HazardType { HAZARD_SPIKE, HAZARD_TRAP };
struct BossHazard {
  int x, y, active, frame, animTimer, hasHit;
  enum HazardType type;
};

enum TraderState {
  TRADER_IDLE_STATE,
  TRADER_WALK_TO_PLAYER,
  TRADER_TURN_STATE,
  TRADER_PROMPT_INTERACT,
  TRADER_SHOW_KEY,
  TRADER_TRADING,
  TRADER_TRADE_MENU,
  TRADER_WALK_AWAY,
  TRADER_WALK_BACK,
  TRADER_DONE
};

struct TraderNPC {
  int x, y, frame, animTimer;
  enum TraderState state;
  int stateTimer, facingRight, active, traded, initialX;
  int tradeMenuOpen, mouseX, mouseY, hoveredItem;
  int tradedSwiftness, tradedSoul, tradedKey;
};

struct BossDoor {
  int x, y, frame, animTimer, locked, opening, opened;
};

struct Arrow {
	int x, y;
	bool active;
	bool facingRight;
	bool shootingUp;
	int frame;
	int animTimer;
};

struct Camera {
  float x, y;
  float targetX, targetY;
};

struct Background {
  int x;
  unsigned int texture;
  int tunnelTransitionY;
};

struct Tile {
  int x, y;
  unsigned int texture;
  int active;
  float width, height;
  int isJumpThrough;
};

struct Midground {
  struct Tile tiles[MAX_TILES];
  int tileCount;
  unsigned int tileTexture1;
  unsigned int tileTexture2;
  unsigned int tunnelHoleTexture;
};

// =====================================================
// CREATURE (flying "bug" enemy — bug.hpp)
// =====================================================

enum CreatureState {
  CREATURE_INACTIVE,
  CREATURE_LOADING,
  CREATURE_RISING,
  CREATURE_PATROL_LEFT,
  CREATURE_PATROL_RIGHT,
  CREATURE_TURNING,
  CREATURE_CHASING,
  CREATURE_ATTACKING,
  CREATURE_HOVERING,
  CREATURE_DAMAGE,
  CREATURE_DEAD
};

struct Creature {
  int x, y;
  int vx, vy;
  int frame;
  int active;
  enum CreatureState state;
  int targetX, targetY;
  int patrolStartX;
  int subStateTimer;
  int animationTimer;
  int facingRight;
  int maxHealth;
  int currentHealth;
  int invincibilityTimer;
  int damageAnimTimer;
  int damageFrame;

  // Aggro tracking (added so chase behaviour is based on a sticky
  // "has it noticed me" flag with hysteresis, instead of re-testing
  // raw distance every frame — see bug.hpp for why).
  int isAggro;

  // Counts down after an attack finishes; can't start another
  // attack until it hits 0 (see CREATURE_ATTACK_COOLDOWN in bug.hpp).
  int attackCooldown;
};

// =====================================================
// SENTRY (ground enemy — Sentry.hpp)
// =====================================================

enum SentryState {
  SENTRY_INACTIVE,
  SENTRY_WAKING,
  SENTRY_IDLE_STATE,
  SENTRY_WALK_LEFT,
  SENTRY_WALK_RIGHT,
  SENTRY_RUN_LEFT,
  SENTRY_RUN_RIGHT,
  SENTRY_ATTACK_LEFT,
  SENTRY_ATTACK_RIGHT,
  SENTRY_JUMP_ATTACK,
  SENTRY_SLASH_LEFT,
  SENTRY_SLASH_RIGHT,
  SENTRY_TURNING_STATE,
  SENTRY_DAMAGE,
  SENTRY_DYING,
  SENTRY_DYING_AIR
};

struct Sentry {
  int x, y;
  int vx, vy;
  int frame;
  int active;
  enum SentryState state;
  int patrolStartX;
  int subStateTimer;
  int animationTimer;
  int facingRight;
  int maxHealth;
  int currentHealth;
  int invincibilityTimer;
  int damageAnimTimer;
  int damageFrame;

  // Same aggro-with-hysteresis idea as Creature (see Sentry.hpp).
  int isAggro;

  // Counts down after an attack finishes; can't start another
  // attack until it hits 0 (see SENTRY_ATTACK_COOLDOWN in Sentry.hpp).
  int attackCooldown;
};

struct SparkleEffect {
  int x, y;
  int frame;
  int active;
  int animationTimer;
};

struct TitleScreen {
  int bgFrame;
  int bgAnimationTimer;
  int bgState;
  int bgPhaseRepeat;
  unsigned int skyTextures[TITLE_SKY_FRAMES];
  unsigned int fireTextures[TITLE_FIRE_FRAMES];
  unsigned int fireTransTextures[TITLE_TRANSITION_FRAMES];
  unsigned int skyTransTextures[TITLE_TRANSITION_FRAMES];
  unsigned int titleTexture;
  unsigned int startTexture;
  unsigned int achievementsTexture;
  unsigned int creditsTexture;
  unsigned int exitTexture;
  unsigned int cursorTexture;
  unsigned int creditsBgTexture;
  unsigned int backButtonTexture;
  unsigned int controlsBgTexture;
  unsigned int loadingTexture;
  int loadingTimer;
  int mouseX, mouseY;
};

#endif
