#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "camera.hpp"
#include "iGraphics.h"
#include "structs.hpp"
#include "textures.hpp"

void initBackground(struct Background *bg) { bg->x = 0; }

void updateBackground(struct Background *bg, struct Player *player) {
}

void renderBackground(struct Background *bg) {
  int startX = (bg->x / BG_WIDTH) * BG_WIDTH;

  for (int i = -1; i <= 1; i++) {
    int drawX = startX + (i * BG_WIDTH) + bg->x % BG_WIDTH;
    iShowImage(
        drawX, 0, BG_WIDTH, BG_HEIGHT,
        bg->texture);
  }
}

void renderBackgroundWithCamera(struct Camera *camera,
                                unsigned int *textures) {
  for (int segment = 0; segment < BG_SEGMENTS; segment++) {
    int worldX = segment * BG_WIDTH;
    float screenX = getScreenX(worldX, camera);
    float screenY = getScreenY(0, camera);

    if (screenX > -BG_WIDTH && screenX < SCREEN_W) {
      if (textures[segment] != 0) {
        iShowImage(screenX, screenY, BG_WIDTH, BG_HEIGHT, textures[segment]);
      }
    }
  }
}

void renderTunnelBackground(int phase, int offset) {
  if (phase == 0) {
    if (tunnelTextures[0] != 0) iShowImage(0, 0, SCREEN_W, SCREEN_H, tunnelTextures[0]);
  } else if (phase == 1) {
    if (tunnelTextures[1] != 0) iShowImage(0, 0, SCREEN_W, SCREEN_H, tunnelTextures[1]);
  } else if (phase == 2) {
    if (tunnelTextures[0] != 0) iShowImage(0, offset, SCREEN_W, SCREEN_H, tunnelTextures[0]);
    if (tunnelTextures[1] != 0) iShowImage(0, offset - SCREEN_H, SCREEN_W, SCREEN_H, tunnelTextures[1]);
  }
}

void renderCaveBackground() {
  if (caveTexture != 0) {
    iShowImage(0, 0, SCREEN_W, SCREEN_H, caveTexture);
  }
}

#endif
