// Original code source https://stackoverflow.com/questions/44105058/how-does-unitys-getcomponent-work
// Quote: "I'm wondering how the GetComponent() method that Unity implements works."

#ifndef COMPONENTS_HPP
#  define COMPONENTS_HPP

#  include <algorithm> // find_if
#  include <stdexcept> // std::out_of_range
#  include <vector>
#  include <memory> // unique_ptr

// *****************************************************************************
//! \brief FNV-1a 32bit hashing algorithm.
//! See https://gist.github.com/Lee-R/3839813
// *****************************************************************************
constexpr uint32_t hashing(char const* s, std::size_t count)
{
    return ((count ? hashing(s, count - 1) : 2166136261u) ^ uint32_t(s[count]))
            * 16777619u;
}

// *****************************************************************************
//! \brief Create hash of string.
//! \code
//! const uint32_t type = "Component"_hash;
//! \endcode
// *****************************************************************************
constexpr uint32_t operator"" _hash(char const* s, std::size_t count)
{
    return hashing(s, count);
}

// *****************************************************************************
//! \brief Base class for Component
// *****************************************************************************
class Component
{
public:

    // -------------------------------------------------------------------------
    virtual ~Component() = default;

    // -------------------------------------------------------------------------
    //! \brief Emulate RTTI
    // -------------------------------------------------------------------------
    virtual bool isClassType(const std::size_t classType) const
    {
        return classType == Type;
    }

public:

    static constexpr std::size_t Type = "Component"_hash;
};

// *****************************************************************************
//! \brief Give a type for the derived class.
//! \code
//! struct EventA : public Event { RTTI(EventA, Event); };
//! struct EventB : public EventA { RTTI(EventB, EventA); };
//! Event e; EventA a; EventB b;
//! e.isClassType(a.type); // false
//! a.isClassType(e.type); // true
//! b.isClassType(a.type); // true
//! a.isClassType(b.type); // false
//! \endcode
// *****************************************************************************
#  define COMPONENT_CLASSTYPE(CLASS, PARENT)                                \
    public:                                                                 \
       virtual bool isClassType(const std::size_t classType) const {        \
          return (classType == CLASS::type)                                 \
                ? true : PARENT::isClassType(classType);                    \
       }                                                                    \
       static constexpr std::size_t type = #CLASS##_hash

// *****************************************************************************
//! \brief Container for Components
// *****************************************************************************
class Components
{
public:

    // -------------------------------------------------------------------------
    //! \brief Create and store a new component.
    //!
    //! \tparam ComponentType the type of the component (shall derive from \c
    //! Component).
    //! \param params list of param for creating the component.
    //! \return the reference of the newly created component.
    // -------------------------------------------------------------------------
    template<class ComponentType, typename... Args>
    ComponentType& addComponent(Args&&... params)
    {
        //static_assert(std::is_base_of<ComponentType, Component>::value,
        //              "Components: Try adding an instance not deriving from Component");
        m_components.emplace_back(
            std::make_unique<ComponentType>(std::forward<Args>(params)...));
        return *static_cast<ComponentType*>(m_components.back().get());
    }

    // -------------------------------------------------------------------------
    //! \brief Find and return the first component of given type (including if
    //! ancestor are of the given type).
    //!
    //! \throw std::out_of_range if no component has been found.
    //! \return the reference of the component.
    // -------------------------------------------------------------------------
    template<class ComponentType>
    ComponentType& getComponent()
    {
        for (auto && component: m_components)
        {
            if (component->isClassType(ComponentType::Type))
                return *static_cast<ComponentType*>(component.get());
        }

        //return *std::unique_ptr<ComponentType>(nullptr);
        throw std::out_of_range("No component found");
    }

    // -------------------------------------------------------------------------
    //! \brief Remove the component of given type (including if ancestor are of
    //! the given type).
    //! \return true if one component has been found, else return false.
    // -------------------------------------------------------------------------
    template<class ComponentType>
    bool removeComponent()
    {
        if (m_components.empty())
            return false;

        auto index =
                std::find_if(m_components.begin(),
                             m_components.end(),
                             [](std::unique_ptr<Component> & component)
                             {
                                 return component->isClassType(ComponentType::Type);
                             });

        bool success = index != m_components.end();
        if (success)
            m_components.erase(index);

        return success;
    }

    // -------------------------------------------------------------------------
    //! \brief Return the list of components of given type (including if
    //! ancestor are of the given type).
    //! \return the std::vector of components (may be empty).
    // -------------------------------------------------------------------------
    template<class ComponentType>
    std::vector<ComponentType*> getComponents()
    {
        std::vector<ComponentType*> componentsOfType;

        for (auto && component : m_components)
        {
            if (component->isClassType(ComponentType::Type))
                componentsOfType.emplace_back(static_cast<ComponentType*>(component.get()));
        }

        return componentsOfType;
    }

    // -------------------------------------------------------------------------
    //! \brief Remove all components of given type (including if ancestor are
    //! of the given type).
    //!
    //! \note complexity is O(n^2) where n is the number of elements. Indeed the
    //! container is a liast so he search takes O(n) and removing inside a list
    //! is also O(n).
    //!
    //! \return the number of components removed.
    // -------------------------------------------------------------------------
    template<class ComponentType>
    size_t removeComponents()
    {
        bool success = false;
        size_t numRemoved = 0u;

        if (m_components.empty())
            return numRemoved;

        do
        {
            auto index =
                    std::find_if(m_components.begin(),
                                 m_components.end(),
                                 [](std::unique_ptr<Component> & component)
                                 {
                                     return component->isClassType(ComponentType::Type);
                                 });

            success = (index != m_components.end());
            if (success)
            {
                m_components.erase(index);
                ++numRemoved;
            }
        }
        while (success);

        return numRemoved;
    }

    // -------------------------------------------------------------------------
    //! \brief Erase all components inside the container.
    // -------------------------------------------------------------------------
    void clear()
    {
        m_components.clear();
    }

    // -------------------------------------------------------------------------
    //! \brief Return the number of components hold by this container.
    // -------------------------------------------------------------------------
    size_t countComponents() const
    {
        return m_components.size();
    }

private:

    //! \brief Container of components
    std::vector<std::unique_ptr<Component>> m_components;
};

#endif // COMPONENTS_HPP
