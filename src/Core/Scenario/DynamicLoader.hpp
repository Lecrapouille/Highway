//=====================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2023 Quentin Quadrat <lecrapouille@gmail.com>
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
// along with Highway.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#pragma once

#include "Core/Common/FileSystem.hpp"

#include <dlfcn.h>
#include <sys/stat.h>
#include <string.h>

#include <functional>
#include <string>
#include <stdexcept>



#include <iostream> // TEMP
#include <chrono>
#include <thread>
using namespace std::chrono_literals;

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
    enum class ResolveTime { Lazy = RTLD_LAZY, Now = RTLD_NOW };

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    enum class Visiblity {
        None = 0,
        Global = RTLD_GLOBAL,
        Local = RTLD_LOCAL,
        NoDelete = RTLD_NODELETE,
        NoLoad = RTLD_NOLOAD,
        DeepBind = RTLD_DEEPBIND
    };

    //--------------------------------------------------------------------------
    //! \brief Close the shared library.
    //! \note this will invalidate pointer of symbols you have extracted.
    //--------------------------------------------------------------------------
    ~DynamicLoader()
    {
        close();
    }

    //--------------------------------------------------------------------------
    //! \brief Close the shared library (even if it was not loaded).
    //! \note this will invalidate pointer of symbols you have extracted.
    //--------------------------------------------------------------------------
    void close()
    {
        // Reference counting is done automatically by dlclose(), but the fact
        // that this call is the last one and does unload the library is not
        // indicated.
        if (m_handle == nullptr)
        {
            // Used to test if the library is already resident (dlopen() returns
            // NULL if it is not, or the library's handle if it is resident).
            void* h = ::dlopen(m_libpath.c_str(), RTLD_NOW | RTLD_NOLOAD);
            if (h != nullptr)
            {
                ::dlclose(h);
            }
        }
        else
        {
            // Close the previous call of load(...)
            ::dlclose(m_handle);
        }

        // Clear for a latter usage.
        m_error.clear();
        m_time = -1;
        m_handle = nullptr;

        // Clear previous errors.
        ::dlerror();
    }

    //--------------------------------------------------------------------------
    //! \brief Load the given shared library (.so, .dylib, .dll file).
    //! \param[in] libpath: the path of the shared library (.so, .dll, .dylib)
    //! to open.
    //! \param[in] rt load symbols immediately or in lazy way.
    //--------------------------------------------------------------------------
    bool load(fs::path const& libpath, ResolveTime rt, Visiblity visibility)
    {
        m_libpath = libpath;

        // Be sure reference counting is 0 and have unload symbols.
        this->close();

        // Open the shared library
        int flags = static_cast<int>(rt) | static_cast<int>(visibility);
        m_handle = ::dlopen(libpath.c_str(), flags);
        if (m_handle == nullptr)
        {
            saveError(::dlerror());
            return false;
        }
        else
        {
            m_resolve_time = rt;
            m_visibility_flags = visibility;
            updateFileTime();
            return true;
        }
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    bool reload()
    {
        bool rc;

        if (updateFileTime())
        {
                    std::this_thread::sleep_for(1000ms);
            rc = load(m_libpath.c_str(), m_resolve_time, m_visibility_flags);
        }
        else
        {
            rc = false;
        }
        return rc;
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    template <typename Prototype>
    inline std::function<Prototype> lookup(char const* fname) // const
    {
        return SymbolLookupWrapper<Prototype>::lookup(*this, fname);
    }

    //--------------------------------------------------------------------------
    //! \brief Return if the shared library has been loaded with success.
    //--------------------------------------------------------------------------
    inline bool loaded() const
    {
        return m_handle != nullptr;
    }

    //--------------------------------------------------------------------------
    //! \brief Return the path of the loaded lib.
    //--------------------------------------------------------------------------
    inline fs::path const& path() const
    {
        return m_libpath;
    }

    //--------------------------------------------------------------------------
    //! \brief Return the latest error.
    //--------------------------------------------------------------------------
    inline std::string const& error() const
    {
        return m_error;
    }

private:

    // Helper class to allow for function pointer syntax in template parameter.
    // To allow a call such as lib.lookup<int(int)>("name"), a class template
    // with partial specialization must be used since function templates
    // cannot be partially specialized.
    template <typename Prototype>
    struct SymbolLookupWrapper;

    template <typename Ret, typename... Args>
    struct SymbolLookupWrapper<Ret(Args...)>
    {
        static inline std::function<Ret(Args...)> lookup(DynamicLoader& dylib, char const* fname)
        {
            return dylib.private_lookup<Ret, Args...>(fname);
        }
    };

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    template <typename Ret, typename... Args>
    inline std::function<Ret(Args...)> private_lookup(char const* fname)
    {
        // If the handle is invalid, return an uninitialized optional object.
        if (m_handle == nullptr)
        {
            throw std::logic_error("Handle not open.");
        }

        // Clear previous errors.
        ::dlerror();

        // Lookup the symbol from the dynamic library.
        auto p = ::dlsym(m_handle, fname);
        auto fptr = DynamicLoader::fptr_cast<Ret, Args...>(p);

        // If there was an error, return an uninitialized optional.
        char const* error_message = ::dlerror();
        if (error_message != nullptr)
        {
            saveError(error_message);
            
        }

        return fptr;
    }

    //--------------------------------------------------------------------------
    //! \brief Vonvert a void* to a std::function.
    //! Since casting from a void* to a function pointer is undefined,
    //! we have to do a workaround. I am not positive if this is completely
    //! portable or not.
    //--------------------------------------------------------------------------
    template <typename Ret, typename... Args>
    static std::function<Ret(Args...)> fptr_cast(void* fptr)
    {
        using function_type = Ret (*)(Args...);
        using ptr_size_type = std::conditional_t<sizeof(fptr) == 4, long, long long>;

        return reinterpret_cast<function_type>(reinterpret_cast<ptr_size_type>(fptr));
    }

    //--------------------------------------------------------------------------
    //! \brief Save the current dlerror.
    //--------------------------------------------------------------------------
    void saveError(char const* message)
    {
        if (message == nullptr)
        {
            m_error.clear();
            return ;
        }
        m_error = message;
    }

    //--------------------------------------------------------------------------
    //! \brief Return the date of the shared library.
    //! \pre The shared library shall be loaded.
    //--------------------------------------------------------------------------
    bool updateFileTime()
    {
        struct stat fileStat;
        if (stat(m_libpath.c_str(), &fileStat) >= 0)
        {
            bool reloaded = (m_time != fileStat.st_mtime);
            m_time = fileStat.st_mtime;
            return reloaded;
        }
        else
        {
            // Keep the previous time
            saveError(strerror(errno));
            return false;
        }
    }

private:

    //! \brief The handle on the opened shared library.
    void* m_handle = nullptr;
    //! \brief Memorize the path of the shared library.
    fs::path m_libpath;
    //! Memorize flags.
    ResolveTime m_resolve_time = ResolveTime::Now;
    Visiblity m_visibility_flags = Visiblity::None;
    //! \brief The updated time of the shared lib
    long m_time = -1;
    //! \brief Memorize the latest error.
    std::string m_error;
};

//--------------------------------------------------------------------------
//! \briefAllow ORing two options together.
//--------------------------------------------------------------------------
inline DynamicLoader::Visiblity operator|(DynamicLoader::Visiblity lhs, DynamicLoader::Visiblity rhs)
{
    return static_cast<DynamicLoader::Visiblity>(static_cast<int>(lhs) | static_cast<int>(rhs));
}