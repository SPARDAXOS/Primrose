#pragma once
#include "Component.hpp"
#include "GraphicsResources.hpp"
#include "Systems/TextureStorage.hpp"
#include "Material.hpp" //Currently here since there is no storage class


class SpriteRenderer final : public ComponentBase {
public:
	SpriteRenderer() = delete;
	explicit SpriteRenderer(GameObject& owner, int64 ownerID) noexcept;
	~SpriteRenderer();


	//For now it is not possbile to move or copy components - Maybe copying values should be allowed
	SpriteRenderer(const SpriteRenderer& other) : ComponentBase(other){
		*this = other;
	}
	SpriteRenderer& operator=(const SpriteRenderer& other) {
		if (this == &other)
			return *this;
		else {


			return *this;
		}
	}

	SpriteRenderer(SpriteRenderer&& other) : ComponentBase(std::move(other)) {
		*this = std::move(other);
	}
	SpriteRenderer& operator=(SpriteRenderer&& other) {
		if (this == &other)
			return *this;
		else {

			this->m_Sprite = std::move(other.m_Sprite);
			this->m_Tint = std::move(other.m_Tint);
			this->m_Material = std::move(other.m_Material);
			this->m_FlipX = std::move(other.m_FlipX);
			this->m_FlipY = std::move(other.m_FlipY);

			this->m_BlendEquation = std::move(other.m_BlendEquation);
			this->m_SourceBlendMode = std::move(other.m_SourceBlendMode);
			this->m_DestinationBlendMode = std::move(other.m_DestinationBlendMode);

			this->m_AddressingModeS = std::move(other.m_AddressingModeS);
			this->m_AddressingModeT = std::move(other.m_AddressingModeT);
			this->m_FilteringModeMin = std::move(other.m_FilteringModeMin);
			this->m_FilteringModeMag = std::move(other.m_FilteringModeMag);


			this->m_VAO = std::move(other.m_VAO);
			this->m_VBO = std::move(other.m_VBO);
			this->m_EBO = std::move(other.m_EBO);

			//Clean Other and implement copy semantics


			return *this;
		}
	}

public:
	inline void SetSprite(Texture2D* sprite) noexcept { m_Sprite = sprite; }
	inline void SetTint(Color color) noexcept { m_Tint = color; }
	inline void SetMaterial(Material* material) noexcept { m_Material = material; }
	inline void SetFlipX(bool state) noexcept { m_FlipX = state; }
	inline void SetFlipY(bool state) noexcept { m_FlipY = state; }

	inline void SetBlendEquation(BlendEquation equation) noexcept { m_BlendEquation = equation; }
	inline void SetSourceBlendMode(SourceBlendMode mode) noexcept { m_SourceBlendMode = mode; }
	inline void SetDestinationBlendMode(DestinationBlendMode mode) noexcept { m_DestinationBlendMode = mode; }

	inline void SetAddressingModeS(AddressingMode s) noexcept { m_AddressingModeS = s; }
	inline void SetAddressingModeT(AddressingMode t) noexcept { m_AddressingModeT = t; }
	inline void SetFilteringModeMin(FilteringModeMin min) noexcept { m_FilteringModeMin = min; }
	inline void SetFilteringModeMag(FilteringModeMag mag) noexcept { m_FilteringModeMag = mag; }

public:
	inline Texture2D* GetSprite() const noexcept { return m_Sprite; }
	inline Texture2D*& GetSpriteRef() noexcept { return m_Sprite; }
	inline Color GetTint() const noexcept { return m_Tint; }
	inline Material* GetMaterial() const noexcept { return m_Material; }
	inline Material*& GetMaterialRef() noexcept { return m_Material; }
	inline bool GetFlipX() const noexcept { return m_FlipX; }
	inline bool GetFlipY() const noexcept { return m_FlipY; }

	inline BlendEquation GetBlendEquation() const noexcept { return m_BlendEquation; }
	inline SourceBlendMode GetSourceBlendMode() const noexcept { return m_SourceBlendMode; }
	inline DestinationBlendMode GetDestinationBlendMode() const noexcept { return m_DestinationBlendMode; }

	inline AddressingMode GetAddressingModeS() const noexcept { return m_AddressingModeS; }
	inline AddressingMode GetAddressingModeT() const noexcept { return m_AddressingModeT; }
	inline FilteringModeMin GetFilteringModeMin() const noexcept { return m_FilteringModeMin; }
	inline FilteringModeMag GetFilteringModeMag() const noexcept { return m_FilteringModeMag; }

	inline VAO* GetVAO() const noexcept { return m_VAO; }
	inline EBO* GetEBO() const noexcept { return m_EBO; }


private:
	Texture2D* m_Sprite = nullptr;
	Color m_Tint = Colors::White;
	Material* m_Material = nullptr;
	bool m_FlipX = false;
	bool m_FlipY = false;

private:
	BlendEquation m_BlendEquation = BlendEquation::ADDITIVE;
	SourceBlendMode m_SourceBlendMode = SourceBlendMode::SOURCE_ALPHA;
	DestinationBlendMode m_DestinationBlendMode = DestinationBlendMode::ONE_MINUS_SOURCE_ALPHA;

	AddressingMode m_AddressingModeS = AddressingMode::REPEAT;
	AddressingMode m_AddressingModeT = AddressingMode::REPEAT;
	FilteringModeMin m_FilteringModeMin = FilteringModeMin::LINEAR_MIPMAP_LINEAR;
	FilteringModeMag m_FilteringModeMag = FilteringModeMag::LINEAR;

private:
	Cube m_Primitive; //Change to square for 2D
	VAO* m_VAO = nullptr;
	VBO* m_VBO = nullptr;
	EBO* m_EBO = nullptr;
};