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
	///	@brief	������Ⱦ�����ĵ����߳�
	virtual void MakeCurrent() = 0;

	///	@brief	����������
	virtual void SwapBuffers() = 0;

	///	@brief	�����ɫ����װ��
	virtual n2dShaderWrapper* GetShaderWrapper() = 0;

	///	@brief	�ύģ�;���
	///	@param[in]	Mat	�ύ�ľ���
	virtual void SubmitModelMat(natMat4<> const& Mat) = 0;
	///	@brief	�ύ�۲����
	///	@param[in]	Mat	�ύ�ľ���
	virtual void SubmitViewMat(natMat4<> const& Mat) = 0;
	///	@brief	�ύͶӰ����
	///	@param[in]	Mat	�ύ�ľ���
	virtual void SubmitProjMat(natMat4<> const& Mat) = 0;

	///	@brief	����ǰģ�;���ѹ��ջ
	///	@note	��������MVP�������
	virtual void PushModelMat() = 0;
	///	@brief	����ǰ�۲����ѹ��ջ
	///	@note	��������MVP�������
	virtual void PushViewMat() = 0;
	///	@brief	����ǰͶӰ����ѹ��ջ
	///	@note	��������MVP�������
	virtual void PushProjMat() = 0;

	///	@brief	��ջ�е���1��ģ�;���
	///	@return	�����Ƿ�ɹ�
	virtual nBool PopModelMat() = 0;
	///	@brief	��ջ�е���1���۲����
	///	@return	�����Ƿ�ɹ�
	virtual nBool PopViewMat() = 0;
	///	@brief	��ջ�е���1��ͶӰ����
	///	@return	�����Ƿ�ɹ�
	virtual nBool PopProjMat() = 0;

	///	@brief	����ǰ���о���ѹ��ջ
	virtual void PushMVPMat() = 0;
	///	@brief	��ջ�е���1�����о���
	///	@return	�����Ƿ�ɹ�
	virtual nBool PopMVPMat() = 0;
	///	@brief	��ʼ��MVP����
	///	@note	�����ǵ�ǰջ������
	virtual void InitMVPMat() = 0;

	///	@brief	��õ�ǰģ�;���
	virtual natMat4<> GetCurModelMat() const = 0;
	///	@brief	��õ�ǰ�۲����
	virtual natMat4<> GetCurViewMat() const = 0;
	///	@brief	��õ�ǰͶӰ����
	virtual natMat4<> GetCurProjMat() const = 0;

	///	@brief	���ģ�͹۲�ͶӰ����
	///	@return	��ǰ��ģ�͹۲�ͶӰ����
	virtual natMat4<> const& GetMVPMat() = 0;

	///	@brief	��ù���������
	virtual n2dEngine* GetEngine() = 0;

	///	@brief	������άͼԪ��Ⱦ��
	///	@param[out]		pOut	�����Ķ�άͼԪ��Ⱦ��
	virtual nResult CreateGraphics2D(n2dGraphics2D** pOut) = 0;

	virtual nResult CreateTexture(n2dTexture2D** pOut) = 0;

	virtual nResult CreateTextureFromStream(natStream* pStream, DWORD dwFileType, n2dTexture2D** pOut) = 0;

	virtual nResult CreateModelLoader(n2dModelLoader** pOut) = 0;

	virtual nResult CreateObjLoader(n2dModelLoader** pOut) = 0;
};