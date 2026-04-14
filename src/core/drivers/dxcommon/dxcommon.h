#ifndef SRE_DXCOMMON_H
#define SRE_DXCOMMON_H
#include <winerror.h>

// Convert HRESULT error codes into a constant string
inline static const char* DXHRTOSTRING(HRESULT hr)
{
    #define _FMT_CASE(x) case x: return #x
    switch (hr)
    {
        _FMT_CASE(DXGI_ERROR_ACCESS_DENIED);
        _FMT_CASE(DXGI_ERROR_ACCESS_LOST);
        _FMT_CASE(DXGI_ERROR_ALREADY_EXISTS);
        _FMT_CASE(DXGI_ERROR_CANNOT_PROTECT_CONTENT);
        _FMT_CASE(DXGI_ERROR_DEVICE_HUNG);
        _FMT_CASE(DXGI_ERROR_DEVICE_REMOVED);
        _FMT_CASE(DXGI_ERROR_DEVICE_RESET);
        _FMT_CASE(DXGI_ERROR_DRIVER_INTERNAL_ERROR);
        _FMT_CASE(DXGI_ERROR_FRAME_STATISTICS_DISJOINT);
        _FMT_CASE(DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE);
        _FMT_CASE(DXGI_ERROR_INVALID_CALL);
        _FMT_CASE(DXGI_ERROR_MORE_DATA);
        _FMT_CASE(DXGI_ERROR_NAME_ALREADY_EXISTS);
        _FMT_CASE(DXGI_ERROR_NONEXCLUSIVE);
        _FMT_CASE(DXGI_ERROR_NOT_CURRENTLY_AVAILABLE);
        _FMT_CASE(DXGI_ERROR_NOT_FOUND);
        _FMT_CASE(DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE);
        _FMT_CASE(DXGI_ERROR_SDK_COMPONENT_MISSING);
        _FMT_CASE(DXGI_ERROR_SESSION_DISCONNECTED);
        _FMT_CASE(DXGI_ERROR_UNSUPPORTED);
        _FMT_CASE(DXGI_ERROR_WAIT_TIMEOUT);
        _FMT_CASE(DXGI_ERROR_WAS_STILL_DRAWING);
        
        _FMT_CASE(D3D11_ERROR_FILE_NOT_FOUND);
        _FMT_CASE(D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS);
        _FMT_CASE(D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS);
        _FMT_CASE(D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD);

        _FMT_CASE(D3D12_ERROR_ADAPTER_NOT_FOUND);
        _FMT_CASE(D3D12_ERROR_DRIVER_VERSION_MISMATCH);

        _FMT_CASE(E_FAIL);
        _FMT_CASE(E_INVALIDARG);
        _FMT_CASE(E_OUTOFMEMORY);
        _FMT_CASE(E_NOTIMPL);
        _FMT_CASE(S_FALSE);
        _FMT_CASE(S_OK);

        _FMT_CASE(DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED);
        _FMT_CASE(DXGI_ERROR_REMOTE_OUTOFMEMORY);
        default: return 0;
    }
    #undef _FMT_CASE
}

#ifdef __cplusplus
    #define DX_LOGERR sre::log<sre::LOGCATEGORY_ERROR>
#else
    #define DX_LOGERR(...) sre_log(SRE_LOGCATEGORY_ERROR, __VA_ARGS__)
#endif

#define SRE_DXCALL(x) hr = x;                                                                                               \
do { if (FAILED(hr)) {                                                                                                      \
    DX_LOGERR("[DirectX - error]: '" #x "' failed: '%s' (0x%X) | line: %u ; file: %s", DXHRTOSTRING(hr), hr, __LINE__, __FILE__);  \
                                                                                                                            \
}} while(0)


#endif