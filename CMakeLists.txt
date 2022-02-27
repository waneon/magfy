cmake_minimum_required(VERSION 3.16)

# project
project(magfy VERSION 0.1.0 LANGUAGES CXX)

# enviroments
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vars
set(MAGFY_SOURCES
    "src/main.cpp"
    "src/config/Config.cpp"
    "src/backend/gnome/Magnifier.cpp"
)
set(MAGFY_INCLUDES
    "src/config"
    "src/backend"
    "src/core"
    "src/translate"
)
set(MAGFY_LIBS)
set(VENDOR)

# libs
find_package(spdlog REQUIRED)
list(APPEND MAGFY_LIBS spdlog::spdlog)

find_package(yaml-cpp REQUIRED)
list(APPEND MAGFY_LIBS yaml-cpp)

# vendor setting
if(VENDOR STREQUAL "Windows")
elseif(VENDOR STREQUAL "X11")
    add_compile_definitions(X11)
    list(APPEND MAGFY_SOURCES
        "src/core/x11/core.cpp"
        "src/translate/x11/translate.cpp"
    )
    list(APPEND MAGFY_LIBS X11)
endif()

# debug setting
if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(CONFIG_FILE="${CMAKE_SOURCE_DIR}/debug/config.yaml")
endif()

# includes
include_directories(${MAGFY_INCLUDES})

# magfy
add_executable(main ${MAGFY_SOURCES})
target_link_libraries(main PRIVATE ${MAGFY_LIBS})