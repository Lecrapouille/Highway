#ifndef COLLIDE_HPP
#  define COLLIDE_HPP

#  include <SFML/Graphics/RectangleShape.hpp>

// Separating Axis Theorem (SAT) collision test
// Minimum Translation Vector (MTV) is returned for the first Oriented Bounding Box (OBB)
bool collide(const sf::RectangleShape& obb1, const sf::RectangleShape& obb2,
             sf::Vector2f& mtv);

#endif
