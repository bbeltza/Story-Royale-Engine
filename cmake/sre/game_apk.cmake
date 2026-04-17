# Include to build game on android, it might also have the tooling to make a full apk! (will implement what SRE_BUILD does but android exclusive, but for now it does nothing)

if (NOT ANDROID)
    message(FATAL_ERROR "Make sure to build on android, this file is not supported anywhere else")
endif()

set(SRE_SUPPORTED_ABIS "x86_64;x86;armeabi-v7a;arm64-v8a" CACHE STRING "Currently supported android ABIs" FORCE)
set(SRE_ANDROIDABIS ${ANDROID_ABI} CACHE STRING "Set a list of android ABIs to build on")
set(SRE_ARGS "" CACHE STRING "Extra command line arguments to set")

# Testing... (I might really just want to make a c/c++ or cmd program for that...)
foreach(ABI ${SRE_ANDROIDABIS})
    message(${ABI})
endforeach()


get_cmake_property(CACHE_VARIABLES CACHE_VARIABLES)
foreach(VAR ${CACHE_VARIABLES})
    message("${VAR} = ${${VAR}}")
endforeach()