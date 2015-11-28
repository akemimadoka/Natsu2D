////////////////////////////////////////////////////////////////////////////////
///	@file	n2dKeyState.h
///	@brief	包含键盘类实现
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string.h>
typedef unsigned int uint;

////////////////////////////////////////////////////////////////////////////////
///	@brief	键盘类实现，表示键盘的按键按下情况
////////////////////////////////////////////////////////////////////////////////
class n2dKeyState final
{
public:
	n2dKeyState() {};
	~n2dKeyState() = default;

	///	@brief	清除当前状态
	void Clear() { memset(m_KeyDown, 0, MAX_KEYS); };
	///	@brief	获得按键状态
	bool IsPressed(uint key) const { return key < MAX_KEYS ? m_KeyDown[key] : false; };
	///	@brief	设置按键按下
	void SetPressed(uint key) { if (key < MAX_KEYS) m_KeyDown[key] = true; };
	///	@brief	设置按键释放
	void SetReleased(uint key) { if (key < MAX_KEYS) m_KeyDown[key] = false; };
private:
	enum
	{
		MAX_KEYS = 256	///< @brief	最大按键数
	};
	bool m_KeyDown[MAX_KEYS];
};