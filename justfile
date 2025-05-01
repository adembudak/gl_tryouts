get-dependencies:
  conan install . --output-folder=build --build=missing

configure:
  cmake -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE="build/conan_toolchain.cmake" -S . -B build

build:
  cmake --build build --config Debug

run:
  just build
  ./build/Debug/vibe
