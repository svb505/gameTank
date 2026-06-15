#pragma once
#include "svbmath.h"

class CameraShake{
private:
    float m_duration = 0.0f;
    float m_remainingTime = 0.0f;
    float m_intensity = 0.0f;

    svbmath::Vec3 m_offset = { 0.0f,0.0f,0.0f };
public:
    void Start(float duration, float intensity);
    void Update(float deltaTime);

    svbmath::Vec3 GetOffset() const;
};