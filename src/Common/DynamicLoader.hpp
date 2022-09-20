//=====================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2022 Quentin Quadrat <lecrapouille@gmail.com>
//
// This file is part of Highway.
//
// Highway is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#include <dlfcn.h>
#include <functional>
#include <string>
#include <stdexcept>
#include <sys/stat.h>

// *****************************************************************************
//! \brief Class allowing to load C functions from a given shared library. This
//! class wraps C functions dlopen(), dlsym(), dlerror().
// *****************************************************************************
class DynamicLoader
{
public:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    enum Resolution { LAZY = RTLD_LAZY, NOW = RTLD_NOW };

    //--------------------------------------------------------------------------
    //! \brief Close the shared library.
    //! \note this will invalidate pointer of symbols you have extracted.
    //--------------------------------------------------------------------------
    virtual ~DynamicLoader()
    {
        close();
    }

    //--------------------------------------------------------------------------
    //! \brief Load the given shared library (.so, .dylib, .dll file).
    //! Call the virtual method onLoading() in which you should extract symbols
    //! you want (ie with the method prototype()).
    //! \param[in] lib_name: the path of the shared library (.so, .dll, .dylib)
    //! to open.
    //! \param[in] rt load symbols immediatly or in lazy way.
    //--------------------------------------------------------------------------
    bool load(const char* lib_name, Resolution rt = Resolution::NOW)
    {
        m_path = lib_name;
        close();
        ::dlerror();
        m_handle = ::dlopen(lib_name, rt);
        if (m_handle == nullptr)
        {
            set_error(::dlerror());
            m_prevUpdateTime = 0;
        }
        else
        {
            try
            {
                onLoading();
                m_prevUpdateTime = getFileTime();
            }
            catch(std::logic_error &e)
            {
                return false;
            }
        }

        return m_handle != nullptr;
    }

    //--------------------------------------------------------------------------
    //! \brief Close the shared library and reload its symbols.
    //! \param[in] rt load symbols immediatly or in lazy way.
    //--------------------------------------------------------------------------
    bool reload(Resolution rt = Resolution::NOW)
    {
        return load(m_path.c_str(), rt);
    }

    //--------------------------------------------------------------------------
    //! \brief Call reload() if the time of the shared library has changed.
    //--------------------------------------------------------------------------
    bool reloadIfChanged(Resolution rt = Resolution::NOW)
    {
        long time = getFileTime();
        if (time == 0)
        {
            return false;
        }
        if (time != m_prevUpdateTime)
        {
            m_prevUpdateTime = time;
            return reload(rt);
        }

        return false;
    }

    //--------------------------------------------------------------------------
    //! \brief Check if the shared library has been opened.
    //! \return true if the shared library has been opened with success.
    //--------------------------------------------------------------------------
    inline operator bool() const
    {
        return m_handle != nullptr;
    }

    //--------------------------------------------------------------------------
    //! \brief Close the shared library (even if it was not loaded).
    //! \note this will invalidate pointer of symbols you have extracted.
    //--------------------------------------------------------------------------
    void close()
    {
        if (m_handle != nullptr)
        {
            ::dlclose(m_handle);
            m_handle = nullptr;
        }
        m_error.clear();
    }

    //--------------------------------------------------------------------------
    //! \brief Return the address of the symbol given its name.
    //! \return the address of the symbol if present, else return nullptr.
    //--------------------------------------------------------------------------
    void* address(const char* symbol)
    {
        if (m_handle != nullptr)
        {
            ::dlerror();
            void* addr = ::dlsym(m_handle, symbol);
            const char* error = ::dlerror();
            if ((addr == nullptr) && (error != nullptr))
            {
                set_error(error);
            }
            else
            {
                m_error.clear();
            }
            return addr;
        }
        else
        {
            set_error("shared library not opened!");
            return nullptr;
        }
    }

    //--------------------------------------------------------------------------
    //! \brief Similar to address() but return a std::function and throw an
    //! exception if the symbol was not find.
    //--------------------------------------------------------------------------
    template<typename T>
    std::function<T> prototype(const char* symbol)
    {
        void* addr = address(symbol);
        if (addr == nullptr)
        {
            //std::cerr << error() << std::endl;
            throw std::logic_error(error());
        }
        return reinterpret_cast<T*>(addr);
    }

    //--------------------------------------------------------------------------
    //! \brief Return the path of the shared library.
    //--------------------------------------------------------------------------
    inline std::string const& path() const
    {
        return m_path;
    }

    //--------------------------------------------------------------------------
    //! \brief Return the latest error.
    //--------------------------------------------------------------------------
    inline std::string const& error() const
    {
        return m_error;
    }

private:

    //--------------------------------------------------------------------------
    //! \brief Callback to implement when load() or reload() has ended with
    //! success.
    //--------------------------------------------------------------------------
    virtual void onLoading()
    {}

    //--------------------------------------------------------------------------
    //! \brief Concat error messages.
    //--------------------------------------------------------------------------
    void set_error(const char* msg)
    {
        if (msg == nullptr)
            return ;
        m_error += std::string("- ") + msg + '\n';
    }

    //--------------------------------------------------------------------------
    //! \brief Return the date of the shared library.
    //! \pre The shared library shall be loaded.
    //--------------------------------------------------------------------------
    long getFileTime()
    {
        struct stat fileStat;

        if (stat(m_path.c_str(), &fileStat) < 0)
        {
            set_error("Couldn't stat file");
            return 0;
        }

        return fileStat.st_mtime;
    }

private:

    //! \brief Memorize the path of the shared library.
    std::string m_path;
    //! \brief Memorize the latest error.
    std::string m_error;
    //! \brief The handle on the opened shared library.
    void *m_handle = nullptr;
    //! \brief The date of the shared lib
    long m_prevUpdateTime = 0;
};
