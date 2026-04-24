#ifndef SRE_CAPI_H
#define SRE_CAPI_H

    #ifdef __cplusplus
        #define SRE_CAPI_BEGIN extern "C" {
        #define SRE_CAPI_END }
        #define SRE_EXTERN_C extern "C"
    #else
        #define SRE_CAPI_BEGIN
        #define SRE_CAPI_END
        #define SRE_EXTERN_C extern
    #endif

#endif