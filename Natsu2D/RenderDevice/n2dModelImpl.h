#pragma once
#include "n2dMeshDataImpl.h"

//class n2dModelLoaderImpl;

class n2dModelDataImpl
	: public natRefObjImpl<n2dModelData>
{
	friend class n2dGraphics3DImpl;
	friend class n2dModelLoaderImpl;
public:
	n2dModelDataImpl() = default;
	explicit n2dModelDataImpl(std::vector<natRefPointer<n2dMeshDataImpl>> const& pMeshes);

	nuInt GetMeshCount() const override;
private:
	std::vector<natRefPointer<n2dMeshDataImpl>> m_Meshes;
};