# Direct3D 12 joke renderer! (Not for production just for fun and entertainment... And learning!!)

check_include_file("d3d12.h" SRE_HAVE_D3D12_H)
check_include_file("dxgi1_4.h" SRE_HAVE_DXGI14_H)

if (NOT SRE_HAVE_D3D12_H)
    message(AUTHOR_WARNING "d3d12.h is NOT found. This does not cause any problem since d3d12 is only needed for the d3d12 joke renderer. But you obviously won't be able to use it!")
    return()
endif()

if (NOT SRE_HAVE_DXGI14_H)
    message(SEND_ERROR "dxgi1_4.h is NOT found, but d3d12.h was found. DXGI is the first version that came with D3D12 and thus is needed for this video driver. Maybe check for any header faults?")
    return()
endif()

target_compile_definitions(sre INTERFACE "SRE_USEDRIVER_D3D12=1")
SRE_DRIVERBUILD(sre_d3d12 "d3d12.cpp")