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
#include "random.h"
#include "sounds.h"

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
		p.fallSpeed = (p.type == Type::rainly) ? 45.0f * generateFloat() : 20.0f * generateFloat();

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