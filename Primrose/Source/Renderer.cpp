#include <Renderer.hpp>






void Renderer::Render() const {

}




void Renderer::Clear() noexcept {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}