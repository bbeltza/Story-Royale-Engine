set(SRE_DIR "${CMAKE_CURRENT_LIST_DIR}/share/sre" CACHE PATH "Directory of the Story Royale engine" FORCE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SRE_DIR}/cmake/)
include(sre)
message(${CMAKE_CURRENT_SOURCE_DIR})