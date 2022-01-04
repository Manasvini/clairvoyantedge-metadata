cmake_minimum_required(VERSION 3.19)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_CXX_STANDARD 17)


project(clairvoyantedge-metadata)


add_subdirectory(redis-lib)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(scripts)
