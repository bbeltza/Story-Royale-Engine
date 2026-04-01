include(CheckIncludeFile)

macro(SRE_CHECKHEADER CHEADER VAR TARGET INTERFACE)
    check_include_file(${CHEADER} ${VAR})
    if (${VAR})
        target_compile_definitions(${TARGET} ${INTERFACE} "${VAR}=1")
    endif()
endmacro()

# Engine required header checks

if (NOT ANDROID) # ucontext.h used by base/coroutine.c (Not supported on android, but still has the header)
    SRE_CHECKHEADER(ucontext.h SRE_HAVE_UCONTEXT_H sre INTERFACE)
endif()
if (NOT WIN32) # sys/stat.h used for mkdir. WIN32 has its own mkdir header (sre_mkdir uses the win32 API instead anyways)
    SRE_CHECKHEADER(sys/stat.h SRE_HAVE_SYSSTAT_H sre INTERFACE)
endif()
