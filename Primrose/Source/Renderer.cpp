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


    //Transformations
    const glm::mat4 OrthographicMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);


    glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    glm::mat4 ViewMatrix = glm::mat4(1.0f);
    ViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, -3.0f));


    glEnable(GL_DEPTH_TEST);

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
        Camera* MainCamera = &m_ECSReference->GetMainCamera();

        TargetGameObject->GetTransform().m_Rotation.m_X += 1.0f;

        ShaderProgramTest.SetUniform("uModel", TargetGameObject->GetTransform().GetMatrix());
        ShaderProgramTest.SetUniform("uView", MainCamera->GetViewMatrix());
        ShaderProgramTest.SetUniform("uProjection", MainCamera->GetProjectionMatrix());


        //Get rest from window and camera? This class might as well have a reference to core


        ShaderProgramTest.SetUniform("uDiffuse", TextureUnit::DIFFUSE);

        const Texture2D* Sprite = TargetComponent->GetSprite();
        //Sprite->Bind(); //WTF
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
    const Color ClearColor = Colors::PaleGreen;
    GLCall(glClearColor(ClearColor.m_R, ClearColor.m_G, ClearColor.m_B, ClearColor.m_A));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}
void Renderer::SwapBuffers() const {
    m_WindowReference->SwapBuffers(); //Only once its done!
}