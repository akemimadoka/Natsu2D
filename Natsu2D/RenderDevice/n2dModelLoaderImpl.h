#pragma once
#include "../n2dModel.h"
#include "n2dRenderDeviceImpl.h"
#include <vector>
#include <natVec.h>

class n2dStaticModelDataImpl;
class n2dStaticMeshDataImpl;
struct aiScene;
struct aiNode;

class n2dModelLoaderImpl
	: public natRefObjImpl<n2dModelLoader>
{
public:
	n2dModelLoaderImpl(n2dRenderDeviceImpl* pRenderDevice);

	nResult CreateStaticModelFromStream(natStream* pStream, n2dModelData** pOut) override;
	nResult CreateStaticModelFromFile(nStrView lpPath, n2dModelData** pOut) override;

	nResult CreateDynamicModelFromStream(natStream* pStream, n2dModelData** pOut) override;
	nResult CreateDynamicModelFromFile(nStrView lpPath, n2dModelData** pOut) override;

	void SetDefaultTexture(n2dTexture2D* Texture) override;
private:
	void loadMeshData(n2dStaticModelDataImpl* pModel, const aiScene* pScene, const aiNode* pNode, nFloat fScale) const;

	natRefPointer<n2dTexture2D> m_DefaultTexture;

	n2dRenderDeviceImpl* m_pRenderDevice;
};