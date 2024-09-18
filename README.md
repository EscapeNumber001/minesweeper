A simple Minesweeper clone written in C++ using GTK 3.
![Screenshot from 2024-09-18 19-30-29](https://github.com/user-attachments/assets/33fb210b-1d36-4698-9eab-09329efd416c)


# Building
First ensure you have the correct dependencies installed:
`sudo apt install libgtk-3-dev meson g++`

```bash
meson setup build
cd build
meson compile
./minesweeper
```
