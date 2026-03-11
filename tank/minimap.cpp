#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include "environnement.h"
#include "effects.h"
#include "tank.h"
#include "projectileSystem.h"
#include "camera.h"
#include "shells.h"
#include "enemyes.h"
#include "minimap.h"

void drawMiniMap(int ECRANW, int ECRANH, Tank& playerTank, ProjectileSystem& projectileSystem,
    std::vector<ExplosionEffect*>& explosions, std::vector<SmokeEffect*>& smokes,Camera& cam,
    float dt) {
    int miniW = ECRANW / 4;
    int miniH = ECRANH / 4;

    glViewport(ECRANW - miniW, 10, miniW, miniH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-40, 40, -40, 40, -100, 200);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(
        playerTank.x, 80.0f, playerTank.z,
        playerTank.x, 0.0f, playerTank.z,
        0, 0, -1
    );

    glClear(GL_DEPTH_BUFFER_BIT);

    drawGround(playerTank.x, playerTank.z);
    playerTank.Draw();

    for (auto& p : projectileSystem.projectiles) {
        if (!p.alive) continue;

        if (p.type == ProjectileType::Shell) drawShell();
        else drawBullet();
    }
    for (auto it = explosions.begin(); it != explosions.end(); ++it) (*it)->Draw();
    for (auto it = smokes.begin(); it != smokes.end(); ++it) (*it)->Draw();

    Render(smokes);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, miniW, 0, miniH, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);

    glBegin(GL_LINE_LOOP);
    glVertex2f(0, 0);
    glVertex2f(miniW, 0);
    glVertex2f(miniW, miniH);
    glVertex2f(0, miniH);
    glEnd();

    glViewport(0, 0, ECRANW, ECRANH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(cam.fov, (float)ECRANW / ECRANH, cam.nearPlane, cam.farPlane);
}
