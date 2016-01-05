#include "n2dModelImpl.h"

nuInt n2dStaticModelDataImpl::GetMeshCount() const
{
	return m_Meshes.size();
}

void n2dDynamicModelDataImpl::Update(nuInt nFrame)
{
	m_Mesh.Update(nFrame);
}
