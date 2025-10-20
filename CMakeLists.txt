cmake_minimum_required(VERSION 3.28)

project(NetworkingCoursework VERSION 0.0.1 LANGUAGES CXX)

# -- Dependencies --

include(FetchContent)

# SFML
FetchContent_Declare(SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 3.0.2
  GIT_SHALLOW ON EXCLUDE_FROM_ALL SYSTEM
  FIND_PACKAGE_ARGS 3.0.2
)
FetchContent_MakeAvailable(SFML)

# Box2D & C++ bindings
FetchContent_Declare(box2d
  GIT_REPOSITORY https://github.com/erincatto/box2d GIT_TAG v3.1.1
  GIT_SHALLOW ON EXCLUDE_FROM_ALL SYSTEM
)
FetchContent_MakeAvailable(box2d)

FetchContent_Declare(box2cpp
  GIT_REPOSITORY https://github.com/HolyBlackCat/box2cpp GIT_TAG e3bac1a16fa31911b70cf8f16767858a6c51bd7e # Branch box2d-3.1.1
  EXCLUDE_FROM_ALL SYSTEM
)
FetchContent_MakeAvailable(box2cpp)
add_library(box2cpp INTERFACE)
target_include_directories(box2cpp SYSTEM INTERFACE ${FETCHCONTENT_BASE_DIR}/box2cpp-src/include)
target_link_libraries(box2cpp INTERFACE box2d)

# Quadtree
FetchContent_Declare(quadtree
  GIT_REPOSITORY https://github.com/red-9m/Quadtree.git GIT_TAG f6e221bdedd4ea03dec3c7b2268e6bd550b7d474 # Branch main
  EXCLUDE_FROM_ALL SYSTEM
)
FetchContent_MakeAvailable(quadtree)
add_library(quadtree INTERFACE)
target_include_directories(quadtree SYSTEM INTERFACE ${FETCHCONTENT_BASE_DIR}/quadtree-src/)

# vmath
FetchContent_Declare(vmath.hpp
  GIT_REPOSITORY https://github.com/BlackMATov/vmath.hpp.git GIT_TAG ed47c535f70c6a05a3bd5a65a90400c3843dfefc # Branch main
  EXCLUDE_FROM_ALL SYSTEM
)
FetchContent_MakeAvailable(vmath.hpp)


# -- Executables --

set(COMMON_DEPS SFML::System SFML::Network box2cpp quadtree vmath.hpp)
set(CLIENT_DEPS SFML::Graphics SFML::Window SFML::Audio)

set(COMMON_COMPILE_DEFINITIONS $<$<CONFIG:Debug>:DEBUG> $<$<CONFIG:Release>:RELEASE>)

# Client
set(CLIENT_NAME ${PROJECT_NAME}_client)
add_executable(${CLIENT_NAME})
target_compile_features(${CLIENT_NAME} PUBLIC cxx_std_23)
target_link_libraries(${CLIENT_NAME} PRIVATE ${COMMON_DEPS} ${CLIENT_DEPS})
target_include_directories(${CLIENT_NAME} PRIVATE src/client/)
target_include_directories(${CLIENT_NAME} PRIVATE src/common/)
target_compile_definitions(${CLIENT_NAME} PRIVATE CLIENT ${COMMON_COMPILE_DEFINITIONS})


# Server
set(SERVER_NAME ${PROJECT_NAME}_server)
add_executable(${SERVER_NAME})
target_compile_features(${SERVER_NAME} PUBLIC cxx_std_23)
target_link_libraries(${SERVER_NAME} PRIVATE ${COMMON_DEPS})
target_include_directories(${SERVER_NAME} PRIVATE src/server/)
target_include_directories(${SERVER_NAME} PRIVATE src/common/)
target_compile_definitions(${SERVER_NAME} PRIVATE SERVER ${COMMON_COMPILE_DEFINITIONS})

# Define sources
add_subdirectory(src/common)
add_subdirectory(src/client)
add_subdirectory(src/server)