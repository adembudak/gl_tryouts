buildDir := 'build'

alias c := configure
configure buildDir = buildDir:
  cmake -DVCPKG_MANIFEST_MODE=ON -S . -B {{buildDir}} --toolchain $VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake

alias b := build
build buildDir = buildDir:
  cmake --build {{buildDir}} --clean-first

run buildDir = buildDir:
  cmake --build {{buildDir}}
  ./{{buildDir}}/vibe

debug buildDir = buildDir:
  cmake --build {{buildDir}}
  gdb ./{{buildDir}}/vibe
