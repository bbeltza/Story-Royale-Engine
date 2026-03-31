set(SRE_DIR "${CMAKE_CURRENT_LIST_DIR}/share/sre" CACHE PATH "Directory of the Story Royale engine" FORCE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SRE_DIR}/cmake/)

link_directories(${CMAKE_CURRENT_LIST_DIR}/lib)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include(sre)
include(sre/targets)