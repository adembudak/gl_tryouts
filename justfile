configure:
	cmake -G "Ninja Multi-Config" -S . -B build

build:
	cmake --build build --config Debug

run:
	just build
	./build/Debug/vibe
