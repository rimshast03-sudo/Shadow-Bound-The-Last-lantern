SHADOW BOUND - ZIP 2 BASE + ZIP 1 LEVEL 3 / TRADER / BOSS

Base project:
- Shadow Bound Final project (ZIP 2)

Preserved from ZIP 2:
- Existing player implementation
- Existing player slash attack (X)
- Existing player arrow attack (C)
- Existing Level 1 enemy types and behavior
- Existing cave/Level 1 boss flow
- Existing Level 2 flow

Added using ZIP 1 as the reference:
- Level 3 progression remains in the ZIP 2 project and now acts as the final pre-boss area.
- Level 3 trader NPC with the ZIP 1-style approach/trade/menu flow.
- Trader items: Swiftness, Soul and Key.
- Level 3 boss door and key-gated progression.
- Full-screen boss entry animation (278 frames) and entry music.
- ZIP 1-style final boss, including attacks, phases, teleporting, spikes/trap, minions and health bar.
- Both ZIP 2 player attacks can damage the final boss and boss minions.
- Boss arena background, boss sprites, minion sprites, door sprites, trader sprites and trade UI assets copied from ZIP 1.
- Boss defeat completion screen.

Important:
- ZIP 2 remains the player/enemy baseline. The player was not replaced with ZIP 1's player system.
- ZIP 1 assets were added only where needed for the requested Level 3/trader/boss features.
- The original ZIP 2 cave boss manager is kept intact under boss.hpp; the new final boss implementation is level3boss.hpp to avoid breaking Level 1.

Controls:
- Arrow keys: move
- Space: jump
- Z: dash
- X: existing melee/slash attack
- C: existing arrow attack
- E: open/close trader menu when near the trader
- Mouse: select trader items / activate equipped trader items
