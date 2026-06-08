
**Error Correction Visualiser**

This small project is a terminal-based visualiser for Hamming code error detection and correction. It computes Hamming parity bits for a binary input, and lets you flip a bit to compare "before" and "after" states.

**Requirements**
- C compiler such as `gcc`
- Terminal size: at least `50x20` (smaller terminals are not supported)
- The project includes `termbox2.h` in the repository root; if your system lacks termbox/termbox2, you may need to install or link it.

**Build**
From the project directory run:

```sh
gcc main.c -o errorCorrector
```

If you get linker errors related to termbox, install the appropriate termbox library or add the correct `-I`, `-L`, and `-l` flags.

**Run**

```sh
./errorCorrector
```

**Keyboard Controls**
- `q`: Quit the program
- Enter (on greeting screen): Open the Hamming menu
- `B` / `b`: Return to greeting screen
- `E` / `e`: Enter binary input mode (typing)
- `T` / `t`: Enter traversal mode (disabled if no data)
- `1` / `0`: Append bit while typing
- Backspace / Backspace2: Delete while typing
- Arrow keys: Move selected bit in traversal mode
- Enter (during traversal): Flip the selected bit
- ESC: Return to normal mode from typing or traversal

**Notes / Troubleshooting**
- Input length is bounded by the `inputText` buffer in the source (about 30 chars).
- If the terminal is too small the program displays a size warning — resize the terminal.
- For contribution or bug reports, open an issue in the repository.

**License**
This project is provided as a simple educational/example program. No license is specified; add one if you intend to redistribute.
