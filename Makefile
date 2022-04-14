default: build

cmake:
	test -d ./build || mkdir build
	cd build && cmake ..
	rm compile_commands.json || true
	cp ./build/compile_commands.json .

build: main.cpp
	test -f ./build/Makefile || ${MAKE} cmake
	${MAKE} -C ./build

run: build
	./build/main
