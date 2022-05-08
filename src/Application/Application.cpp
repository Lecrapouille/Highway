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

#include "Application.hpp"

// -----------------------------------------------------------------------------
Application::GUI::GUI(Application& application, const char* name,
                      sf::Color const& color)
    : background_color(color), m_application(application),
      m_renderer(application.renderer()), m_name(name)
{}

// -----------------------------------------------------------------------------
Application::Application(uint32_t const width, uint32_t const height,
                         std::string const& title)
{
    m_renderer.create(sf::VideoMode(width, height), title);
    m_renderer.setFramerateLimit(120);
}

// -----------------------------------------------------------------------------
Application::~Application()
{
    // Clear the satck of GUIs
    std::stack<Application::GUI*>().swap(m_stack);
    // Stop the SFML renderer
    m_renderer.close();
}

// -----------------------------------------------------------------------------
bool Application::font(const char* name, const char* ttf)
{
    std::unique_ptr<sf::Font> font = std::make_unique<sf::Font>();

    if (!font->loadFromFile(ttf))
    {
        if (!font->loadFromFile(std::string("data/") + ttf))
        {
            if (!font->loadFromFile(std::string(DATADIR"/") + ttf))
            {
                std::cerr << "Could not load font file " << ttf<< std::endl;
                return false;
            }
        }
    }

    m_fonts[name] = std::move(font);
    return true;
}

// -----------------------------------------------------------------------------
sf::Font& Application::font(const char* name)
{
    return *m_fonts.at(name);
}

// -----------------------------------------------------------------------------
void Application::push(Application::GUI& gui)
{
    std::cout << "GUI push: " << gui.name() << std::endl;
    if (peek() != nullptr)
    {
        // FIXME never call m_application.push(m_application.gui<GUIxx>("xxx"));
        // from the deactivate() callback!!
        peek()->deactivate();
    }
    m_stack.push(&gui);
    gui.activate();
    m_gui = &gui;
}

// -----------------------------------------------------------------------------
void Application::pop()
{
    m_gui = peek();
    if (m_gui == nullptr)
    {
        std::cout << "Warning! Cannot pop GUI from empty stack" << std::endl;
        return ;
    }

    std::cout << "GUI poped: " << m_gui->name() << std::endl;
    m_stack.pop();
    m_gui->deactivate();
}

// -----------------------------------------------------------------------------
void Application::loop()
{
    sf::Clock clock;

    while (m_renderer.isOpen())
    {
        float dt = clock.restart().asSeconds();

        m_gui = peek();
        if (m_gui != nullptr)
        {
            m_gui->handleInput();
            m_gui->update(dt);
            m_renderer.clear(m_gui->background_color);
            m_gui->draw();
        }
        m_renderer.display();
    }
}

// -----------------------------------------------------------------------------
void Application::loop(Application::GUI& starting_gui)
{
    push(starting_gui);
    loop();
    pop();
}
