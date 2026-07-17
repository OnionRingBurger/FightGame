#include "Input.h"
#include "Defines.h"

//--- グローバル変数
constexpr float InputDeadzone = 4000;




bool Input::IsKeyPress(BYTE key) const
{
	return g_keyTable[key] & 0x80;
}
bool Input::IsKeyTrigger(BYTE key) const
{
	return (g_keyTable[key] ^ g_oldTable[key]) & g_keyTable[key] & 0x80;
}
bool Input::IsKeyRelease(BYTE key) const
{
	return (g_keyTable[key] ^ g_oldTable[key]) & g_oldTable[key] & 0x80;
}
bool Input::IsKeyRepeat(BYTE key) const
{
	return false;
}

bool Input::IsButtonTrigger(WORD key) const
{
	if (!isGetController) return false;
	return currentStroke.Flags & XINPUT_KEYSTROKE_KEYDOWN && currentStroke.VirtualKey == key;
}

bool Input::IsButtonPress(WORD key) const
{
	if (!isGetController) return false;
	return currentStroke.Flags & XINPUT_KEYSTROKE_REPEAT && currentStroke.VirtualKey == key;
}

Input::Input()
	: m_leftAxis(0.0f, 0.0f, 0.0f)
	, m_rightAxis(0.0f, 0.0f, 0.0f)
	, m_mouseMove(0.0f, 0.0f)
	, m_keyAxis(0.0f, 0.0f, 0.0f)
{
	// 一番最初の入力
	GetKeyboardState(g_keyTable);
	memcpy_s(g_oldTable, sizeof(g_oldTable), g_keyTable, sizeof(g_keyTable));
	isGetCursorPos = GetCursorPos(&currentPosPoint);
	currentMousePos.x = (float)(currentPosPoint.x - kMouseFixedPosX);
	currentMousePos.y = (float)(currentPosPoint.y - kMouseFixedPosY);
	oldMousePos = currentMousePos;

	UpdateInput();
}

void Input::UpdateInput()
{
	// 古い入力を更新
	memcpy_s(g_oldTable, sizeof(g_oldTable), g_keyTable, sizeof(g_keyTable));
	// 現在の入力を取得
	GetKeyboardState(g_keyTable);

	oldMousePos = currentMousePos;
	isGetCursorPos = GetCursorPos(&currentPosPoint);
	currentMousePos.x = (float)(currentPosPoint.x - kMouseFixedPosX);
	currentMousePos.y = (float)(currentPosPoint.y - kMouseFixedPosY);

	UpdateControllerInput();

	UpdateKeybordInput();

	UpdateMouseInput();
}

const Axis& Input::GetLeftAxis()  const
{
	return m_leftAxis;
}

const Axis& Input::GetRightAxis() const
{
	return m_rightAxis;
}

const MouseMove& Input::GetMouseAxis() const
{
	return m_mouseMove;
}

const Axis& Input::GetKeyAxis() const
{
	return m_keyAxis;
}

void Input::RegisterKey(std::string a_name, BYTE a_type)
{
	keyMap[a_name] = a_type;
}

void Input::RegisterButton(std::string a_name, WORD a_type)
{
	buttonMap[a_name] = a_type;
}

bool Input::IsRegisterTrigger(std::string a_type) const
{
	return IsRegisterKeyTrigger(a_type) || IsRegisterButtonTrigger(a_type);
}

bool Input::IsRegisterKeyTrigger(std::string a_type) const
{
	if (keyMap.find(a_type) == keyMap.end()) return false;
	return IsKeyTrigger(keyMap.at(a_type));
}

bool Input::IsRegisterButtonPress(std::string a_type) const
{
	if (buttonMap.find(a_type) == buttonMap.end()) return false;
	return IsButtonPress(buttonMap.at(a_type));
}

bool Input::IsRegisterPress(std::string a_type) const
{
	return IsRegisterKeyPress(a_type) || IsRegisterButtonPress(a_type);
}

bool Input::IsRegisterKeyPress(std::string a_type) const
{
	if (keyMap.find(a_type) == keyMap.end()) return false;
	return IsKeyPress(keyMap.at(a_type));
}

bool Input::IsRegisterButtonTrigger(std::string a_type) const
{
	if (buttonMap.find(a_type) == buttonMap.end()) return false;
	return IsButtonTrigger(buttonMap.at(a_type));
}



std::optional<XINPUT_STATE> Input::GetController()
{
	DWORD dwResult;
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		dwResult = XInputGetState(i, &state);

		if (dwResult == ERROR_SUCCESS)
		{
			return state;
		}

	}

	return std::nullopt;
}

Axis Input::NormalizeControllerAxis(float stickX, float stickY)
{
	// コントローラーのスティック位置を正規化
	
	float magnitube = sqrtf(stickX * stickX + stickY * stickY);

	float normalizedX = stickX / magnitube;
	float normalizedY = stickY / magnitube;

	float normalizedMagnitube = 0;

	// コントローラーの移動量を正規化
	if (magnitube > InputDeadzone)
	{
		if (magnitube > 32767) magnitube = 32767;

		magnitube -= InputDeadzone;

		normalizedMagnitube = magnitube / (32767 - InputDeadzone);
	}
	else
	{
		magnitube = 0.0f;
		normalizedMagnitube = 0.0f;
	}

	return Axis{ normalizedX, normalizedY, normalizedMagnitube };
}

Axis Input::NormalizeKeyAxis(float2 a_keyMove)
{
	float length = sqrtf(powf(a_keyMove.x, 2) + powf(a_keyMove.y, 2));
	if (length == 0.0f) return Axis(0.0f, 0.0f, 0.0f);

	return Axis(a_keyMove.x / length, a_keyMove.y / length, 1.0f);
}

void Input::UpdateControllerInput()
{
	isGetController = XInputGetKeystroke(0, 0, &currentStroke) == ERROR_SUCCESS;

	std::optional<XINPUT_STATE> stateOpt = GetController();
	if (stateOpt == std::nullopt)
	{
		m_leftAxis = Axis(0.0f, 0.0f, 0.0f);
		m_rightAxis = Axis(0.0f, 0.0f, 0.0f);

		return;
	}

	m_controllerState = stateOpt.value();

	float LX = m_controllerState.Gamepad.sThumbLX;
	float LY = m_controllerState.Gamepad.sThumbLY;
	m_leftAxis = NormalizeControllerAxis(LX, LY);

	float RX = m_controllerState.Gamepad.sThumbRX;
	float RY = m_controllerState.Gamepad.sThumbRY;
	m_rightAxis = NormalizeControllerAxis(RX, RY);
}

void Input::UpdateKeybordInput()
{
	float2 keyMove = float2();
	if (IsKeyPress('W'))
	{
		keyMove.y = 1.0f;
	}
	else if (IsKeyPress('S'))
	{
		keyMove.y = -1.0f;
	}

	if (IsKeyPress('A'))
	{
		keyMove.x = -1.0f;
	}
	else if (IsKeyPress('D'))
	{
		keyMove.x = 1.0f;
	}

	m_keyAxis = NormalizeKeyAxis(keyMove);

}

void Input::UpdateMouseInput()
{

	 m_mouseMove.x = (currentMousePos.x);
	 m_mouseMove.y = (currentMousePos.y);


}
