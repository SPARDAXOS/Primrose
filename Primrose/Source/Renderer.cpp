#include <Renderer.hpp>
#include "Window.hpp"






void Renderer::Render() const {

}
void Renderer::TestRender(const VAO& vao) const {
    Clear();

    //glBindVertexArray(VertexArrayObject.m_ID);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject.m_ID);
    //glDrawArrays(GL_TRIANGLES, 0, 3);

    vao.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
    m_WindowReference->SwapBuffers();
}




void Renderer::Clear() const noexcept {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}