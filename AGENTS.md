# Repository Guidelines

## Project Structure & Module Organization
- Root: `Makefile` builds the terminal 3D renderer binary `./dungeon3d`.
- Source: `src/` holds C99 modules: `renderer.*`, `tui.*`, `math3d.*`, `dungeon.*`, `player.*`.
- Build artifacts: `obj/` (auto-created during compilation).
- Tests: none yet; add future tests under `tests/` (see below).

## Build, Test, and Development Commands
- `make` — compile all sources into `./dungeon3d`.
- `make run` — build then run the binary.
- `make clean` — remove `obj/` and the binary.
- `make install` — install ncurses headers/libs on Debian/Ubuntu.
- Prereqs: GCC and ncurses. Debian/Ubuntu: `sudo apt-get install libncurses5-dev`. macOS: `brew install ncurses` and ensure linker/include paths are set.

## Coding Style & Naming Conventions
- C99 (`-std=c99`) with `-Wall -Wextra` enabled.
- Indentation: 4 spaces, no tabs; aim for ≤100 columns.
- One module per pair (e.g., `renderer.c` + `renderer.h`).
- Functions/variables: `snake_case` (e.g., `camera_update_direction`).
- Types/structs: Capitalized (e.g., `Vec3`, `Map`, `Dungeon`).
- Macros/guards: ALL_CAPS (e.g., `#ifndef RENDERER_H`).
- Headers: include only what you use; keep `*.h` minimal and focused.

## Testing Guidelines
- No framework in use. If adding tests:
- Place files in `tests/` as small, focused C programs.
- Name pattern: `tests/test_<module>_*.c` (e.g., `tests/test_math3d_vec.c`).
- Prefer tiny self-contained binaries built via Make targets.
- Example pattern: add a target to compile `tests/test_math3d_vec` and run it directly.

## Commit & Pull Request Guidelines
- Commits: imperative, concise subject (≤72 chars); include scope when helpful.
- Example: `renderer: fix fisheye error in side distance`.
- PRs: clear description, rationale, and visuals (screenshots/asciinema) when applicable; link issues.
- Keep diffs focused; avoid drive-by refactors. Update docs/Makefile when relevant.

## Security & Configuration Tips
- Validate inputs from generators; guard against out-of-bounds map access.
- Keep `MAP_WIDTH/HEIGHT`-based loops consistent and bounded.
- When using ncurses, ensure proper cleanup via `tui_cleanup` on exit.
