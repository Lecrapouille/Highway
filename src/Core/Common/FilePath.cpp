//==============================================================================
// MyLogger: A basic logger.
// Copyright 2018 Quentin Quadrat <lecrapouille@gmail.com>
//
// This file is part of MyLogger.
//
// MyLogger is free software: you can redistribute it and/or modify it
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
// along with MyLogger.  If not, see <http://www.gnu.org/licenses/>.
//==============================================================================

#include "Core/Common/FilePath.hpp"
#include <sstream>
#include <sys/stat.h>
#ifdef __APPLE__
#  include <CoreFoundation/CFBundle.h>
#endif

//------------------------------------------------------------------------------
#ifdef __APPLE__
fs::path osx_get_resources_dir(fs::path const& file)
{
    struct stat exists; // folder exists ?
    fs::path path;

    CFURLRef resourceURL = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
    char resourcePath[PATH_MAX];
    if (CFURLGetFileSystemRepresentation(resourceURL, true,
                                         reinterpret_cast<UInt8 *>(resourcePath),
                                         PATH_MAX))
    {
        if (resourceURL != NULL)
        {
            CFRelease(resourceURL);
        }

        path = fs::path(resourcePath) + "/" + file;
        if (stat(path.c_str(), &exists) == 0)
        {
            return path;
        }
    }

#ifdef DATADIR
    path = fs::path(DATADIR) + "/" + file;
    if (stat(path.c_str(), &exists) == 0)
    {
        return path;
    }
#endif

    path = "data/" + file;
    if (stat(path.c_str(), &exists) == 0)
    {
        return path;
    }

    return file;
}
#endif

//------------------------------------------------------------------------------
namespace File
{
    inline static bool exist(fs::path const& path)
    {
        struct stat buffer;
        return stat(path.c_str(), &buffer) == 0;
    }
} // namespace

//------------------------------------------------------------------------------
FilePath::FilePath(fs::path const& path)
{
    split(path);
}

//------------------------------------------------------------------------------
FilePath& FilePath::add(fs::path const& path)
{
    if (!path.empty())
    {
        split(path);
    }
    return *this;
}

//------------------------------------------------------------------------------
FilePath& FilePath::reset(fs::path const& path)
{
    m_search_paths.clear();
    split(path);
    return *this;
}

//------------------------------------------------------------------------------
FilePath& FilePath::clear()
{
    m_search_paths.clear();
    m_string_path.clear();
    return *this;
}

//------------------------------------------------------------------------------
FilePath& FilePath::remove(fs::path const& path)
{
    m_search_paths.remove(path);
    update();
    return *this;
}

//------------------------------------------------------------------------------
std::pair<fs::path, bool> FilePath::find(fs::path const& filename) const
{
    if (File::exist(filename))
        return std::make_pair(filename, true);

    if (!m_stack_path.empty())
    {
        fs::path temporary_file;
        temporary_file = top();
        temporary_file += filename;
        if (File::exist(temporary_file))
        {
            return std::make_pair(temporary_file, true);
        }
    }

    for (auto const& it: m_search_paths)
    {
        fs::path file(it / filename);
        if (File::exist(file))
        {
            return std::make_pair(file, true);
        }
    }

    // Not found
    return std::make_pair(fs::path(), false);
}

//------------------------------------------------------------------------------
fs::path FilePath::expand(fs::path const& filename) const
{
    for (auto const& it: m_search_paths)
    {
        fs::path file(it / filename);
        if (File::exist(file))
            return file;
    }

    return filename;
}

//------------------------------------------------------------------------------
fs::path const &FilePath::toString() const
{
    return m_string_path;
}

//------------------------------------------------------------------------------
void FilePath::update()
{
    m_string_path.clear();
    m_string_path += ".";
    m_string_path += m_delimiter;
    if (!m_stack_path.empty())
    {
        m_string_path += top();
        m_string_path += m_delimiter;
    }
    for (auto const& it: m_search_paths)
    {
        m_string_path += it;
        //m_string_path.pop_back(); // Remove the '/' char
        m_string_path += m_delimiter;
    }
}

//------------------------------------------------------------------------------
void FilePath::split(fs::path const& path)
{
    std::stringstream ss(path);
    std::string directory;

    while (std::getline(ss, directory, m_delimiter))
    {
        if (directory.empty())
            continue ;

        if ((*directory.rbegin() == '\\') || (*directory.rbegin() == '/'))
            m_search_paths.push_back(directory);
        else
            m_search_paths.push_back(directory + "/");
    }
    update();
}
