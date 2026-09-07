// <rp3d.h> -*- C++ -*-

#pragma once

/**
 * @brief Helper include for reactphysics3d to remove raylib colour macros before
 * including the reactphysics3d header.
 */

#pragma push_macro("RED")
#pragma push_macro("GREEN")
#pragma push_macro("BLUE")
#pragma push_macro("BLACK")
#pragma push_macro("WHITE")
#pragma push_macro("YELLOW")
#pragma push_macro("MAGENTA")
#pragma push_macro("CYAN")

#ifdef RED
#undef RED
#endif

#ifdef GREEN
#undef GREEN
#endif

#ifdef BLUE
#undef BLUE
#endif

#ifdef BLACK
#undef BLACK
#endif

#ifdef WHITE
#undef WHITE
#endif

#ifdef YELLOW
#undef YELLOW
#endif

#ifdef MAGENTA
#undef MAGENTA
#endif

#ifdef CYAN
#undef CYAN
#endif

#include <reactphysics3d/reactphysics3d.h>

#pragma pop_macro("CYAN")
#pragma pop_macro("MAGENTA")
#pragma pop_macro("YELLOW")
#pragma pop_macro("WHITE")
#pragma pop_macro("BLACK")
#pragma pop_macro("BLUE")
#pragma pop_macro("GREEN")
#pragma pop_macro("RED")
