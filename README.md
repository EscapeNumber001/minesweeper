A simple Minesweeper clone written in C++ using GTK 3.

# Building
First ensure you have the correct dependencies installed:
`sudo apt install libgtk-3-dev meson g++`

```bash
meson setup build
cd build
meson compile
./minesweeper
```
