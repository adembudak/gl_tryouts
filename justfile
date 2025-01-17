
build:
	cmake --build build

run:
	just build
	./build/vibe
