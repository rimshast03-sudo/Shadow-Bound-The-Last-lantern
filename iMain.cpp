#include "iGraphics.h"
#include "config.hpp"
#include "structs.hpp"
#include "player.hpp"
#include "camera.hpp"
#include "background.hpp"
#include "midground.hpp"
#include "bug.hpp"
#include "Sentry.hpp"
#include "game.hpp"
#include "healthbar.hpp"
#include "staminabar.hpp"
#include "title.hpp"
#include "sounds.hpp"
#include "textures.hpp"
#include "inventory.hpp"
#include "cave.hpp"
#include "tradernpc.hpp"
#include "boss.hpp"
#include "level3boss.hpp"
#include <time.h>
#include <cstdlib>

int gameState = TITLE_SCREEN_STATE;
int level1ClearedTimer = 0;

Player player;
struct Camera camera;
struct Midground midground;
struct Creature creatures[MAX_CREATURES];
struct Sentry sentries[MAX_SENTRIES];
struct TitleScreen titleScreen;
struct Pickup pickups[MAX_PICKUPS];
struct GlowProjectile glow;
struct Arrow arrows[MAX_ARROWS];
struct TraderNPC traderNpc;
struct BossDoor bossDoor;
struct Boss boss;
struct BossMinion bossMinions[MAX_BOSS_MINIONS];
struct BossHazard bossHazards[MAX_BOSS_HAZARDS];

static void resetCaveCamera()
{
    camera.x = 0;
    camera.targetX = 0;
}

static void enterCave()
{
    gameState = CAVE_STATE;
    level1ClearedTimer = 0;
    player.x = CAVE_LAND_X;
    player.y = CAVE_LAND_Y;
    player.vx = 0;
    player.vy = 0;
    player.onGround = true;
    resetCaveCamera();
    startCaveEncounter(player.x);
    stopBGMusic();
}

static void enterLevel2()
{
    level1ClearedTimer = 0;
    gameState = LEVEL2_STATE;
    startLevel2(player, creatures, sentries, &camera, &midground);
}

void iDraw()
{
    iClear();

    if (gameState == TITLE_SCREEN_STATE)
        renderTitleScreen(&titleScreen);
    else if (gameState == LOADING_STATE)
        renderLoadingScreen(&titleScreen);
    else if (gameState == CREDITS_STATE)
        renderCredits(&titleScreen);
    else if (gameState == CONTROLS_STATE)
        renderControls(&titleScreen);
    else if (gameState == PLAYING_STATE || gameState == LEVEL2_STATE || gameState == LEVEL3_STATE)
    {
        unsigned int *bgSet = (gameState == PLAYING_STATE) ? backgroundTextures
                            : (gameState == LEVEL3_STATE)  ? level3BackgroundTextures
                                                            : level2BackgroundTextures;

        renderBackgroundWithCamera(&camera, bgSet);
        renderMidground(&midground, &camera, gameState);
        renderCreatures(creatures, &camera);
        renderSentries(sentries, &camera);

        if (gameState == LEVEL3_STATE) {
            renderTraderNPC(&traderNpc, &camera);
            renderBossDoor(&bossDoor, &camera);
        }

        float sx = getScreenX(player.x, &camera);
        float sy = getScreenY(player.y, &camera);
        drawPlayer(player, sx, sy);

        renderHealthBar(&player);
        renderStaminaBar(&player);
        renderInventoryUI(&player);
        renderPickups(pickups, &camera);
        renderGlowProjectile(&glow, &camera);
        renderArrows(arrows, &camera);

        if (gameState == LEVEL3_STATE) {
            renderTradeMenu(&traderNpc, &player);
            renderEquippedIcons(&player);
        }

        iSetColor(255, 255, 255);
        char levelLabel[32];
        sprintf_s(levelLabel, sizeof(levelLabel), "Level %d",
                  (gameState == LEVEL3_STATE) ? 3 : (gameState == LEVEL2_STATE) ? 2 : 1);
        iText(SCREEN_W - 120, SCREEN_H - 30, levelLabel,
              GLUT_BITMAP_HELVETICA_18);
    }
    else if (gameState == CAVE_STATE)
    {
        resetCaveCamera();
        renderBackgroundWithCamera(&camera, backgroundTextures);
        drawCave();
        drawPlayer(player, player.x, player.y);
        renderHealthBar(&player);
        renderStaminaBar(&player);
        renderInventoryUI(&player);
        renderArrowsScreenSpace(arrows);
    }
    else if (gameState == LEVEL1_CLEARED_STATE)
    {
        resetCaveCamera();
        renderBackgroundWithCamera(&camera, backgroundTextures);
        drawPlayer(player, player.x, player.y);
        renderHealthBar(&player);
        renderStaminaBar(&player);

        iSetColor(255, 215, 0);
        iText(SCREEN_W / 2 - 130, SCREEN_H / 2 + 20,
              "LEVEL 1 CLEARED!", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(255, 255, 255);
        iText(SCREEN_W / 2 - 180, SCREEN_H / 2 - 20,
              "Press ENTER or SPACE to continue.");
    }
    else if (gameState == BOSS_ENTRY_STATE)
    {
        unsigned int tex = bossEntryFrames[player.bossEntryFrame];
        if (tex != 0)
            iShowImage(0, 0, SCREEN_W, SCREEN_H, tex);
        iSetColor(255, 255, 255);
        iText(20, 20, "The guardian awakens...", GLUT_BITMAP_HELVETICA_18);
    }
    else if (gameState == BOSS_STATE)
    {
        resetCaveCamera();
        if (bossBackgroundTexture != 0)
            iShowImage(0, 0, SCREEN_W, SCREEN_H, bossBackgroundTexture);
        else
            renderBackgroundWithCamera(&camera, level3BackgroundTextures);

        renderHazards(bossHazards, &camera);
        renderMinions(bossMinions, &camera);
        renderLevel3Boss(&boss, &camera);
        drawPlayer(player, player.x, player.y);
        renderHealthBar(&player);
        renderStaminaBar(&player);
        renderInventoryUI(&player);
        renderArrowsScreenSpace(arrows);
    }
    else if (gameState == CONGRATS_STATE)
    {
        iSetColor(8, 8, 12);
        iFilledRectangle(0, 0, SCREEN_W, SCREEN_H);
        if (congratsTex != 0)
            iShowImage(0, 0, SCREEN_W, SCREEN_H, congratsTex);
        else {
            iSetColor(255, 215, 0);
            iText(SCREEN_W / 2 - 130, SCREEN_H / 2 + 25,
                  "BOSS DEFEATED!", GLUT_BITMAP_TIMES_ROMAN_24);
            iSetColor(255, 255, 255);
            iText(SCREEN_W / 2 - 170, SCREEN_H / 2 - 20,
                  "You completed Level 3.", GLUT_BITMAP_HELVETICA_18);
        }
    }

    if (titleScreen.cursorTexture != 0 &&
        (gameState == TITLE_SCREEN_STATE || gameState == CREDITS_STATE ||
         gameState == CONTROLS_STATE))
    {
        iShowImage(titleScreen.mouseX - 16, titleScreen.mouseY - 16,
                   32, 32, titleScreen.cursorTexture);
    }
}

void animate()
{
    if (gameState == LOADING_STATE)
    {
        updateLoadingScreen(&titleScreen, &gameState);
    }
    else if (gameState == TITLE_SCREEN_STATE ||
             gameState == CREDITS_STATE ||
             gameState == CONTROLS_STATE)
    {
        updateTitleAnimation(&titleScreen);
    }
    else if (gameState == PLAYING_STATE)
    {
        updatePlayer(player, &midground, arrows);

        if (player.x >= CAVE_ENTRY_X)
        {
            enterCave();
            return;
        }

        updateGame(player, creatures, sentries, &camera, &midground, &gameState, arrows);
        updateFootstepSounds(&player, &midground, gameState);
        updateHealthSound(&player);
    }
    else if (gameState == CAVE_STATE)
    {
        resetCaveCamera();
        updatePlayer(player, &midground, arrows);
        updateCave(player, arrows);

        if (isBossDefeated())
        {
            gameState = LEVEL1_CLEARED_STATE;
            level1ClearedTimer = 0;
            stopBGMusic();
        }
    }
    else if (gameState == LEVEL1_CLEARED_STATE)
    {
        if (++level1ClearedTimer >= LEVEL1_CLEARED_FRAMES)
            enterLevel2();
    }
    else if (gameState == LEVEL2_STATE)
    {
        updatePlayer(player, &midground, arrows);
        updateGame(player, creatures, sentries, &camera, &midground, &gameState, arrows);
        updateFootstepSounds(&player, &midground, gameState);
        updateHealthSound(&player);
    }
    else if (gameState == LEVEL3_STATE)
    {
        if (!traderNpc.tradeMenuOpen)
            updatePlayer(player, &midground, arrows, LEVEL3_GROUND_Y);
        updateGame(player, creatures, sentries, &camera, &midground, &gameState, arrows);
        updateFootstepSounds(&player, &midground, gameState);
        updateHealthSound(&player);
    }
    else if (gameState == BOSS_ENTRY_STATE)
    {
        updateGame(player, creatures, sentries, &camera, &midground, &gameState, arrows);
    }
    else if (gameState == BOSS_STATE)
    {
        // Keep the same Zip 2 player, including its slash and arrow attacks.
        updatePlayer(player, &midground, arrows, BOSS_GROUND_Y);
        if (player.x < 50) player.x = 50;
        if (player.x > 850) player.x = 850;
        updateGame(player, creatures, sentries, &camera, &midground, &gameState, arrows);
        updateHealthSound(&player);
    }
}

void iMouseMove(int mx, int my)
{
    handleTitleMouseMove(&titleScreen, mx, my);
    traderNpc.mouseX = mx;
    traderNpc.mouseY = my;
}

void iPassiveMouseMove(int mx, int my)
{
    handleTitleMouseMove(&titleScreen, mx, my);
    traderNpc.mouseX = mx;
    traderNpc.mouseY = my;
}

void iMouse(int button, int state, int mx, int my)
{
    handleTitleMouseClick(&titleScreen, button, state, mx, my, &gameState);

    if (gameState == LEVEL3_STATE && state == GLUT_DOWN) {
        traderNpc.mouseX = mx;
        traderNpc.mouseY = my;
        if (traderNpc.tradeMenuOpen) {
            handleTradeClick(&traderNpc, &player, mx, my);
        } else {
            handleEquippedIconClick(&player, mx, my);
        }
    }
}

void iKeyboard(unsigned char key)
{
    if (key == 27)
    {
        exit(0);
    }

    if (gameState == TITLE_SCREEN_STATE && (key == ' ' || key == 13))
    {
        playStartButtonClickSound();
        gameState = LOADING_STATE;
        titleScreen.loadingTimer = 0;
        return;
    }

    if (gameState == LEVEL1_CLEARED_STATE && (key == ' ' || key == 13))
    {
        enterLevel2();
        return;
    }

    if (gameState == LEVEL3_STATE && (key == 'e' || key == 'E'))
    {
        int dx = player.x - traderNpc.x;
        if (dx < 0) dx = -dx;
        if (dx <= TRADER_STOP_RADIUS + 35 &&
            (traderNpc.state == TRADER_PROMPT_INTERACT || traderNpc.state == TRADER_TRADE_MENU)) {
            traderNpc.tradeMenuOpen = !traderNpc.tradeMenuOpen;
            traderNpc.state = traderNpc.tradeMenuOpen ? TRADER_TRADE_MENU : TRADER_PROMPT_INTERACT;
            traderNpc.stateTimer = 0;
        }
        return;
    }
}

void iSpecialKeyboard(unsigned char key) {}

int main()
{
    srand((unsigned int)time(NULL));

    iInitialize(SCREEN_W, SCREEN_H, "Shadow Bound");

    initTitleScreen(&titleScreen);
    loadTitleTextures(&titleScreen);
    initSounds();

    initPlayer(player);
    initCamera(&camera);
    initMidground(&midground);
    loadMidgroundTextures(&midground);
    midground.tunnelHoleTexture = 0;

    initCreatures(creatures);
    initSentries(sentries);

    loadImages();
    loadInventoryTextures();

    initHealthBar();
    initStaminaBar();
    initPlayerInventory(&player);
    initPickups(pickups);
    initCave();
    initArrows(arrows);
    initLevel3Boss(&boss);
    initLevel3BossMinions(bossMinions);
    initLevel3BossHazards(bossHazards);
    initTraderNPC(&traderNpc);
    initBossDoor(&bossDoor);

    iSetTimer(15, animate);
    iStart();

    return 0;
}
