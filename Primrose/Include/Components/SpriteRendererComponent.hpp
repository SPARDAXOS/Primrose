#pragma once
#include "Component.hpp"
#include "GraphicsResources.hpp"
#include "TextureStorage.hpp"

class GameObject;

class SpriteRenderer final : public ComponentBase {
public:
	SpriteRenderer() = delete;
	SpriteRenderer(GameObject& owner, uint64 ownerID) noexcept;
	~SpriteRenderer();


	//For now it is not possbile to move or copy components
	SpriteRenderer(const SpriteRenderer& other) = delete;
	SpriteRenderer& operator=(const SpriteRenderer& other) = delete;

	SpriteRenderer(SpriteRenderer&& other) = delete;
	SpriteRenderer& operator=(SpriteRenderer&& other) = delete;

public:
	inline void SetSprite(Texture2D* sprite) noexcept { m_Sprite = sprite; }
	inline void SetTint(Color color) noexcept { m_Tint = color; }
	inline void SetFlipX(bool state) noexcept { m_FlipX = state; }
	inline void SetFlipY(bool state) noexcept { m_FlipY = state; }

	inline void SetBlendEquation(BlendEquation equation) noexcept { m_BlendEquation = equation; }
	inline void SetSourceBlendMode(SourceBlendMode mode) noexcept { m_SourceBlendMode = mode; }
	inline void SetDestinationBlendMode(DestinationBlendMode mode) noexcept { m_DestinationBlendMode = mode; }

	inline void SetAddressingMode(AddressingMode s, AddressingMode t) noexcept { m_AddressingModeS = s; m_AddressingModeT = t; }
	inline void SetFilteringMode(FilteringMode min, FilteringMode mag) noexcept { m_FilteringModeMin = min; m_FilteringModeMag = mag; }


public:
	inline Texture2D* GetSprite() const noexcept { return m_Sprite; }
	inline Color GetTint() const noexcept { return m_Tint; }
	inline bool GetFlipX() const noexcept { return m_FlipX; }
	inline bool GetFlipY() const noexcept { return m_FlipY; }

	inline BlendEquation GetBlendEquation() const noexcept { return m_BlendEquation; }
	inline SourceBlendMode GetSourceBlendMode() const noexcept { return m_SourceBlendMode; }
	inline DestinationBlendMode GetDestinationBlendMode() const noexcept { return m_DestinationBlendMode; }

	inline AddressingMode GetAddressingModeS() const noexcept { return m_AddressingModeS; }
	inline AddressingMode GetAddressingModeT() const noexcept { return m_AddressingModeT; }
	inline FilteringMode GetFilteringModeMin() const noexcept { return m_FilteringModeMin; }
	inline FilteringMode GetFilteringModeMag() const noexcept { return m_FilteringModeMag; }

	inline VAO* GetVAO() const noexcept { return m_VAO; }
	inline EBO* GetEBO() const noexcept { return m_EBO; }


private:
	Texture2D* m_Sprite = nullptr;
	Color m_Tint = Colors::White;
	bool m_FlipX = false;
	bool m_FlipY = false;

private:
	BlendEquation m_BlendEquation = BlendEquation::ADDITIVE;
	SourceBlendMode m_SourceBlendMode = SourceBlendMode::SOURCE_ALPHA;
	DestinationBlendMode m_DestinationBlendMode = DestinationBlendMode::ONE_MINUS_SOURCE_ALPHA;

	AddressingMode m_AddressingModeS = AddressingMode::REPEAT;
	AddressingMode m_AddressingModeT = AddressingMode::REPEAT;
	FilteringMode m_FilteringModeMin = FilteringMode::LINEAR_MIPMAP_LINEAR;
	FilteringMode m_FilteringModeMag = FilteringMode::LINEAR;

private:
	Cube m_Primitive; //Change to square for 2D
	VAO* m_VAO = nullptr;
	VBO* m_VBO = nullptr;
	EBO* m_EBO = nullptr;
};