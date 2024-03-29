#include "Systems/Renderer.hpp"
#include "Systems/Core.hpp"



Renderer::Renderer(Core& core)
    : m_Core(&core)
{
    m_Window = m_Core->GetWindow();
    m_TextureStorage = m_Core->GetTextureStorage();
    m_ModelStorage = m_Core->GetModelStorage();
    m_ECS = m_Core->GetECS();
    m_Input = m_Core->GetInput();


    SetupShaderPrograms();

    glEnable(GL_DEPTH_TEST); //TODO: move somewhere else
    glEnable(GL_BLEND); //TODO: move somewhere else
};
Renderer::~Renderer() {
    //NOTE: Diagnostics and such can be evaluated in here and sent to GUI
}


bool Renderer::Update() {

    CheckInput(); //NOTE: Should input checking be on separate tickrate than rendering? Should this even be here? seems like editor thing

    //This is kinda nonesense here
    bool RendererStatus = true;

    if (!Render2D())
        RendererStatus = false;
    if (!Render3D())
        RendererStatus = false;

    return true; //NOTE: Should represent the overall status of the renderer doing its job. Its fine if a system or two fail. Then it sends out warnings and errors!
}

void Renderer::SetupShaderPrograms() {

    m_DefaultLitShaderProgram.AttachShader(m_DefaultLitVertex);
    m_DefaultLitShaderProgram.AttachShader(m_DefaultLitFragment);
    if (!m_DefaultLitShaderProgram.LinkShaderProgram())
        m_Core->SystemLog("Renderer failed to link DefautlLit shader program");

    m_DepthViewShaderProgram.AttachShader(m_DepthViewVertex);
    m_DepthViewShaderProgram.AttachShader(m_DepthViewFragment);
    if (!m_DepthViewShaderProgram.LinkShaderProgram())
        m_Core->SystemLog("Renderer failed to link DepthView shader program");

    m_CurrentShaderProgram = &m_DefaultLitShaderProgram;
    m_CurrentRenderingView = RenderingView::LIT;
}


bool Renderer::Render2D() {
    

    BindShaderProgram();

    //TODO: Register error message when it happens here!
    //TODO: Drop this approach and simply get a copy of the vector since its faster cause of something i forgot what it was called and its just a vector of ptrs
    //- This still might be faster tho since no copying is happening and im just getting them in order as defined by the ECS. There are definitely some implications to
    //- think of.

    

    MemoryBlocksBucket<SpriteRenderer>* AllSpriteRenderers = &m_ECS->GetAllComponentsOfType<SpriteRenderer>();
    if (AllSpriteRenderers->size() > 0 && !m_Render2DReady)
        Setup2DRenderingData();

    for (uint32 i = 0; i < AllSpriteRenderers->size(); i++) {
        const SpriteRenderer* TargetComponent = &(*AllSpriteRenderers)[i];
        if (!Validate(TargetComponent))
            return false;

        GameObject* TargetGameObject = TargetComponent->GetOwner();
        if (!Validate(TargetGameObject))
            return false;


        //Check shader

        SetupMaterial(*m_CurrentShaderProgram, TargetComponent); //WORKS ONLY FOR SPRITERENDERES
        SetupLightUniforms(*m_CurrentShaderProgram);

        //IMPORTNAT NOTE: I got an error once when moving an object "5eeee". It had something to do with gluniform3f()



        //TODO: Add clearcolor to camera! It would require some restructuring
        Transform* TargetTransform = &TargetGameObject->GetTransform();
        glm::mat4* TargetMatrix = &TargetTransform->GetMatrix();
        //The target matrix could be improved upon even further. I could check if the target has moved for an example and not update it if thats the case
        //The only thing left for the heirarchy is that if i unparent then their current values set them whereever the it would in world space. So it doesnt stay where
        //-it is and adapt its transform, it switches!. Test unparenting and you will notice it. It could be solved somehow but i forget how as im writing this
        //There is one bug left i think which is that for some reason, when doing the double unparenting thing, some cubes will be offset even though its at 0 until 
        //-i change its scale to 1 then it actually moves back to correct position. Idk if this is a bug or something im missing.
        //Scaling moves the thing for some reason, this really seems like a bug. Maybe i shoulnt have just multiplied the matrices but instead tranformed on top of it!

        //Flipping - Needs to be done before MVP - It changes to model matrix
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
        Camera* ViewportCamera = &m_ECS->GetViewportCamera();
        SetupMVP(ViewportCamera, TargetMatrix);

        m_SpriteVAO.Bind();
        //TargetComponent->GetVAO().Bind();

        //TODO: Too important to be out like this. Move into func.
        //GLCall(glDrawElements(GL_TRIANGLES, TargetComponent->GetEBO().GetCount(), GL_UNSIGNED_INT, nullptr));
        GLCall(glDrawElements(GL_TRIANGLES, m_SpriteEBO.GetCount(), GL_UNSIGNED_INT, nullptr));

        UnbindAllTextures(TargetComponent);

        m_SpriteVAO.Unbind();
        //TargetComponent->GetVAO().Unbind();
    }

    UnbindShaderProgram();
    return true;
}
bool Renderer::Render3D() {

    BindShaderProgram();

    //TODO: Move into func!
    auto SkeletalMeshes = m_ECS->GetSkeletalMeshes();
    for (auto& SkeletalMesh : SkeletalMeshes) {
        //This is pretty much standard for all types

        if (!Validate(SkeletalMesh))
            continue;

        GameObject* TargetGameObject = m_ECS->FindGameObject(SkeletalMesh->GetOwnerID());
        if (!Validate(TargetGameObject))
            continue;

        //Lights
        SetupLightUniforms(m_DefaultLitShaderProgram);

        //MVP - This could be dumped down a bit into a function too. Those could be set once?
        Transform* TargetTransform = &TargetGameObject->GetTransform();
        glm::mat4* ModelMatrix = &TargetTransform->GetMatrix();
        Camera* ViewportCamera = &m_ECS->GetViewportCamera();

        SetupMVP(ViewportCamera, ModelMatrix);

        //Texture Filtering/Sampling
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLuint)SkeletalMesh->GetAddressingModeS()));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLuint)SkeletalMesh->GetAddressingModeT()));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLuint)SkeletalMesh->GetFilteringModeMin()));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLuint)SkeletalMesh->GetFilteringModeMag()));

        //Blending
        GLCall(glBlendFunc((GLuint)SkeletalMesh->GetSourceBlendMode(), (GLuint)SkeletalMesh->GetDestinationBlendMode()));
        GLCall(glBlendEquation((GLuint)SkeletalMesh->GetBlendEquation()));

        //Second loop over all the meshes in the model
        auto Meshes = SkeletalMesh->m_Model->GetMeshes();
        for (auto& Mesh : Meshes) {
            if (Mesh == nullptr)
                continue;

            //Need to figure this out. How to know which is which
            //Activate -> Bind -> Activate Next -> Bind : Luckily its only one now
            m_TextureStorage->SetActiveTextureUnit(TextureUnit::TEXTURE0);
            //for (auto& Texture : Mesh->m_Textures) {
            //    Texture->Bind();
            //}
            if (Mesh->m_Textures.size() > 0)
                Mesh->m_Textures[0]->Bind();
            m_DefaultLitShaderProgram.SetUniform("uMaterial.Diffuse", TextureType::DIFFUSE); //??
            
            //Directional is kinda borked!
            m_DefaultLitShaderProgram.SetUniform("uMaterial.Ambient", TextureType::DIFFUSE);
            m_DefaultLitShaderProgram.SetUniform("uMaterial.Specular", TextureType::DIFFUSE);
            m_DefaultLitShaderProgram.SetUniform("uMaterial.AmbientStrength", 0.1f);
            m_DefaultLitShaderProgram.SetUniform("uMaterial.SpecularShininess", 32);
            m_DefaultLitShaderProgram.SetUniform("uMaterial.SpecularStrength", 0.5f);
            m_DefaultLitShaderProgram.SetUniform("uTint", Colors::White); //This! will lead to Alpha ending up as 0

            Mesh->m_VAO->Bind();
            GLCall(glDrawElements(GL_TRIANGLES, Mesh->m_EBO->GetCount(), GL_UNSIGNED_INT, nullptr));


            m_TextureStorage->SetActiveTextureUnit(TextureUnit::TEXTURE0);
           //for (auto& Texture : Mesh->m_Textures) {
           //    Texture->Unbind();
           //}
            if (Mesh->m_Textures.size() > 0)
                Mesh->m_Textures[0]->Unbind();
            Mesh->m_VAO->Unbind();
        }
    }

    UnbindShaderProgram();

    return true;
}
void Renderer::CheckInput() noexcept {

    if (m_Input->GetKey(Keycode::F1)) {
        m_CurrentRenderingView = RenderingView::LIT;
        m_CurrentShaderProgram = &m_DefaultLitShaderProgram;
    }
    else if (m_Input->GetKey(Keycode::F2)) {
        m_CurrentRenderingView = RenderingView::UNLIT;
        m_CurrentShaderProgram = &m_DefaultLitShaderProgram;
    }
    else if (m_Input->GetKey(Keycode::F3)) {
        m_CurrentRenderingView = RenderingView::WIREFRAME;
        m_CurrentShaderProgram = &m_DefaultLitShaderProgram;
    }
    else if (m_Input->GetKey(Keycode::F4)) {
        m_CurrentRenderingView = RenderingView::DEPTH_ONLY;
        m_CurrentShaderProgram = &m_DepthViewShaderProgram;
    }
}


//NOTE: Not needed!
void Renderer::BindShaderProgram() const noexcept {

    m_CurrentShaderProgram->Bind();
}
void Renderer::UnbindShaderProgram() const noexcept {

    m_CurrentShaderProgram->Unbind();
}


void Renderer::SetupMVP(Camera* viewport, glm::mat4* model) {

    if (viewport == nullptr || model == nullptr)
        return;

    //MVP
    m_CurrentShaderProgram->SetUniform("uMVP.Model", *model); //Construct matrix here instead of getting to apply the flipx anmd y?
    m_CurrentShaderProgram->SetUniform("uMVP.View", viewport->GetViewMatrix());
    m_CurrentShaderProgram->SetUniform("uMVP.Projection", viewport->GetProjectionMatrix());

    //TODO: Take out into another function - These seem to be frag shader related!
    switch (m_CurrentRenderingView) {
    case RenderingView::LIT: {
        //Would probably be the main camera in case of play mode being on
        m_CurrentShaderProgram->SetUniform("uViewCameraPosition", viewport->GetOwner()->GetTransform().m_Position);
        m_CurrentShaderProgram->SetUniform("uNormalMatrix", glm::mat3(glm::transpose(glm::inverse(*model)))); //Inverse operations are costly in shaders

    }break;
    case RenderingView::DEPTH_ONLY: {
        m_CurrentShaderProgram->SetUniform("uNear", viewport->GetNearClipPlane());
        m_CurrentShaderProgram->SetUniform("uFar", viewport->GetFarClipPlane());

    }break;

    }


}
void Renderer::SetupLightUniforms(ShaderProgram& program) {

    //IMPORTANT NOTE: Test that sourceRadius doesnt exceed attenuation. Maybe do that in editor? also here!

    //Directional Light
    DirectionalLight* DirectionalLightComp = m_ECS->GetMainDirectionalLight();
    if (DirectionalLightComp != nullptr) { //Directional Light!

        Vector4f LightDirection = DirectionalLightComp->GetDirection();

        program.SetUniform("uDirectionalLight.LightDirection", LightDirection);
        program.SetUniform("uDirectionalLight.LightColor", DirectionalLightComp->m_Tint);
        program.SetUniform("uDirectionalLight.Intensity", DirectionalLightComp->m_Intensity);
    }

    //PointLights
    std::vector<PointLight*> AllPointLights = m_ECS->GetPointLights();
    if (AllPointLights.size() > 0) {
        program.SetUniform("uPointLightsCount", static_cast<int>(AllPointLights.size()));
        for (uint32 LightIndex = 0; LightIndex < AllPointLights.size(); LightIndex++) {

            auto Light = AllPointLights.at(LightIndex);
            program.SetUniform(std::string("uPointLights[" + std::to_string(LightIndex) + "].LightPosition"), Light->GetOwner()->GetTransform().m_Position);
            program.SetUniform("uPointLights[" + std::to_string(LightIndex) + "].LightColor", Light->m_Tint);
            program.SetUniform("uPointLights[" + std::to_string(LightIndex) + "].Intensity", Light->m_Intensity);
            program.SetUniform("uPointLights[" + std::to_string(LightIndex) + "].Attenuation", Light->m_Attenuation);
            program.SetUniform("uPointLights[" + std::to_string(LightIndex) + "].SourceRadius", Light->m_SourceRadius);
        }
    }

    //SpotLights
    std::vector<SpotLight*> AllSpotLights = m_ECS->GetSpotLights();
    if (AllSpotLights.size() > 0) {
        program.SetUniform("uSpotLightsCount", static_cast<int>(AllSpotLights.size()));
        for (uint32 LightIndex = 0; LightIndex < AllSpotLights.size(); LightIndex++) {

            const auto Light = AllSpotLights.at(LightIndex);
            const Vector4f LightDirection = Light->GetDirection();

            program.SetUniform("uSpotLights[" + std::to_string(LightIndex) + "].LightPosition", Light->GetOwner()->GetTransform().m_Position);
            program.SetUniform("uSpotLights[" + std::to_string(LightIndex) + "].LightDirection", LightDirection);
            program.SetUniform("uSpotLights[" + std::to_string(LightIndex) + "].LightColor", Light->m_Tint);
            program.SetUniform("uSpotLights[" + std::to_string(LightIndex) + "].Intensity", Light->m_Intensity);
            program.SetUniform("uSpotLights[" + std::to_string(LightIndex) + "].Attenuation", Light->m_Attenuation);
            program.SetUniform("uSpotLights[" + std::to_string(LightIndex) + "].SourceRadius", Light->m_SourceRadius);
            program.SetUniform("uSpotLights[" + std::to_string(LightIndex) + "].InnerCutoffAngle", Light->GetInnerAngleCosine());
            program.SetUniform("uSpotLights[" + std::to_string(LightIndex) + "].OuterCutoffAngle", Light->GetOuterAngleCosine());
        }
    }
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
    m_TextureStorage->SetActiveTextureUnit(TextureUnit::TEXTURE0); //TODO: Create own abstraction for texture units
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
            if (m_TextureStorage->GetEditorTexture2DByName("NoDiffuse.jpg", Sprite))
                Sprite->Bind();
        }
    }

    if (CompMaterial != nullptr) {

        //Ambient
        m_TextureStorage->SetActiveTextureUnit(TextureUnit::TEXTURE1); //TODO: Create own abstraction for texture units
        if (CompMaterial->m_Ambient != nullptr) {
            CompMaterial->m_Ambient->Bind();
        }
        //TODO: Otherwise use sprite/diffuse as ambient!
        //Specular
        m_TextureStorage->SetActiveTextureUnit(TextureUnit::TEXTURE2); //TODO: Create own abstraction for texture units
        if (CompMaterial->m_Specular != nullptr) {
            CompMaterial->m_Specular->Bind();
        }

        program.SetUniform("uMaterial.Ambient", TextureType::AMBIENT);
        program.SetUniform("uMaterial.Specular", TextureType::SPECULAR);
        program.SetUniform("uMaterial.AmbientStrength", CompMaterial->m_AmbientStrength);
        program.SetUniform("uMaterial.SpecularShininess", CompMaterial->m_SpecularShininess);
        program.SetUniform("uMaterial.SpecularStrength", CompMaterial->m_SpecularStrength);
    }


    program.SetUniform("uTint", component->GetTint());

    program.SetUniform("uMaterial.Diffuse", TextureType::DIFFUSE);
}
void Renderer::UnbindAllTextures(const SpriteRenderer* component) {

    //TODO: Divide this function into 2 types since sprites use the sprite instead of diffuse but will use diffuse if sprite does not exist!
    //TODO: This function does not work due to the combinations of bind and unbind that could be happening in the previous above function !!!!!!!


    Texture2D* Sprite = component->GetSprite();
    Material* CompMaterial = component->GetMaterial();

    //Diffuse
    if (Sprite != nullptr) {
        m_TextureStorage->SetActiveTextureUnit(TextureUnit::TEXTURE0);
        Sprite->Unbind();
    }


    if (CompMaterial == nullptr)
        return;

    //Ambient
    if (CompMaterial->m_Ambient != nullptr) {
        m_TextureStorage->SetActiveTextureUnit(TextureUnit::TEXTURE1);
        CompMaterial->m_Ambient->Unbind();
    }

    //Specular
    if (CompMaterial->m_Specular != nullptr) {
        m_TextureStorage->SetActiveTextureUnit(TextureUnit::TEXTURE2);
        CompMaterial->m_Specular->Unbind();
    }

}


bool Renderer::Validate(const SpriteRenderer* component) const noexcept {

    if (component == nullptr)
        return false;
    if (!component->GetEnabled())
        return false;

    return true;
}
bool Renderer::Validate(const SkeletalMesh* component) const noexcept {

    if (component == nullptr)
        return false;
    if (!component->GetEnabled())
        return false;
    if (component->GetModel() == nullptr)
        return false;

    return true;
}
bool Renderer::Validate(const GameObject* object) const noexcept {

    if (object == nullptr)
        return false;
    if (!object->GetActiveInHeirarchy())
        return false;

    return true;
}


void Renderer::Setup2DRenderingData() noexcept {
    if (m_Render2DReady)
        return;

    m_SpriteVAO.Bind();
    m_SpriteVBO.SetData(m_SpritePrimitive.m_Data, sizeof(m_SpritePrimitive.m_Data));
    m_SpriteEBO.SetData(m_SpritePrimitive.m_Indices, sizeof(m_SpritePrimitive.m_Indices), sizeof(m_SpritePrimitive.m_Indices) / sizeof(GLuint));

    m_SpriteVBO.Bind();
    m_SpriteEBO.Bind();

    m_SpriteVAO.Unbind();
    m_SpriteVBO.Unbind();
    m_SpriteEBO.Unbind();

    m_Render2DReady = true;
}

void Renderer::CheckRendererAPIVersion() const {
    const unsigned char* convert = static_cast<const unsigned char*>(glGetString(GL_VERSION));
    //TODO: Change depending on renderer API and make a function for all systems that need to be called at the start. 2 step init?
    std::string Version("OpenGL ");
    Version.append(reinterpret_cast<const char*>(convert)); //TODO: Clean this up.
    m_Core->SystemLog(Version);
}