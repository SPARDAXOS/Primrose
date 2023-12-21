#pragma once
#include "Component.hpp"
#include "GraphicsResources.hpp"
#include "Systems/TextureStorage.hpp"
#include "Material.hpp" //Currently here since there is no storage class


class SpriteRenderer final : public ComponentBase {
public:
	SpriteRenderer() = delete;
	explicit SpriteRenderer(GameObject& owner, uint64 ownerID) noexcept;
	~SpriteRenderer();


	//For now it is not possbile to move or copy components - Maybe copying values should be allowed
	SpriteRenderer(const SpriteRenderer& other) : ComponentBase(other) {
		*this = other;
	}
	SpriteRenderer& operator=(const SpriteRenderer& other) noexcept {
		if (this == &other)
			return *this;
		else {

			//this is whats broken and maybe  creating an infinite loop somwhow

			this->m_Sprite = other.m_Sprite;
			this->m_Tint = other.m_Tint;
			this->m_Material = other.m_Material;
			this->m_FlipX = other.m_FlipX;
			this->m_FlipY = other.m_FlipY;

			this->m_BlendEquation = other.m_BlendEquation;
			this->m_SourceBlendMode = other.m_SourceBlendMode;
			this->m_DestinationBlendMode = other.m_DestinationBlendMode;

			this->m_AddressingModeS = other.m_AddressingModeS;
			this->m_AddressingModeT = other.m_AddressingModeT;
			this->m_FilteringModeMin = other.m_FilteringModeMin;
			this->m_FilteringModeMag = other.m_FilteringModeMag;

			return *this;
		}
	}

	SpriteRenderer(SpriteRenderer&& other) noexcept : ComponentBase(std::move(other)) {
		*this = std::move(other);
	}
	SpriteRenderer& operator=(SpriteRenderer&& other) noexcept {
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


			other.m_Sprite = nullptr;
			other.m_Tint = Colors::White;
			other.m_Material = nullptr;
			other.m_FlipX = false;
			other.m_FlipY = false;

			other.m_BlendEquation = BlendEquation::ADDITIVE;
			other.m_SourceBlendMode = SourceBlendMode::SOURCE_ALPHA;
			other.m_DestinationBlendMode = DestinationBlendMode::ONE_MINUS_SOURCE_ALPHA;

			other.m_AddressingModeS = AddressingMode::REPEAT;
			other.m_AddressingModeT = AddressingMode::REPEAT;
			other.m_FilteringModeMin = FilteringModeMin::LINEAR_MIPMAP_LINEAR;
			other.m_FilteringModeMag = FilteringModeMag::LINEAR;

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


private:
	BlendEquation m_BlendEquation = BlendEquation::ADDITIVE;
	SourceBlendMode m_SourceBlendMode = SourceBlendMode::SOURCE_ALPHA;
	DestinationBlendMode m_DestinationBlendMode = DestinationBlendMode::ONE_MINUS_SOURCE_ALPHA;

	AddressingMode m_AddressingModeS = AddressingMode::REPEAT;
	AddressingMode m_AddressingModeT = AddressingMode::REPEAT;
	FilteringModeMin m_FilteringModeMin = FilteringModeMin::LINEAR_MIPMAP_LINEAR;
	FilteringModeMag m_FilteringModeMag = FilteringModeMag::LINEAR;

private:
	Texture2D* m_Sprite = nullptr;
	Color m_Tint = Colors::White;
	Material* m_Material = nullptr;
	bool m_FlipX = false;
	bool m_FlipY = false;
};