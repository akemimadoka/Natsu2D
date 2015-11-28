////////////////////////////////////////////////////////////////////////////////
///	@file	n2dKeyState.h
///	@brief	����������ʵ��
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string.h>
typedef unsigned int uint;

////////////////////////////////////////////////////////////////////////////////
///	@brief	������ʵ�֣���ʾ���̵İ����������
////////////////////////////////////////////////////////////////////////////////
class n2dKeyState final
{
public:
	n2dKeyState() {};
	~n2dKeyState() = default;

	///	@brief	�����ǰ״̬
	void Clear() { memset(m_KeyDown, 0, MAX_KEYS); };
	///	@brief	��ð���״̬
	bool IsPressed(uint key) const { return key < MAX_KEYS ? m_KeyDown[key] : false; };
	///	@brief	���ð�������
	void SetPressed(uint key) { if (key < MAX_KEYS) m_KeyDown[key] = true; };
	///	@brief	���ð����ͷ�
	void SetReleased(uint key) { if (key < MAX_KEYS) m_KeyDown[key] = false; };
private:
	enum
	{
		MAX_KEYS = 256	///< @brief	��󰴼���
	};
	bool m_KeyDown[MAX_KEYS];
};