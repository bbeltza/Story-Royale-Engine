#include <Base/Error.h>
#include <Base/Log.h>

#include <SDL_thread.h>

#include <utils/mem.h>

#include <stdio.h>
#include <assert.h>

#define START_STACKSIZE 4096

struct sreErr
{
    int size;

    int level;
    int category;

    char msg[];
};

struct sreErrStack
{
    size_t size;
    size_t pos;

    char* ptr; // array of sreErr structures, but because its size can vary, it's a char pointer. Might be promoted to an int pointer since sreErr is may need to be aligned
};

static struct
{
    sreErrorCallback critical;
    sreErrorCallback error;

    bool critical_override;
    bool error_override;
} s_errcallbacks;

static void errstack_destroy(void* _errstack)
{
    struct sreErrStack* errstack = _errstack;

    sre_delete(errstack->ptr);
    sre_delete(errstack);
}

static struct sreErrStack* errstack_getthreadlocal()
{
    static SDL_TLSID s_tlsid;

    if (!s_tlsid) // TODO: You'd definitely need to make it thread safe (e.g: like in SDL's SDL_GetErrBuf function defined in SDL_thread.c)
    {
        s_tlsid = SDL_TLSCreate();
    }

    struct sreErrStack* errstack = SDL_TLSGet(s_tlsid);
    if (!errstack)
    {
        errstack = sre_new(sizeof(struct sreErrStack));
        errstack->size = START_STACKSIZE;
        errstack->pos = START_STACKSIZE; // The stack grows down!!!
        errstack->ptr = sre_new(START_STACKSIZE);

        SDL_TLSSet(s_tlsid, errstack, errstack_destroy);
    }

    return errstack;
}

static const char* err_getfmtstr(unsigned cat)
{
    static const char* FMT_STRINGS[] = {
        "There're no errors.",                          /* SRE_ERR_NOERROR */
        "Internal error: %s",                           /* SRE_ERR_CORE */
        "Object creation error: %s",                    /* SRE_ERR_CREATE */

        "Parameter '%s' is invalid.",                   /* SRE_ERR_INVALID_PARAMETER */
        "Invalid value from '%s': %X",                  /* SRE_ERR_INVALID_VALUE */
        "Invalid hint value from 'SRE_HINT_%s': %X",    /* SRE_ERR_INVALID_HINT */
        "State error: %s",                              /* SRE_ERR_INVALID_STATE */ 

        "%s is unavailable or unimplemented",           /* SRE_ERR_UNAVAILABLE */ 

        "Fail: %s",                                     /* SRE_ERR_FAIL */
        "SDL Error: %s",                                /* SRE_ERR_SDL */
        
        "Failed opening file '%s': %s",                  /* SRE_ERR_FILE_OPENING */

        "DirectX error: %s '%s' (%X)",
        "DirectX error: %s '%s' (%X) | line: %d | file: %s"
    };
    assert(ut_arrcount(FMT_STRINGS) == SRE_ERROR_LAST && "Mismatched format strings?");
    assert(ut_arrcount(FMT_STRINGS) > cat && "category out of bounds!");

    return FMT_STRINGS[cat];
}

static struct sreErr* err_getlast()
{
    struct sreErrStack* errstack = errstack_getthreadlocal();
    if (errstack->pos == errstack->size)
        return NULL;

    return (struct sreErr*)(errstack->ptr + errstack->pos);
}

static int err_logsingle(struct sreErr* err)
{
    extern int sre_internallogmsg(int type, int category, const char* msg, int length);

    if (!err)
    {
        // Print NOERROR's message, still do the job that you've asked to do
        return sre_internallogmsg(SRE_LOGGING_TYPE, 0x03, err_getfmtstr(SRE_ERR_NOERROR), 0);
    }

    bool iscritical = err->level == SRE_CRITICAL;
    return sre_internallogmsg(SRE_LOGGING_TYPE, 0x03 + iscritical, err->msg, err->size - sizeof(*err));
}

//
sreErrorCategory sre_geterror()
{
    struct sreErr* lasterr = err_getlast();
    if (!lasterr)
        return SRE_ERR_NOERROR;
    
    return lasterr->category;
}

bool sre_poperror(void)
{
    // Pop the last error
    struct sreErrStack* errstack = errstack_getthreadlocal();   
    if (errstack->pos == errstack->size)
        return false;

    struct sreErr* lasterr = (struct sreErr*)(errstack->ptr + errstack->pos);
    errstack->pos += lasterr->size;
    assert(errstack->pos <= errstack->size);

    return true;
}

bool sre_clearerrors(void)
{
    struct sreErrStack* errstack = errstack_getthreadlocal();   
    if (errstack->pos == errstack->size)
        return false;

    errstack->pos = errstack->size;
    return true;
}

struct sreErr* sre_pusherror(sreErrorLevel level, sreErrorCategory category, va_list fmt_va)
{
    struct sreErrStack* errstack = errstack_getthreadlocal();
    const char* fmtstring = err_getfmtstr(category);
    int formattedlen; {
        va_list fmt_vacpy;
        va_copy(fmt_vacpy, fmt_va);
        formattedlen = vsnprintf(NULL, 0, fmtstring, fmt_vacpy) + 1;
    }

    size_t errlen = sizeof(struct sreErr) + formattedlen;
    if (errstack->pos < errlen)
    {
        size_t new_size = errstack->size*2 + errlen;
        char* new_ptr = sre_new(new_size);
        size_t sizediff = new_size - errstack->size;
        memcpy(new_ptr + sizediff, errstack->ptr, errstack->size);

        errstack->size = new_size;
        errstack->pos += sizediff;
        errstack->ptr = new_ptr;
    }

    errstack->pos -= errlen;
    struct sreErr* err = (struct sreErr*)(errstack->ptr + errstack->pos);
    err->level = level;
    err->category = category;
    err->size = (int)errlen;
    vsnprintf(err->msg, formattedlen, fmtstring, fmt_va);

    return err;
}

int sre_errorEx(sreErrorLevel level, sreErrorCategory category, va_list fmt_va)
{
    if (category == SRE_ERR_NOERROR)
        return 0;

    struct sreErr* err = sre_pusherror(level, category, fmt_va);

    switch (level)
    {
    case SRE_ERROR:
        if (s_errcallbacks.error)
            s_errcallbacks.error(category, err->msg);
        
        if (!s_errcallbacks.error_override || !s_errcallbacks.error)
            err_logsingle(err);
        break;
    case SRE_CRITICAL:
        if (s_errcallbacks.critical)
            s_errcallbacks.critical(category, err->msg);
        
        if (!s_errcallbacks.critical_override || !s_errcallbacks.critical)
            err_logsingle(err);
        break;
    default:
        break;
    }

    return -1;
}

int sre_error(sreErrorCategory category, ...)
{
    int result;
    va_list va;
    va_start(va, category);
        result = sre_errorEx(SRE_ERROR, category, va);
    va_end(va);

    return result; // Always returns -1, unless you specified SRE_ERR_NOERROR in `category`
}

int sre_CRITICAL(sreErrorCategory category, ...)
{
    int result;
    va_list va;
    va_start(va, category);
        result = sre_errorEx(SRE_CRITICAL, category, va);
    va_end(va);

    // If you're on WinMain, specially on release you should get a nice message box in case of any critical errors
    abort();

    return result;
}

// Log the last error in the current thread, and do it again if it was automatically logged
int sre_logerror()
{
    struct sreErr* err = err_getlast();
    return err_logsingle(err);
}

// Set error callback
void sre_errorcallback(sreErrorLevel level, sreErrorCallback callback, bool override)
{
    switch (level)
    {
        // If level is none of the sreErrorLevel members (SRE_ERROR ; SRE_CRITICAL) then it is a no-op
        default: return;
        case SRE_ERROR:
            s_errcallbacks.error = callback;
            s_errcallbacks.error_override = override;
            break;
        case SRE_CRITICAL:
            s_errcallbacks.critical = callback;
            s_errcallbacks.critical_override = override;
            break;
    }
}