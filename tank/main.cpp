#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include <ctime>
#include <format>
#include <GL/glu.h>
#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include "tank.h"
#include "projectileSystem.h"
#include "Info.h"
#include "text.h"
#include "enemyes.h"
#include "environnement.h"
#include "effects.h"
#include "HUD.h"
#include "camera.h"
#include "sounds.h"
#include "shells.h"

#define COUNT 55
#define ECRANW 1600
#define ECRANH 1000

Camera cam;
Tank playerTank;
Sound sound;
HUD hud;

void processTankInput(GLFWwindow* window, float dt,ProjectileSystem& projectileSystem)
{
    float moveSpeed = 6.0f * dt;
    float rotateSpeed = 60.0f * dt;
    float turretSpeed = 60.0f * dt;
    float gunSpeed = 30.0f * dt;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
        playerTank.selectedShell = shellType::APFSDS;
        playerTank.shellSpeed = 400;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
        playerTank.selectedShell = shellType::HE;
        playerTank.shellSpeed = 100;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        playerTank.selectedShell = shellType::SMOKE;
        playerTank.shellSpeed = 100;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
       playerTank.bodyYaw += rotateSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        playerTank.bodyYaw -= rotateSpeed;
    }
        

    float bodyRad = (playerTank.bodyYaw + 90.0f) * 3.1415926f / 180.0f;
    float dirX = -sin(bodyRad);
    float dirZ = -cos(bodyRad);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        playerTank.x += dirX * moveSpeed;
        playerTank.z += dirZ * moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        playerTank.x -= dirX * moveSpeed;
        playerTank.z -= dirZ * moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        playerTank.turretYaw += turretSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        playerTank.turretYaw -= turretSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        playerTank.gunPitch = std::max(playerTank.gunPitch - gunSpeed, -10.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        playerTank.gunPitch = std::min(playerTank.gunPitch + gunSpeed, 10.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        cam.cameraYaw += 45.0f * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        cam.cameraYaw -= 45.0f * dt;
    }
        

    static bool lastShift = false;
    bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
    if (shift && !lastShift)
        playerTank.aimMode = !playerTank.aimMode;
    lastShift = shift;

    static bool lastFire = false;
    bool fire = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    if (fire && !lastFire && playerTank.finishReload <= 0.0f && playerTank.totalShells > 0) {
        float yaw = playerTank.bodyYaw + playerTank.turretYaw;

        projectileSystem.spawnShell(playerTank.x,playerTank.y + 1.6f,playerTank.z,
            yaw,playerTank.gunPitch,playerTank.selectedShell,playerTank.shellSpeed);

        alSourceStop(sound.shootSource);
        alSourcePlay(sound.shootSource);

        --playerTank.totalShells;

        playerTank.finishReload = playerTank.reloadTime; 
    }
    lastFire = fire;

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        float yaw = playerTank.bodyYaw + playerTank.turretYaw;

        projectileSystem.spawnBullet(playerTank.x,playerTank.y + 1.0f,playerTank.z,yaw);

        alSourceStop(sound.mgunSource);
        alSourcePlay(sound.mgunSource);
    }
}
int main(){
    sound.setupBuffers();
    sound.createSources();

    int width, height, channels;
    unsigned char* pixels = stbi_load("icon.png", &width, &height, &channels, 4);


    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(ECRANW,ECRANH, "Tank", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    if (pixels) {
        GLFWimage icon{ width, height, pixels };
        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(pixels);
    }
    
    glfwMakeContextCurrent(window);
    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);;
    BuildFont();

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(cam.left, cam.right, cam.bottom, cam.top, cam.nearPlane, cam.farPlane);

    glMatrixMode(GL_MODELVIEW);

    std::unordered_map<int, Entity> enemyes;
    std::vector<int> toDelete;
    std::vector<ExplosionEffect*> explosions;
    std::vector<SmokeEffect*> smokes;
    ProjectileSystem projectileSystem;
    
    double lastTime = glfwGetTime();
    double deltaTime = 0.0;
    float fpsTimer = 0.0f;
    int frames = 0;
    float fps = 0.0f;

    generateEnemyes(enemyes,COUNT);

    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        fpsTimer += deltaTime;
        frames++;

        if (fpsTimer >= 1.0f) {
            fps = frames / fpsTimer;
            frames = 0;
            fpsTimer = 0.0f;
        }


        if (playerTank.finishReload > 0.0f) playerTank.finishReload -= deltaTime;

        processTankInput(window, (float)deltaTime, projectileSystem);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawSky();
        cam.setupCamera(playerTank, playerTank.aimMode);
        drawGround(cam.cameraX, cam.cameraZ);
        playerTank.Draw();

        Update(deltaTime);
        Render(smokes);

        projectileSystem.update((float)deltaTime,enemyes,healths, bounds,explosions,smokes,sound.explosionSource);

        for (auto& p : projectileSystem.projectiles) {
            if (!p.alive) continue;

            glPushMatrix();
            glTranslatef(p.x, p.y, p.z);

            if (p.type == ProjectileType::Shell) drawShell();
            else drawBullet();
               
            glPopMatrix();
        }
        for (auto it = explosions.begin(); it != explosions.end();) {
            (*it)->Update(deltaTime);
            (*it)->Draw();

            if ((*it)->IsFinished()) {
                delete* it;
                it = explosions.erase(it);
            }
            else ++it;
        }
        for (auto& smoke : smokes) { smoke->Update(deltaTime); smoke->Draw();}


        hud.Draw3DAim(playerTank);

        glDisable(GL_DEPTH_TEST);
        hud.drawHud(ECRANW, ECRANH, playerTank,55,fps);
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
