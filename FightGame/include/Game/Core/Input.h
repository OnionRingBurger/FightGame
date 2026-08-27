#ifndef __INPUT_H__
#define __INPUT_H__

#include <Windows.h>
#include <optional>
#include <string>
#include <unordered_map>

#include "MathAssist.h"
#include "xinput.h"
#undef max
#undef min



struct Axis
{
	// Œü‚«
	float x;
	float y;
	// “ü—Í—Ê
	float magnitube;

	Axis(float a_x, float a_y, float a_magnitube)
		: x(a_x)
		, y(a_y)
		, magnitube(a_magnitube)
	{}
};

using MouseMove = float2;

class Input
{
public:

	Input();
	~Input() = default;
	void UpdateInput();

	const Axis& GetLeftAxis() const;
	const Axis& GetRightAxis() const;
	const MouseMove& GetMouseAxis() const;
	const Axis& GetKeyAxis() const;

	void RegisterKey(std::string a_name, BYTE a_type);
	void RegisterButton(std::string a_name, WORD  a_type);

	bool IsRegisterTrigger(std::string) const;
	bool IsRegisterKeyTrigger(std::string) const;
	bool IsRegisterButtonTrigger(std::string) const;

	bool IsRegisterPress(std::string) const;
	bool IsRegisterKeyPress(std::string) const;
	bool IsRegisterButtonPress(std::string) const;

	
private:
	std::optional<XINPUT_STATE> GetController();
	Axis NormalizeControllerAxis(float stickX, float stickY);
	Axis NormalizeKeyAxis(float2);

	void UpdateControllerInput();
	void UpdateKeybordInput();
	void UpdateMouseInput();

	bool IsKeyPress(BYTE key) const;
	bool IsKeyTrigger(BYTE key) const;
	bool IsKeyRelease(BYTE key) const;
	bool IsKeyRepeat(BYTE key) const;

	bool IsButtonTrigger(WORD key) const;
	bool IsButtonPress(WORD key) const;

	XINPUT_STATE m_controllerState;
	Axis m_leftAxis;
	Axis m_rightAxis;
	MouseMove m_mouseMove;
	Axis m_keyAxis;
	std::unordered_map<std::string, BYTE> keyMap;
	std::unordered_map<std::string, WORD> buttonMap;

	POINT currentPosPoint;
	float2 currentMousePos;
	float2 oldMousePos;
	XINPUT_KEYSTROKE currentStroke;
	bool isGetController;

	BOOL isGetCursorPos;
	BYTE g_keyTable[256];
	BYTE g_oldTable[256];

	WORD currentButton;
	WORD oldButton;
	WORD lastPacket;
	
};




#endif // __INPUT_H__