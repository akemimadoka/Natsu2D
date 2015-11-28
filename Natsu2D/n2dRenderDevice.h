#pragma once
#include "n2dInterface.h"
#include <natMat.h>

struct n2dShaderWrapper;
struct n2dEngine;
struct n2dGraphics2D;
struct n2dTexture2D;
struct n2dModelLoader;

struct natStream;

struct n2dRenderDevice
	: n2dInterface
{
	///	@brief	关联渲染上下文到此线程
	virtual void MakeCurrent() = 0;

	///	@brief	交换缓冲区
	virtual void SwapBuffers() = 0;

	///	@brief	获得着色器包装器
	virtual n2dShaderWrapper* GetShaderWrapper() = 0;

	///	@brief	提交模型矩阵
	///	@param[in]	Mat	提交的矩阵
	virtual void SubmitModelMat(natMat4<> const& Mat) = 0;
	///	@brief	提交观察矩阵
	///	@param[in]	Mat	提交的矩阵
	virtual void SubmitViewMat(natMat4<> const& Mat) = 0;
	///	@brief	提交投影矩阵
	///	@param[in]	Mat	提交的矩阵
	virtual void SubmitProjMat(natMat4<> const& Mat) = 0;

	///	@brief	将当前模型矩阵压入栈
	///	@note	不会引发MVP矩阵更新
	virtual void PushModelMat() = 0;
	///	@brief	将当前观察矩阵压入栈
	///	@note	不会引发MVP矩阵更新
	virtual void PushViewMat() = 0;
	///	@brief	将当前投影矩阵压入栈
	///	@note	不会引发MVP矩阵更新
	virtual void PushProjMat() = 0;

	///	@brief	从栈中弹出1个模型矩阵
	///	@return	操作是否成功
	virtual nBool PopModelMat() = 0;
	///	@brief	从栈中弹出1个观察矩阵
	///	@return	操作是否成功
	virtual nBool PopViewMat() = 0;
	///	@brief	从栈中弹出1个投影矩阵
	///	@return	操作是否成功
	virtual nBool PopProjMat() = 0;

	///	@brief	将当前所有矩阵压入栈
	virtual void PushMVPMat() = 0;
	///	@brief	从栈中弹出1个所有矩阵
	///	@return	操作是否成功
	virtual nBool PopMVPMat() = 0;
	///	@brief	初始化MVP矩阵
	///	@note	仅覆盖当前栈顶矩阵
	virtual void InitMVPMat() = 0;

	///	@brief	获得当前模型矩阵
	virtual natMat4<> GetCurModelMat() const = 0;
	///	@brief	获得当前观察矩阵
	virtual natMat4<> GetCurViewMat() const = 0;
	///	@brief	获得当前投影矩阵
	virtual natMat4<> GetCurProjMat() const = 0;

	///	@brief	获得模型观察投影矩阵
	///	@return	当前的模型观察投影矩阵
	virtual natMat4<> const& GetMVPMat() = 0;

	///	@brief	获得关联的引擎
	virtual n2dEngine* GetEngine() = 0;

	///	@brief	创建二维图元渲染器
	///	@param[out]		pOut	创建的二维图元渲染器
	virtual nResult CreateGraphics2D(n2dGraphics2D** pOut) = 0;

	virtual nResult CreateTexture(n2dTexture2D** pOut) = 0;

	virtual nResult CreateTextureFromStream(natStream* pStream, DWORD dwFileType, n2dTexture2D** pOut) = 0;

	virtual nResult CreateModelLoader(n2dModelLoader** pOut) = 0;

	virtual nResult CreateObjLoader(n2dModelLoader** pOut) = 0;
};