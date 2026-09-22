#ifndef TRADERNPC_HPP
#define TRADERNPC_HPP

#include "camera.hpp"
#include "config.hpp"
#include "iGraphics.h"
#include "player.hpp"
#include "structs.hpp"
#include "textures.hpp"
#include <math.h>

void initTraderNPC(struct TraderNPC *t) {
  t->x = TRADER_NPC_X;
  t->y = TRADER_NPC_Y;
  t->frame = 0;
  t->animTimer = 0;
  t->state = TRADER_IDLE_STATE;
  t->stateTimer = 0;
  t->facingRight = 0;
  t->active = 1;
  t->traded = 0;
  t->initialX = TRADER_NPC_X;
  t->tradeMenuOpen = 0;
  t->mouseX = 0;
  t->mouseY = 0;
  t->hoveredItem = 0;
  t->tradedSwiftness = 0;
  t->tradedSoul = 0;
  t->tradedKey = 0;
}

int getTradeHoveredItem(int mx, int my, struct TraderNPC *t) {
  if (!t->tradedSwiftness && mx >= TRADE_SWIFTNESS_X1 && mx <= TRADE_SWIFTNESS_X2 &&
      my >= TRADE_SWIFTNESS_Y1 && my <= TRADE_SWIFTNESS_Y2)
    return 1;
  if (!t->tradedSoul && mx >= TRADE_SOUL_X1 && mx <= TRADE_SOUL_X2 &&
      my >= TRADE_SOUL_Y1 && my <= TRADE_SOUL_Y2)
    return 2;
  if (!t->tradedKey && mx >= TRADE_KEY_X1 && mx <= TRADE_KEY_X2 &&
      my >= TRADE_KEY_Y1 && my <= TRADE_KEY_Y2)
    return 3;
  return 0;
}

void handleTradeClick(struct TraderNPC *t, struct Player *player, int mx, int my) {
  if (mx < TRADE_LIST_X || mx > TRADE_LIST_X + TRADE_LIST_W ||
      my < TRADE_LIST_Y || my > TRADE_LIST_Y + TRADE_LIST_H) {
    t->tradeMenuOpen = 0;
    return;
  }

  int item = getTradeHoveredItem(mx, my, t);
  if (item == 0) return;

  int cost = 0;
  switch (item) {
    case 1: cost = TRADE_ITEM_COST_SWIFTNESS; break;
    case 2: cost = TRADE_ITEM_COST_SOUL; break;
    case 3: cost = TRADE_ITEM_COST_KEY; break;
  }

  if (player->fragments < cost) return; 

  player->fragments -= cost;

  switch (item) {
    case 1:
      player->hasSwiftness = 1;
      t->tradedSwiftness = 1;
      break;
    case 2:
      player->hasSoul = 1;
      t->tradedSoul = 1;
      break;
    case 3:
      player->hasKeyItem = 1;
      t->tradedKey = 1;
      break;
  }
}

void updateTraderNPC(struct TraderNPC *t, struct Player *player) {
  if (!t->active || t->state == TRADER_DONE)
    return;

  float dx = (float)(player->x - t->x);
  float dy = (float)(player->y - t->y);
  float distance = sqrt(dx * dx + dy * dy);

  t->stateTimer++;
  t->animTimer++;

  if (t->animTimer >= TRADER_ANIM_SPEED) {
    t->animTimer = 0;

    switch (t->state) {
    case TRADER_IDLE_STATE:
    case TRADER_PROMPT_INTERACT:
    case TRADER_TRADE_MENU:
      t->frame = (t->frame + 1) % TRADER_IDLE_FRAMES;
      break;
    case TRADER_WALK_TO_PLAYER:
      if (t->facingRight)
        t->frame = (t->frame + 1) % TRADER_WALK_R_FRAMES;
      else
        t->frame = (t->frame + 1) % TRADER_WALK_L_FRAMES;
      break;
    case TRADER_TURN_STATE:
      t->frame++;
      if (t->frame >= TRADER_TURN_FRAMES)
        t->frame = TRADER_TURN_FRAMES - 1;
      break;
    case TRADER_TRADING:
      t->frame = (t->frame + 1) % TRADER_TRADE_FRAMES;
      break;
    case TRADER_WALK_AWAY:
    case TRADER_WALK_BACK:
      if (t->facingRight)
        t->frame = (t->frame + 1) % TRADER_WALK_R_FRAMES;
      else
        t->frame = (t->frame + 1) % TRADER_WALK_L_FRAMES;
      break;
    default:
      break;
    }
  }

  switch (t->state) {
  case TRADER_IDLE_STATE:
    if (distance <= TRADER_DETECTION_RADIUS && !t->traded) {
      t->state = TRADER_WALK_TO_PLAYER;
      t->frame = 0;
      t->stateTimer = 0;
      t->facingRight = (dx > 0) ? 1 : 0;
    }
    break;

  case TRADER_WALK_TO_PLAYER:
    if (dx > 0) {
      t->x += TRADER_WALK_SPEED;
      t->facingRight = 1;
    } else {
      t->x -= TRADER_WALK_SPEED;
      t->facingRight = 0;
    }

    if (distance <= TRADER_STOP_RADIUS) {
      t->state = TRADER_PROMPT_INTERACT;
      t->frame = 0;
      t->stateTimer = 0;
    }
    break;

  case TRADER_PROMPT_INTERACT:
    break;

  case TRADER_TURN_STATE:
    if (t->stateTimer >= TRADER_TURN_FRAMES * TRADER_ANIM_SPEED) {
      t->facingRight = (dx > 0) ? 1 : 0;
      t->state = TRADER_TRADE_MENU;
      t->tradeMenuOpen = 1;
      t->frame = 0;
      t->stateTimer = 0;
    }
    break;

  case TRADER_TRADE_MENU:
    t->hoveredItem = getTradeHoveredItem(t->mouseX, t->mouseY, t);
    if (distance > TRADER_DETECTION_RADIUS + 100) {
      t->tradeMenuOpen = 0;
      t->state = TRADER_WALK_BACK;
      t->facingRight = (t->initialX > t->x) ? 1 : 0;
      t->frame = 0;
      t->stateTimer = 0;
    }
    break;

  case TRADER_TRADING:
    if (t->stateTimer >= TRADER_TRADE_FRAMES * TRADER_ANIM_SPEED * 8) {
      t->state = TRADER_TRADE_MENU;
      t->tradeMenuOpen = 1;
      t->frame = 0;
      t->stateTimer = 0;
    }
    break;

  case TRADER_WALK_BACK:
    {
      int ddx = t->initialX - t->x;
      if (ddx > 0) {
        t->x += TRADER_WALK_SPEED;
        t->facingRight = 1;
      } else if (ddx < 0) {
        t->x -= TRADER_WALK_SPEED;
        t->facingRight = 0;
      }
      if (abs(ddx) < TRADER_WALK_SPEED + 2) {
        t->x = t->initialX;
        t->state = TRADER_IDLE_STATE;
        t->frame = 0;
        t->stateTimer = 0;
      }
    }
    break;

  case TRADER_WALK_AWAY:
    if (t->facingRight)
      t->x += TRADER_WALK_SPEED;
    else
      t->x -= TRADER_WALK_SPEED;

    if (t->x < -200 || t->x > TOTAL_BG_WIDTH + 200 ||
        t->stateTimer > 200) {
      t->state = TRADER_DONE;
      t->active = 0;
    }
    break;

  case TRADER_DONE:
    break;
  default:
    break;
  }
}

void renderTraderNPC(struct TraderNPC *t, struct Camera *camera) {
  if (!t->active || t->state == TRADER_DONE)
    return;

  unsigned int tex = 0;

  switch (t->state) {
  case TRADER_IDLE_STATE:
  case TRADER_PROMPT_INTERACT:
  case TRADER_SHOW_KEY:
  case TRADER_TRADE_MENU:
    tex = traderIdle[t->frame % TRADER_IDLE_FRAMES];
    break;
  case TRADER_WALK_TO_PLAYER:
  case TRADER_WALK_BACK:
    if (t->facingRight)
      tex = traderWalkR[t->frame % TRADER_WALK_R_FRAMES];
    else
      tex = traderWalkL[t->frame % TRADER_WALK_L_FRAMES];
    break;
  case TRADER_TURN_STATE:
    tex = traderTurn[t->frame % TRADER_TURN_FRAMES];
    break;
  case TRADER_TRADING:
    tex = traderTrade[t->frame % TRADER_TRADE_FRAMES];
    break;
  case TRADER_WALK_AWAY:
    if (t->facingRight)
      tex = traderWalkR[t->frame % TRADER_WALK_R_FRAMES];
    else
      tex = traderWalkL[t->frame % TRADER_WALK_L_FRAMES];
    break;
  default:
    tex = traderIdle[0];
    break;
  }

  if (tex != 0) {
    float screenX = getScreenX((float)t->x, camera);
    float screenY = getScreenY((float)t->y, camera);
    iShowImage(screenX, screenY, TRADER_SIZE, TRADER_SIZE, tex);
  }

  if (t->state == TRADER_PROMPT_INTERACT) {
    float screenX = getScreenX((float)t->x, camera);
    float screenY = getScreenY((float)t->y, camera);
    iSetColor(255, 255, 255);
    iText((int)(screenX - 30), (int)(screenY + TRADER_SIZE + 15),
          "Enter E to trade", GLUT_BITMAP_HELVETICA_18);
  }
}

void renderTradeMenu(struct TraderNPC *t, struct Player *player) {
  if (!t->tradeMenuOpen) return;

  if (tradeListTex != 0) {
    iShowImage(TRADE_LIST_X, TRADE_LIST_Y, TRADE_LIST_W, TRADE_LIST_H, tradeListTex);
  }

  if (tradeFragmentTex != 0) {
    iShowImage(TRADE_FRAGMENT_X, TRADE_FRAGMENT_Y, TRADE_FRAGMENT_W, TRADE_FRAGMENT_H, tradeFragmentTex);
  }
  char fragText[32];
  sprintf_s(fragText, "%d", player->fragments);
  iSetColor(255, 255, 255); 
  iText(TRADE_TOTAL_FRAGMENT_X, TRADE_TOTAL_FRAGMENT_Y, fragText, GLUT_BITMAP_TIMES_ROMAN_24);


  if (!t->tradedSwiftness && tradeSwiftnessTex != 0) {
    int yOff = (t->hoveredItem == 1) ? TRADE_HOVER_OFFSET : 0;
    iShowImage(TRADE_SWIFTNESS_X1, TRADE_SWIFTNESS_Y1 + yOff,
               TRADE_ITEM_W, TRADE_ITEM_H, tradeSwiftnessTex);
    if (tradeFragmentTex != 0 && TRADE_COST_ICON_W > 0) {
      iShowImage(TRADE_SWIFTNESS_X1 + 20, TRADE_SWIFTNESS_Y1 + TRADE_COST_ICON_Y_OFFSET,
                 TRADE_COST_ICON_W, TRADE_COST_ICON_H, tradeFragmentTex);
    }
  }

  if (!t->tradedSoul && tradeSoulTex != 0) {
    int yOff = (t->hoveredItem == 2) ? TRADE_HOVER_OFFSET : 0;
    iShowImage(TRADE_SOUL_X1, TRADE_SOUL_Y1 + yOff,
               TRADE_ITEM_W, TRADE_ITEM_H, tradeSoulTex);
    if (tradeFragmentTex != 0 && TRADE_COST_ICON_W > 0) {
      iShowImage(TRADE_SOUL_X1 + 20, TRADE_SOUL_Y1 + TRADE_COST_ICON_Y_OFFSET,
                 TRADE_COST_ICON_W, TRADE_COST_ICON_H, tradeFragmentTex);
    }
  }

  if (!t->tradedKey && tradeKeyImageTex != 0) {
    int yOff = (t->hoveredItem == 3) ? TRADE_HOVER_OFFSET : 0;
    iShowImage(TRADE_KEY_X1, TRADE_KEY_Y1 + yOff,
               TRADE_ITEM_W, TRADE_ITEM_H, tradeKeyImageTex);
    if (tradeFragmentTex != 0 && TRADE_COST_ICON_W > 0) {
      iShowImage(TRADE_KEY_X1 + 20, TRADE_KEY_Y1 + TRADE_COST_ICON_Y_OFFSET,
                 TRADE_COST_ICON_W, TRADE_COST_ICON_H, tradeFragmentTex);
    }
  }

  if (t->hoveredItem == 1 && tradeDesc1Tex != 0) {
    iShowImage(TRADE_DESC_X, TRADE_DESC_Y, TRADE_DESC_W, TRADE_DESC_H, tradeDesc1Tex);
  }
  if (t->hoveredItem == 2 && tradeDesc2Tex != 0) {
    iShowImage(TRADE_DESC_X, TRADE_DESC_Y, TRADE_DESC_W, TRADE_DESC_H, tradeDesc2Tex);
  }
  if (t->hoveredItem == 3 && tradeDesc3Tex != 0) {
    iShowImage(TRADE_DESC_X, TRADE_DESC_Y, TRADE_DESC_W, TRADE_DESC_H, tradeDesc3Tex);
  }
}

void renderEquippedIcons(struct Player *player) {
  int iconIdx = 0;

  if (player->hasSwiftness && !player->swiftnessUsed && swiftnessIconTex != 0) {
    int ix = EQUIPPED_ICON_X - iconIdx * EQUIPPED_ICON_SPACING;
    int iy = EQUIPPED_ICON_Y;
    iShowImage(ix, iy, EQUIPPED_ICON_SIZE, EQUIPPED_ICON_SIZE, swiftnessIconTex);
    iconIdx++;
  }

  if (player->hasSoul && !player->soulUsed && soulIconTex != 0) {
    int ix = EQUIPPED_ICON_X - iconIdx * EQUIPPED_ICON_SPACING;
    int iy = EQUIPPED_ICON_Y;
    iShowImage(ix, iy, EQUIPPED_ICON_SIZE, EQUIPPED_ICON_SIZE, soulIconTex);
    iconIdx++;
  }

  if (player->hasKeyItem && !player->keyUsed && keyIconTex != 0) {
    int ix = EQUIPPED_ICON_X - iconIdx * EQUIPPED_ICON_SPACING;
    int iy = EQUIPPED_ICON_Y;
    iShowImage(ix, iy, EQUIPPED_ICON_SIZE, EQUIPPED_ICON_SIZE, keyIconTex);
    iconIdx++;
  }
}

void handleEquippedIconClick(struct Player *player, int mx, int my) {
  int iconIdx = 0;

  if (player->hasSwiftness && !player->swiftnessUsed) {
    int ix = EQUIPPED_ICON_X - iconIdx * EQUIPPED_ICON_SPACING;
    int iy = EQUIPPED_ICON_Y;
    if (mx >= ix && mx <= ix + EQUIPPED_ICON_SIZE &&
        my >= iy && my <= iy + EQUIPPED_ICON_SIZE) {
      player->swiftnessUsed = 1;
      player->swiftnessActive = 1;
      player->speedMultiplier = SWIFTNESS_SPEED_MULTIPLIER;
      printf("Swiftness activated! Speed x1.5\n");
    }
    iconIdx++;
  }

  if (player->hasSoul && !player->soulUsed) {
    int ix = EQUIPPED_ICON_X - iconIdx * EQUIPPED_ICON_SPACING;
    int iy = EQUIPPED_ICON_Y;
    if (mx >= ix && mx <= ix + EQUIPPED_ICON_SIZE &&
        my >= iy && my <= iy + EQUIPPED_ICON_SIZE) {
      player->soulUsed = 1;
      player->soulActive = 1;
      printf("Soul activated! Heal on minion kill\n");
    }
    iconIdx++;
  }

  if (player->hasKeyItem && !player->keyUsed) {
    int ix = EQUIPPED_ICON_X - iconIdx * EQUIPPED_ICON_SPACING;
    int iy = EQUIPPED_ICON_Y;
    if (mx >= ix && mx <= ix + EQUIPPED_ICON_SIZE &&
        my >= iy && my <= iy + EQUIPPED_ICON_SIZE) {
      player->keyUsed = 1;
      player->hasUsedKey = 1; 
      printf("Key used! Door unlocked\n");
    }
    iconIdx++;
  }
}

void initBossDoor(struct BossDoor *door) {
  door->x = DOOR_X;
  door->y = DOOR_Y;
  door->frame = 0;
  door->animTimer = 0;
  door->locked = 1;
  door->opening = 0;
  door->opened = 0;
}

void updateBossDoor(struct BossDoor *door, struct Player *player) {
  if (door->opening) {
    door->animTimer++;
    if (door->animTimer >= DOOR_ANIM_SPEED) {
      door->animTimer = 0;
      door->frame++;
      if (door->frame >= DOOR_OPEN_FRAMES) {
        door->frame = DOOR_OPEN_FRAMES - 1;
        door->opened = 1;
        door->opening = 0;
      }
    }
  }

  if (door->locked && player->keyUsed) {
    int ddx = player->x - door->x;
    if (abs(ddx) < 120) {
      door->locked = 0;
      door->opening = 1;
      door->frame = 0;
      door->animTimer = 0;
    }
  }
}

void renderBossDoor(struct BossDoor *door, struct Camera *camera) {
  float screenX = getScreenX((float)door->x, camera);
  float screenY = getScreenY((float)door->y, camera);

  unsigned int tex = 0;
  if (door->opened || door->opening) {
    tex = doorOpenTex[door->frame % DOOR_OPEN_FRAMES];
  } else {
    tex = doorLockedTex[door->frame % DOOR_LOCKED_FRAMES];
  }

  if (tex != 0) {
    iShowImage(screenX, screenY, DOOR_W, DOOR_H, tex);
  }
}

#endif
