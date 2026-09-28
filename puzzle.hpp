#ifndef PUZZLE_HPP
#define PUZZLE_HPP

// "Twin Switch Gate" puzzle (Level 3)
//
// Two pressure switches (puzzleSwitches[0], puzzleSwitches[1]) sit on the
// floor of the puzzle room. One is pressed by the player standing on it,
// the other by shoving the push block onto it. Once both are pressed at
// the same time, puzzleGate unlocks and plays its open animation, the
// same way bossDoor does for the boss room (see tradernpc.hpp).
//
// Written in the same style as the rest of the project: plain structs,
// unsigned int textures loaded via iLoadImage (see textures.hpp), and
// iShowImage() for drawing. Texture loading itself already lives in
// textures.hpp (loadPuzzleTextures) — this file only holds the gameplay
// logic and rendering for the puzzle objects.

#include "camera.hpp"
#include "config.hpp"
#include "iGraphics.h"
#include "structs.hpp"
#include "textures.hpp"
#include <windows.h>
#include <stdlib.h>

// ---------------------------------------------------------------------
// Init
// ---------------------------------------------------------------------

void initPuzzleSwitches(struct PuzzleSwitch switches[]) {
  switches[0].x = SWITCH_1_X;
  switches[0].y = SWITCH_1_Y;
  switches[0].pressed = 0;
  switches[0].frame = 0;
  switches[0].animTimer = 0;

  switches[1].x = SWITCH_2_X;
  switches[1].y = SWITCH_2_Y;
  switches[1].pressed = 0;
  switches[1].frame = 0;
  switches[1].animTimer = 0;
}

void initPushBlock(struct PushBlock *block) {
  block->x = PUSH_BLOCK_START_X;
  block->y = LEVEL3_GROUND_Y;
  block->vy = 0;
  block->grounded = 1;
  block->startX = PUSH_BLOCK_START_X;
}

void initPuzzleGate(struct PuzzleGate *gate) {
  gate->x = PUZZLE_GATE_X;
  gate->y = PUZZLE_GATE_Y;
  gate->frame = 0;
  gate->animTimer = 0;
  gate->locked = 1;
  gate->opening = 0;
  gate->opened = 0;
}

// ---------------------------------------------------------------------
// Update
// ---------------------------------------------------------------------

// Lets the player shove the block left/right along the flat puzzle-room
// floor. Approaching from one side while holding the matching arrow key
// pushes the block that way; otherwise the block just blocks movement
// like a solid wall. Movement is clamped to the puzzle room bounds.
//
// The block has to end up resting on whichever switch sits nearer the
// gate to solve the puzzle, which puts it directly between the player
// and the now-open gate. So once the gate has opened, the block stops
// colliding with the player entirely -- otherwise it permanently walls
// off the exit and the player can never get through.
void updatePushBlock(struct PushBlock *block, struct Player *player, struct PuzzleGate *gate) {
  if (gate->opened) {
    return;
  }

  int px1 = player->x;
  int px2 = player->x + PLAYER_WIDTH;
  int py1 = player->y;
  int py2 = player->y + PLAYER_HEIGHT;

  int bx1 = block->x;
  int bx2 = block->x + PUSH_BLOCK_W;
  int by1 = block->y;
  int by2 = block->y + PUSH_BLOCK_H;

  bool overlapY = (py1 < by2) && (py2 > by1);
  bool overlapX = (px1 < bx2) && (px2 > bx1);

  if (!overlapY || !overlapX)
    return;

  bool rightHeld = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;
  bool leftHeld = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;

  // Player approaching from the left, holding right -> push block right.
  if (rightHeld && px1 < bx1) {
    int newX = block->x + PUSH_BLOCK_SPEED;
    if (newX + PUSH_BLOCK_W <= PUZZLE_ROOM_MAX_X) {
      block->x = newX;
    }
    player->x = block->x - PLAYER_WIDTH;
  }
  // Player approaching from the right, holding left -> push block left.
  else if (leftHeld && px1 > bx1) {
    int newX = block->x - PUSH_BLOCK_SPEED;
    if (newX >= PUZZLE_ROOM_MIN_X) {
      block->x = newX;
    }
    player->x = block->x + PUSH_BLOCK_W;
  }
  // Not actively pushing -- block just behaves like a solid obstacle.
  else if (px1 < bx1) {
    player->x = bx1 - PLAYER_WIDTH;
  } else {
    player->x = bx2;
  }
}

// A switch is pressed while the player or the push block is resting on
// top of it (flat floor, so this is just a horizontal-range check).
void updatePuzzleSwitches(struct PuzzleSwitch switches[], struct Player *player, struct PushBlock *block) {
  for (int i = 0; i < MAX_PUZZLE_SWITCHES; i++) {
    int sx1 = switches[i].x;
    int sx2 = switches[i].x + SWITCH_W;

    int px1 = player->x;
    int px2 = player->x + PLAYER_WIDTH;
    bool playerOn = (px2 > sx1) && (px1 < sx2);

    int bx1 = block->x;
    int bx2 = block->x + PUSH_BLOCK_W;
    bool blockOn = (bx2 > sx1) && (bx1 < sx2);

    switches[i].pressed = (playerOn || blockOn) ? 1 : 0;
  }
}

// Opens once both switches are pressed at the same time and then latches
// open permanently -- once solved, walking off the switches (or shoving
// the block off) no longer relocks the gate.
void updatePuzzleGate(struct PuzzleGate *gate, struct PuzzleSwitch switches[]) {
  bool allPressed = true;
  for (int i = 0; i < MAX_PUZZLE_SWITCHES; i++) {
    if (!switches[i].pressed) {
      allPressed = false;
      break;
    }
  }

  if (gate->opening) {
    gate->animTimer++;
    if (gate->animTimer >= PUZZLE_GATE_ANIM_SPEED) {
      gate->animTimer = 0;
      gate->frame++;
      if (gate->frame >= PUZZLE_GATE_OPEN_FRAMES) {
        gate->frame = PUZZLE_GATE_OPEN_FRAMES - 1;
        gate->opened = 1;
        gate->opening = 0;
      }
    }
  }

  if (gate->locked && allPressed) {
    gate->locked = 0;
    gate->opening = 1;
    gate->frame = 0;
    gate->animTimer = 0;
  }
  // No relock branch: once opened, the gate stays open for the rest of
  // the level regardless of switch state.
}

// ---------------------------------------------------------------------
// Render
// ---------------------------------------------------------------------

void renderPuzzleSwitches(struct PuzzleSwitch switches[], struct Camera *camera) {
  for (int i = 0; i < MAX_PUZZLE_SWITCHES; i++) {
    float screenX = getScreenX((float)switches[i].x, camera);
    float screenY = getScreenY((float)switches[i].y, camera);

    unsigned int tex = switches[i].pressed ? switchPressedTex : switchUnpressedTex;
    if (tex != 0) {
      iShowImage((int)screenX, (int)screenY, SWITCH_W, SWITCH_H, tex);
    }
  }
}

void renderPushBlock(struct PushBlock *block, struct Camera *camera) {
  float screenX = getScreenX((float)block->x, camera);
  float screenY = getScreenY((float)block->y, camera);

  if (pushBlockTex != 0) {
    iShowImage((int)screenX, (int)screenY, PUSH_BLOCK_W, PUSH_BLOCK_H, pushBlockTex);
  }
}

void renderPuzzleGate(struct PuzzleGate *gate, struct Camera *camera) {
  float screenX = getScreenX((float)gate->x, camera);
  float screenY = getScreenY((float)gate->y, camera);

  unsigned int tex = 0;
  if (gate->opened || gate->opening) {
    tex = puzzleGateOpenTex[gate->frame % PUZZLE_GATE_OPEN_FRAMES];
  } else {
    tex = puzzleGateLockedTex[gate->frame % PUZZLE_GATE_LOCKED_FRAMES];
  }

  if (tex != 0) {
    iShowImage((int)screenX, (int)screenY, PUZZLE_GATE_W, PUZZLE_GATE_H, tex);
  }
}

#endif
