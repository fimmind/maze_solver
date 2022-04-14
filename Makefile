default: build

cmake:
	test -d ./build || mkdir build
	cd build && cmake ..

build:
	test -f ./build/Makefile || ${MAKE} cmake
	${MAKE} -C ./build

run: build
	./build/main
