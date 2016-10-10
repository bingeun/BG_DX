#include "bgObject.h"

bgObject::bgObject()
{
}

bgObject::~bgObject()
{
}

void bgObject::Clear()
{
	m_Scene.Clear();
	m_MeshList.clear();
	m_matNodeList.clear();
}

void bgObject::Interpolate(bgMesh* pMesh, float fTick, D3DXMATRIX* matParent)
{
}
