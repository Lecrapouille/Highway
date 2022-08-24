// 2021 -- 2022 Quentin Quadrat quentin.quadrat@gmail.com
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

#include "Renderer/FontManager.hpp"
#include <iostream>
#ifdef __APPLE__
#  include <CoreFoundation/CFBundle.h>
#endif

//-----------------------------------------------------------------------------
FontManager::FontManager()
    : path("")
{
#  ifdef __APPLE__

    // Add the resources folder inside MacOS bundle application
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

        path.add(std::string(resourcePath));
    }

#endif

    path.add(".:data:" DATADIR);
}

// -----------------------------------------------------------------------------
bool FontManager::load(const char* name, const char* ttf)
{
    std::unique_ptr<sf::Font> font = std::make_unique<sf::Font>();
    if (font->loadFromFile(path.expand(ttf)))
    {
        m_fonts[name] = std::move(font);
        return true;
    }

    // An error is already displayed on the console
    return false;
}

// -----------------------------------------------------------------------------
sf::Font& FontManager::font(const char* name)
{
    try
    {
        return *m_fonts.at(name);
    }
    catch (...)
    {
        std::string e("Fatal: Unkown font '");
        throw std::runtime_error(e + name + "'");
    }
}
