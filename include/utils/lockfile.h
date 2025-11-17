// File locking macros
#if defined(_WIN32)

#define flockfile _lock_file
#define funlockfile _unlock_file

#elif defined(__unix__)

#define flockfile flockfile
#define funlockfile funlockfile

#else

#ifndef flockfile
#define flockfile
#endif
#ifndef funlockfile
#define funlockfile
#endif

#endif