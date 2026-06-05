BUILD_DIR            := build
BUILD_TESTING        := Off
GENERATOR            := Ninja  # 'Unix Make'
CMAKE_INSTALL_PREFIX ?= /usr/local


all: generate


generate: $(BUILD_DIR)
	cmake --build $(BUILD_DIR) --parallel


$(BUILD_DIR): configure


configure:
	cmake -G $(GENERATOR) \
	      -B $(BUILD_DIR) \
	      -S . \
	      -DBUILD_TESTING=$(BUILD_TESTING) \
	      -DCMAKE_INSTALL_PREFIX=$(CMAKE_INSTALL_PREFIX)


test: tests
tests:
	ctest --test-dir $(BUILD_DIR)


install: all
	cmake --install $(BUILD_DIR)


clean:
	cmake --build $(BUILD_DIR) --target $@


cleanall:
	$(RM) -rv ./$(BUILD_DIR)


.PHONY: all generate configure pbuild parallel-build test tests install  clean cleanall
