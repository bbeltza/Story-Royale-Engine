#ifndef SRE_UT_LOCKFILE_H
#define SRE_UT_LOCKFILE_H
// File locking-related utilities

#if defined(_WIN32)

#define flockfile _lock_file
#define funlockfile _unlock_file

#elif defined(__unix__)

#define flockfile flockfile
#define funlockfile funlockfile

#else

#ifndef flockfile(x) (void)0
#define flockfile(x) (void)0

#endif

#endif

#if defined(_MSC_VER)

#define fclose_unlocked _fclose_nolock
#define fflush_unlocked _fflush_nolock
#define fgetc_unlocked _fgetc_nolock
#define fputc_unlocked _fputc_nolock
#define fread_unlocked _fread_nolock
#define fseek_unlocked _fseek_nolock
#define ftell_unlocked _ftell_nolock
#define fwrite_unlocked _fwrite_nolock
#define getc_unlocked _getc_nolock
#define putc_unlocked _putc_nolock
#define ungetc_unlocked _ungetc_nolock

#elif defined(__unix__)

#define fseek_unlocked fseek
#define ftell_unlocked ftell

#else

#define fclose_unlocked fclose
#define fflush_unlocked fflush
#define fgetc_unlocked fgetc
#define fputc_unlocked fputc
#define fread_unlocked fread
#define fseek_unlocked fseek
#define ftell_unlocked ftell
#define fwrite_unlocked fwrite
#define getc_unlocked getc
#define putc_unlocked putc
#define ungetc_unlocked ungetc

#endif

#endif