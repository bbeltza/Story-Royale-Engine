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
    typedef std::unordered_map<std::string, File::FileInfo> Map;

public:
    File();
    File(File&& moved);
    File(const File& other) = delete;
    
    ~File();

    void Load(const char* path);
    void* GetUserData() const {return m_userdata;}
    void* SetUserData(void* userdata) {m_userdata = userdata; return m_userdata;}

    inline const FileInfo& getInfo() const {return get_loaded().at(m_filepath);}
    inline const Type getType() const { return m_type; }
    inline const void* getRawData() const { return (void*)getInfo().data; }
    inline size_t getSize() const { return getInfo().size; }

    static inline bool areResourcesBound() { return _game_res != nullptr; }

private:
    friend class DrawingDevice;

    Type m_type;

    void* m_userdata = nullptr;
    std::string m_filepath;

    static Map& get_loaded();

    inline FileInfo& m_info() const { return get_loaded()[m_filepath]; }
};