#pragma once
#include "n2dModelImpl.h"
#include <unordered_map>
#include <algorithm>

class n2dBone
{
	friend class n2dSelekton;
public:
	n2dBone() = default;
	n2dBone(natVec3<> const& position, natQuat<> const& orientation, n2dBone* parent, nBool isLimitAngleX);

	void SetTranslation(natVec3<> const& v);
	void SetRotation(natQuat<> const& q);
	void Update();

	natQuat<> const& GetRotation() const;
	natMat4<> const& GetTransform() const;
	natMat4<> const& GetMatrix() const;
	natVec3<> const& GetInitialPosition() const;
	natVec3<> const& GetPosition() const;
	n2dBone* GetParent() const;
	nBool IsLimitAngleX() const;
private:
	nBool m_isLimitAngleX;
	n2dBone* m_pParent;
	natVec3<> m_InitialPosition;
	natVec3<> m_Position;
	natVec3<> m_Translation;
	natQuat<> m_Orientation;
	natQuat<> m_Rotation;
	natMat4<> m_Transform;
	natMat4<> m_Matrix;
};

class n2dSelekton
{
	friend class n2dDynamicMeshDataImpl;
	friend class n2dMotionManagerImpl;
	friend class n2dIK;
public:
	n2dSelekton() = default;
	~n2dSelekton() = default;

	void CreateFromDynamicModel(const n2dDynamicModelDataImpl* pModel);
	void Update();
private:
	std::vector<n2dBone> m_Bones;
	std::vector<n2dBone*> m_UpdateList;
};

class n2dIK
{
public:
	n2dIK() = default;
	~n2dIK() = default;

	void CreateFromDynamicModel(n2dDynamicModelDataImpl* pModel, nuInt IKID);
	void Solve();
private:
	n2dSelekton* m_pSelekton;
	n2dBone* m_Target;
	n2dBone* m_Effector;
	std::vector<n2dBone*> m_Chain;

	nuInt m_nIteration;
	nFloat m_MaxRotation;
};

enum class VMDVersion
{
	Old,
	New,
};

struct BoneFrameData
{
	nTString TargetBone;
	nuInt FrameNumber;
	natVec3<> Translation;
	natQuat<> Rotation;
	nByte Interpolation[64];
};

struct MorphFrameData
{
	nTString TargetMorph;
	nuInt nFrame;
	nFloat Weight;
};

struct MotionKeyFrame
{
	nuInt nFrame;

	nBool operator<(MotionKeyFrame const& other) const
	{
		return nFrame < other.nFrame;
	}
};

struct BoneKeyFrame
	: MotionKeyFrame
{
	natVec3<> Translation;
	natQuat<> Rotation;

	// ignore nFrame
	BoneKeyFrame& operator=(BoneKeyFrame const& other) = default;
	/*{
		Translation = other.Translation;
		Rotation = other.Rotation;

		return *this;
	}*/

	BoneKeyFrame& operator=(BoneFrameData const& data)
	{
		nFrame = data.FrameNumber;
		Rotation = data.Rotation;
		Translation = data.Translation;

		return *this;
	}
};

struct MorphKeyFrame
	: MotionKeyFrame
{
	nFloat Weight;

	// ignore nFrame
	MorphKeyFrame& operator=(MorphKeyFrame const& other) = default;
	/*{
		Weight = other.Weight;

		return *this;
	}*/

	MorphKeyFrame& operator=(MorphFrameData const& data)
	{
		Weight = data.Weight;

		return *this;
	}
};

template <typename T>
class MotionTimeLine
{
	friend class n2dMotionManagerImpl;
public:
	MotionTimeLine()
	{
		static_assert(std::is_base_of<MotionKeyFrame, T>::value, "T should be a MotionKeyFrame");
	}
	~MotionTimeLine()
	{
	}

	T GetFrame(nuInt nFrame)
	{
		T tRet;

		tRet.nFrame = nFrame;
		auto tItea = std::lower_bound(m_KeyFrames.begin(), m_KeyFrames.end(), tRet);

		/*if (m_KeyFrames.end() == tItea)
		{
			tRet = *(--tItea);
			return tRet;
		}
		else if (m_KeyFrames.end() - 1 == tItea)
		{
			tRet = *tItea;
			return tRet;
		}*/

		if (m_KeyFrames.begin() != tItea)
		{
			--tItea;
		}
		if (m_KeyFrames.end() == tItea)
		{
			tItea = m_KeyFrames.end() - 1;
		}

		auto tIteaNext = tItea + 1;
		if (m_KeyFrames.end() == tIteaNext)
		{
			tIteaNext = tItea;
		}

		nFloat alpha = 0.0f;

		if (tItea->nFrame < tIteaNext->nFrame)
		{
			alpha = static_cast<nFloat>(nFrame - tItea->nFrame) / (tIteaNext->nFrame - tItea->nFrame);
		}

		if (alpha < 0.0f)
		{
			alpha = 0.0f;
		}
		else if (alpha > 1.0f)
		{
			alpha = 1.0f;
		}

		tRet = mix(*tItea, *tIteaNext, alpha);
		tRet.nFrame = nFrame;

		return tRet;
	}
private:
	std::vector<T> m_KeyFrames;
};

struct MotionData
{
	VMDVersion Ver;
	nTString OriginModelName;

	std::unordered_map<nTString, nuInt> BoneMap;
	std::unordered_map<nTString, nuInt> MorphMap;

	std::vector<BoneFrameData> BoneFrames;
	std::vector<MorphFrameData> MorphFrames;

	std::vector<MotionTimeLine<BoneKeyFrame>> BoneTimelines;
	std::vector<MotionTimeLine<MorphKeyFrame>> MorphTimelines;
};

inline BoneKeyFrame mix(BoneKeyFrame const& a, BoneKeyFrame const& b, nFloat alpha)
{
	BoneKeyFrame tRet;
	tRet.Translation = natTransform::mix(a.Translation, b.Translation, alpha);
	tRet.Rotation = natTransform::slerp(a.Rotation, b.Rotation, alpha);

	return tRet;
}

inline MorphKeyFrame mix(MorphKeyFrame const& a, MorphKeyFrame const& b, nFloat alpha)
{
	MorphKeyFrame tRet;
	tRet.Weight = a.Weight * (1 - alpha) + b.Weight * alpha;

	return tRet;
}

class n2dMotionManagerImpl
	: public natRefObjImpl<n2dMotionManager>
{
public:
	n2dMotionManagerImpl();
	~n2dMotionManagerImpl();

	nResult LoadMotionFromStream(ncTStr lpName, natStream* pStream) override;
	nResult LoadMotionFromFile(ncTStr lpName, ncTStr lpPath) override;

	nResult ApplyToModel(ncTStr lpName, n2dModelData* pModel) override;
	void RestoreMotion(n2dModelData* pModel) override;
	void RestoreMorph(n2dModelData* pModel) override;

	void applyMotion(n2dDynamicMeshDataImpl* pMesh, MotionData* Motion, nuInt nFrame);
	void applyMorph(n2dDynamicMeshDataImpl* pMesh, MotionData* Motion, nuInt nFrame);

	void doRestoreMotion(n2dDynamicMeshDataImpl* pMesh);
	void doRestoreMorph(n2dDynamicMeshDataImpl* pMesh);

	void doApplyMorph(n2dDynamicMeshDataImpl* pMesh, nuInt MorphID, nFloat Weight);
private:
	std::unordered_map<nTString, MotionData> m_MotionMap;
};