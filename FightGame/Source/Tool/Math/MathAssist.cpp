#include "MathAssist.h"
#include <cmath>
// 行列用
#include <DirectXMath.h>

constexpr int kNoiseTableSize = 128;

constexpr float kNoiseTable[kNoiseTableSize] =
{
	0.12, -0.45, 0.78, -0.23, 0.56, -0.91, 0.34, -0.67,
	0.89, -0.11, 0.43, -0.76, 0.21, -0.54, 0.97, -0.32,
	0.65, -0.88, 0.14, -0.49, 0.72, -0.27, 0.51, -0.93,
	0.38, -0.61, 0.84, -0.19, 0.47, -0.70, 0.26, -0.58,
	0.91, -0.36, 0.68, -0.82, 0.17, -0.53, 0.75, -0.29,
	0.44, -0.66, 0.99, -0.13, 0.57, -0.85, 0.31, -0.74,
	0.62, -0.95, 0.08, -0.41, 0.79, -0.22, 0.50, -0.69,
	0.28, -0.60, 0.87, -0.34, 0.46, -0.77, 0.19, -0.55,
	0.93, -0.25, 0.71, -0.90, 0.15, -0.48, 0.82, -0.30,
	0.53, -0.64, 0.96, -0.18, 0.67, -0.83, 0.24, -0.59,
	0.74, -0.92, 0.10, -0.37, 0.80, -0.21, 0.42, -0.73,
	0.33, -0.62, 0.88, -0.28, 0.49, -0.79, 0.16, -0.57,
	0.94, -0.31, 0.69, -0.86, 0.20, -0.52, 0.77, -0.24,
	0.55, -0.68, 0.98, -0.14, 0.63, -0.81, 0.29, -0.72,
	0.40, -0.65, 0.85, -0.26, 0.48, -0.78, 0.18, -0.56,
	0.92, -0.35, 0.70, -0.89, 0.13, -0.47, 0.83, -0.33
};

float DotFloat3(float3 a_vecA, float3 a_vecB)
{
	return a_vecA.x * a_vecB.x + a_vecA.y * a_vecB.y + a_vecA.z * a_vecB.z;
}

Matrix3X3 CollectMatrix(std::vector<float3> a_vertex)
{
	float3 m;

	for (auto it : a_vertex)
	{
		m.x += it.x;
		m.y += it.y;
		m.z += it.z;
	}

	m.x /= (float)a_vertex.size();
	m.y /= (float)a_vertex.size();
	m.z /= (float)a_vertex.size();

	float c11 = 0, c22 = 0, c33 = 0, c12 = 0, c13 = 0, c23 = 0;

	for (auto it : a_vertex)
	{
		c11 += (it.x - m.x) * (it.x - m.x);
		c22 += (it.y - m.y) * (it.y - m.y);
		c33 += (it.z - m.z) * (it.z - m.z);

		c12 += (it.x - m.x) * (it.y - m.y);
		c13 += (it.x - m.x) * (it.z - m.z);
		c23 += (it.y - m.y) * (it.z - m.z);
	}

	c11 /= a_vertex.size();
	c22 /= a_vertex.size();
	c33 /= a_vertex.size();
	c12 /= a_vertex.size();
	c13 /= a_vertex.size();
	c23 /= a_vertex.size();

	Matrix3X3 ret =
	{
		std::array<float,3>{ c11, c12, c13 },
		std::array<float,3>{ c12, c22, c23 },
		std::array<float,3>{ c13, c23, c33 },
	};

	return ret;
}

Matrix3X3 GetEigenVectors(const Matrix3X3& a_matrix)
{
	static const float maxOffDiagonal = 0.0001f;

	Matrix3X3 matrix = a_matrix;

	Matrix3X3 eigenVectors;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (i == j)
			{
				eigenVectors[i][j] = 1.0f;
			}
			else
			{
				eigenVectors[i][j] = 0.0f;
			}
		}
	}

	int limit = 100;
	int count = 0;
	int p;
	int q;

	// 中で必ずbreakする
	while (true)
	{
		count++;

		if (count >= limit)
		{
			break;
		}

		float epsilon = GetMaxValue(matrix, p, q);

		if (epsilon <= maxOffDiagonal)
		{
			break;
		}

		float app = matrix[p][p];
		float apq = matrix[p][q];
		float aqq = matrix[q][q];

		float alpha = (app - aqq) / 2.0f;
		float beta = -apq;

		if (std::fabs(apq) < 1e-8f)
		{
			continue;
		}
		float gamma = std::abs(alpha) / std::sqrtf(alpha * alpha + beta * beta);

		float sin = std::sqrtf((1.0f - gamma) / 2.0f);
		float cos = std::sqrtf((1.0f + gamma) / 2.0f);

		if (alpha * beta < 0)
		{
			sin = -sin;
		}

		for (int i = 0; i < 3; i++)
		{
			matrix[i][p] = matrix[p][i];
			matrix[i][q] = matrix[q][i];
		}

		matrix[p][p] = cos * cos * app + sin * sin * aqq - 2 * sin * cos * apq;
		matrix[p][q] = sin * cos * (app - aqq) + (cos * cos - sin * sin) * apq;
		matrix[q][p] = sin * cos * (app - aqq) + (cos * cos - sin * sin) * apq;
		matrix[q][q] = sin * sin * app + cos * cos * aqq + 2 * sin * cos * apq;

		for (int i = 0; i < 3; i++)
		{
			float temp = cos * eigenVectors[i][p] - sin * eigenVectors[i][q];
			eigenVectors[i][q] = sin * eigenVectors[i][p] + cos * eigenVectors[i][q];
			eigenVectors[i][p] = temp;
		}
	}

	return eigenVectors;

}

float GetMaxValue(const Matrix3X3& matrix, int& p, int& q)
{
	p = 0;
	q = 0;

	int rank = 3;

	// 最低値を入れて毎回最大値を更新していく
	float max = std::numeric_limits<float>::lowest();

	for (int i = 0; i < rank; i++)
	{
		for (int j = 0; j < matrix[i].size(); j++)
		{
			if (i == j)
			{
				continue;
			}

			float absmax = abs(matrix[i][j]);
			if (max <= absmax)
			{
				max = absmax;
				p = i;
				q = j;
			}
		}
	}

	if (p > q)
	{
		int temp = p;
		p = q;
		q = temp;
	}

	return max;
}

float3 MultiplyVector(float3 a_rotation, float3 a_scale, float3 a_vector)
{
	return float3();
}

float3 Normalize(const float3& a_vector)
{

	float lengthSq = a_vector.x * a_vector.x + a_vector.y * a_vector.y + a_vector.z * a_vector.z;

	if (lengthSq < 1e-8f)
	{
		return float3{ 0, 0, 0 };
	}

	float invLen = 1.0f / std::sqrtf(lengthSq);
	return float3{ a_vector.x * invLen, a_vector.y * invLen, a_vector.z * invLen };
}

float NormalizeAngle(float radAngle)
{
	while (radAngle < -2 * PI)
	{
		radAngle += 2 * PI;
	}
	while (radAngle > 2 * PI)
	{
		radAngle -= 2 * PI;
	}
	return radAngle;
}

// 外積を求める
// ワールドの上方向と自身の向いている方向を使って自身から見た横方向や上方向を取ったり出来る。
float3 Cross(float3 vecA, float3 vecB)
{
	float3 ret;

	ret.x = vecA.y * vecB.z - vecA.z * vecB.y;
	ret.y = vecA.z * vecB.x - vecA.x * vecB.z;
	ret.z = vecA.x * vecB.y - vecA.y * vecB.x;

	return ret;
}

float RSqrt(float x)
{
	return 1.0f / std::sqrt(x);
}

float3 Sign(float3 num)
{
	return float3(
		Sign(num.x),
		Sign(num.y),
		Sign(num.z)
	);
}

float Sign(float num)
{
	return (num > 0) - (num < 0);
}

float3 ExponentialRad(const float3& currentRad, const float3& targetRad, float speed, float dt)
{
	float3 AngleDiff(
		NormalizeAngle((targetRad.x - currentRad.x)),
		NormalizeAngle((targetRad.y - currentRad.y)),
		NormalizeAngle((targetRad.z - currentRad.z))
	);

	float3 ret = currentRad + AngleDiff * (1 - std::expf(-speed * dt));

	return ret;
}

float3 GetForward(float3 rotation)
{
	const float pitch = rotation.x * RAD;
	const float yaw = rotation.y * RAD;
	const float roll = rotation.z * RAD;

	const DirectX::XMMATRIX rot = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	const DirectX::XMVECTOR forwardVector = DirectX::XMVector3TransformNormal(
		DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rot);

	DirectX::XMFLOAT3 forward;
	DirectX::XMStoreFloat3(&forward, forwardVector);
	return float3(forward.x, forward.y, forward.z);
}

float GetLength(float3 thisPos, float3 otherPos)
{
	return std::sqrtf(
		std::pow(thisPos.x - otherPos.x, 2) +
		std::pow(thisPos.y - otherPos.y, 2) +
		std::pow(thisPos.z - otherPos.z, 2)
	);
}

float Frac(float a_value)
{
	return 	a_value - floor(a_value);
}

float Noise(float a_time)
{
	// 値を範囲内に設定
	float value = std::fmod(a_time ,kNoiseTableSize);
	// 整数の値を使ってTableの値を選択する
	int startTableIndex = (int)floor(a_time) % kNoiseTableSize;
	int endTableIndex = (int)floor(a_time) % kNoiseTableSize;
	// 小数点以下の値を使って振動の進行度を補完する
	float lerpValue = Frac(a_time);
	
	// 補完する
	return Lerp(kNoiseTable[startTableIndex], kNoiseTable[endTableIndex], lerpValue) /  - 0.5f;
}

float Lerp(float a_start, float a_end, float a_rate)
{
	return (a_end - a_start) * a_rate + a_start;
}

float NormalizeLength(const float x, const float y, const float z)
{

	float lengthSq = x * x + y * y + z * z;

	if (lengthSq < 1e-8f)
	{
		return 0.0f;
	}

	return std::sqrtf(lengthSq);
}
