#pragma once
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include "text.h"
#include "tank.h"

constexpr float myPI = 3.1415926f;

class Replishment {
private:
	float replishTime = 5.0f;
public:
	float x, z, radius;

	void setCoordinates(float _radius, float _x, float _z) { radius = _radius; x = _x; z = _z; }
	void drawReplCircle(int segments) {
		glPushMatrix();
		glTranslatef(x, 0.5f, z);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINE_LOOP);

		for (int i = 0; i < segments; i++) {
			float angle = 2.0f + myPI * i / segments;
			float x = radius * cosf(angle);
			float z = radius * sinf(angle);

			glVertex3f(x, 0.0f, z);
		}
		glPopMatrix();
		glEnd();

		RenderTextWorld(x,3.0f,z,1,1,1,"Replish ammo");
	}
	bool isInCircle(float tX, float tZ) {
		float dx = tX - x;
		float dz = tZ - z;

		return (dx * dx + dz * dz) <= (radius * radius);
	}
	void startReplish(float dt,Tank& tank,int wH,int wW) {
		if (tank.totalShells < tank.maxShells) {
			replishTime -= dt;
			RenderTextHUD((float)wW / 2, (float)wH / 2, 1, 1, 1, std::format("Replish ammo: {:.1f} s", replishTime).c_str(),wW,wH);
			if (replishTime <= 0) { tank.totalShells++; replishTime = 5.0f; }
		}
	}
};

