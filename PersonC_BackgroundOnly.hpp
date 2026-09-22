// ============================================================
// PersonC_BackgroundOnly.hpp
// Person C — just the Camera and Level 1 Background.
// No player, no platforms, no collision — camera auto-scrolls
// on its own so you can see the background move without needing
// anyone else's code.
// ============================================================
#ifndef PERSONC_BACKGROUND_ONLY_HPP
#define PERSONC_BACKGROUND_ONLY_HPP

#include "structs.hpp"
#include "iGraphics.h"
#include "player.hpp"   // needed for Camera::follow(Player&)

// ------------------------------------------------------------
// CAMERA — auto-scrolls left/right on its own (no player needed)
// ------------------------------------------------------------
class Camera {
private:
    float x, y;
    float direction; // 1 = moving right, -1 = moving left

public:
    Camera() {
        x = 0.0f;
        y = 0.0f;
        direction = 1.0f;
    }

    // Slowly scrolls across the level and bounces at each end,
    // so you can see every background segment without a player.
    // Keeps the player roughly centered, without scrolling past
    // either end of the level. This replaces autoScroll() now that
    // there's a real player to follow.
    void follow(Player &player) {
        float speed = 0.1f;
        float desiredX = (float)player.x - (SCREEN_W / 2.0f);
        float rightEdge = (float)TOTAL_BG_WIDTH - SCREEN_W;

        if (desiredX < 0.0f) desiredX = 0.0f;
        if (desiredX > rightEdge) desiredX = rightEdge;

        x += (desiredX - x) * speed;
    }

    // Kept for reference / testing without a player — no longer
    // used in the normal game loop.
    void autoScroll() {
        float speed = 2.0f;
        float rightEdge = (float)TOTAL_BG_WIDTH - SCREEN_W;

        x += speed * direction;

        if (x >= rightEdge) {
            x = rightEdge;
            direction = -1.0f;
        }
        if (x <= 0.0f) {
            x = 0.0f;
            direction = 1.0f;
        }
    }

    float toScreenX(float worldX) const { return worldX - x; }
    float toScreenY(float worldY) const { return worldY - y; }
};


// ------------------------------------------------------------
// LEVEL 1 BACKGROUND — the 5-segment scrolling background
// ------------------------------------------------------------
class Level1Background {
private:
    unsigned int textures[BG_SEGMENTS];

public:
    Level1Background() {
        for (int i = 0; i < BG_SEGMENTS; i++) {
            textures[i] = 0;
        }
    }

    // Loads Assets/Background/bg (1..5).png
    void load() {
        char path[128];
        for (int i = 0; i < BG_SEGMENTS; i++) {
            sprintf_s(path, sizeof(path), "Assets/Background/bg (%d).png", i + 1);
            textures[i] = iLoadImage(path);
        }
    }

    void draw(Camera &cam) {
        for (int segment = 0; segment < BG_SEGMENTS; segment++) {
            int worldX = segment * BG_WIDTH;

            float screenX = cam.toScreenX((float)worldX);
            float screenY = cam.toScreenY(0.0f);

            bool onScreen = (screenX > -BG_WIDTH) && (screenX < SCREEN_W);
            bool hasTexture = (textures[segment] != 0);

            if (onScreen && hasTexture) {
                iShowImage((int)screenX, (int)screenY, BG_WIDTH, BG_HEIGHT, textures[segment]);
            }
        }
    }
};

#endif
