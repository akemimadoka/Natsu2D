#include "n2dMotionManagerImpl.h"
#include <natStream.h>
#include <natUtil.h>

#include <set>

namespace
{
	ncTStr PMDBoneKneeName =
#ifdef _UNICODE
		_T("\x5082\x505e");
#else
		_T("\x82\xd0\x82\xb4");
#endif
}

n2dBone::n2dBone(natVec3<> const& position, natQuat<> const& orientation, n2dBone * parent, nBool isLimitAngleX)
	: m_isLimitAngleX(isLimitAngleX),
	m_pParent(parent),
	m_Position(position),
	m_Orientation(orientation)
{
}

void n2dBone::SetTranslation(natVec3<> const& v)
{
	m_Translation = v;
}

void n2dBone::SetRotation(natQuat<> const& q)
{
	m_Rotation = q;
}

void n2dBone::Update()
{
	if (m_pParent)
	{
		m_Transform = m_pParent->GetTransform() * natTransform::translate(natMat4<>(1.0f), m_InitialPosition - m_pParent->GetInitialPosition() + m_Translation) * natMat4<>((m_Orientation * m_Rotation).cast<natMat3<>>());
	}
	else
	{
		m_Transform = natTransform::translate(natMat4<>(1.0f), m_InitialPosition + m_Translation) * natMat4<>((m_Orientation * m_Rotation).cast<natMat3<>>());
	}

	m_Position = natVec3<>(m_Transform * natVec4<>(0, 0, 0, 1));

	m_Matrix = m_Transform * natTransform::translate(natMat4<>(1.0f), -m_InitialPosition);
}

natQuat<> const& n2dBone::GetRotation() const
{
	return m_Rotation;
}

natMat4<> const& n2dBone::GetTransform() const
{
	return m_Transform;
}

natMat4<> const& n2dBone::GetMatrix() const
{
	return m_Matrix;
}

natVec3<> const& n2dBone::GetInitialPosition() const
{
	return m_InitialPosition;
}

natVec3<> const& n2dBone::GetPosition() const
{
	return m_Position;
}

n2dBone * n2dBone::GetParent() const
{
	return m_pParent;
}

nBool n2dBone::IsLimitAngleX() const
{
	return m_isLimitAngleX;
}

n2dMotionManagerImpl::n2dMotionManagerImpl()
{
}

n2dMotionManagerImpl::~n2dMotionManagerImpl()
{
}

nResult n2dMotionManagerImpl::LoadMotionFromStream(ncTStr lpName, natStream * pStream)
{
	// Parse file start
	if (!pStream)
	{
		return NatErr_InvalidArg;
	}

	if (m_MotionMap.end() != m_MotionMap.find(lpName))
	{
		return NatErr_InternalErr;
	}

	MotionData& tMotion = m_MotionMap[lpName];

	nByte tBuf[31] = { 0 };

	pStream->ReadBytes(tBuf, 30ull);
	if (0 == strcmp("Vocaloid Motion Data file", reinterpret_cast<ncStr>(tBuf)))
	{
		tMotion.Ver = VMDVersion::Old;
	}
	else if (0 == strcmp("Vocaloid Motion Data 0002", reinterpret_cast<ncStr>(tBuf)))
	{
		tMotion.Ver = VMDVersion::New;
	}
	else
	{
		return NatErr_InternalErr;
	}

	memset(tBuf, 0, 21);

	switch (tMotion.Ver)
	{
	case VMDVersion::Old:
		pStream->ReadBytes(tBuf, 10ull);
		break;
	case VMDVersion::New:
		pStream->ReadBytes(tBuf, 20ull);
		break;
	default:
		return NatErr_InternalErr;
	}

	tMotion.OriginModelName =
#ifdef _UNICODE
		natUtil::MultibyteToUnicode(reinterpret_cast<ncStr>(tBuf));
#else
		reinterpret_cast<ncStr>(tBuf);
#endif

	pStream->ReadBytes(tBuf, 4ull);
	tMotion.BoneFrames.resize(*reinterpret_cast<nuInt*>(tBuf));
	
	for (auto& tBoneFrame : tMotion.BoneFrames)
	{
		tBuf[15] = 0u;
		pStream->ReadBytes(tBuf, 15ull);

		tBoneFrame.TargetBone =
#ifdef _UNICODE
			natUtil::MultibyteToUnicode(reinterpret_cast<ncStr>(tBuf));
#else
			reinterpret_cast<ncStr>(tBuf);
#endif

		pStream->ReadBytes(reinterpret_cast<nData>(&tBoneFrame.FrameNumber), 4ull);

		pStream->ReadBytes(reinterpret_cast<nData>(&tBoneFrame.Translation), 12ull);
		tBoneFrame.Translation.z = -tBoneFrame.Translation.z;
		pStream->ReadBytes(reinterpret_cast<nData>(&tBoneFrame.Rotation), 16ull);
		tBoneFrame.Rotation.x = -tBoneFrame.Rotation.x;
		tBoneFrame.Rotation.y = -tBoneFrame.Rotation.y;
		pStream->ReadBytes(tBoneFrame.Interpolation, 64ull);
	}

	pStream->ReadBytes(tBuf, 4ull);
	tMotion.MorphFrames.resize(*reinterpret_cast<nuInt*>(tBuf));

	for (auto& tMorphFrame : tMotion.MorphFrames)
	{
		memset(tBuf, 0, 16);
		pStream->ReadBytes(tBuf, 15ull);

		tMorphFrame.TargetMorph =
#ifdef _UNICODE
			natUtil::MultibyteToUnicode(reinterpret_cast<ncStr>(tBuf));
#else
			reinterpret_cast<ncStr>(tBuf);
#endif

		pStream->ReadBytes(reinterpret_cast<nData>(&tMorphFrame.nFrame), 4ull);
		pStream->ReadBytes(reinterpret_cast<nData>(&tMorphFrame.Weight), 4ull);
	}
	// Parse file end

	// Parse bone timeline start
	std::vector<nuInt> KeyFrames(tMotion.BoneFrames.size());
	nuInt Index = 0u;
	for (auto& tBoneFrame : tMotion.BoneFrames)
	{
		auto tItea = tMotion.BoneMap.find(tBoneFrame.TargetBone);
		if (tMotion.BoneMap.end() == tItea)
		{
			tMotion.BoneMap[tBoneFrame.TargetBone] = Index;
			KeyFrames[Index] = 1u;
			++Index;
		}
		else
		{
			++KeyFrames[tItea->second];
		}
	}

	tMotion.BoneTimelines.resize(Index);
	for (nuInt i = 0; i < Index; ++i)
	{
		tMotion.BoneTimelines[i].m_KeyFrames.resize(KeyFrames[i]);
	}

	std::vector<nuInt> keyFrameIter(Index);
	for (auto& tBoneFrame : tMotion.BoneFrames)
	{
		nuInt LineID = tMotion.BoneMap[tBoneFrame.TargetBone];
		nuInt KeyFrameID = keyFrameIter[LineID]++;
		tMotion.BoneTimelines[LineID].m_KeyFrames[KeyFrameID] = tBoneFrame;
	}

	for (auto& tTimeline : tMotion.BoneTimelines)
	{
		std::sort(tTimeline.m_KeyFrames.begin(), tTimeline.m_KeyFrames.end());
	}
	// Parse bone timeline end

	// Parse morph timeline start
	KeyFrames.clear();
	KeyFrames.resize(tMotion.MorphFrames.size());
	Index = 0u;
	for (auto& tMorphFrame : tMotion.MorphFrames)
	{
		auto tItea = tMotion.MorphMap.find(tMorphFrame.TargetMorph);
		if (tMotion.MorphMap.end() == tItea)
		{
			tMotion.MorphMap[tMorphFrame.TargetMorph] = Index;
			KeyFrames[Index] = 1u;
			++Index;
		}
		else
		{
			++KeyFrames[tItea->second];
		}
	}

	tMotion.MorphTimelines.resize(Index);
	for (nuInt i = 0; i < Index; ++i)
	{
		tMotion.MorphTimelines[i].m_KeyFrames.resize(KeyFrames[i]);
	}

	keyFrameIter.clear();
	keyFrameIter.resize(Index);
	for (auto& tMorphFrame : tMotion.MorphFrames)
	{
		nuInt LineID = tMotion.MorphMap[tMorphFrame.TargetMorph];
		nuInt KeyFrameID = keyFrameIter[LineID]++;
		auto& kf = tMotion.MorphTimelines[LineID].m_KeyFrames[KeyFrameID];
		kf = tMorphFrame;
		kf.nFrame = tMorphFrame.nFrame;
	}

	for (auto& tTimeline : tMotion.MorphTimelines)
	{
		std::sort(tTimeline.m_KeyFrames.begin(), tTimeline.m_KeyFrames.end());
	}
	// Parse morph timeline end

	return NatErr_OK;
}

nResult n2dMotionManagerImpl::LoadMotionFromFile(ncTStr lpName, ncTStr lpPath)
{
	if (!lpName || !lpPath)
	{
		return NatErr_InvalidArg;
	}

	natFileStream* pStream = new natFileStream(lpPath, true, false);
	nResult tRet = LoadMotionFromStream(lpName, pStream);
	SafeRelease(pStream);

	return tRet;
}

nResult n2dMotionManagerImpl::ApplyToModel(ncTStr lpName, n2dModelData * pModel)
{
	if (!lpName || !pModel || pModel->IsStatic())
	{
		return NatErr_InvalidArg;
	}

	n2dDynamicModelDataImpl* pDynamicModel = dynamic_cast<n2dDynamicModelDataImpl*>(pModel);
	if (!pDynamicModel)
	{
		return NatErr_InvalidArg;
	}

	auto tItea = m_MotionMap.find(lpName);
	if (m_MotionMap.end() == tItea)
	{
		return NatErr_InternalErr;
	}

	for (nuInt i = 0u; i < pDynamicModel->m_Mesh.m_Bones.size(); ++i)
	{
		auto itea = tItea->second.BoneMap.find(pDynamicModel->m_Mesh.m_Bones[i].Name);
		pDynamicModel->m_Mesh.m_BoneMap[i] = tItea->second.BoneMap.end() != itea ? itea->second : nuInt(-1);
	}

	for (nuInt i = 0u; i < pDynamicModel->m_Mesh.m_Morphes.size(); ++i)
	{
		auto itea = tItea->second.MorphMap.find(pDynamicModel->m_Mesh.m_Morphes[i].Name);
		pDynamicModel->m_Mesh.m_MorphMap[i] = tItea->second.MorphMap.end() != itea ? itea->second : nuInt(-1);
	}

	doRestoreMotion(&pDynamicModel->m_Mesh);
	doRestoreMorph(&pDynamicModel->m_Mesh);
	
	pDynamicModel->m_Mesh.m_pMotion = &tItea->second;
	pDynamicModel->m_Mesh.m_pMotionManager = this;
	pDynamicModel->m_Mesh.m_LastFrame = nuInt(-1);

	return NatErr_OK;
}

void n2dMotionManagerImpl::RestoreMotion(n2dModelData * pModel)
{
	if (!pModel || pModel->IsStatic())
	{
		return;
	}

	n2dDynamicModelDataImpl* pDynamicModel = dynamic_cast<n2dDynamicModelDataImpl*>(pModel);
	if (!pDynamicModel)
	{
		return;
	}

	doRestoreMotion(&pDynamicModel->m_Mesh);
}

void n2dMotionManagerImpl::RestoreMorph(n2dModelData * pModel)
{
	if (!pModel || pModel->IsStatic())
	{
		return;
	}

	n2dDynamicModelDataImpl* pDynamicModel = dynamic_cast<n2dDynamicModelDataImpl*>(pModel);
	if (!pDynamicModel)
	{
		return;
	}

	doRestoreMorph(&pDynamicModel->m_Mesh);
}

void n2dMotionManagerImpl::applyMotion(n2dDynamicMeshDataImpl* pMesh, MotionData* Motion, nuInt nFrame)
{
	for (nuInt i = 0u; i < pMesh->m_BoneMap.size(); ++i)
	{
		nuInt timeLineID = pMesh->m_BoneMap[i];
		if (timeLineID == nuInt(-1))
		{
			continue;
		}
		n2dBone& b = pMesh->m_Selekton->m_Bones[i];
		auto const& kf = Motion->BoneTimelines[timeLineID].GetFrame(nFrame);
		b.SetRotation(kf.Rotation);
		b.SetTranslation(kf.Translation);
	}

	pMesh->m_Selekton->Update();
}

void n2dMotionManagerImpl::applyMorph(n2dDynamicMeshDataImpl* pMesh, MotionData* Motion, nuInt nFrame)
{
	doRestoreMorph(pMesh);
	for (nuInt i = 1u; i < pMesh->m_MorphMap.size(); ++i)
	{
		nuInt timeLineID = pMesh->m_MorphMap[i];
		if (timeLineID == nuInt(-1))
		{
			continue;
		}
		auto const& kf = Motion->MorphTimelines[timeLineID].GetFrame(nFrame);

		doApplyMorph(pMesh, i, kf.Weight);
	}
}

void n2dMotionManagerImpl::doRestoreMotion(n2dDynamicMeshDataImpl* pMesh)
{
	for (auto& tBone : pMesh->m_Selekton->m_Bones)
	{
		tBone.SetRotation(natQuat<>());
		tBone.SetTranslation(natVec3<>(0.0f));
	}
}

void n2dMotionManagerImpl::doRestoreMorph(n2dDynamicMeshDataImpl* pMesh)
{
	n2dDynamicMeshDataImpl::MorphData& morph = pMesh->m_Morphes[0];
	for (nuInt i = 0u; i < morph.Vertexes.size(); ++i)
	{
		pMesh->m_Vert[morph.Vertexes[i]].vert = morph.Translation[i];
	}
}

void n2dMotionManagerImpl::doApplyMorph(n2dDynamicMeshDataImpl * pMesh, nuInt MorphID, nFloat Weight)
{
	auto& md = pMesh->m_Morphes[MorphID];
	auto& basemd = pMesh->m_Morphes[0u];

	for (nuInt j = 0u; j < md.Vertexes.size(); ++j)
	{
		pMesh->m_Vert[basemd.Vertexes[md.Vertexes[j]]].vert += Weight * md.Translation[MorphID];
	}
}

void n2dSkeleton::CreateFromDynamicModel(const n2dDynamicModelDataImpl * pModel)
{
	std::set<n2dBone*> tBoneSet;
	nuInt nBone = pModel->m_Mesh.m_Bones.size();
	m_Bones.resize(nBone);
	m_UpdateList.resize(nBone);

	for (nuInt i = 0u; i < nBone; ++i)
	{
		auto& tBone = pModel->m_Mesh.m_Bones[i];
		nuShort parentID = tBone.Parent;
		m_Bones[i].m_InitialPosition = tBone.Pos;
		m_Bones[i].m_pParent = (parentID == nuShort(-1) ? nullptr : &m_Bones[parentID]);
		m_Bones[i].m_isLimitAngleX = tBone.Name.find(PMDBoneKneeName) != nTString::npos;
		tBoneSet.insert(&m_Bones[i]);
	}

	for (nuInt i = 0u; i < nBone; ++i)
	{
		n2dBone* pBone = *tBoneSet.begin();
		//nuInt Count = 0u;
		while (pBone->m_pParent != nullptr && tBoneSet.find(pBone->GetParent()) != tBoneSet.end())
		{
			pBone = pBone->m_pParent;
			/*if (Count++ == nBone)
			{
				nat_Throw(natException, _T("Invalid bone data"));
			}*/
		}
		m_UpdateList[i] = pBone;
		tBoneSet.erase(pBone);
	}

	Update();
}

void n2dSkeleton::Update()
{
	for (auto& tBone : m_UpdateList)
	{
		tBone->Update();
	}
}

void n2dIK::CreateFromDynamicModel(n2dDynamicModelDataImpl* pModel, nuInt IKID)
{
	n2dDynamicMeshDataImpl::IKData& tIKD = pModel->m_Mesh.m_IKs[IKID];
	m_Target = &pModel->m_Mesh.m_Selekton->m_Bones[tIKD.Target];
	m_Effector = &pModel->m_Mesh.m_Selekton->m_Bones[tIKD.Effector];
	m_Chain.resize(tIKD.LinkList.size());
	for (nuInt i = 0u; i < m_Chain.size(); ++i)
	{
		m_Chain[i] = &pModel->m_Mesh.m_Selekton->m_Bones[tIKD.LinkList[i]];
	}
	m_nIteration = tIKD.MaxIteration;
	m_MaxRotation = tIKD.MaxRotation;
	m_pSelekton = pModel->m_Mesh.m_Selekton.get();
}

void n2dIK::Solve()
{
	for (nuInt i = 0u; i < m_nIteration; ++i)
	{
		for (nuInt j = 0u; j < m_Chain.size(); ++j)
		{
			n2dBone* pBone = m_Chain[j];
			if (pBone == m_Effector || pBone == m_Target)
			{
				continue;
			}

			natVec3<> dir = natTransform::normalize(m_Effector->GetPosition() - pBone->GetPosition());
			natVec3<> tar = natTransform::normalize(m_Target->GetPosition() - pBone->GetPosition());

			natQuat<> r(static_cast<natMat3<>>(pBone->GetTransform()));
			r.w = -r.w;
			natMat3<> tMat = r.cast<natMat3<>>();
			dir = tMat * dir;
			tar = tMat * tar;

			tar = natTransform::normalize(tar + dir);
			r = natQuat<>(dir, tar);

			nFloat angle = 2.0f * acos(r.w);
			if (fabs(angle) > m_MaxRotation)
			{
				angle = angle > 0 ? m_MaxRotation : -m_MaxRotation;
				r = natQuat<>(cos(angle / 2.0f), natTransform::normalize(natVec3<>(r.x, r.y, r.z))*sin(angle / 2.0f));
			}

			if (pBone->IsLimitAngleX())
			{
				natVec3<> euler = natTransform::eulerAngles(r);
				if (i == 0u)
				{
					euler.x = 0.5f;
				}
				else
				{
					euler.x = euler.x > 0.0f ? euler.x : 0.0f;
				}
				euler.y = 0.0f;
				euler.z = 0.0f;
				r = natQuat<>(euler);
			}
			pBone->SetRotation(pBone->GetRotation() * r);
			for (nInt k = static_cast<nInt>(j); k >= 0; --k)
			{
				m_Chain[k]->Update();
			}
			m_Effector->Update();
		}
	}
}
