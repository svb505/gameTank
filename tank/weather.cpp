#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "weather.h"
#include "camera.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include "sounds.h"
#include "svbmath.h"

void Weather::drawSphere(float radius, int stacks, int slices) {
	for (int i = 0; i < stacks; i++) {
		float lat0 = PI * (-0.5f + (float)i / stacks);
		float lat1 = PI * (-0.5f + (float)(i + 1) / stacks);

		float y0 = sinf(lat0);
		float y1 = sinf(lat1);

		float r0 = cosf(lat0);
		float r1 = cosf(lat1);

		glBegin(GL_QUAD_STRIP);

		for (int j = 0; j <= slices; j++) {
			float lon = 2.0f * PI * (float)j / slices;

			float x = cosf(lon);
			float z = sinf(lon);

			glVertex3f(radius * x * r0, radius * y0, radius * z * r0);
			glVertex3f(radius * x * r1, radius * y1, radius * z * r1);
		}

		glEnd();
	}
}
void Weather::drawSnowPile(float radius, float height) {
	float c = svbmath::randFloat(0.85f, 1.0f);
	glColor3f(c, c, c);

	glPushMatrix();
	glScalef(radius, height, radius);

	drawSphere(1.0f, 16, 16);

	glPopMatrix();
}
void Weather::draw() {
	for (auto& p : particles) {
		if (p.type == Type::rainly) {
			glPushMatrix();
			glTranslatef(p.x, p.y, p.z);

			glColor3f(51.0f / 255.0f, 153.0f / 255.0f, 255.0f / 255.0f);

			glLineWidth(2.0f);
			glBegin(GL_LINES);

			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(0.0f, -1.0f, 0.0f);

			glEnd();
			glPopMatrix();
		}
		if (p.type == Type::snowly) {
			glPushMatrix();
			glTranslatef(p.x, p.y, p.z);

			glColor3f(1.0f, 1.0f, 1.0f);

			glPointSize(3.0f);
			glBegin(GL_POINTS);

			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);

			glEnd();
			glPopMatrix();
		}
	}
}
void Weather::generate(const Type& type,Camera& cam) {
	int toGenerate = std::min<int>(count - static_cast<int>(particles.size()), 10);

	for (int i = 0; i < toGenerate; i++) {
		Particle p;

		p.type = type;

		p.x = cam.cameraX + (rand() % int(diapazone * 2)) - diapazone;
		p.y = cam.cameraY + p.height;
		p.z = cam.cameraZ + (rand() % int(diapazone * 2)) - diapazone;

		p.vx = 0.0f; p.vz = 0.0f;
		p.fallSpeed = (p.type == Type::rainly) ? 45.0f * svbmath::randFloat(0.5f,1.0f) : 20.0f * svbmath::randFloat(0.5f, 1.0f);

		p.vy = -p.fallSpeed;

		particles.push_back(p);
	}
}
void Weather::update(Camera& cam,float dt) {
	for (auto& p : particles) {
		p.vy -= p.gravity * dt;
		p.y += p.vy * dt;
		
		if (p.y <= 0.0f) {
			p.y = cam.cameraY + p.height;
			p.vy = -p.fallSpeed;
		}
	}
}
void Weather::generateSnowPiles(int count, float areaSize) {
	snowPiles.clear();

	for (int i = 0; i < count; i++) {
		SnowPiles s;

		s.pos.x = svbmath::randFloat(-areaSize, areaSize);
		s.pos.z = svbmath::randFloat(-areaSize, areaSize);
		s.pos.y = 0.0f;

		s.radius = svbmath::randFloat(0.5f, 1.5f);
		s.height = svbmath::randFloat(0.3f, 1.0f);

		snowPiles.emplace_back(s);
	}
}
void Weather::renderSnowPiles() {
	for (const auto& s : snowPiles) {
		glPushMatrix();
		glTranslatef(s.pos.x, s.pos.y, s.pos.z);

		drawSnowPile(s.height,s.height);

		glPopMatrix();
	}
}
