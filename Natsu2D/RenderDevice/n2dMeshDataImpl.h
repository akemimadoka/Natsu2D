#pragma once
#include "../n2dCommon.h"
#include "../n2dModel.h"
#include "../n2dGraphics.h"
#include "n2dBufferImpl.h"
#include "n2dRenderDeviceImpl.h"
#include <vector>
#include <memory>
#include <natMat.h>
#include <unordered_map>

class n2dModelLoaderImpl;
struct MotionData;

class n2dStaticMeshDataImpl
	: public natRefObjImpl<n2dStaticMeshDataImpl, n2dMeshData>
{
	friend class n2dGraphics3DImpl;
	friend class n2dModelLoaderImpl;

public:
	n2dStaticMeshDataImpl();
	~n2dStaticMeshDataImpl();
	
	void ApplyMaterial(Material const& MeshMaterial);

	n2dBufferImpl* GetVertexBuffer();
	n2dBufferImpl* GetIndexBuffer();

	nuInt GetVertexCount() const override;
	nuInt GetIndexCount() const override;

	nBool IsStatic() const override;
	nResult ApplyExpression(nStrView const& Name, nFloat Weight) override
	{
		return NatErr_NotImpl;
	}

	void SetZoom(nFloat Factor)
	{
		m_Zoom = Factor;
	}
private:
	Material m_Material;
	natMat4<> m_Transform;

	n2dRenderDeviceImpl* m_pRenderDevice;
	natRefPointer<n2dBufferImpl> m_VB, m_IB;

	nuInt m_VertCount, m_IndexCount;

	std::vector<n2dGraphics3DVertex> m_Vert;

	std::vector<nuShort> m_Index;

	nFloat m_Zoom;
};

class n2dDynamicMeshDataImpl
	: public natRefObjImpl<n2dDynamicMeshDataImpl, n2dMeshData>
{
	friend class n2dMotionManagerImpl;
	friend class n2dGraphics3DImpl;
	friend class n2dModelLoaderImpl;
	friend class n2dSkeleton;
	friend class n2dIK;
public:
	n2dDynamicMeshDataImpl();
	~n2dDynamicMeshDataImpl();

	void UpdateVertexBuffer();

	n2dBufferImpl* GetVertexBuffer();
	n2dBufferImpl* GetIndexBuffer();

	nuInt GetVertexCount() const override;
	nuInt GetIndexCount() const override;

	nBool IsStatic() const override;
	nResult ApplyExpression(nStrView const& Name, nFloat Weight) override;

	void Update(nuInt nFrame);

	void SetZoom(nFloat Factor)
	{
		m_Zoom = Factor;
		m_Updated = true;
	}
private:
	struct DynamicMeshVertex
	{
		nuShort TargetBone[2];
		nByte Weight;
		nByte EdgeFlag;
	};

	struct BoneData
	{
		nString Name;
		nuShort Parent;
		nuShort Child;
		nByte Type;
		nuShort Target;
		natVec3<> Pos;
	};

	struct MorphData
	{
		nString Name;
		nByte Type;
		std::vector<nuInt> Vertexes;
		std::vector<natVec3<>> Translation;
	};

	struct IKData
	{
		nuShort Target;
		nuShort Effector;

		std::vector<nuShort> LinkList;

		nuShort MaxIteration;
		nFloat MaxRotation;
	};

	std::shared_ptr<n2dSkeleton> m_Selekton;
	std::vector<n2dIK> m_IK;

	std::vector<DynamicMaterial> m_Materials;
	std::vector<BoneData> m_Bones;
	std::vector<MorphData> m_Morphes;
	std::vector<IKData> m_IKs;

	std::vector<nuInt> m_BoneMap;
	std::vector<nuInt> m_MorphMap;

	std::unordered_map<nString, nuInt> m_Expression;

	MotionData* m_pMotion;
	n2dMotionManagerImpl* m_pMotionManager;

	natMat4<> m_Transform;

	std::vector<n2dGraphics3DVertex> m_Vert;
	std::vector<DynamicMeshVertex> m_VertAdd;

	std::vector<nuShort> m_Index;

	n2dRenderDeviceImpl* m_pRenderDevice;
	natRefPointer<n2dBufferImpl> m_VB, m_IB;

	nBool m_Updated;

	nFloat m_Zoom;
	nuInt m_LastFrame;
};
