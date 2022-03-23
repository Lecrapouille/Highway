// 2021 Quentin Quadrat quentin.quadrat@gmail.com
//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <https://unlicense.org>

#ifndef MONITORING_HPP
#  define MONITORING_HPP

#  include <fstream>
#  include <map>
#  include <memory>

class Monitoring
{
public:

    //--------------------------------------------------------------------------
    ~Monitoring();

    //--------------------------------------------------------------------------
    template<typename T1, typename ... T2>
    void log(std::string const& name, const T1& head, const T2&... tail)
    {
        std::ofstream* fd = stream(name);
        if (fd != nullptr)
        {
            *fd << head << " ";
            log(*fd, tail...);
            *fd << ";" << std::endl;
        }
    }

    //--------------------------------------------------------------------------
    void close(std::string const& name)
    {
        std::ofstream* fd = stream(name);
        if (fd != nullptr)
        {
            *fd << "];" << std::endl;
        }
        m_files[name] = nullptr;
    }

private:

    //--------------------------------------------------------------------------
    template<typename T1, typename ... T2>
    void log(std::ofstream& fd, const T1& head, const T2&... tail)
    {
        fd << head << " ";
        log(fd, tail...);
    }

    //--------------------------------------------------------------------------
    void log(std::ofstream& fd)
    {
        // Do nothing
    }

    //--------------------------------------------------------------------------
    std::ofstream* stream(std::string const& name);

private:

    std::map<std::string, std::unique_ptr<std::ofstream>> m_files;
};

#endif
