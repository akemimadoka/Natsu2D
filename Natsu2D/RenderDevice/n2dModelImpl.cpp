#include "n2dModelImpl.h"


n2dModelDataImpl::n2dModelDataImpl(std::vector<natRefPointer<n2dMeshDataImpl>> const& pMeshes)
	: m_Meshes(pMeshes)
{
}

nuInt n2dModelDataImpl::GetMeshCount() const
{
	return m_Meshes.size();
}