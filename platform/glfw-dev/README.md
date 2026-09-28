# GLFW development frontend

`mbgl-glfw-dev` is the Linux/OpenGL experimental GLFW frontend. It keeps GLFW
input handling and all `mln::Map` camera mutations (`moveBy`, `scaleBy`,
`rotateBy`, and related gesture operations) on the GLFW event-loop thread. A
dedicated render thread owns the graphics context and calls
`Renderer::render()`.

`GLFWRendererFrontend::update()` transfers immutable `UpdateParameters` across
the thread boundary under a mutex. Render requests are coalesced: multiple
camera changes before a frame cause only the latest update to be drawn.
Framebuffer-size changes are likewise transferred to the render thread before
the next frame.

The target is enabled together with `MLN_WITH_GLFW` on Linux and is installed
as `mbgl-glfw-dev`.
