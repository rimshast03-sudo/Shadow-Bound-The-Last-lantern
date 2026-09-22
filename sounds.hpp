#ifndef SOUNDS_HPP
#define SOUNDS_HPP

#include "config.hpp"
#include "structs.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <mmsystem.h>
#include <windows.h>

#pragma comment(lib, "winmm.lib")

#define FOOTSTEP_LEVEL1_COUNT 8
#define FOOTSTEP_LEVEL2_COUNT 7
#define FOOTSTEP_TILE_COUNT 5
#define FOOTSTEP_INTERVAL 12

static int g_footstepTimer = 0;
static int g_bgMusicPlaying = 0;
static int g_lastHealthForSound = PLAYER_MAX_HEALTH;
static int g_lastHoveredButton = -1;

struct WavSound {
  WAVEFORMATEX fmt;
  BYTE *pcm;
  DWORD pcmSize;
  HWAVEOUT hWave;
  WAVEHDR hdr;
};

static void loadWavSound(const char *path, WavSound *snd) {
  snd->pcm = NULL;
  snd->pcmSize = 0;

  HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    return;

  DWORD fileSize = GetFileSize(hFile, NULL);
  BYTE *buf = (BYTE *)HeapAlloc(GetProcessHeap(), 0, fileSize);
  if (!buf) {
    CloseHandle(hFile);
    return;
  }

  DWORD bytesRead = 0;
  ReadFile(hFile, buf, fileSize, &bytesRead, NULL);
  CloseHandle(hFile);

  DWORD pos = 12;
  while (pos + 8 <= fileSize) {
    DWORD chunkSize = *(DWORD *)(buf + pos + 4);
    if (memcmp(buf + pos, "fmt ", 4) == 0 && chunkSize >= 16) {
      WAVEFORMATEX *wf = (WAVEFORMATEX *)(buf + pos + 8);
      snd->fmt = *wf;
      snd->fmt.cbSize = 0;
    } else if (memcmp(buf + pos, "data", 4) == 0) {
      snd->pcm = (BYTE *)HeapAlloc(GetProcessHeap(), 0, chunkSize);
      snd->pcmSize = chunkSize;
      if (snd->pcm)
        memcpy(snd->pcm, buf + pos + 8, chunkSize);
    }
    pos += 8 + ((chunkSize + 1) & ~1u);
  }
  HeapFree(GetProcessHeap(), 0, buf);

  if (snd->pcm) {
    if (waveOutOpen(&snd->hWave, WAVE_MAPPER, &snd->fmt, 0, 0, CALLBACK_NULL) ==
        MMSYSERR_NOERROR) {
      ZeroMemory(&snd->hdr, sizeof(WAVEHDR));
      snd->hdr.lpData = (LPSTR)snd->pcm;
      snd->hdr.dwBufferLength = snd->pcmSize;
      waveOutPrepareHeader(snd->hWave, &snd->hdr, sizeof(WAVEHDR));
      snd->hdr.dwFlags |= WHDR_DONE;
    }
  }
}

static void playWavSound(WavSound *snd) {
  if (!snd || !snd->hWave)
    return;
  if (!(snd->hdr.dwFlags & WHDR_DONE)) {
    waveOutReset(snd->hWave);
  }
  waveOutWrite(snd->hWave, &snd->hdr, sizeof(WAVEHDR));
}

static WavSound g_sndJump, g_sndFall, g_sndLand, g_sndDash;
static WavSound g_sndAttack, g_sndDownslash, g_sndDeath, g_sndEnemyKill;
static WavSound g_sndOnButton, g_sndButtonClick, g_sndStartClick;
static WavSound g_sndItemCollect, g_sndLoseLife;
static WavSound g_sndBg;
static WavSound g_sndBg2;
static WavSound g_sndBossBg;
static WavSound g_sndFootL1[FOOTSTEP_LEVEL1_COUNT];
static WavSound g_sndFootL2[FOOTSTEP_LEVEL2_COUNT];
static WavSound g_sndFootTile[FOOTSTEP_TILE_COUNT];

void applyVolume(WavSound *snd, float volumeFact) {
  if (!snd || !snd->pcm)
    return;
  if (snd->fmt.wBitsPerSample == 16) {
    short *samples = (short *)snd->pcm;
    DWORD count = snd->pcmSize / 2;
    for (DWORD i = 0; i < count; i++) {
      float val = samples[i] * volumeFact;
      if (val > 32767.0f)
        val = 32767.0f;
      if (val < -32768.0f)
        val = -32768.0f;
      samples[i] = (short)val;
    }
  } else if (snd->fmt.wBitsPerSample == 8) {
    BYTE *samples = snd->pcm;
    for (DWORD i = 0; i < snd->pcmSize; i++) {
      float val = (samples[i] - 128) * volumeFact;
      val += 128.0f;
      if (val > 255.0f)
        val = 255.0f;
      if (val < 0.0f)
        val = 0.0f;
      samples[i] = (BYTE)val;
    }
  }
}

static HANDLE hBgThread = NULL;
static volatile int g_stopBgThread = 0;

static DWORD WINAPI BgMusicThreadProc(LPVOID p) {
  if (!g_sndBg.hWave)
    return 0;

  while (!g_stopBgThread) {
    if (g_sndBg.hdr.dwFlags & WHDR_DONE) {
      waveOutWrite(g_sndBg.hWave, &g_sndBg.hdr, sizeof(WAVEHDR));
    }
    Sleep(10);
  }
  return 0;
}

void playBGMusic() {
  if (hBgThread)
    return;
  if (!g_sndBg.pcm)
    return;

  g_stopBgThread = 0;
  hBgThread = CreateThread(NULL, 0, BgMusicThreadProc, NULL, 0, NULL);
  g_bgMusicPlaying = 1;
}

void stopBGMusic() {
  if (hBgThread) {
    g_stopBgThread = 1;
    waveOutReset(g_sndBg.hWave);
    WaitForSingleObject(hBgThread, INFINITE);
    CloseHandle(hBgThread);
    hBgThread = NULL;
  }
  g_bgMusicPlaying = 0;
}

void restartBGMusic() {
  stopBGMusic();
  playBGMusic();
}

static HANDLE hBg2Thread = NULL;
static volatile int g_stopBg2Thread = 0;

static DWORD WINAPI Bg2MusicThreadProc(LPVOID p) {
  if (!g_sndBg2.hWave)
    return 0;

  while (!g_stopBg2Thread) {
    if (g_sndBg2.hdr.dwFlags & WHDR_DONE) {
      waveOutWrite(g_sndBg2.hWave, &g_sndBg2.hdr, sizeof(WAVEHDR));
    }
    Sleep(10);
  }
  return 0;
}

void playBG2Music() {
  if (hBg2Thread)
    return;
  if (!g_sndBg2.pcm)
    return;

  g_stopBg2Thread = 0;
  hBg2Thread = CreateThread(NULL, 0, Bg2MusicThreadProc, NULL, 0, NULL);
}

void stopBG2Music() {
  if (hBg2Thread) {
    g_stopBg2Thread = 1;
    waveOutReset(g_sndBg2.hWave);
    WaitForSingleObject(hBg2Thread, INFINITE);
    CloseHandle(hBg2Thread);
    hBg2Thread = NULL;
  }
}

void restartBG2Music() {
  stopBG2Music();
  playBG2Music();
}

static HANDLE hBossBgThread = NULL;
static volatile int g_stopBossBgThread = 0;

static DWORD WINAPI BossBgMusicThreadProc(LPVOID p) {
  if (!g_sndBossBg.hWave)
    return 0;

  while (!g_stopBossBgThread) {
    if (g_sndBossBg.hdr.dwFlags & WHDR_DONE) {
      waveOutWrite(g_sndBossBg.hWave, &g_sndBossBg.hdr, sizeof(WAVEHDR));
    }
    Sleep(10);
  }
  return 0;
}

void playBossBGMusic() {
  if (hBossBgThread)
    return;
  if (!g_sndBossBg.pcm)
    return;

  g_stopBossBgThread = 0;
  hBossBgThread = CreateThread(NULL, 0, BossBgMusicThreadProc, NULL, 0, NULL);
}

void stopBossBGMusic() {
  if (hBossBgThread) {
    g_stopBossBgThread = 1;
    waveOutReset(g_sndBossBg.hWave);
    WaitForSingleObject(hBossBgThread, INFINITE);
    CloseHandle(hBossBgThread);
    hBossBgThread = NULL;
  }
}

void restartBossBGMusic() {
  stopBossBGMusic();
  playBossBGMusic();
}

void playBossEntryMusic() {
  mciSendStringA("open \"Audios/Background/bossentrybg.mp3\" type mpegvideo "
                 "alias bossentry",
                 NULL, 0, NULL);
  mciSendStringA("play bossentry from 0", NULL, 0, NULL);
}

void stopBossEntryMusic() {
  mciSendStringA("stop bossentry", NULL, 0, NULL);
  mciSendStringA("close bossentry", NULL, 0, NULL);
}

void playGameEndingMusic() {
  mciSendStringA(
      "open \"Audios/Background/ending1.wav\" type waveaudio alias gameending",
      NULL, 0, NULL);
  mciSendStringA("play gameending from 0", NULL, 0, NULL);
}

void stopGameEndingMusic() {
  mciSendStringA("stop gameending", NULL, 0, NULL);
  mciSendStringA("close gameending", NULL, 0, NULL);
}

static WavSound g_sndCongrats;
static HANDLE hCongratsThread = NULL;
static volatile int g_stopCongratsThread = 0;

static DWORD WINAPI CongratsMusicThreadProc(LPVOID p) {
  if (!g_sndCongrats.hWave)
    return 0;

  while (!g_stopCongratsThread) {
    if (g_sndCongrats.hdr.dwFlags & WHDR_DONE) {
      waveOutWrite(g_sndCongrats.hWave, &g_sndCongrats.hdr, sizeof(WAVEHDR));
    }
    Sleep(10);
  }
  return 0;
}

void playCongratsMusic() {
  if (hCongratsThread)
    return;
  if (!g_sndCongrats.pcm)
    return;

  g_stopCongratsThread = 0;
  hCongratsThread =
      CreateThread(NULL, 0, CongratsMusicThreadProc, NULL, 0, NULL);
}

void stopCongratsMusic() {
  if (hCongratsThread) {
    g_stopCongratsThread = 1;
    waveOutReset(g_sndCongrats.hWave);
    WaitForSingleObject(hCongratsThread, INFINITE);
    CloseHandle(hCongratsThread);
    hCongratsThread = NULL;
  }
}

void initSounds() {
  g_footstepTimer = 0;
  g_lastHealthForSound = PLAYER_MAX_HEALTH;
  g_lastHoveredButton = -1;

  loadWavSound("Audios/movements/move/jump.wav", &g_sndJump);
  loadWavSound("Audios/movements/move/jump.wav", &g_sndFall);
  loadWavSound("Audios/movements/move/land.wav", &g_sndLand);
  loadWavSound("Audios/movements/move/dash.wav", &g_sndDash);
  loadWavSound("Audios/movements/move/attack.wav", &g_sndAttack);
  loadWavSound("Audios/movements/move/downslash.wav", &g_sndDownslash);
  loadWavSound("Audios/movements/move/death.wav", &g_sndDeath);
  loadWavSound("Audios/movements/move/enemykill.wav", &g_sndEnemyKill);

  loadWavSound("Audios/Background/bg.wav", &g_sndBg);
  applyVolume(&g_sndBg, 0.6f);

  loadWavSound("Audios/Background/bg2.wav", &g_sndBg2);
  applyVolume(&g_sndBg2, 0.6f);

  loadWavSound("Audios/Background/bossbg.wav", &g_sndBossBg);
  applyVolume(&g_sndBossBg, 0.6f);

  loadWavSound("Audios/Background/ending2.wav", &g_sndCongrats);
  applyVolume(&g_sndCongrats, 0.6f);

  loadWavSound("Audios/UI/on_button.wav", &g_sndOnButton);
  loadWavSound("Audios/UI/button_click.wav", &g_sndButtonClick);
  loadWavSound("Audios/UI/start_button_click.wav", &g_sndStartClick);
  loadWavSound("Audios/UI/item_collect.wav", &g_sndItemCollect);
  loadWavSound("Audios/UI/lose_life.wav", &g_sndLoseLife);

  char path[128];
  for (int i = 0; i < FOOTSTEP_LEVEL1_COUNT; i++) {
    sprintf_s(path,
              "Audios/movements/footsteps/level 1/footstep_level1(%d).wav",
              i + 1);
    loadWavSound(path, &g_sndFootL1[i]);
  }
  for (int i = 0; i < FOOTSTEP_LEVEL2_COUNT; i++) {
    sprintf_s(path,
              "Audios/movements/footsteps/level 2/footstep_level2(%d).wav",
              i + 1);
    loadWavSound(path, &g_sndFootL2[i]);
  }
  for (int i = 0; i < FOOTSTEP_TILE_COUNT; i++) {
    sprintf_s(path, "Audios/movements/footsteps/tile/footstep_tile(%d).wav",
              i + 1);
    loadWavSound(path, &g_sndFootTile[i]);
  }
}

void playJumpSound() { playWavSound(&g_sndJump); }
void playFallSound() { playWavSound(&g_sndFall); }
void playLandSound() { playWavSound(&g_sndLand); }
void playDashSound() { playWavSound(&g_sndDash); }
void playAttackSound() { playWavSound(&g_sndAttack); }
void playDownslashSound() { playWavSound(&g_sndDownslash); }
void playDeathSound() { playWavSound(&g_sndDeath); }
void playEnemyKillSound() { playWavSound(&g_sndEnemyKill); }
void playOnButtonSound() { playWavSound(&g_sndOnButton); }
void playButtonClickSound() { playWavSound(&g_sndButtonClick); }
void playStartButtonClickSound() { playWavSound(&g_sndStartClick); }
void playItemCollectSound() { playWavSound(&g_sndItemCollect); }
void playLoseLifeSound() { playWavSound(&g_sndLoseLife); }

void playFootstepLevel1() {
  playWavSound(&g_sndFootL1[rand() % FOOTSTEP_LEVEL1_COUNT]);
}
void playFootstepLevel2() {
  playWavSound(&g_sndFootL2[rand() % FOOTSTEP_LEVEL2_COUNT]);
}
void playFootstepTile() {
  playWavSound(&g_sndFootTile[rand() % FOOTSTEP_TILE_COUNT]);
}

int isPlayerOnTile(struct Player *player, struct Midground *mg) {
  int spriteW = PLAYER_WIDTH;
  int playerLeft = player->x;
  int playerRight = player->x + spriteW;
  int playerBottom = player->y;
  for (int i = 0; i < mg->tileCount; i++) {
    if (!mg->tiles[i].active)
      continue;
    int tileLeft = mg->tiles[i].x;
    int tileRight = mg->tiles[i].x + (int)mg->tiles[i].width;
    int tileTop = mg->tiles[i].y + (int)mg->tiles[i].height;
    int margin = 40;
    if (playerRight > tileLeft + margin && playerLeft < tileRight - margin)
      if (abs(playerBottom - tileTop) <= 100 ||
          abs(playerBottom - (tileTop + 20)) <= 100)
        return 1;
  }
  return 0;
}

void updateFootstepSounds(struct Player *player, struct Midground *mg,
                          int gameState) {
  // Shadow Bound 5's Player has no animation-state enum (WALK/JUMP/etc.) —
  // just a "moving" flag — so footsteps play whenever the player is
  // actually walking on the ground.
  if (!player->moving || !player->onGround) {
    g_footstepTimer = FOOTSTEP_INTERVAL;
    return;
  }
  if (++g_footstepTimer >= FOOTSTEP_INTERVAL) {
    g_footstepTimer = 0;
    if (isPlayerOnTile(player, mg))
      playFootstepTile();
    else if (gameState == PLAYING_STATE)
      playFootstepLevel1();
    else if (gameState == LEVEL2_STATE)
      playFootstepLevel2();
    else if (gameState == LEVEL3_STATE)
      playFootstepLevel2();
  }
}

void updateHealthSound(struct Player *player) {
  if (player->health < g_lastHealthForSound && player->health > 0)
    playLoseLifeSound();
  g_lastHealthForSound = player->health;
}

int getTitleButtonAtPos(int mx, int my) {
  if (mx >= 400 && mx <= 600 && my >= 200 && my <= 280)
    return 0;
  if (mx >= 400 && mx <= 600 && my >= 150 && my <= 190)
    return 1;
  if (mx >= 400 && mx <= 600 && my >= 100 && my <= 140)
    return 2;
  if (mx >= 400 && mx <= 600 && my >= 40 && my <= 100)
    return 3;
  return -1;
}

void updateButtonHoverSound(int mx, int my) {
  int btn = getTitleButtonAtPos(mx, my);
  if (btn != -1 && btn != g_lastHoveredButton)
    playOnButtonSound();
  g_lastHoveredButton = btn;
}

#endif
