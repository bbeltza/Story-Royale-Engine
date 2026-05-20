#ifndef SRE_ERROR_H
#define SRE_ERROR_H
#include <C_API.h>
#include <stdbool.h>

typedef enum sreErrorLevel
{
    SRE_NONE = 0,
    SRE_ERROR = 1,
    SRE_CRITICAL = 2
} sreErrorLevel;

typedef enum sreErrorCategory
{
    SRE_ERR_NOERROR = 0x00,

    SRE_ERR_CORE,
    SRE_ERR_CREATE,

    SRE_ERR_INVALID_PARAMETER,
    SRE_ERR_INVALID_VALUE,
    SRE_ERR_INVALID_HINT,
    SRE_ERR_INVALID_STATE,

    SRE_ERR_FAIL,
    SRE_ERR_SDL,

    SRE_ERR_FILE_OPENING,

    // DirectX HRESULT failing errors
    // The one with _DBG just adds the line and the file in which the error happened
    SRE_ERR_DIRECTX_HR,
    SRE_ERR_DIRECTX_HR_DBG,

    SRE_ERROR_LAST
} sreErrorCategory;

SRE_CAPI_BEGIN

// Get the last error, this doesn't clear any errors. It returns `SRE_ERR_NOERROR` if there are no errors
enum sreErrorCategory sre_geterror(void);
// Pop the last error, or return `false` if there were no errors
bool sre_poperror();
// Clear every single error in the current thread, or return `false` if there weren't any
bool sre_clearerrors();

// Push an error to the current thread
int sre_error(sreErrorCategory category, ...);
// Drop a critical error, and possibly crash the program. Critical errors tell the program that it cannot proceed anymore
int sre_CRITICAL(sreErrorCategory category, ...);

typedef void (*sreErrorCallback)(sreErrorCategory, const char* msg);

// Set a callback function that will be run everytime an error occurs (unimplemented)
// @param level The level of the error, whether the error comes from `sre_error` or `sre_CRITICAL`, so either `SRE_ERROR` or `SRE_CRITICAL`
// @param callback The callback function to be run
// @param override Whether to override any default behavior on the error (logging for normal errors or terminating on top of that for critical errors) or keep the default behavior
// @remark if `override` is set to `false`, then the callback function will always run before the default behavior gets run.
void sre_errorcallback(sreErrorLevel level, void (*callback)(sreErrorCategory category, const char* msg), bool override);

SRE_CAPI_END

#ifdef __cplusplus
    namespace sre
    {
        template <typename... Args>
        inline int error(sreErrorCategory category, Args... args) { return sre_error(category, args...); }
        template <typename... Args>
        inline int critical(sreErrorCategory category, Args... args) { return sre_CRITICAL(category, args...); }
    }
#endif

#endif