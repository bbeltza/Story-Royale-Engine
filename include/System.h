#pragma once
#include <string>
#include <setjmp.h>
#include <fmt/format.h>

struct System
{
    
    enum ErrorId: unsigned char
    {
        NO_ERROR = 0,
        FILE_NOT_FOUND,
        SDL_ERROR
    };
    
    template <class... T> static void Log(const char* fmtstr, const T&... args) {fmt::print(fmtstr, args...);}
    template <class... T> static inline void Error(ErrorId id, const T&... args)
    {
        last_err.assign("ERROR:\n\n");

        switch (id)
        {
        case FILE_NOT_FOUND:
            last_err.append(fmt::format("Couldn't find '{}', no such file or directory", args...));
            break;
        case SDL_ERROR:
            last_err.append(fmt::format("SDL Error: '{}'", GetSDLError()));
            break;
        default:
            last_err.append(fmt::format("An error has occurred!"));
            break;
        }

        last_err.append(fmt::format("\n(Code: {})", (int)id));

        //__debugbreak();
        
        longjmp(err, id);
    }

    static void Exit() {exit(0);}
    static void Sleep(float duration);
    
private:
    System() {};
    friend int main(void);

    static jmp_buf err;
    static std::string last_err;

    static const char* GetSDLError();
};