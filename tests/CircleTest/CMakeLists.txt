message(${CMAKE_MODULE_PATH})
include(srEngineResources)

project(CircleTest)

add_executable(CircleTest
    GameSettings.cpp
    GuiTest.cpp
    TestCharacter.cpp
)

target_include_directories(CircleTest PUBLIC ../../include)
target_include_directories(CircleTest PRIVATE ../../3rdparty/SDL/include)
target_include_directories(CircleTest PRIVATE ../../3rdparty/SDL_ttf)
target_include_directories(CircleTest PRIVATE ../../3rdparty/SDL_image)
target_include_directories(CircleTest PRIVATE ../../3rdparty/fmt/include)

target_link_libraries(CircleTest StoryRoyaleEngine)

srEngine_set_resource_file(CircleTest res)