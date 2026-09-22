Shadow Bound — Merged Build
============================

This package merges the two supplied Shadow Bound projects.

Base:
- Shadow Bound_2.1.1: scrolling Level 1/Level 2, inventory/pickups, midground,
  creatures and sentries.

Merged:
- Cave encounter system from Shadow Bound 5:
  Bugs -> Goblins -> Grim Master -> Level 1 Cleared -> Level 2.
- Cave assets and support headers.
- Unified config constants.
- Single iMain.cpp and single Visual Studio project/solution.

Build:
1. Open "Shadow Bound Merged.sln" in Visual Studio 2013/compatible VS with
   Win32 desktop C++ support.
2. Build > Rebuild Solution.
3. Run the Win32 build from the project directory so relative Assets/ and
   Audios/ paths resolve correctly.

The generated package intentionally excludes old Debug/Release intermediate
files, .sdf/.suo/.pdb/.idb/.tlog files and duplicate project artifacts.
