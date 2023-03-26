#pragma once

#ifdef WITH_GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

namespace cubos::core::io
{
    /// Handle for a custom mouse cursor.
    class Cursor
    {
    public:
        /// Identifiers for the different standard cursors.
        enum class Standard
        {
            Arrow,      ///< Standard arrow cursor.
            IBeam,      ///< Standard I-beam cursor.
            Cross,      ///< Standard crosshair cursor.
            Hand,       ///< Standard hand cursor.
            EWResize,   ///< Standard horizontal resize cursor.
            NSResize,   ///< Standard vertical resize cursor.
            NWSEResize, ///< Standard diagonal resize cursor (top-left to bottom-right).
            NESWResize, ///< Standard diagonal resize cursor (top-right to bottom-left).
            AllResize,  ///< Standard all directions resize cursor.
            NotAllowed, ///< Standard not allowed cursor.
        };

        ~Cursor();

    private:
#ifdef WITH_GLFW
        friend class GLFWWindow;

        Cursor(GLFWcursor* handle);

        GLFWcursor* mGlfwHandle; ///< Handle for the GLFW cursor.
#endif
    };
} // namespace cubos::core::io
