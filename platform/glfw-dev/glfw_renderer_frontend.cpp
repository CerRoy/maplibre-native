#include "glfw_renderer_frontend.hpp"

#include <mln/renderer/renderer.hpp>
#include <mln/gfx/backend_scope.hpp>
#include <mln/gfx/renderer_backend.hpp>
#include <mln/util/instrumentation.hpp>

GLFWRendererFrontend::GLFWRendererFrontend(std::unique_ptr<mln::Renderer> renderer_, GLFWView& glfwView_)
    : glfwView(glfwView_),
      renderer(std::move(renderer_)) {
    glfwView.setRenderFrontend(this);
}

GLFWRendererFrontend::~GLFWRendererFrontend() = default;

void GLFWRendererFrontend::reset() {
    std::scoped_lock lock(rendererMutex);
    assert(renderer);
    renderer.reset();
}

void GLFWRendererFrontend::setObserver(mln::RendererObserver& observer) {
    std::scoped_lock lock(rendererMutex);
    assert(renderer);
    renderer->setObserver(&observer);
}

void GLFWRendererFrontend::update(std::shared_ptr<mln::UpdateParameters> params) {
    {
        std::scoped_lock lock(updateMutex);
        updateParameters = std::move(params);
    }
    glfwView.invalidate();
}

const mln::TaggedScheduler& GLFWRendererFrontend::getThreadPool() const {
    return glfwView.getRendererBackend().getThreadPool();
}

void GLFWRendererFrontend::render() {
    MLN_TRACE_FUNC();

    std::scoped_lock rendererLock(rendererMutex);
    assert(renderer);

    std::shared_ptr<mln::UpdateParameters> updateParameters_;
    {
        std::scoped_lock lock(updateMutex);
        updateParameters_ = updateParameters;
    }
    if (!updateParameters_) return;

    mln::gfx::BackendScope guard{glfwView.getRendererBackend(), mln::gfx::BackendScope::ScopeType::Implicit};

    // Take ownership of the latest immutable parameters before rendering so
    // camera updates from the GLFW event thread can safely replace the next
    // frame while this one is in progress.
    renderer->render(updateParameters_);
}
