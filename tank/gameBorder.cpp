#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "svbmath.h"
#include "gameBorder.h"
#include "tank.h"
#include "text.h"
#include <format>

svbmath::Vec3 min = {-200,0,-200};
svbmath::Vec3 max = {200,0,200};

float timer = 10.0f;
float stepBorder = 7.0f;
float Y = 50.0f;

void drawBorders() {
	for (int x = min.x; x < max.x; x += stepBorder) {
		glColor3f(1.0f, 0.0f, 0.0f);
		
		glBegin(GL_LINES);

		glVertex3f(x,Y,min.z);
		glVertex3f(x + stepBorder,0.0f,min.z);

		glVertex3f(x, Y, max.z);
		glVertex3f(x + stepBorder, 0.0f, max.z);

		glEnd();
	}
	for (int z = min.z; z < max.z; z += stepBorder) {
		glColor3f(1.0f, 0.0f, 0.0f);

		glBegin(GL_LINES);

		glVertex3f(min.x, Y, z);
		glVertex3f(min.x + stepBorder, 0.0f, z);

		glVertex3f(max.x, Y, z);
		glVertex3f(max.x + stepBorder, 0.0f, z);

		glEnd();
	}
}
void checkBorders(Tank& tank,float dt,int ScrH,int ScrW) {
	svbmath::Vec3 pos = tank.getCurrentPos();

	if (pos.x <= min.x || pos.x >= max.x || pos.z <= min.z || pos.z >= max.z) {
		if (timer > 0.0f) {
			timer -= dt;

			std::string str = std::format("Quit the danger zone in {:.0f} s", timer);

			RenderTextHUD(ScrW / 2, ScrH / 2, 1.0f, 0.0f, 0.0f, str.c_str(), ScrW, ScrH);
		}
		else {
			timer = 10.0f;

			tank.respawn(tank.getSelectedSpawn());
		}

		
	}
}