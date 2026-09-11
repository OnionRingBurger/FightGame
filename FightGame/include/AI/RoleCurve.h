#pragma once

// !!!New!!!
struct RoleCurveKnobs
{
	float distFocus = 0.5f;
	float farnessPref = 0.5f;
	float hitSensitivity = 0.5f;
	float stanceStickiness = 0.5f;
};

// !!!New!!!
struct RoleCurveParams
{
	float distCoefficient = 1.0f;
	float farnessCoefficient = 1.0f;
	float hitCoefficient = 3.0f;
	float stanceCoefficient = 1.2f;
};

// !!!New!!!
float ClampRoleKnob01(float value);

// !!!New!!!
float DistFocusToCoefficient(float distFocus01);

// !!!New!!!
float FarnessPrefToCoefficient(float farnessPref01);

// !!!New!!!
float HitSensitivityToCoefficient(float hitSensitivity01);

// !!!New!!!
float StanceStickinessToCoefficient(float stanceStickiness01);

// !!!New!!!
RoleCurveParams MapRoleCurveKnobsToParams(const RoleCurveKnobs& knobs);
