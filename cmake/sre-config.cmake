set(SRE_DIR "${CMAKE_INSTALL_PREFIX}/share/sre" CACHE PATH "Directory of the Story Royale engine" FORCE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR})

link_directories(${CMAKE_INSTALL_PREFIX}/lib)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(OpenGL REQUIRED)

include(sre/targets)
include(sre/game_build)

include(sre/drivers)
target_link_libraries(srelib INTERFACE sre_drivers)

