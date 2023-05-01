cmake_minimum_required(VERSION 3.25)
project(OctomapRaylibSimulation)

set(CMAKE_CXX_STANDARD 17)

# -- To link raylib --
# cd raylib/src/
# make PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=SHARED
# sudo make install RAYLIB_LIBTYPE=SHARED
find_library(RAYLIB raylib)
include_directories(include)

# -- To link octomap --
# Build libraries (follow the instructions from the README)
# sudo make install
find_package(octomap REQUIRED)
include_directories(${OCTOMAP_INCLUDE_DIRS})
link_libraries(${OCTOMAP_LIBRARIES})

# --- Load src/ files --- #
file(GLOB src_files "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# --- Function to create executables for every src/ file
function(create_exec)
    foreach (file IN LISTS src_files)
        get_filename_component(filename ${file} NAME_WE)
        message(STATUS "Executable: ${filename}")
        add_executable(${filename} ${file})
        target_link_libraries(${filename} PRIVATE ${RAYLIB})
        target_link_libraries(${filename} PRIVATE ${OCTOMAP})
    endforeach ()
endfunction()

# --- EXECUTABLES --- #
create_exec()