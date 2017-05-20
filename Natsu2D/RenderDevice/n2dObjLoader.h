////////////////////////////////////////////////////////////////////////////////
///	@file	n2dObjLoader.h
///	@brief	Obj模型加载
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <natType.h>
#include <natVec.h>
#include "../n2dModel.h"
#include "../n2dRenderDevice.h"
#include <vector>

////////////////////////////////////////////////////////////////////////////////
///	@brief	Obj加载器
///	@deprecated	已经失效，请勿使用
////////////////////////////////////////////////////////////////////////////////
class n2dObjLoader final
	: public natRefObjImpl<n2dObjLoader, n2dModelLoader>
{
public:
	n2dObjLoader();

	///	@brief	构造时加载构造函数
	///	@note	加载失败会抛出异常
	///	@param[in]	filename	文件名
	explicit n2dObjLoader(nStrView filename);
	~n2dObjLoader();

	///	@brief	加载Obj模型
	///	@param[in]	filename	文件名
	nResult CreateStaticModelFromFile(nStrView lpPath, natRefPointer<n2dModelData>& pOut) override;
	nResult CreateStaticModelFromStream(natRefPointer<natStream> pStream, natRefPointer<n2dModelData>& pOut) override;

	nResult CreateDynamicModelFromStream(natRefPointer<natStream> pStream, natRefPointer<n2dModelData>& pOut) override
	{
		return NatErr_NotImpl;
	}

	nResult CreateDynamicModelFromFile(nStrView lpPath, natRefPointer<n2dModelData>& pOut) override
	{
		return NatErr_NotImpl;
	}

	void SetDefaultTexture(natRefPointer<n2dTexture2D> Texture) override;

	n2dModelData* GetModel();

	///	@brief	获得顶点缓存
	///	@note	若未创建过顶点缓存将会创建一个，否则直接返回已创建的顶点缓存，其余缓存行为相同
	nuInt GetVertexBuffer();
	///	@brief	获得UV缓存
	///	@see	n2dObjLoader::GetVertexBuffer
	nuInt GetUVBuffer();
	///	@brief	获得法线缓存
	///	@see	n2dObjLoader::GetVertexBuffer
	nuInt GetNormalBuffer();
	///	@brief	获得元素索引缓存
	///	@note	由于会改变原有缓存数据，若在创建元素索引缓存之前创建过其他缓存会先删除之前的缓存计算索引后再生成，若创建过元素索引缓存则直接返回已创建的缓存
	nuInt GetIndexBuffer();

	///	@brief	获得顶点数目
	nuInt GetVertexCount() const;
	///	@brief	获得元素索引数目
	nuInt GetIndexCount() const;
private:
	std::vector<natVec3<>> m_Vertices;
	std::vector<natVec2<>> m_UVs; 
	std::vector<natVec3<>> m_Normals;
	std::vector<nuShort> m_ElementIndexes;

	natRefPointer<n2dTexture2D> m_Texture;
	nuInt m_VertexBuffer, m_UVBuffer, m_NormalBuffer, m_ElementBuffer;

	void init();
};