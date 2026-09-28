# Shadow Bound

A 2D side-scrolling action-platformer for Windows, written in C++ with OpenGL/GLUT on top of the **iGraphics** library. Fight through three levels, solve a switch-and-block puzzle, trade with a mysterious merchant, and face the final boss.

---

## Features

- **Three levels + boss fights**
  - **Level 1:** scrolling stage with creatures, ending in a cave encounter (Bugs → Goblins → Grim Master boss)
  - **Level 2:** scrolling stage with creatures and sentries
  - **Level 3:** trader NPC, "Twin Switch Gate" puzzle, Grim Master enemies, a key-locked boss door and the final boss
- **Final boss fight** with a cinematic entry (278-frame animation and music), multiple attacks (slash, dash, teleport, spikes/traps, fire bats), minions and a health bar
- **Two player attacks:** melee slash and arrow shot, plus dash, evade and down-stab
- **Stamina system:** dashing and down-stabbing cost stamina, which regenerates over time
- **Inventory and pickups:** health, stamina and power items dropped by enemies
- **Trader NPC (Level 3):** spend fragments on Swiftness and Soul, and pick up the boss-door Key
- **Puzzle:** press one switch yourself, push the block onto the other, and the gate opens
- **Full audio:** per-level music, footsteps per surface, combat and UI sounds
- **Title screen** with Start, Controls, Credits and Exit, plus loading, game-over and ending sequences

---

## Controls

| Input | Action |
|---|---|
| `←` / `→` | Move |
| `Space` | Jump |
| `Z` | Dash |
| `X` | Melee slash |
| `C` | Arrow attack (hold `↑` to aim up) |
| `2` | Use potion (double damage for a limited time) |
| `3` | Use power (homing glow projectile) |
| `4` | Use heart (health regeneration over time) |
| `5` | Use gain (stamina regeneration over time) |
| `6` | Use key near the Level 3 boss door |
| `E` | Open/close the trader menu (when near the trader) |
| Mouse | Select trader items, activate equipped Swiftness/Soul/Key icons, navigate the title screen |
| `Enter` / `Space` | Start game from title; continue after "Level 1 Cleared" |
| `Esc` | Quit |

---

## Building

**Requirements**

- Windows
- Visual Studio 2013 (toolset `v120`) or a compatible version with Win32 desktop C++ support
- Everything else (iGraphics, GLUT, GLAUX and the required `.lib` files) is bundled in the project folder

**Steps**

1. Open `Shadow Bound Merged.sln` in Visual Studio.
2. Select **Win32** with either **Debug** or **Release**.
3. **Build → Rebuild Solution.**
4. Run the game **from the project directory** so the relative `Assets/` and `Audios/` paths resolve correctly.

`GLUT32.DLL` must sit next to the executable (or on your `PATH`). A copy is included in the project folder.

The project links against `winmm.lib`, `opengl32.lib`, `glu32.lib`, `glut32.lib` and `glaux.lib`.

---

## Project Structure

```
Shadow bound final touch/
├── iMain.cpp            Entry point: game loop, state machine, input, rendering
├── iGraphics.h, glut.h, glaux.h, stb_image.h   Graphics/image libraries
├── config.hpp           All tunable constants (screen size, states, speeds, damage, positions)
├── structs.hpp          Player, enemy, boss, pickup and puzzle structs
├── textures.hpp         Texture loading for all sprites and backgrounds
├── sounds.hpp           Music and sound-effect handling
│
├── player.hpp           Player movement, attacks, dash, arrows
├── camera.hpp           Scrolling camera
├── background.hpp / midground.hpp   Parallax backgrounds and platform tiles
├── healthbar.hpp / staminabar.hpp / inventory.hpp   HUD, items and pickups
│
├── bug.hpp              Bug and creature enemies
├── Flyingcreature.hpp   Flying creature enemy
├── goblin.hpp           Goblin enemy (cave)
├── Sentry.hpp           Sentry enemy (Level 2)
├── LightningThrower.hpp Lightning-thrower enemy
├── cave.hpp / caveboss.hpp   Level 1 cave encounter and boss
├── boss.hpp             Level 1 cave boss manager
│
├── game.hpp             Level setup, respawn, boss-entry/fight transitions
├── tradernpc.hpp        Trader NPC, trade menu, boss door, equipped items
├── puzzle.hpp           Twin Switch Gate puzzle (switches, push block, gate)
├── grimmaster.hpp       Level 3 Grim Master enemies and fireballs
├── level3boss.hpp       Final boss, minions and hazards
├── title.hpp            Title, loading, controls and credits screens
│
├── Assets/              Sprites, backgrounds and UI art
│   ├── mc/              Player animations
│   ├── Boss/, BossEntry/   Final boss and entry cinematic
│   ├── Cave/            Bug, goblin, flying creature, Lightner, Grim Master
│   ├── Level 2/, Level 3/  Level backgrounds, sentry, trader, boss door, trade items
│   ├── Puzzle/          Switches, push block, gate
│   ├── Title Screen/, UI/, GameOver/, GameEnding/
│   └── Background/, Midground/
└── Audios/              Music, footsteps, movement and UI sounds
```

---

## Game Flow

```
Title Screen → Loading → Level 1 → Cave (Bugs → Goblins → Grim Master)
   → "Level 1 Cleared" → Level 2 → Level 3 (trader + puzzle + Grim Masters)
   → Boss door (key required) → Boss entry cinematic → Final boss → Ending
```

---

## Configuration

Gameplay tuning lives in `config.hpp`. Some useful values:

| Constant | Meaning |
|---|---|
| `SCREEN_W` / `SCREEN_H` | Window size (1000 × 600) |
| `PLAYER_MAX_HEALTH` / `PLAYER_MAX_STAMINA` | Player health and stamina caps (100 each) |
| `STAMINA_COST_DASH` / `STAMINA_COST_DOWNSTAB` | Stamina spent per move (20 each) |
| `BOSS_MAX_HEALTH` | Final boss health (400) |
| `GRIM_MAX_HEALTH` | Grim Master health (250) |
| `TRADE_ITEM_COST_SWIFTNESS` / `_SOUL` / `_KEY` | Trader prices in fragments (5 / 5 / 0) |
| `PICKUP_DROP_CHANCE` | Chance for an enemy to drop an item (75) |

---

## Notes

- Windows only: the game uses the Win32 API (`GetAsyncKeyState`, `winmm`) for input and audio.
- Keep the working directory at the project folder when launching, or assets and audio will fail to load.
- The `Debug/` folder and the `.sdf`, `.suo`, `.pdb`, `.ilk` and `.idb` files are build and IDE artifacts and are not needed to build from source. The `.sdf` file alone is about 32 MB, so you can leave them out of version control.
- Files ending in `.png~` are stray backup copies and can be deleted.
- The older notes in `README.txt`, `MERGED_BUILD_README.txt`, `MERGED_LEVEL3_BOSS_README.txt` and `MODIFICATIONS.txt` document how earlier builds were merged and can be kept for history.

---

## Credits

Shadow Bound was made by:

1. **Jebin Akter Tofa** (00725105101166)
2. **Sameeha Rimsha Tanisha** (00725105101167)
3. **Md. Sharid Shafin** (00725105101171)

Supervised by:

- **Mr. Shaha Reno**, Assistant Professor
- **Md. Zahid Hossain**, Lecturer, Grade-1
