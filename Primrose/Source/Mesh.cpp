#include "Mesh.hpp"
#include "ASSIMP/scene.h"

Mesh::Mesh(aiMesh& data) noexcept
	:	m_Data(&data)
{

}