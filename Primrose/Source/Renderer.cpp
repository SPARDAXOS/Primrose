#include <Renderer.hpp>
#include "Window.hpp"
#include "EntityComponentSystem.hpp"






void Renderer::Update() const {
    Clear();
    Render2D();
    Render3D();
    SwapBuffers();
}
void Renderer::TestRender(const VAO& vao) const {
    //Clear(); //Only at the start of a new frame!

    //glBindVertexArray(VertexArrayObject.m_ID);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject.m_ID);
    //glDrawArrays(GL_TRIANGLES, 0, 3);

    vao.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
}


void Renderer::Render2D() const noexcept {
    




    //Get amount from ECS
    //Call loop depending on amount from ECS
    //Get Transform using component Object ID
    //Render using them
}
void Renderer::Render3D() const noexcept {

}



void Renderer::Clear() const noexcept {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
void Renderer::SwapBuffers() const {
    m_WindowReference->SwapBuffers(); //Only once its done!
}