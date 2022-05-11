# Helper routines for general purpose

- `backward.[ch]pp`: allow to display a nice stack trace when a crash occured. See https://github.com/bombela/backward-cpp
- `Components.[ch]pp`: an Entity Component System (ECS) like done in Unity but implemented in a naive way. A nice implementation would be https://austinmorlan.com/posts/entity_component_system/
- `DynamicLoader.hpp`: to load a shared library and extract C function symbols.
- `Monotoring.[ch]pp`: to be reworked: save simulation data in a CSV file to be replayed in an application such as Matlab.
- `NonCopyable.hpp`: to make a class non copyable.
- `Singleton.hpp`: to make a class a singleton.
- `Path.[ch]pp`: for searching file like into several folders in the same way that Linux `$PATH` but in our case not necessary for looking executables.
- `SpatialHashGrid.[ch]pp`: allow to hash actor position in the aim to mimize the number of iterations for searching other actors around them (i.e. for doing collision detection).
- `StateMachine.hpp`: Base class for creating state machine and hiding their implementation.
