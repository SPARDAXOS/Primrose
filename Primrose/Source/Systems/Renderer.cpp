#include "Systems/Renderer.hpp"
#include "Systems/Core.hpp"



Renderer::Renderer(Core& core) noexcept
    : m_EngineCore(&core)
{
    m_WindowReference = m_EngineCore->GetWindow();
    m_TextureStorageReference = m_EngineCore->GetTextureStorage();
    m_ECSReference = m_EngineCore->GetECS();

    glEnable(GL_DEPTH_TEST); //TODO: move somewhere else
    glEnable(GL_BLEND); //TODO: move somewhere else
};


bool Renderer::Update() {

    bool RendererStatus = true;
    if (!Render2D() && !Render3D())
        RendererStatus = false;

    return RendererStatus;
}


bool Renderer::Render2D() {
    
    //Shaders
    Shader VertexShader(GL_VERTEX_SHADER, "Resources/Shaders/Vertex.glsl");
    Shader FragmentShader(GL_FRAGMENT_SHADER, "Resources/Shaders/Frag_PhongLighting.glsl");

    ShaderProgram ShaderProgramTest;
    ShaderProgramTest.AttachShader(VertexShader);
    ShaderProgramTest.AttachShader(FragmentShader);
    if (!ShaderProgramTest.LinkShaderProgram())
        m_EngineCore->SystemLog("Renderer failed to link shader program");
    ShaderProgramTest.Bind();



    //TODO: Register error message when it happens here!
    //TODO: Drop this approach and simply get a copy of the vector since its faster cause of something i forgot what it was called and its just a vector of ptrs
    //- This still might be faster tho since no copying is happening and im just getting them in order as defined by the ECS. There are definitely some implications to
    //- think of.
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


        SetupMaterial(ShaderProgramTest, TargetComponent);


        //Tasks for next time
        //Simple window with text saying whats happening in the project
        //When the project is fully loaded and ready, it opens the editor



        GameObject* DirectionalLightGO = m_ECSReference->GetDirecitonalLightTEST();
        DirectionalLight* DirectionalLightComp = DirectionalLightGO->GetComponent<DirectionalLight>();
        //DirectionalLightComp->m_Tint = Color(1.0f, 1.0f, 1.0f, 1.0f);



        ShaderProgramTest.SetUniform("uLightColor", DirectionalLightComp->m_Tint);
        ShaderProgramTest.SetUniform("uLightPosition", DirectionalLightGO->GetTransform().m_Position);
        

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


        //MVP
        Camera* ViewportCamera = &m_ECSReference->GetViewportCamera();

        //Would probably be the main camera in case of play mode being on
        ShaderProgramTest.SetUniform("uViewCameraPosition", ViewportCamera->GetOwner()->GetTransform().m_Position);

        ShaderProgramTest.SetUniform("uMVP.Model", *TargetMatrix); //Construct matrix here instead of getting to apply the flipx anmd y?
        ShaderProgramTest.SetUniform("uMVP.View", ViewportCamera->GetViewMatrix());
        ShaderProgramTest.SetUniform("uMVP.Projection", ViewportCamera->GetProjectionMatrix());
        ShaderProgramTest.SetUniform("uNormalMatrix", glm::mat3(glm::transpose(glm::inverse(*TargetMatrix)))); //Inverse operations are costly in shaders
        
        TargetComponent->GetVAO()->Bind();
        GLCall(glDrawElements(GL_TRIANGLES, TargetComponent->GetEBO()->GetCount(), GL_UNSIGNED_INT, nullptr));

        UnbindAllTextures(TargetComponent);
    }

    return true;
}
bool Renderer::Render3D() {

    return true;
}

void Renderer::SetupMaterial(ShaderProgram& program, const SpriteRenderer* component) {


    //TODO: Refactor this function further!

    //Texture
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLuint)component->GetAddressingModeS()));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLuint)component->GetAddressingModeT()));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLuint)component->GetFilteringModeMin()));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLuint)component->GetFilteringModeMag()));

    //Blending
    GLCall(glBlendFunc((GLuint)component->GetSourceBlendMode(), (GLuint)component->GetDestinationBlendMode()));
    GLCall(glBlendEquation((GLuint)component->GetBlendEquation()));

    Texture2D* Sprite = component->GetSprite();
    Material* CompMaterial = component->GetMaterial();


    //Sprite/Diffuse
    m_TextureStorageReference->SetActiveTextureUnit(GL_TEXTURE0); //TODO: Create own abstraction for texture units
    if (Sprite != nullptr)
        Sprite->Bind();
    else {
        //If Material has diffuse instead
        if (CompMaterial != nullptr) {
            Sprite = CompMaterial->m_Diffuse;
            if (Sprite != nullptr)
                Sprite->Bind();
        }
        else {
            if (m_TextureStorageReference->GetEditorTexture2DByName("NoDiffuse", Sprite))
                Sprite->Bind();
        }
    }

    if (CompMaterial != nullptr) {

        //Ambient
        m_TextureStorageReference->SetActiveTextureUnit(GL_TEXTURE1); //TODO: Create own abstraction for texture units
        if (CompMaterial->m_Ambient != nullptr) {
            CompMaterial->m_Ambient->Bind();
        }
        //Specular
        m_TextureStorageReference->SetActiveTextureUnit(GL_TEXTURE2); //TODO: Create own abstraction for texture units
        if (CompMaterial->m_Specular != nullptr) {
            CompMaterial->m_Specular->Bind();
        }

        program.SetUniform("uMaterial.Ambient", TextureUnit::AMBIENT);
        program.SetUniform("uMaterial.Specular", TextureUnit::SPECULAR);
        program.SetUniform("uMaterial.AmbientStrength", CompMaterial->m_AmbientStrength);
        program.SetUniform("uMaterial.SpecularShininess", CompMaterial->m_SpecularShininess);
        program.SetUniform("uMaterial.SpecularStrength", CompMaterial->m_SpecularStrength);
    }


    program.SetUniform("uTint", component->GetTint());

    program.SetUniform("uMaterial.Diffuse", TextureUnit::DIFFUSE);
}
void Renderer::UnbindAllTextures(const SpriteRenderer* component) {

    //TODO: Divide this function into 2 types since sprites use the sprite instead of diffuse but will use diffuse if sprite does not exist!

    Texture2D* Sprite = component->GetSprite();
    Material* CompMaterial = component->GetMaterial();

    //Diffuse
    if (Sprite != nullptr) {
        m_TextureStorageReference->SetActiveTextureUnit(GL_TEXTURE0);
        Sprite->Unbind();
    }


    if (CompMaterial == nullptr)
        return;

    //Ambient
    if (CompMaterial->m_Ambient != nullptr) {
        m_TextureStorageReference->SetActiveTextureUnit(GL_TEXTURE1);
        CompMaterial->m_Ambient->Unbind();
    }

    //Specular
    if (CompMaterial->m_Specular != nullptr) {
        m_TextureStorageReference->SetActiveTextureUnit(GL_TEXTURE2);
        CompMaterial->m_Specular->Unbind();
    }

}


void Renderer::CheckRendererAPIVersion() {
    const unsigned char* convert = static_cast<const unsigned char*>(glGetString(GL_VERSION));
    //TODO: Change depending on renderer API and make a function for all systems that need to be called at the start. 2 step init?
    std::string Version("OpenGL ");
    Version.append(reinterpret_cast<const char*>(convert));
    m_EngineCore->SystemLog(Version);
}