#include <Renderer.hpp>
#include "Window.hpp"
#include "EntityComponentSystem.hpp"
#include "GameObject.hpp"






bool Renderer::Update() const {
    Clear();

    bool RendererStatus = true;
    if (!Render2D() && !Render3D())
        RendererStatus = false;

    SwapBuffers();

    return RendererStatus;
}
void Renderer::TestRender(const VAO& vao) const {
    //Clear(); //Only at the start of a new frame!

    //glBindVertexArray(VertexArrayObject.m_ID);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject.m_ID);
    //glDrawArrays(GL_TRIANGLES, 0, 3);

    vao.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
}


bool Renderer::Render2D() const {
    
    //Shaders
    Shader VertexShader(GL_VERTEX_SHADER, "Resources/Shaders/Vertex.glsl");
    Shader FragmentShader(GL_FRAGMENT_SHADER, "Resources/Shaders/Fragment.glsl");

    ShaderProgram ShaderProgramTest;
    ShaderProgramTest.AttachShader(VertexShader);
    ShaderProgramTest.AttachShader(FragmentShader);
    if (!ShaderProgramTest.LinkShaderProgram())
        PrintMessage("LinkShaderProgram Failed");
    ShaderProgramTest.Bind();


    //TODO: Register error message when it happens here!

    const uint32 Amount = m_ECSReference->GetComponentsAmount<SpriteRenderer>();
    for (uint32 index = 0; index < Amount; index++) {
        //TODO: Check for nullness

        const SpriteRenderer* TargetComponent = m_ECSReference->GetComponentForUpdate<SpriteRenderer>();


        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLuint)TargetComponent->GetAddressingModeS()));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLuint)TargetComponent->GetAddressingModeT()));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLuint)TargetComponent->GetFilteringModeMin()));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLuint)TargetComponent->GetFilteringModeMag()));



        GameObject* TargetGameObject = m_ECSReference->FindGameObject(TargetComponent->GetOwnerID());

        ShaderProgramTest.SetUniform("uTransform", TargetGameObject->GetTransform().GetMatrix());
        ShaderProgramTest.SetUniform("uDiffuse", TextureUnit::DIFFUSE);

        const Texture2D* Sprite = TargetComponent->GetSprite();
        //Sprite->Bind(); WTF
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Sprite->GetWidth(), Sprite->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, Sprite->GetData()));
        GLCall(glGenerateMipmap(GL_TEXTURE_2D));

        TargetComponent->GetVAO()->Bind();
        GLCall(glDrawElements(GL_TRIANGLES, TargetComponent->GetEBO()->GetCount(), GL_UNSIGNED_INT, nullptr));
    }

    return true;
}
bool Renderer::Render3D() const {

    return true;
}



void Renderer::Clear() const noexcept {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
void Renderer::SwapBuffers() const {
    m_WindowReference->SwapBuffers(); //Only once its done!
}