# Batman: Arkham Knight ships as a 64-bit binary only, and the class layouts in
# "GameDefines.hpp" describe that build (8 byte pointers at 4 byte alignment).
set(ENGINE_POINTER_SIZE 8)
