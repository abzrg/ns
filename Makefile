BUILD_DIR            := build
PRESET               ?= debug
CMAKE_INSTALL_PREFIX ?= /usr/local


# --- Default

all: build


# --- Workflow shortcuts (configure + build + test in one shot)

debug:
	cmake --workflow --preset debug

release:
	cmake --workflow --preset release


# --- Individual steps (use PRESET=debug|release make <target>)

configure:
	cmake --preset $(PRESET)

build: configure
	cmake --build --preset $(PRESET) --parallel

test tests: build
	ctest --preset $(PRESET)

install: build
	cmake --install $(BUILD_DIR)/$(PRESET) --prefix $(CMAKE_INSTALL_PREFIX)


# --- Housekeeping

clean:
	cmake --build --preset $(PRESET) --target clean

cleanall:
	$(RM) -rv ./$(BUILD_DIR)


.PHONY: all debug release configure build test tests install clean cleanall
