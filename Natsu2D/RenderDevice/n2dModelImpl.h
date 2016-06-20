#pragma once
#include "n2dMeshDataImpl.h"

class n2dStaticModelDataImpl
	: public natRefObjImpl<n2dModelData>
{
	friend class n2dGraphics3DImpl;
	friend class n2dModelLoaderImpl;
public:
	n2dStaticModelDataImpl() = default;

	nBool IsStatic() const override
	{
		return true;
	}

	void Update(nuInt) override {}

	nuInt GetMeshCount() const override;
	n2dMeshData* GetMesh(nuInt Index) override
	{
		if (Index >= m_Meshes.size())
		{
			return nullptr;
		}

		return m_Meshes[Index];
	}

	void SetZoom(nFloat Factor) override
	{
		for (auto& pMesh : m_Meshes)
		{
			pMesh->SetZoom(Factor);
		}
	}
private:
	std::vector<natRefPointer<n2dStaticMeshDataImpl>> m_Meshes;
};

class n2dDynamicModelDataImpl
	: public natRefObjImpl<n2dModelData>
{
	friend class n2dMotionManagerImpl;
	friend class n2dGraphics3DImpl;
	friend class n2dModelLoaderImpl;
	friend class n2dSkeleton;
	friend class n2dIK;
public:
	n2dDynamicModelDataImpl() = default;
	~n2dDynamicModelDataImpl() = default;

	nBool IsStatic() const override
	{
		return false;
	}

	void Update(nuInt nFrame) override;

	nuInt GetMeshCount() const override
	{
		return 1u;
	}

	n2dMeshData* GetMesh(nuInt Index) override
	{
		if (0u == Index)
		{
			return &m_Mesh;
		}

		return nullptr;
	}

	void SetZoom(nFloat Factor) override
	{
		m_Mesh.SetZoom(Factor);
	}
private:
	n2dDynamicMeshDataImpl m_Mesh;

	nTString m_Name, m_Comment;
};