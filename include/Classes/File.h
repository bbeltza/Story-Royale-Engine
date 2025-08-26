#pragma once
#include <standard.h>

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
    Sound,
    Font,

    Unknown
};

private:
struct FileInfo
    {
        ~FileInfo() {if (data && !resbind) {delete[] data; fclose((FILE*)handle);}}
        size_t size;
        void* handle;
        unsigned char* data = nullptr;
        // Whether the file points to the embedded resources or not 
        bool resbind;
    };

public:
    File();
    File(File&& moved);
    File(const File& other) = delete;
    
    ~File();

    void Load(const char* path);
    void* GetUserData() const {return m_userdata;}
    void* SetUserData(void* userdata) {m_userdata = userdata; return m_userdata;}

    inline const FileInfo& getInfo() const {return s_loadedfiles->at(m_filepath);}
    inline const Type getType() const { return m_type; }
    inline const unsigned char* getRawData() const { return getInfo().data; }
    inline size_t getSize() const { return getInfo().size; }

    static inline bool areResourcesBound() {return s_resbind;}

private:
    struct Initializer
    {
        inline ~Initializer() {
            if (s_loadedfiles)
                delete s_loadedfiles;
            
            s_loadedfiles = nullptr;
        }
    };

    friend class Initializer;
    friend class DrawingDevice;

    Type m_type;

    void* m_userdata = nullptr;
    std::string m_filepath;

    // This one is different, tells if the resources are binded, m_info.resbind is always false if this constant is false
    static const bool s_resbind;

    static std::unordered_map<std::string, FileInfo> *s_loadedfiles;
    static const Initializer s_init;

    inline FileInfo& m_info() const { return (*s_loadedfiles)[m_filepath]; }
};