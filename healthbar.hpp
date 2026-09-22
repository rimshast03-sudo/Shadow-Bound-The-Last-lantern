#ifndef HEALTHBAR_HPP
#define HEALTHBAR_HPP

#include "config.hpp"
#include "iGraphics.h"
#include "structs.hpp"
#include <stdio.h>

unsigned int healthBarTextures[6];

void initHealthBar() {
  char filename[100];
  for (int i = 0; i < 6; i++) {
    sprintf_s(filename, sizeof(filename), "Assets\\UI\\Healthbar\\health (%d).png", i + 1);
    healthBarTextures[i] = iLoadImage(filename);
  }
}

void renderHealthBar(struct Player *player) {
  if (player->maxHealth <= 0)
    return; 

  float healthPercent = (float)player->health / (float)player->maxHealth;
  int imageIndex = 0;

  if (player->health >= player->maxHealth)
    imageIndex = 0; 
  else if (player->health >= player->maxHealth * 0.8)
    imageIndex = 1; 
  else if (player->health >= player->maxHealth * 0.6)
    imageIndex = 2; 
  else if (player->health >= player->maxHealth * 0.4)
    imageIndex = 3; 
  else if (player->health >= 5)
    imageIndex = 4; 
  else
    imageIndex = 5; 

  iShowImage(UI_BAR_X, UI_BAR_Y, UI_BAR_WIDTH, UI_BAR_HEIGHT, healthBarTextures[imageIndex]);
}
#endif 
