# Direct3D 12 joke renderer! (Not for production just for fun and entertainment... And learning!!)

SRE_DRIVERBUILD(sre_d3d12 "d3d12.cpp" "d3d12imgui.cpp")
target_link_libraries(sre_d3d12 PRIVATE D3D12.lib DXGI.lib)