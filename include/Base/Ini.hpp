#ifndef SRE_INI_HPP
#define SRE_INI_HPP
#include <standard>
#include <ints.h>

namespace sre
{
    class File;

    class IniFile
    {
        public:
        using string = std::string;
        using var_map = std::unordered_map<string, string>;
        using section_map = std::unordered_map<string, var_map>;

        public:
        bool load(const char* from_path);
        bool load(const File& from_file);
        bool load_text(const char* text);

        bool save(const char* path);

        section_map sections;
    };
}

#endif