#ifdef INTERNAL_DEF
#define __def_internal(x) extern "C" void x()
#define __friend_internal(x) friend void ::x()
#else
#define __def_internal(x)
#define __friend_internal(x)
#endif