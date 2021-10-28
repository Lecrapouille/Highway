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

#include "Utils/Monitoring.hpp"

Monitoring::~Monitoring()
{
    for (auto& it: m_files)
    {
        *it.second << "];" << std::endl;
    }
}

std::ofstream* Monitoring::stream(std::string const& name)
{
    auto it = m_files.find(name);
    if ((it == m_files.end()) || (it->second == nullptr))
    {
        auto ptr = std::make_unique<std::ofstream>();
        std::ofstream& fd = *ptr;

        fd.open(name + ".jl");
        if (fd.bad())
            return nullptr;

        //fd << "using Plots" << std::endl << std::endl;
        fd << "# Time [s], Throttle [%], Longitudinal vehicle speed [m/s], "
           << "Gravitational forces [N], Total longitudinal reistance load [N], "
           << "Aero forces [N], Load forces [N], Longitudinal vehicle "
           << "acceleration [m/s/s]" << std::endl;
        fd << "D=[" << std::endl;

        m_files[name] = std::move(ptr);
        return m_files[name].get();
    }
    return it->second.get();
}
