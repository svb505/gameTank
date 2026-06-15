#pragma once
#include "svbmath.h"
#include "cameraShake.h"

svbmath::Vec3 CameraShake::GetOffset() const{
    return m_offset;
}
void CameraShake::Start(float duration, float intensity){
    m_duration = duration;
    m_remainingTime = duration;
    m_intensity = intensity;
}

void CameraShake::Update(float deltaTime){
    if (m_remainingTime <= 0.0f){
        m_offset = svbmath::Vec3(0.0f);
        return;
    }

    m_remainingTime -= deltaTime;

    float t = m_remainingTime / m_duration;
    float currentIntensity = m_intensity * t;

    m_offset.x = svbmath::randFloat(-1.0f, 1.0f) * currentIntensity;
    m_offset.y = svbmath::randFloat(-1.0f, 1.0f) * currentIntensity;
    m_offset.z = svbmath::randFloat(-1.0f, 1.0f) * currentIntensity * 0.25f;
}