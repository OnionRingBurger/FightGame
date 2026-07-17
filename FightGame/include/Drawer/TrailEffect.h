#pragma once
#include "PolylineEffect.h"
#include "MathAssist.h"
#include <map>

struct TrailUpdateData
{
	//! Œ»İÀ•W
	float3 currentPos;
	//! •‚Ì‘å‚«‚³
	float widthSize;
	//! •‚Ìûk—¦
	float widthShrinkage;

	TrailUpdateData(
		float3 a_currentPos,
		float a_widthSize,
		float a_widthShrinkage
	)
		: currentPos(a_currentPos)
		, widthSize(a_widthSize)
		, widthShrinkage(a_widthShrinkage)
	{
	}
};



class TrailEffect : public PolylineEffect
{
public:

	using UpdateDatas = std::map<LineID, TrailUpdateData>;

	TrailEffect(int maxVerticesv = 1000);
	~TrailEffect() = default;

	void AddUpdateLine(LineID a_id, TrailUpdateData a_data);

	void Update() override;

private:
	virtual void UpdateControlPoints(LineID id, ControlPoints& controlPoints) override;

	UpdateDatas updateData;
};