buildDir := 'build'

alias c := configure
configure buildDir = buildDir:
  cmake -G 'Ninja Multi-Config' -S . -B {{buildDir}}

alias b := build
build buildDir = buildDir:
  cmake --build {{buildDir}} --config Debug --clean-first

run buildDir = buildDir:
  cmake --build {{buildDir}} --config Debug
  ./{{buildDir}}/Debug/vibe

debug buildDir = buildDir:
  cmake --build {{buildDir}} --config Debug
  gdb ./{{buildDir}}/Debug/vibe
