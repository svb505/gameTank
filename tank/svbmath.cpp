#include "svbmath.h"
#include <cmath>

namespace svbmath{
    Vec3 Normalize(const Vec3& v){
        float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        return { v.x / len, v.y / len, v.z / len };
    }

    Vec3 DirectionToTarget(const Vec3& enemy, const Vec3& target){
        return Normalize({ target.x - enemy.x,target.y - enemy.y,target.z - enemy.z });
    }
	float NormalizeAngle(float angle) {
		while (angle > PI)  angle -= 2.0f * PI;
		while (angle < -PI) angle += 2.0f * PI;
		return angle;
	}
	float GetYaw(const Vec3& dir) {
		return atan2(dir.x, dir.z); 
	}
	float Dot(const Vec3& a, const Vec3& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	bool InFOV(const Vec3& forward, const Vec3& dirToTarget, float fovDeg) {
		float cosFov = cos(fovDeg * 0.5f * PI / 180.0f);
		return Dot(forward, dirToTarget) > cosFov;
	}
	float RotateTowards(float current, float target, float speed, float dt) {
		float diff = target - current;

		while (diff > PI) diff -= 2 * PI;
		while (diff < -PI) diff += 2 * PI;

		float step = speed * dt;

		if (fabs(diff) <= step)
			return target; // дошли до цели
		return current + (diff > 0 ? step : -step);
	}
	float LengthSq(const Vec3& v) {
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}

}

