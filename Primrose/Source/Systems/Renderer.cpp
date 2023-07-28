#include "Systems/Renderer.hpp"
#include "Systems/Core.hpp"



Renderer::Renderer(Core& core) noexcept
    : m_EngineCore(&core)
{
    m_WindowReference = m_EngineCore->GetWindow();
    m_ECSReference = m_EngineCore->GetECS();
};


bool Renderer::Update() const {

    bool RendererStatus = true;
    if (!Render2D() && !Render3D())
        RendererStatus = false;

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
        if (!TargetGameObject->GetActiveInHeirarchy())
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
        if (Sprite != nullptr)
            Sprite->Bind();

        ShaderProgramTest.SetUniform("uDiffuse", TextureUnit::DIFFUSE);
        ShaderProgramTest.SetUniform("uTint", TargetComponent->GetTint());

        //TODO: Add clearcolor to camera! It would require some restructuring


        Transform* TargetTransform = &TargetGameObject->GetTransform();
        glm::mat4* TargetMatrix = &TargetTransform->GetMatrix();
        //The target matrix could be improved upon even further. I could check if the target has moved for an example and not update it if thats the case
        //The only thing left for the heirarchy is that if i unparent then their current values set them whereever the it would in world space. So it doesnt stay where
        //-it is and adapt its transform, it switches!. Test unparenting and you will notice it. It could be solved somehow but i forget how as im writing this
        //There is one bug left i think which is that for some reason, when doing the double unparenting thing, some cubes will be offset even though its at 0 until 
        //-i change its scale to 1 then it actually moves back to correct position. Idk if this is a bug or something im missing.
        //Scaling moves the thing for some reason, this really seems like a bug. Maybe i shoulnt have just multiplied the matrices but instead tranformed on top of it!

        //Flipping - Needs to be done before MVP
        if (TargetComponent->GetFlipX() || TargetComponent->GetFlipY()) {
            float ScaleX = TargetTransform->m_Scale.m_X;
            float ScaleY = TargetTransform->m_Scale.m_Y;
            float ScaleZ = TargetTransform->m_Scale.m_Z;

            if (ScaleX > 1.0f || ScaleX < 1.0f)
                ScaleX *= 2.0f;
            if (ScaleY > 1.0f || ScaleY < 1.0f)
                ScaleY *= 2.0f;
            if (ScaleZ > 1.0f || ScaleZ < 1.0f)
                ScaleZ *= 2.0f;


            //if (TargetGameObject->GetParent() == nullptr) {
            //    ScaleX *= 2;
            //    ScaleY *= 2;
            //    ScaleZ *= 2;
            //}

            if (TargetComponent->GetFlipX())
                ScaleX *= -1.0f;

            if (TargetComponent->GetFlipY())
                ScaleY *= -1.0f;

            //Add parent position in case of disconnecting from it to fix last heriarchy bug

            *TargetMatrix = glm::scale(*TargetMatrix, glm::vec3(ScaleX, ScaleY, ScaleZ)); //This is kinda sus
        }


        ////Testing
        //TargetGameObject->GetTransform().m_Rotation.m_X += 1.0f;
        //TargetGameObject->GetTransform().m_Rotation.m_Y += 1.0f;

        //MVP
        Camera* ViewportCamera = &m_ECSReference->GetViewportCamera();

        ShaderProgramTest.SetUniform("uModel", *TargetMatrix); //Construct matrix here instead of getting to apply the flipx anmd y?
        ShaderProgramTest.SetUniform("uView", ViewportCamera->GetViewMatrix());
        ShaderProgramTest.SetUniform("uProjection", ViewportCamera->GetProjectionMatrix());


        TargetComponent->GetVAO()->Bind();
        GLCall(glDrawElements(GL_TRIANGLES, TargetComponent->GetEBO()->GetCount(), GL_UNSIGNED_INT, nullptr));

        if (Sprite != nullptr)
            Sprite->Unbind();
    }

    return true;
}
bool Renderer::Render3D() const {

    return true;
}


void Renderer::CheckRendererAPIVersion() {
    const unsigned char* convert = static_cast<const unsigned char*>(glGetString(GL_VERSION));
    //TODO: Change depending on renderer API and make a function for all systems that need to be called at the start. 2 step init?
    std::string Version("OpenGL ");
    Version.append(reinterpret_cast<const char*>(convert));
    m_EngineCore->SystemLog(Version);
}