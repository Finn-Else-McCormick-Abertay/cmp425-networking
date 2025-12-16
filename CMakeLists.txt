cmake_minimum_required(VERSION 3.28)

project(NetworkingCoursework VERSION 0.0.1 LANGUAGES CXX)

# -- Dependencies --

include(FetchContent)

# SFML
FetchContent_Declare(SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 0fa201c969e48ecc253581c5841ce73f44d42f49 # 3.0.2
  GIT_SHALLOW ON EXCLUDE_FROM_ALL SYSTEM
  FIND_PACKAGE_ARGS 3.0.2
)
FetchContent_MakeAvailable(SFML)

# fmt
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt GIT_TAG 407c905e45ad75fc29bf0f9bb7c5c2fd3475976f # 12.1.0
  EXCLUDE_FROM_ALL SYSTEM
)
FetchContent_MakeAvailable(fmt)

# vmath
FetchContent_Declare(vmath.hpp
  GIT_REPOSITORY https://github.com/BlackMATov/vmath.hpp.git GIT_TAG ed47c535f70c6a05a3bd5a65a90400c3843dfefc # Branch main
  EXCLUDE_FROM_ALL SYSTEM
)
FetchContent_MakeAvailable(vmath.hpp)

# Glaze
FetchContent_Declare(glaze
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git GIT_TAG f2ffb15
  GIT_SHALLOW TRUE EXCLUDE_FROM_ALL SYSTEM
)
FetchContent_MakeAvailable(glaze)

# sparse_map
#[[
FetchContent_Declare(sparse_map
  GIT_REPOSITORY https://github.com/Tessil/sparse-map.git GIT_TAG 4f19a68c3984189d356f96267726d439ee153c2d # Branch main
  EXCLUDE_FROM_ALL SYSTEM
)
FetchContent_MakeAvailable(sparse_map)
]]

# Lyra
FetchContent_Declare(lyra
  GIT_REPOSITORY https://github.com/bfgroup/Lyra.git GIT_TAG 5a4ef1abaf7139ef00204992a2627127ad394499 # 1.7.0
  EXCLUDE_FROM_ALL SYSTEM
)
FetchContent_MakeAvailable(lyra)


# -- Executables --

set(COMMON_DEPS SFML::System SFML::Network fmt::fmt vmath.hpp glaze::glaze lyra)
set(CLIENT_DEPS SFML::Graphics SFML::Window SFML::Audio)
set(SERVER_DEPS SFML::Window)

set(COMMON_COMPILE_DEFINITIONS $<$<CONFIG:Debug>:DEBUG> $<$<CONFIG:Release>:RELEASE>)
set(COMMON_COMPILE_OPTIONS
  $<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-mpopcnt>
  $<$<CXX_COMPILER_ID:Clang>:-Wno-unqualified-std-cast-call>
)

# Client
set(CLIENT_NAME ${PROJECT_NAME}_client)
add_executable(${CLIENT_NAME})
target_compile_features(${CLIENT_NAME} PUBLIC cxx_std_23)
target_compile_options(${CLIENT_NAME} PRIVATE ${COMMON_COMPILE_OPTIONS})
target_link_libraries(${CLIENT_NAME} PRIVATE ${COMMON_DEPS} ${CLIENT_DEPS})
target_include_directories(${CLIENT_NAME} PRIVATE src/client/ src/common/ src/util/)
target_compile_definitions(${CLIENT_NAME} PRIVATE CLIENT ${COMMON_COMPILE_DEFINITIONS})

# Server
set(SERVER_NAME ${PROJECT_NAME}_server)
add_executable(${SERVER_NAME})
target_compile_features(${SERVER_NAME} PUBLIC cxx_std_23)
target_compile_options(${SERVER_NAME} PRIVATE ${COMMON_COMPILE_OPTIONS})
target_link_libraries(${SERVER_NAME} PRIVATE ${COMMON_DEPS} ${SERVER_DEPS})
target_include_directories(${SERVER_NAME} PRIVATE src/server/ src/common/ src/util/)
target_compile_definitions(${SERVER_NAME} PRIVATE SERVER ${COMMON_COMPILE_DEFINITIONS})

# Define sources
add_subdirectory(src/common)
add_subdirectory(src/client)
add_subdirectory(src/server)
