////////////////////////////////////////////////////////////////////////////////
///	@file	n2dRenderDeviceImpl.h
///	@brief	渲染设备相关
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "..\n2dRenderDevice.h"
#include "n2dShaderWrapperImpl.h"
#include <natType.h>
#include <natMath.h>
#include <stack>

////////////////////////////////////////////////////////////////////////////////
///	@brief	渲染设备实现
////////////////////////////////////////////////////////////////////////////////
class n2dRenderDeviceImpl final
	: public natRefObjImpl<n2dRenderDevice>
{
public:
	///	@brief	渲染设备构造函数
	///	@param[in]	pEngine		要关联的引擎
	explicit n2dRenderDeviceImpl(n2dEngine* pEngine);
	~n2dRenderDeviceImpl() = default;

	///	@brief	关联渲染上下文到此线程
	void MakeCurrent() override;

	///	@brief	交换缓冲区
	void SwapBuffers() override;

	///	@brief	获得着色器包装器
	n2dShaderWrapper* GetShaderWrapper() override;

	///	@brief	提交模型矩阵
	///	@param[in]	Mat	提交的矩阵
	void SubmitModelMat(natMat4<> const& Mat) override;
	///	@brief	提交观察矩阵
	///	@param[in]	Mat	提交的矩阵
	void SubmitViewMat(natMat4<> const& Mat) override;
	///	@brief	提交投影矩阵
	///	@param[in]	Mat	提交的矩阵
	void SubmitProjMat(natMat4<> const& Mat) override;

	///	@brief	将当前模型矩阵压入栈
	///	@note	不会引发MVP矩阵更新
	void PushModelMat() override;
	///	@brief	将当前观察矩阵压入栈
	///	@note	不会引发MVP矩阵更新
	void PushViewMat() override;
	///	@brief	将当前投影矩阵压入栈
	///	@note	不会引发MVP矩阵更新
	void PushProjMat() override;

	///	@brief	从栈中弹出1个模型矩阵
	///	@return	操作是否成功
	nBool PopModelMat() override;
	///	@brief	从栈中弹出1个观察矩阵
	///	@return	操作是否成功
	nBool PopViewMat() override;
	///	@brief	从栈中弹出1个投影矩阵
	///	@return	操作是否成功
	nBool PopProjMat() override;

	///	@brief	将当前所有矩阵压入栈
	void PushMVPMat() override;
	///	@brief	从栈中弹出1个所有矩阵
	///	@return	操作是否成功
	nBool PopMVPMat() override;
	///	@brief	初始化MVP矩阵
	///	@note	仅覆盖当前栈顶矩阵
	void InitMVPMat() override;

	///	@brief	获得当前模型矩阵
	natMat4<> GetCurModelMat() const override;
	///	@brief	获得当前观察矩阵
	natMat4<> GetCurViewMat() const override;
	///	@brief	获得当前投影矩阵
	natMat4<> GetCurProjMat() const override;

	///	@brief	获得模型观察投影矩阵
	///	@return	当前的模型观察投影矩阵
	natMat4<> const& GetMVPMat() override;

	///	@brief	获得关联的引擎
	n2dEngine* GetEngine() override;

	///	@brief	创建二维图元渲染器
	///	@param[out]		pOut	创建的二维图元渲染器
	nResult CreateGraphics2D(n2dGraphics2D** pOut) override;

	nResult CreateTexture(n2dTexture2D** pOut) override;

	nResult CreateTextureFromStream(natStream* pStream, DWORD dwFileType, n2dTexture2D** pOut) override;

	nResult CreateModelLoader(n2dModelLoader** pOut) override;

	///	@deprecated		仅实验用
	nResult CreateObjLoader(n2dModelLoader** pOut) override;
private:
	void updateMVP();

	n2dEngine* m_pEngine;
	n2dShaderWrapperImpl m_Shader;

	nBool m_bUpdated;

	std::stack<natMat4<>> m_ModelMatStack;
	std::stack<natMat4<>> m_ViewMatStack;
	std::stack<natMat4<>> m_ProjMatStack;

	natMat4<> m_MVPMat;
};