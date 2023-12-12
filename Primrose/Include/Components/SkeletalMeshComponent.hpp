#pragma once
#include "Component.hpp"
#include "Texture2D.hpp"

class Model;

class SkeletalMesh final : public ComponentBase {
public:
	SkeletalMesh() = delete;
	SkeletalMesh(GameObject& owner, uint64 ownerID);
	~SkeletalMesh() = default;

	//For now it is not possbile to move or copy components - Copy only it should be? idk think about it
	//They should be. Its not like it ownes any resources. Components has no resource just references to them.
	SkeletalMesh(const SkeletalMesh& other) = delete;
	SkeletalMesh& operator=(const SkeletalMesh& other) = delete;

	SkeletalMesh(SkeletalMesh&& other) = delete;
	SkeletalMesh& operator=(SkeletalMesh&& other) = delete;

public:
	inline void SetModel(Model* model) noexcept { m_Model = model; }
	inline void SetMaterial(Material* material) noexcept { m_Material = material; }

	inline void SetBlendEquation(BlendEquation equation) noexcept { m_BlendEquation = equation; }
	inline void SetSourceBlendMode(SourceBlendMode mode) noexcept { m_SourceBlendMode = mode; }
	inline void SetDestinationBlendMode(DestinationBlendMode mode) noexcept { m_DestinationBlendMode = mode; }

	inline void SetAddressingModeS(AddressingMode s) noexcept { m_AddressingModeS = s; }
	inline void SetAddressingModeT(AddressingMode t) noexcept { m_AddressingModeT = t; }
	inline void SetFilteringModeMin(FilteringModeMin min) noexcept { m_FilteringModeMin = min; }
	inline void SetFilteringModeMag(FilteringModeMag mag) noexcept { m_FilteringModeMag = mag; }


public:
	inline Model* GetModel() const noexcept { return m_Model; }
	inline Material* GetMaterial() const noexcept { return m_Material; }

	inline BlendEquation GetBlendEquation() const noexcept { return m_BlendEquation; }
	inline SourceBlendMode GetSourceBlendMode() const noexcept { return m_SourceBlendMode; }
	inline DestinationBlendMode GetDestinationBlendMode() const noexcept { return m_DestinationBlendMode; }

	inline AddressingMode GetAddressingModeS() const noexcept { return m_AddressingModeS; }
	inline AddressingMode GetAddressingModeT() const noexcept { return m_AddressingModeT; }
	inline FilteringModeMin GetFilteringModeMin() const noexcept { return m_FilteringModeMin; }
	inline FilteringModeMag GetFilteringModeMag() const noexcept { return m_FilteringModeMag; }

public:
	Model* m_Model			{ nullptr };
	Material* m_Material	{ nullptr };

private: //NOTE: Could these be part of the material class?
	BlendEquation m_BlendEquation = BlendEquation::ADDITIVE;
	SourceBlendMode m_SourceBlendMode = SourceBlendMode::SOURCE_ALPHA;
	DestinationBlendMode m_DestinationBlendMode = DestinationBlendMode::ONE_MINUS_SOURCE_ALPHA;

	AddressingMode m_AddressingModeS = AddressingMode::REPEAT;
	AddressingMode m_AddressingModeT = AddressingMode::REPEAT;
	FilteringModeMin m_FilteringModeMin = FilteringModeMin::LINEAR_MIPMAP_LINEAR;
	FilteringModeMag m_FilteringModeMag = FilteringModeMag::LINEAR;



};