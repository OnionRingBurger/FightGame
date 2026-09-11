#include "AI/RoleCurve.h"

#include <algorithm>

#include "Tool/Math/MathAssist.h"

// Roleの値から曲線用への値へ変換する関数群

namespace
{
	// 得意距離、不利距離判定の最大、最低数値
	constexpr float kDistCoeffLoose = 30.0f;
	constexpr float kDistCoeffTight = 1.0f;

	// 近距離、遠距離判定の最大、最低数値
	constexpr float kFarnessCoeffClose = 10.0f;
	constexpr float kFarnessCoeffFar = 0.5f;

	// 攻撃的中判定の最大、最低数値
	constexpr float kHitCoeffSlow = 8.0f;
	constexpr float kHitCoeffFast = 0.5f;

	// 方針連続継続の最大、最低数値
	constexpr float kStanceCoeffSwitchy = 8.0f;
	constexpr float kStanceCoeffSticky = 1.05f;
}

// Clampのラッパー
float ClampRoleKnob01(float value)
{
	return std::clamp(value, 0.0f, 1.0f);
}

// !!!New!!!
float DistFocusToCoefficient(float distFocus01)
{
	const float t = ClampRoleKnob01(distFocus01);
	return Lerp(kDistCoeffLoose, kDistCoeffTight, t);
}

// !!!New!!!
float FarnessPrefToCoefficient(float farnessPref01)
{
	const float t = ClampRoleKnob01(farnessPref01);
	return Lerp(kFarnessCoeffClose, kFarnessCoeffFar, t);
}

// !!!New!!!
float HitSensitivityToCoefficient(float hitSensitivity01)
{
	const float t = ClampRoleKnob01(hitSensitivity01);
	return Lerp(kHitCoeffSlow, kHitCoeffFast, t);
}

// !!!New!!!
float StanceStickinessToCoefficient(float stanceStickiness01)
{
	const float t = ClampRoleKnob01(stanceStickiness01);
	return Lerp(kStanceCoeffSwitchy, kStanceCoeffSticky, t);
}

// 渡されたロールを全て曲線用パラメーターに変換する
RoleCurveParams MapRoleCurveKnobsToParams(const RoleCurveKnobs& knobs)
{
	RoleCurveParams params;
	params.distCoefficient = DistFocusToCoefficient(knobs.distFocus);
	params.farnessCoefficient = FarnessPrefToCoefficient(knobs.farnessPref);
	params.hitCoefficient = HitSensitivityToCoefficient(knobs.hitSensitivity);
	params.stanceCoefficient = StanceStickinessToCoefficient(knobs.stanceStickiness);
	return params;
}
