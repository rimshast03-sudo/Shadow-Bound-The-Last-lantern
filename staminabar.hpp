#ifndef STAMINABAR_HPP
#define STAMINABAR_HPP

#include "config.hpp"
#include "iGraphics.h"
#include "structs.hpp"
#include <stdio.h>

unsigned int staminaBarTextures[6];

void initStaminaBar() {
  char filename[100];
  for (int i = 0; i < 6; i++) {
    sprintf_s(filename, sizeof(filename),"Assets\\UI\\Staminabar\\stamina (%d).png", i + 1);
    staminaBarTextures[i] = iLoadImage(filename);
  }
}

void renderStaminaBar(struct Player *player) {
  if (player->maxStamina <= 0)
    return; 

  float staminaPercent = (float)player->stamina / (float)player->maxStamina;
  int imageIndex = 0;

  if (player->stamina >= player->maxStamina)
    imageIndex = 0;
  else if (player->stamina >= player->maxStamina * 0.8)
    imageIndex = 1;
  else if (player->stamina >= player->maxStamina * 0.6)
    imageIndex = 2;
  else if (player->stamina >= player->maxStamina * 0.4)
    imageIndex = 3;
  else if (player->stamina >= 20)
    imageIndex = 4;
  else
    imageIndex = 5;

  iShowImage(UI_BAR_X, UI_BAR_Y, UI_BAR_WIDTH, UI_BAR_HEIGHT, staminaBarTextures[imageIndex]);
}
#endif
