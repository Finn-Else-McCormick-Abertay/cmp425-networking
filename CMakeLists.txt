cmake_minimum_required(VERSION 3.28)

project(NetworkingCoursework VERSION 0.0.1 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 3.0.2
  GIT_SHALLOW ON EXCLUDE_FROM_ALL SYSTEM
  FIND_PACKAGE_ARGS 3.0.2
)
FetchContent_MakeAvailable(SFML)

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


# Shared
file(GLOB SHARED_SOURCES CONFIGURE_DEPENDS src/shared/*.cpp src/shared/*.h src/shared/*.hpp)
add_library(${PROJECT_NAME}_shared STATIC ${SHARED_SOURCES})
target_compile_features(${PROJECT_NAME}_shared PUBLIC cxx_std_23)
target_include_directories(${PROJECT_NAME}_shared PUBLIC src/shared/)
target_link_libraries(${PROJECT_NAME}_shared PUBLIC SFML::Network box2cpp)

# Client
file(GLOB CLIENT_SOURCES CONFIGURE_DEPENDS src/client/*.cpp src/client/*.h src/client/*.hpp)
add_executable(${PROJECT_NAME}_client ${CLIENT_SOURCES})
target_compile_features(${PROJECT_NAME}_client PUBLIC cxx_std_23)
target_link_libraries(${PROJECT_NAME}_client PRIVATE ${PROJECT_NAME}_shared SFML::Graphics SFML::Window SFML::Audio)

# Server
file(GLOB SERVER_SOURCES CONFIGURE_DEPENDS src/server/*.cpp src/server/*.h src/server/*.hpp)
add_executable(${PROJECT_NAME}_server ${SERVER_SOURCES})
target_compile_features(${PROJECT_NAME}_client PUBLIC cxx_std_23)
target_link_libraries(${PROJECT_NAME}_server PRIVATE ${PROJECT_NAME}_shared)