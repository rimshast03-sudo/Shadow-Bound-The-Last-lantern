STAMINABAR + INVENTORY MODULE
==============================
Files included:
  staminabar.hpp
  inventory.hpp
  Assets/UI/Staminabar/   (6 stamina bar frames)
  Assets/UI/Inventory/    (inventory base + heart/potion/power/gain icons)
  Assets/UI/Pickables/    (world pickup icons: heart/potion/power/gain)
  Assets/UI/Glow/         (5-frame "power" projectile animation)

To drop these into another project, place staminabar.hpp / inventory.hpp
next to your other headers, and copy the Assets/UI/... folders into your
project's Assets/UI/ folder (keep the same relative paths — the code
loads images by these exact paths).

You will also need the following in your target project, since the two
files depend on them:

------------------------------------------------------------
1) config.hpp macros
------------------------------------------------------------
#define PLAYER_MAX_STAMINA 100
#define STAMINA_COST_DASH 20
#define STAMINA_COST_DOWNSTAB 20
#define STAMINA_REGEN_RATE 1
#define STAMINA_REGEN_INTERVAL 20

#define UI_BAR_X 20
#define UI_BAR_Y 490
#define UI_BAR_WIDTH 250
#define UI_BAR_HEIGHT 100
#define UI_BAR_SPACING 50

#define INVENTORY_W 150
#define INVENTORY_H 300
#define INVENTORY_X -10
#define INVENTORY_Y 215

#define PICKUP_SIZE 48
#define PICKUP_DROP_CHANCE 75
#define MAX_PICKUPS 10

#define POTION_DURATION 500
#define POTION_DAMAGE_MULTIPLIER 2

#define POWER_DAMAGE_PERCENT 20
#define GLOW_FRAMES 5
#define GLOW_SIZE 48
#define GLOW_SPEED 10

#define HEART_DURATION 500
#define HEART_REGEN_INTERVAL 150
#define HEART_REGEN_AMOUNT 20

#define GAIN_DURATION 600
#define GAIN_REGEN_INTERVAL 180
#define GAIN_REGEN_AMOUNT 20

------------------------------------------------------------
2) structs.hpp additions
------------------------------------------------------------
// Inside struct Player, add:
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

// New enum + structs:
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

Note: inventory.hpp's handleItemInput() references creature/sentry/boss
structs and states (Creature, Sentry, Boss, CREATURE_DEAD, SENTRY_DYING,
SENTRY_DYING_AIR, BOSS_DEATH_ANIM_STATE, BOSS_DEATH_RISE_STATE,
LEVEL2_STATE, LEVEL3_STATE, PLAYING_STATE, BOSS_STATE, MAX_CREATURES,
MAX_SENTRIES, LEVEL3_END_X). If your other project doesn't have a boss/
creature/sentry system, you'll need to trim those sections out of
handleItemInput() and updateGlowProjectile(), or stub them out.

------------------------------------------------------------
3) textures.hpp additions
------------------------------------------------------------
unsigned int inventoryBaseTex = 0;
unsigned int inventoryHeartTex = 0;
unsigned int inventoryPotionTex = 0;
unsigned int inventoryPowerTex = 0;
unsigned int inventoryGainTex = 0;
unsigned int pickableHeartTex = 0;
unsigned int pickablePotionTex = 0;
unsigned int pickablePowerTex = 0;
unsigned int pickableGainTex = 0;
unsigned int glowTextures[GLOW_FRAMES];

// Helper used to bulk-load numbered frame sequences (e.g. glow (%d).png):
void loadSet(unsigned int *arr, int n, const char *fmt) {
  char filename[100];
  for (int i = 0; i < n; i++) {
    sprintf_s(filename, sizeof(filename), fmt, i + 1);
    arr[i] = iLoadImage(filename);
  }
}

void loadInventoryTextures() {
  inventoryBaseTex   = iLoadImage("Assets/UI/Inventory/inventory.png");
  inventoryHeartTex  = iLoadImage("Assets/UI/Inventory/heart.png");
  inventoryPotionTex = iLoadImage("Assets/UI/Inventory/potion.png");
  inventoryPowerTex  = iLoadImage("Assets/UI/Inventory/power.png");
  inventoryGainTex   = iLoadImage("Assets/UI/Inventory/gain.png");
  pickableHeartTex   = iLoadImage("Assets/UI/Pickables/heart.png");
  pickablePotionTex  = iLoadImage("Assets/UI/Pickables/potion.png");
  pickablePowerTex   = iLoadImage("Assets/UI/Pickables/power.png");
  pickableGainTex    = iLoadImage("Assets/UI/Pickables/gain.png");
  loadSet(glowTextures, GLOW_FRAMES, "Assets/UI/Glow/glow (%d).png");
}
// Call loadInventoryTextures() once during your asset-loading setup.

------------------------------------------------------------
4) Usage in your game loop
------------------------------------------------------------
initStaminaBar();          // once, at startup
initPlayerInventory(&player);
initPickups(pickups);

// each frame:
renderStaminaBar(&player);
renderInventoryUI(&player);
updatePickups(pickups, &player);
updateItemEffects(&player);
handleItemInput(&player, &glow, creatures, sentries, gameState);
updateGlowProjectile(&glow, creatures, sentries);
renderPickups(pickups, &camera);
renderGlowProjectile(&glow, &camera);

