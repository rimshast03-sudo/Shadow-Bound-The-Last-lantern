#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "structs.hpp"
#include <math.h>

void initCamera(struct Camera *camera) {
  camera->x = 0;
  camera->y = 0;
  camera->targetX = 0;
  camera->targetY = 0;
}

void updateCamera(struct Camera *camera, struct Player *player) {
  float playerScreenX = player->x - camera->x;
  float targetX = (float)(player->x - SCREEN_W / 2); 
  float targetY = 0; 
  if (targetX < 0)
    targetX = 0;
  if (targetX > TOTAL_BG_WIDTH - SCREEN_W)
    targetX = TOTAL_BG_WIDTH - SCREEN_W;
  camera->x += (float)((targetX - camera->x) * CAMERA_FOLLOW_SPEED);
  camera->y = targetY; 
}

float getScreenX(float worldX, struct Camera *camera) {
  return worldX - camera->x;
}

float getScreenY(float worldY, struct Camera *camera) {
  return worldY - camera->y;
}
#endif 
