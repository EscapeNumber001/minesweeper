project('minesweeper', 'cpp')

sources = ['src/main.cpp', 'src/game.cpp', 'src/interface.cpp']
include_dir = include_directories('include')
gtk_dep = dependency('gtk+-3.0')

run_command('cp', '-r', 'assets/', 'build/', check: true)
executable('minesweeper', sources: sources, include_directories: include_dir, dependencies: gtk_dep)
