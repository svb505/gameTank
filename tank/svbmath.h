#pragma once

constexpr float PI = 3.14159265358979323846f;

namespace svbmath {
	struct Vec3 {
		float x, y, z;
		
		Vec3() : x(0), y(0), z(0) {}
		Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

		Vec3 operator+(const Vec3& v) const { return { x + v.x, y + v.y, z + v.z }; }
		Vec3 operator-(const Vec3& v) const { return { x - v.x, y - v.y, z - v.z }; }
		Vec3 operator*(float s) const { return { x * s, y * s, z * s }; }
		Vec3 operator/(float s) const { return { x / s, y / s, z / s }; }
	};
	struct Vec2 {
		float x, y;

		Vec2() : x(0), y(0) {}
		Vec2(float x, float y) : x(x), y(y) {}

		Vec2 operator+(const Vec2& v) const { return { x + v.x, y + v.y}; }
		Vec2 operator-(const Vec2& v) const { return { x - v.x, y - v.y}; }
		Vec2 operator*(float s) const { return { x * s, y * s}; }
		Vec2 operator/(float s) const { return { x / s, y / s}; }
	};

    Vec3 Normalize(const Vec3& v);
	Vec3 DirectionToTarget(const Vec3& enemy, const Vec3& target);
	Vec3 Cross(const Vec3& a, const Vec3& b);
	Vec3 SteerTowards(const Vec3& currentDir, const Vec3& targetDir, float maxAngle);

	float randFloat(float min, float max);
	float NormalizeAngle(float angle);
	float GetYaw(const Vec3& dir);
	float Dot(const Vec3& a, const Vec3& b);
	float RotateTowards(float current, float target, float speed, double dt);
	float LengthSq(const Vec3& v);
	float Length(const Vec3& v);
	bool InFOV(const Vec3& forward, const Vec3& dirToTarget, float fovDeg);
	bool InRadius(const svbmath::Vec3& childPos, const svbmath::Vec3 parentPos, float radius);
}

