# Build

> [!NOTE]
> If you would like to enable testing, edit `Makefile` and set `BUILD_TESTING` to `On`.

There are different ways to build the project, but we need to get the source code, first.
You can download the project from the releases section, or clone it directly.

```console
#! Clone the project over HTTPS
$ git clone https://github.com/abzrg/ns.git

#! or via SSH
$ git clone git@github.com:abzrg/ns.git
```

Then, check out your preffered branch/tag/commit, if needed, and go ahead and configure the project.

```console
$ git checkout <branch|tag|commit>

#! Configure the project
$ make configure
```

Before installing, you may want to specify the installation prefix and change it from the default value of `/usr/local`.

```console
$ CMAKE_INSTALL_PREFIX=<prefix> make install
```

After installation, header files, CMake package files (`find_package`), and pkg-config files
are placed in the appropriate locations under the install prefix.

---

## Use NS via a compiler

1. Clone or download the project into your dependency directory, e.g. `deps/ns/`.
2. Compile with the following flags:

```console
$ c++ -I./deps/ns/include -std=c++23 main.cpp -o myapp
```

If NS is already installed (e.g. under the default prefix `/usr/local`), point the compiler there instead:

```console
$ c++ -I/usr/local/include -std=c++23 main.cpp -o myapp
```

---

## Use NS via pkg-config

If NS is already installed, pkg-config can look up the necessary flags for you automatically:

```console
$ c++ $(pkg-config --cflags --libs ns) -std=c++23 main.cpp -o myapp
```

To verify that NS is visible to pkg-config, or to inspect its flags individually:

```console
$ pkg-config --exists ns && echo "found"
$ pkg-config --cflags ns    # include paths
$ pkg-config --libs   ns    # linker flags
```

If NS was installed to a non-standard prefix, make sure `PKG_CONFIG_PATH` includes the `lib/pkgconfig` (or `share/pkgconfig`) directory under that prefix:

```console
$ export PKG_CONFIG_PATH=<prefix>/lib/pkgconfig:$PKG_CONFIG_PATH

#! For example, I test installation under /var/tmp/local, so
$ export PKG_CONFIG_PATH="/var/tmp/local/lib/pkgconfig:$PKG_CONFIG_PATH"
```

---

## Use find_package (CMake)

If NS is installed, CMake can locate it with `find_package`:

```cmake
find_package(ns REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE ns::ns)
```

If NS was installed to a non-standard prefix, pass it to CMake at configure time:

```console
$ cmake -DCMAKE_PREFIX_PATH=<prefix> ..

#! Again, as an example, I use /var/tmp/local, so
$ export CMAKE_PREFIX_PATH="/var/tmp/local/lib/cmake:$CMAKE_PREFIX_PATH"
```

---

## Use FetchContent (CMake)

`FetchContent` downloads and builds NS as part of your own configure step&mdash;no prior installation required.

```cmake
include(FetchContent)

FetchContent_Declare(
  ns
  GIT_REPOSITORY https://github.com/abzrg/ns.git
  GIT_TAG        main   # pin to a tag or commit hash for reproducible builds
)

FetchContent_MakeAvailable(ns)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE ns::ns)
```

---

## Use add_subdirectory (CMake)

The simplest zero-config approach: drop NS into your source tree and point CMake at it.

1. Clone (or copy) NS into your dependency directory:

```console
$ git clone https://github.com/abzrg/ns.git deps/ns
```

2. In your `CMakeLists.txt`:

```cmake
add_subdirectory(deps/ns)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE ns::ns)
```

The `ns::ns` target is available immediately — no installation step needed.
A common pattern is to track NS as a git submodule so the revision is pinned in your repo:

```console
$ git submodule add https://github.com/abzrg/ns.git deps/ns
$ git submodule update --init --recursive
```
