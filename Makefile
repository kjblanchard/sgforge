all: rebuild

rebuild: clean
	@mkdir build
	@cmake -DCMAKE_BUILD_TYPE=Debug -B build .
	@cmake --build build

install:
	@cmake --install build

clean:
	@rm -rf build
