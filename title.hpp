#ifndef TITLE_HPP
#define TITLE_HPP

#include "glut.h"
#include "iGraphics.h"
#include "sounds.hpp"
#include "structs.hpp"
#include <cstdio>
#include <stdlib.h>

#define TITLE_SKY_STATE_1 0
#define TITLE_SKY_STATE_2 1
#define TITLE_FIRE_TRANSITION_STATE 2
#define TITLE_FIRE_STATE_1 3
#define TITLE_FIRE_STATE_2 4
#define TITLE_SKY_TRANSITION_STATE 5

void initTitleScreen(struct TitleScreen *title) {
  title->bgFrame = 0;
  title->bgAnimationTimer = 0;
  title->bgState = TITLE_SKY_STATE_1;
  title->bgPhaseRepeat = 0;
  title->mouseX = 0;
  title->mouseY = 0;
  title->creditsBgTexture = 0;
  title->backButtonTexture = 0;
  title->controlsBgTexture = 0;
  title->loadingTexture = 0;
  title->loadingTimer = 0;
}

void loadTitleTextures(struct TitleScreen *title) {
  char name[64];

  for (int i = 0; i < TITLE_SKY_FRAMES; i++) {
    sprintf_s(name, sizeof(name),
              "Assets/Title Screen/Background/Sky/sky (%d).png", i + 1);
    title->skyTextures[i] = iLoadImage(name);
  }

  for (int i = 0; i < TITLE_FIRE_FRAMES; i++) {
    sprintf_s(name, sizeof(name),
              "Assets/Title Screen/Background/Fire/fire (%d).png", i + 1);
    title->fireTextures[i] = iLoadImage(name);
  }

  for (int i = 0; i < TITLE_TRANSITION_FRAMES; i++) {
    sprintf_s(name, sizeof(name),
              "Assets/Title Screen/Background/FireTransition/trans (%d).png",
              i + 1);
    title->fireTransTextures[i] = iLoadImage(name);

    sprintf_s(name, sizeof(name),
              "Assets/Title Screen/Background/SkyTransition/trans (%d).png",
              i + 1);
    title->skyTransTextures[i] = iLoadImage(name);
  }

  title->titleTexture = iLoadImage("Assets/Title Screen/Title/title.png");
  title->startTexture = iLoadImage("Assets/Title Screen/Start/start.png");
  title->achievementsTexture =
      iLoadImage("Assets/Title Screen/Achievements/achievements.png");
  title->creditsTexture = iLoadImage("Assets/Title Screen/Credits/credits.png");
  title->exitTexture = iLoadImage("Assets/Title Screen/Exit/exit.png");
  title->cursorTexture = iLoadImage("Assets/Title Screen/Cursor/cursor.png");

  title->creditsBgTexture =
      iLoadImage("Assets/Title Screen/CreditsBg/credits.png");
  title->backButtonTexture = iLoadImage("Assets/Title Screen/Back/back.png");

  title->controlsBgTexture =
      iLoadImage("Assets/Title Screen/Controls/controls.png");
  title->loadingTexture =
      iLoadImage("Assets/Title Screen/LoadingScreen/loading.png");
}

void updateTitleAnimation(struct TitleScreen *title) {
  title->bgAnimationTimer++;

  if (title->bgAnimationTimer >= TITLE_ANIMATION_SPEED) {
    title->bgAnimationTimer = 0;
    title->bgFrame++;

    switch (title->bgState) {
    case TITLE_SKY_STATE_1:
      if (title->bgFrame >= TITLE_SKY_FRAMES) {
        title->bgFrame = 0;
        title->bgState = TITLE_SKY_STATE_2;
      }
      break;

    case TITLE_SKY_STATE_2:
      if (title->bgFrame >= TITLE_SKY_FRAMES) {
        title->bgFrame = 0;
        title->bgState = TITLE_FIRE_TRANSITION_STATE;
      }
      break;

    case TITLE_FIRE_TRANSITION_STATE:
      if (title->bgFrame >= TITLE_TRANSITION_FRAMES) {
        title->bgFrame = 0;
        title->bgState = TITLE_FIRE_STATE_1;
      }
      break;

    case TITLE_FIRE_STATE_1:
      if (title->bgFrame >= TITLE_FIRE_FRAMES) {
        title->bgFrame = 0;
        title->bgState = TITLE_FIRE_STATE_2;
      }
      break;

    case TITLE_FIRE_STATE_2:
      if (title->bgFrame >= TITLE_FIRE_FRAMES) {
        title->bgFrame = 0;
        title->bgState = TITLE_SKY_TRANSITION_STATE;
      }
      break;

    case TITLE_SKY_TRANSITION_STATE:
      if (title->bgFrame >= TITLE_TRANSITION_FRAMES) {
        title->bgFrame = 0;
        title->bgState = TITLE_SKY_STATE_1;
      }
      break;
    }
  }
}

void renderTitleScreen(struct TitleScreen *title) {
  unsigned int currentBgTexture = 0;

  switch (title->bgState) {
  case TITLE_SKY_STATE_1:
  case TITLE_SKY_STATE_2:
    currentBgTexture = title->skyTextures[title->bgFrame];
    break;
  case TITLE_FIRE_TRANSITION_STATE:
    currentBgTexture = title->fireTransTextures[title->bgFrame];
    break;
  case TITLE_FIRE_STATE_1:
  case TITLE_FIRE_STATE_2:
    currentBgTexture = title->fireTextures[title->bgFrame];
    break;
  case TITLE_SKY_TRANSITION_STATE:
    currentBgTexture = title->skyTransTextures[title->bgFrame];
    break;
  }

  if (currentBgTexture != 0) {
    iShowImage(0, 0, SCREEN_W, SCREEN_H, currentBgTexture);
  }

  if (title->titleTexture != 0) {
    iShowImage(1, 1, SCREEN_W, SCREEN_H, title->titleTexture);
    iShowImage(-1, -1, SCREEN_W, SCREEN_H, title->titleTexture);
    iShowImage(1, -1, SCREEN_W, SCREEN_H, title->titleTexture);
    iShowImage(-1, 1, SCREEN_W, SCREEN_H, title->titleTexture);
    iShowImage(0, 0, SCREEN_W, SCREEN_H, title->titleTexture);
  }

  int hoveredBtn = getTitleButtonAtPos(title->mouseX, title->mouseY);

  if (title->startTexture != 0) {
    if (hoveredBtn == 0) {
      iShowImage(0, 4, SCREEN_W, SCREEN_H, title->startTexture);
    } else {
      iShowImage(0, 0, SCREEN_W, SCREEN_H, title->startTexture);
    }
  }

  if (title->achievementsTexture != 0) {
    if (hoveredBtn == 1) {
      iShowImage(0, 4, SCREEN_W, SCREEN_H, title->achievementsTexture);
    } else {
      iShowImage(0, 0, SCREEN_W, SCREEN_H, title->achievementsTexture);
    }
  }

  if (title->creditsTexture != 0) {
    if (hoveredBtn == 2) {
      iShowImage(0, 4, SCREEN_W, SCREEN_H, title->creditsTexture);
    } else {
      iShowImage(0, 0, SCREEN_W, SCREEN_H, title->creditsTexture);
    }
  }

  if (title->exitTexture != 0) {
    if (hoveredBtn == 3) {
      iShowImage(0, 4, SCREEN_W, SCREEN_H, title->exitTexture);
    } else {
      iShowImage(0, 0, SCREEN_W, SCREEN_H, title->exitTexture);
    }
  }

}

void renderCredits(struct TitleScreen *title) {
  if (title->creditsBgTexture != 0) {
    iShowImage(0, 0, SCREEN_W, SCREEN_H, title->creditsBgTexture);
  } else {
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_W, SCREEN_H);
  }
}

void renderControls(struct TitleScreen *title) {
  if (title->controlsBgTexture != 0) {
    iShowImage(0, 0, SCREEN_W, SCREEN_H, title->controlsBgTexture);
  } else {
    if (title->skyTextures[0] != 0) {
      iShowImage(0, 0, SCREEN_W, SCREEN_H, title->skyTextures[0]);
    } else {
      iSetColor(20, 20, 20);
      iFilledRectangle(0, 0, SCREEN_W, SCREEN_H);
    }
  }
}

void renderLoadingScreen(struct TitleScreen *title) {
  if (title->loadingTexture != 0) {
    iShowImage(0, 0, SCREEN_W, SCREEN_H, title->loadingTexture);
  } else {
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_W, SCREEN_H);
  }
}

void updateLoadingScreen(struct TitleScreen *title, int *gameState) {
  title->loadingTimer++;
  if (title->loadingTimer >= 250) {
    *gameState = PLAYING_STATE;
    restartBGMusic();
    title->loadingTimer = 0;
    glutSetCursor(GLUT_CURSOR_NONE);
  }
}

void handleTitleMouseMove(struct TitleScreen *title, int mx, int my) {
  title->mouseX = mx;
  title->mouseY = my;
}

void handleTitleMouseClick(struct TitleScreen *title, int button, int state,
                           int mx, int my, int *gameState) {
  if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
    return;

  if (*gameState == TITLE_SCREEN_STATE) {
    if (mx >= 400 && mx <= 600 && my >= 200 && my <= 280) {
      playStartButtonClickSound();
      *gameState = LOADING_STATE;
      title->loadingTimer = 0;
    }

    if (mx >= 400 && mx <= 600 && my >= 150 && my <= 190) {
      playButtonClickSound();
      *gameState = CONTROLS_STATE;
    }

    if (mx >= 400 && mx <= 600 && my >= 100 && my <= 140) {
      playButtonClickSound();
      *gameState = CREDITS_STATE;
    }

    if (mx >= 400 && mx <= 600 && my >= 40 && my <= 100) {
      playButtonClickSound();
      exit(0);
    }
  } else if (*gameState == CREDITS_STATE) {
    if (mx >= 0 && mx <= SCREEN_W && my >= 0 && my <= SCREEN_H) {
      playButtonClickSound();
      *gameState = TITLE_SCREEN_STATE;
      glutSetCursor(GLUT_CURSOR_NONE);
    }
  } else if (*gameState == CONTROLS_STATE) {
    if (mx >= 0 && mx <= SCREEN_W && my >= 0 && my <= SCREEN_H) {
      playButtonClickSound();
      *gameState = TITLE_SCREEN_STATE;
      glutSetCursor(GLUT_CURSOR_NONE);
    }
  }
}

#endif
