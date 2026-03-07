#ifndef SRE_CAPI_H
#define SRE_CAPI_H

    #ifdef __cplusplus
        #define SRE_CAPI_BEGIN extern "C" {
        #define SRE_CAPI_END }
    #else
        #define SRE_CAPI_BEGIN
        #define SRE_CAPI_END
    #endif

#endif