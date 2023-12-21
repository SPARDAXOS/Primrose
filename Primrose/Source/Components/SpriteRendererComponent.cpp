#include "Components/SpriteRendererComponent.hpp"
#include "GameObject.hpp"

SpriteRenderer::SpriteRenderer(GameObject& owner, uint64 ownerID) noexcept
	: ComponentBase(owner, ownerID)
{


}
SpriteRenderer::~SpriteRenderer() {

}