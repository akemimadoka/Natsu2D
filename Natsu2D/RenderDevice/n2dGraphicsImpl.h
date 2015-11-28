////////////////////////////////////////////////////////////////////////////////
///	@file	n2dGraphicsImpl.h
///	@brief	渲染器实现
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "..\n2dGraphics.h"

#include <vector>
#include "..\n2dInterface.h"

struct n2dModelLoader;
struct n2dRenderDevice;
struct n2dTexture2D;

////////////////////////////////////////////////////////////////////////////////
///	@brief	2D渲染器
////////////////////////////////////////////////////////////////////////////////
class n2dGraphics2DImpl final
	: public natRefObjImpl<n2dGraphics2D>
{
public:
	explicit n2dGraphics2DImpl(n2dRenderDevice* pRenderDevice);
	~n2dGraphics2DImpl();

	nBool IsRendering() override;

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

	nResult DrawRaw(n2dTexture2D* pTex, n2dModelLoader& Model) override;
private:
	////////////////////////////////////////////////////////////////////////////////
	///	@brief	渲染命令
	////////////////////////////////////////////////////////////////////////////////
	struct RenderCommand
	{
		nBool bKeepBuffer;
		n2dTexture2D* pTex;	///< @brief	纹理
		nuInt VertexBuffer;
		nuInt UVBuffer;
		nuInt NormalBuffer;
		nuInt IndexBuffer;
		nuInt cVertex;		///< @brief	顶点数量
		nuInt cIndex;		///< @brief	索引数量
	};

	void pushCommand(n2dTexture2D* pTex, nuInt cVertex, nuInt cIndex, const n2dGraphics2DVertex* varr, const nuShort* iarr);
	void flush();

	n2dRenderDevice* m_pRenderDevice;

	std::vector<RenderCommand> m_Commands;

	nBool m_bIsRendering;
};