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


    glEnable(GL_DEPTH_TEST); //TODO: move somewhere else
    glEnable(GL_BLEND); //TODO: move somewhere else

    //glBlendEquation();

    //TODO: Register error message when it happens here!

    const uint32 Amount = m_ECSReference->GetComponentsAmount<SpriteRenderer>();
    for (uint32 index = 0; index < Amount; index++) {
        //TODO: Check for nullness


        const SpriteRenderer* TargetComponent = m_ECSReference->GetComponentForUpdate<SpriteRenderer>();
        if (TargetComponent == nullptr)
            continue;
        if (!TargetComponent->GetEnabled())
            continue;
        GameObject* TargetGameObject = m_ECSReference->FindGameObject(TargetComponent->GetOwnerID());
        if (TargetGameObject == nullptr)
            continue;
        if (!TargetGameObject->GetEnabled())
            continue;

        //Texture
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLuint)TargetComponent->GetAddressingModeS()));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLuint)TargetComponent->GetAddressingModeT()));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLuint)TargetComponent->GetFilteringModeMin()));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLuint)TargetComponent->GetFilteringModeMag()));

        //Blending
        GLCall(glBlendFunc((GLuint)TargetComponent->GetSourceBlendMode(), (GLuint)TargetComponent->GetDestinationBlendMode()));
        GLCall(glBlendEquation((GLuint)TargetComponent->GetBlendEquation()));

        const Texture2D* Sprite = TargetComponent->GetSprite();
        //Sprite->Bind(); //WTF Check why this doesnt matter whether its called or not to render!
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Sprite->GetWidth(), Sprite->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, Sprite->GetData()));
        GLCall(glGenerateMipmap(GL_TEXTURE_2D));

        ShaderProgramTest.SetUniform("uDiffuse", TextureUnit::DIFFUSE);
        ShaderProgramTest.SetUniform("uTint", TargetComponent->GetTint());


        Transform* TargetTransform = &TargetGameObject->GetTransform();
        glm::mat4 TargetMatrix = TargetTransform->GetMatrix();

        //Flipping - Needs to be done before MVP
        if (TargetComponent->GetFlipX() || TargetComponent->GetFlipY()) {
            float ScaleX = TargetTransform->m_Scale.m_X;
            float ScaleY = TargetTransform->m_Scale.m_Y;

            if (TargetComponent->GetFlipX())
                ScaleX *= -1;
            else
                ScaleX = 0;
            if (TargetComponent->GetFlipY())
                ScaleY *= -1;
            else
                ScaleY = 0;

            TargetMatrix = glm::scale(TargetMatrix, glm::vec3(ScaleX, ScaleY, 0.0f)); //This is kinda sus
        }


        //Testing
        TargetGameObject->GetTransform().m_Rotation.m_X += 1.0f;
        TargetGameObject->GetTransform().m_Rotation.m_Y += 1.0f;

        //MVP
        Camera* ViewportCamera = &m_ECSReference->GetViewportCamera();

        ShaderProgramTest.SetUniform("uModel", TargetMatrix); //Construct matrix here instead of getting to apply the flipx anmd y?
        ShaderProgramTest.SetUniform("uView", ViewportCamera->GetViewMatrix());
        ShaderProgramTest.SetUniform("uProjection", ViewportCamera->GetProjectionMatrix());


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
void Renderer::SwapBuffers() const noexcept {
    glfwSwapBuffers(m_WindowReference->GetWindowResource().m_ptr);
}