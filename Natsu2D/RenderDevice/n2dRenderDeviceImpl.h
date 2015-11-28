////////////////////////////////////////////////////////////////////////////////
///	@file	n2dRenderDeviceImpl.h
///	@brief	��Ⱦ�豸���
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "..\n2dRenderDevice.h"
#include "n2dShaderWrapperImpl.h"
#include <natType.h>
#include <natMath.h>
#include <stack>

////////////////////////////////////////////////////////////////////////////////
///	@brief	��Ⱦ�豸ʵ��
////////////////////////////////////////////////////////////////////////////////
class n2dRenderDeviceImpl final
	: public natRefObjImpl<n2dRenderDevice>
{
public:
	///	@brief	��Ⱦ�豸���캯��
	///	@param[in]	pEngine		Ҫ����������
	explicit n2dRenderDeviceImpl(n2dEngine* pEngine);
	~n2dRenderDeviceImpl() = default;

	///	@brief	������Ⱦ�����ĵ����߳�
	void MakeCurrent() override;

	///	@brief	����������
	void SwapBuffers() override;

	///	@brief	�����ɫ����װ��
	n2dShaderWrapper* GetShaderWrapper() override;

	///	@brief	�ύģ�;���
	///	@param[in]	Mat	�ύ�ľ���
	void SubmitModelMat(natMat4<> const& Mat) override;
	///	@brief	�ύ�۲����
	///	@param[in]	Mat	�ύ�ľ���
	void SubmitViewMat(natMat4<> const& Mat) override;
	///	@brief	�ύͶӰ����
	///	@param[in]	Mat	�ύ�ľ���
	void SubmitProjMat(natMat4<> const& Mat) override;

	///	@brief	����ǰģ�;���ѹ��ջ
	///	@note	��������MVP�������
	void PushModelMat() override;
	///	@brief	����ǰ�۲����ѹ��ջ
	///	@note	��������MVP�������
	void PushViewMat() override;
	///	@brief	����ǰͶӰ����ѹ��ջ
	///	@note	��������MVP�������
	void PushProjMat() override;

	///	@brief	��ջ�е���1��ģ�;���
	///	@return	�����Ƿ�ɹ�
	nBool PopModelMat() override;
	///	@brief	��ջ�е���1���۲����
	///	@return	�����Ƿ�ɹ�
	nBool PopViewMat() override;
	///	@brief	��ջ�е���1��ͶӰ����
	///	@return	�����Ƿ�ɹ�
	nBool PopProjMat() override;

	///	@brief	����ǰ���о���ѹ��ջ
	void PushMVPMat() override;
	///	@brief	��ջ�е���1�����о���
	///	@return	�����Ƿ�ɹ�
	nBool PopMVPMat() override;
	///	@brief	��ʼ��MVP����
	///	@note	�����ǵ�ǰջ������
	void InitMVPMat() override;

	///	@brief	��õ�ǰģ�;���
	natMat4<> GetCurModelMat() const override;
	///	@brief	��õ�ǰ�۲����
	natMat4<> GetCurViewMat() const override;
	///	@brief	��õ�ǰͶӰ����
	natMat4<> GetCurProjMat() const override;

	///	@brief	���ģ�͹۲�ͶӰ����
	///	@return	��ǰ��ģ�͹۲�ͶӰ����
	natMat4<> const& GetMVPMat() override;

	///	@brief	��ù���������
	n2dEngine* GetEngine() override;

	///	@brief	������άͼԪ��Ⱦ��
	///	@param[out]		pOut	�����Ķ�άͼԪ��Ⱦ��
	nResult CreateGraphics2D(n2dGraphics2D** pOut) override;

	nResult CreateTexture(n2dTexture2D** pOut) override;

	nResult CreateTextureFromStream(natStream* pStream, DWORD dwFileType, n2dTexture2D** pOut) override;

	nResult CreateModelLoader(n2dModelLoader** pOut) override;

	///	@deprecated		��ʵ����
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