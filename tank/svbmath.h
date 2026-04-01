#pragma once

constexpr float PI = 3.14159265358979323846f;

namespace svbmath {
	struct Vec3 {
		float x, y, z;
		Vec3 operator-(const Vec3& other) const {
			return { x - other.x, y - other.y, z - other.z };
		}
	};
    Vec3 Normalize(const Vec3& v);
	Vec3 DirectionToTarget(const Vec3& enemy, const Vec3& target);
	float NormalizeAngle(float angle);
	float GetYaw(const Vec3& dir);
	float Dot(const Vec3& a, const Vec3& b);
	bool InFOV(const Vec3& forward, const Vec3& dirToTarget, float fovDeg);
	float RotateTowards(float current, float target, float speed, float dt);
	float LengthSq(const Vec3& v);
}

