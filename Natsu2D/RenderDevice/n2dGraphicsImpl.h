////////////////////////////////////////////////////////////////////////////////
///	@file	n2dGraphicsImpl.h
///	@brief	渲染器实现
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include "../n2dGraphics.h"
#include "n2dModelImpl.h"
#include "n2dBufferImpl.h"

struct n2dStaticModelLoader;
class n2dRenderDeviceImpl;
struct n2dTexture2D;

////////////////////////////////////////////////////////////////////////////////
///	@brief	2D渲染器
////////////////////////////////////////////////////////////////////////////////
class n2dGraphics2DImpl final
	: public natRefObjImpl<n2dGraphics2D>
{
public:
	explicit n2dGraphics2DImpl(n2dRenderDeviceImpl* pRenderDevice);
	~n2dGraphics2DImpl();

	nBool IsRendering() const override;

	nResult Begin() override;
	nResult Flush() override;
	nResult End() override;

	nResult DrawQuad(
		n2dTexture2D* pTex,
		n2dGraphics2DVertex const& v1,
		n2dGraphics2DVertex const& v2,
		n2dGraphics2DVertex const& v3,
		n2dGraphics2DVertex const& v4
		) override;

	nResult DrawQuad(n2dTexture2D* pTex, const n2dGraphics2DVertex* varr) override;

	nResult DrawRaw(n2dTexture2D* pTex, nuInt cVertex, nuInt cIndex, const n2dGraphics2DVertex* varr, const nuShort* iarr) override;
private:
	////////////////////////////////////////////////////////////////////////////////
	///	@brief	渲染命令
	////////////////////////////////////////////////////////////////////////////////
	struct RenderCommand
	{
		nBool bKeepBuffer;
		natRefPointer<n2dTexture2D> pTex;	///< @brief	纹理
		nuInt VertexBuffer;
		nuInt UVBuffer;
		nuInt NormalBuffer;
		nuInt IndexBuffer;
		nuInt cVertex;		///< @brief	顶点数量
		nuInt cIndex;		///< @brief	索引数量
	};

	void pushCommand(n2dTexture2D* pTex, nuInt cVertex, nuInt cIndex, const n2dGraphics2DVertex* varr, const nuShort* iarr);
	void flush();

	n2dRenderDeviceImpl* m_pRenderDevice;

	natRefPointer<n2dBuffer> m_VB, m_IB;

	std::vector<RenderCommand> m_Commands;

	nBool m_bIsRendering;
};

class n2dGraphics3DImpl
	: public natRefObjImpl<n2dGraphics3D>
{
public:
	explicit n2dGraphics3DImpl(n2dRenderDeviceImpl* pRenderDevice);
	~n2dGraphics3DImpl();

	nBool IsRendering() const override;

	nResult Begin() override;
	nResult Flush() override;
	nResult End() override;

	nResult RenderModel(n2dModelData* pModelData) override;

private:
	////////////////////////////////////////////////////////////////////////////////
	///	@brief	渲染命令
	////////////////////////////////////////////////////////////////////////////////
	struct RenderCommand
	{
		nBool bStatic;
		std::vector<nuInt> iMaterial;	///< @brief	材质索引
		n2dBufferImpl* VertexBuffer;
		n2dBufferImpl* IndexBuffer;
		nuInt cVertex;		///< @brief	顶点数量
		nuInt cIndex;		///< @brief	索引数量
	};

	void flush();

	GLuint m_MaterialID;

	n2dRenderDeviceImpl* m_pRenderDevice;
	n2dBufferImpl* m_MaterialBuffer;

	std::vector<RenderCommand> m_Commands;
	std::vector<n2dMeshData::Material*> m_StaticMaterials;
	std::vector<n2dMeshData::DynamicMaterial*> m_DynamicMaterials;

	nBool m_bIsRendering;
};