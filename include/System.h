#pragma once
#include <string>
#include <setjmp.h>
#include <fmt/format.h>

struct System
{
    
    enum ErrorId: unsigned char
    {
        NO_ERROR = 0,
        FILE_NOT_FOUND
    };
    
    template <class... T> static void Log(const char* fmtstr, const T&... args) {fmt::print(fmtstr, args...);}
    template <class... T> static inline void Error(ErrorId id, const T&... args)
    {
        last_err.assign("ERROR:\n\n");

        switch (id)
        {
        case FILE_NOT_FOUND:
            last_err.append(fmt::format("Couldn't find '{}', no such directory", args...));
            break;
        default:
            last_err.append(fmt::format("An error has occurred!"));
            break;
        }

        last_err.append(fmt::format("\n(Code: {})", (int)id));
        
        longjmp(err, id);
    }
    
private:
    System() {};
    friend int main(void);

    static jmp_buf err;
    static std::string last_err;
};