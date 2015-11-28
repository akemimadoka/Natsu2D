////////////////////////////////////////////////////////////////////////////////
///	@file	n2dWindowImpl.h
///	@brief	Natsu2D�������
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "..\n2dWindow.h"
#include <Windows.h>
#include <natType.h>

class n2dEngineImpl;

////////////////////////////////////////////////////////////////////////////////
///	@addtogroup	Natsu2D����
///	@brief		Natsu2D���ڲ���
///	@{

////////////////////////////////////////////////////////////////////////////////
///	@brief	Natsu2D������
////////////////////////////////////////////////////////////////////////////////
class n2dWindowImpl
	: public natRefObjImpl<n2dWindow>
{
public:
	///	@brief	GL���ڹ��캯��
	///	@param[in]	pEngine			Natsu2D����
	///	@param[in]	X				����ģʽ��x����
	///	@param[in]	Y				����ģʽ��y����
	///	@param[in]	WindowWidth		���ڿ��
	///	@param[in]	WindowHeight	���ڸ߶�
	///	@param[in]	ScreenWidth		ȫ�����
	///	@param[in]	ScreenHeight	ȫ���߶�
	///	@param[in]	BitsPerPixel	ɫ�����
	///	@param[in]	fullscreen		����ʱ�Ƿ�Ϊȫ��
	///	@note	ɫ�����Ŀǰ��֧��16λ��32λ
	explicit n2dWindowImpl(n2dEngineImpl* pEngine, nuInt X = 0u, nuInt Y = 0u, nuInt WindowWidth = 0u, nuInt WindowHeight = 0u, nuInt ScreenWidth = 0u, nuInt ScreenHeight = 0u, nuInt BitsPerPixel = 0u, nBool fullscreen = false);
	virtual ~n2dWindowImpl();

	///	@brief	��������
	///	@note	��ʹ��Show������ʾ����
	///	@param[in]	title		���ڱ���
	///	@param[in]	classname	��������
	///	@param[in]	hInstance	ʵ�����
	///	@param[in]	lpParam		��������
	nBool Create(ncTStr lpTitle, ncTStr lpClassname, HINSTANCE hInstance, LPVOID lpParam) override;

	///	@brief	��ʾ/���ش���
	///	@param[in]	show	�Ƿ���ʾ����
	void Show(nBool show = true) override;
	///	@brief	���ٴ���
	void Destroy() override;
	///	@brief	�ı���Ļ����
	///	@note	�������л�ȫ��
	nBool ChangeScreenSetting() override;
	///	@brief	�޸�gl��������Ӧ��ǰ��Ļ
	void ReshapeGL() override;
	///	@brief	����������
	void SwapBuffers() override;

	///	@brief	����X����
	///	@note	������ģʽ��Ч��ȫ��ģʽ�²�Ӱ���κ�����
	///	@param[in]	X	Ҫ���õ�����
	void SetPosX(nuInt X) override;
	///	@brief	���X����
	///	@note	������ģʽ��Ч��ȫ��ģʽ��ʼ�շ���0u
	nuInt GetPosX() override;

	///	@brief	����Y����
	///	@note	������ģʽ��Ч��ȫ��ģʽ�²�Ӱ���κ�����
	///	@param[in]	Y	Ҫ���õ�����
	void SetPosY(nuInt Y) override;
	///	@brief	���Y����
	///	@note	������ģʽ��Ч��ȫ��ģʽ��ʼ�շ���0u
	nuInt GetPosY() override;

	///	@brief	���ô���/ȫ��ģʽ�¿��
	///	@note	��Ӱ�쵱ǰģʽ������
	///	@param[in]	Width	Ҫ���õĿ��
	void SetWidth(nuInt Width) override;
	///	@brief	��ô���/ȫ��ģʽ�¿��
	///	@note	���ɻ�õ�ǰģʽ������
	nuInt GetWidth() override;

	///	@brief	���ô���/ȫ��ģʽ�¸߶�
	///	@note	��Ӱ�쵱ǰģʽ������
	///	@param[in]	Height	Ҫ���õĸ߶�
	void SetHeight(nuInt Height) override;
	///	@brief	��ô���/ȫ��ģʽ�¸߶�
	///	@note	���ɻ�õ�ǰģʽ������
	nuInt GetHeight() override;

	///	@brief	����ɫ������Ϊ��ǿɫ
	///	@note	���ڴ���֮ǰ������Ч
	void SetHiColor() override;
	///	@brief	����ɫ������Ϊ���ɫ
	///	@note	���ڴ���֮ǰ������Ч
	void SetTrueColor() override;

	///	@brief	����ȫ��ģʽ
	///	@param[in]	mode	�Ƿ�ȫ��ģʽ
	void SetFullScreen(nBool mode) override;
	///	@brief	��õ�ǰ�Ƿ�Ϊȫ��ģʽ
	nBool GetFullScreen() override;

	///	@brief	����ɫ������
	///	@param[in]	ct	Ҫ���õ�ɫ������
	void SetColorType(colorType ct) override;
	///	@brief	��õ�ǰ��ɫ������
	colorType GetColorType() override;

	__declspec(property(put = SetColorType, get = GetColorType))
	///	@brief	ɫ��ģʽ
	colorType ColorType;

	__declspec(property(put = SetPosX, get = GetPosX))
	///	@brief	X����
	nuInt PosX;
	__declspec(property(put = SetPosY, get = GetPosY))
	///	@brief	Y����
	nuInt PosY;

	__declspec(property(put = SetWidth, get = GetWidth))
	///	@brief	���
	nuInt Width;
	__declspec(property(put = SetHeight, get = GetHeight))
	///	@brief	�߶�
	nuInt Height;

	__declspec(property(put = SetFullScreen, get = GetFullScreen))
	///	@brief	�Ƿ�ȫ��
	nBool FullScreen;

	///	@brief	��ô��ھ��
	HWND GetWnd() override;

	///	@brief	����豸������
	HDC GetDC() override;

	///	@brief	�����Ⱦ������
	HGLRC GetRC() override;

	n2dEngineImpl* GetEngine() override;

private:
	HWND m_hWnd;
	HDC m_hDC;
	HGLRC m_hRC;

	n2dEngineImpl* m_pEngine;

	nuInt m_WindowPosX;
	nuInt m_WindowPosY;
	nuInt m_WindowWidth;
	nuInt m_WindowHeight;
	nuInt m_ScreenWidth;
	nuInt m_ScreenHeight;
	nuInt m_BitsPerPixel;
	nBool m_IsFullScreen;
};

///	@}