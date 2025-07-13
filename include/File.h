#pragma once
#include <string>
#include "Enums.h"

extern "C"
{
    extern const unsigned char* const _game_res;
};

class File
{
public:
    enum struct Type : unsigned char
{
    Uninitialized = 0,
    Text,
    Image,
    Font,

    Unknown
};

public:
    File();
    ~File();

    void Load(const char* path);
    void* GetUserData() const {return m_userdata;}

    inline const Type getType() const { return m_type; }
    inline const unsigned char* getRawData() const { return m_info.data; }
    inline size_t getSize() const { return m_info.size; }

    static inline bool areResourcesBound() {return s_resbind;}

private:
    friend class DrawingDevice;

    Type m_type;

    struct FileInfo
    {
        ~FileInfo() {if (!File::areResourcesBound()) delete[] data;}
        size_t size;
        void* handle;
        unsigned char* data = nullptr;
    } m_info;
    void* m_userdata = nullptr;
    std::string m_filepath;

    static const bool s_resbind;
};