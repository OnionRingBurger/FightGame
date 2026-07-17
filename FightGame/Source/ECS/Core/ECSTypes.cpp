#include "ECSTypes.h"



RigidTransform::RigidTransform(float3 a_position, float3 a_rotation, float3 a_scale)
	: position(a_position)
	, scale(a_scale)
{

	float3 radRotation = a_rotation * RAD;
	
	float sinX = sinf(radRotation.x);
	float cosX = cosf(radRotation.x);

	float sinY = sinf(radRotation.y);
	float cosY = cosf(radRotation.y);

	float sinZ = sinf(radRotation.z);
	float cosZ = cosf(radRotation.z);


	float rotationCmp[3][3] =
	{
		{ cosZ * cosY,  cosZ * sinY * sinX - sinZ * cosX,  cosZ * sinY * cosX + sinZ * sinX },
		{ sinZ * cosY,  sinZ * sinY * sinX + cosZ * cosX,  sinZ * sinY * cosX - cosZ * sinX },
		{ -sinY,        cosY * sinX,                       cosY * cosX }
	};


	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			rotation[i][j] = rotationCmp[i][j];
		}
	}
	
}

float3 RigidTransform::MultiplyPoint(float3 a_point)
{
	float3 rotated = {};
	float3 ret = {};

	// c‚ªs‚Ì‚½‚ß‚±‚¤‘‚­
	rotated.x =
		rotation[0][0] * a_point.x +
		rotation[0][1] * a_point.y +
		rotation[0][2] * a_point.z;

	rotated.y =
		rotation[1][0] * a_point.x +
		rotation[1][1] * a_point.y +
		rotation[1][2] * a_point.z;

	rotated.z =
		rotation[2][0] * a_point.x +
		rotation[2][1] * a_point.y +
		rotation[2][2] * a_point.z;
	
	ret.x = rotated.x + position.x;
	ret.y = rotated.y + position.y;
	ret.z = rotated.z + position.z;
	
	return ret;
}

float3 RigidTransform::MultiplyVector(float3 a_vector)
{

	float3 ret = {};

	// c‚ªs‚Ì‚½‚ß‚±‚¤‘‚­
	ret.x =
		rotation[0][0] * a_vector.x +
		rotation[0][1] * a_vector.y +
		rotation[0][2] * a_vector.z;

	ret.y =
		rotation[1][0] * a_vector.x +
		rotation[1][1] * a_vector.y +
		rotation[1][2] * a_vector.z;

	ret.z =
		rotation[2][0] * a_vector.x +
		rotation[2][1] * a_vector.y +
		rotation[2][2] * a_vector.z;

	return ret;
}
