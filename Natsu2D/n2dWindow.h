#pragma once
#include "n2dInterface.h"
#include <Windows.h>

class n2dEngineImpl;

struct n2dWindow
	: n2dInterface
{
	///	@brief	��������
	///	@note	��ʹ��Show������ʾ����
	///	@param[in]	title		���ڱ���
	///	@param[in]	classname	��������
	///	@param[in]	hInstance	ʵ�����
	///	@param[in]	lpParam		��������
	virtual nBool Create(nStrView lpTitle, nStrView lpClassname, HINSTANCE hInstance, LPVOID lpParam) = 0;

	///	@brief	��ʾ/���ش���
	///	@param[in]	show	�Ƿ���ʾ����
	virtual void Show(nBool show = true) = 0;
	///	@brief	���ٴ���
	virtual void Destroy() = 0;
	///	@brief	�ı���Ļ����
	///	@note	�������л�ȫ��
	virtual nBool ChangeScreenSetting() = 0;
	///	@brief	�޸�gl��������Ӧ��ǰ��Ļ
	virtual void ReshapeGL() = 0;
	///	@brief	����������
	virtual void SwapBuffers() = 0;

	///	@brief	����X����
	///	@note	������ģʽ��Ч��ȫ��ģʽ�²�Ӱ���κ�����
	///	@param[in]	X	Ҫ���õ�����
	virtual void SetPosX(nuInt X) = 0;
	///	@brief	���X����
	///	@note	������ģʽ��Ч��ȫ��ģʽ��ʼ�շ���0
	virtual nuInt GetPosX() = 0;

	///	@brief	����Y����
	///	@note	������ģʽ��Ч��ȫ��ģʽ�²�Ӱ���κ�����
	///	@param[in]	Y	Ҫ���õ�����
	virtual void SetPosY(nuInt Y) = 0;
	///	@brief	���Y����
	///	@note	������ģʽ��Ч��ȫ��ģʽ��ʼ�շ���0
	virtual nuInt GetPosY() = 0;

	///	@brief	���ô���/ȫ��ģʽ�¿��
	///	@note	��Ӱ�쵱ǰģʽ������
	///	@param[in]	Width	Ҫ���õĿ��
	virtual void SetWidth(nuInt Width) = 0;
	///	@brief	��ô���/ȫ��ģʽ�¿��
	///	@note	���ɻ�õ�ǰģʽ������
	virtual nuInt GetWidth() = 0;

	///	@brief	���ô���/ȫ��ģʽ�¸߶�
	///	@note	��Ӱ�쵱ǰģʽ������
	///	@param[in]	Height	Ҫ���õĸ߶�
	virtual void SetHeight(nuInt Height) = 0;
	///	@brief	��ô���/ȫ��ģʽ�¸߶�
	///	@note	���ɻ�õ�ǰģʽ������
	virtual nuInt GetHeight() = 0;

	///	@brief	����ɫ������Ϊ��ǿɫ
	///	@note	���ڴ���֮ǰ������Ч
	virtual void SetHiColor() = 0;
	///	@brief	����ɫ������Ϊ���ɫ
	///	@note	���ڴ���֮ǰ������Ч
	virtual void SetTrueColor() = 0;

	///	@brief	����ȫ��ģʽ
	///	@param[in]	mode	�Ƿ�ȫ��ģʽ
	virtual void SetFullScreen(nBool mode) = 0;
	///	@brief	��õ�ǰ�Ƿ�Ϊȫ��ģʽ
	virtual nBool GetFullScreen() = 0;

	///	@brief	ɫ������
	enum class colorType
	{
		HiColor,	///< @brief	��ǿɫ
		TrueColor	///< @brief	���ɫ
	};

	///	@brief	����ɫ������
	///	@param[in]	ct	Ҫ���õ�ɫ������
	virtual void SetColorType(colorType ct) = 0;
	///	@brief	��õ�ǰ��ɫ������
	virtual colorType GetColorType() = 0;

	///	@brief	��ô��ھ��
	virtual HWND GetWnd() = 0;

	///	@brief	����豸������
	virtual HDC GetDC() = 0;

	///	@brief	�����Ⱦ������
	virtual HGLRC GetRC() = 0;

	virtual n2dEngineImpl* GetEngine() = 0;
};
