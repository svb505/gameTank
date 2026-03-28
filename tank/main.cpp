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
#include "replenishmentAmmo.h"
#include "minimap.h"
#include "lightning.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GUI.h"
#include "artillery.h"

#define COUNT 55
#define ECRANW 1600
#define ECRANH 1000

Camera cam;
Tank playerTank;
Sound sound;
HUD hud;
Replishment repl;
MiniMap mnMap;
Light light;
GUI gui;
Artillery art;

void processTankInput(GLFWwindow* window, float dt,ProjectileSystem& projectileSystem){
    float moveSpeed = 6.0f * dt;
    float rotateSpeed = 60.0f * dt;
    float turretSpeed = 60.0f * dt;
    float gunSpeed = 30.0f * dt;
    float bodyRad = (playerTank.bodyYaw + 90.0f) * 3.1415926f / 180.0f;
    float dirX = -sin(bodyRad);
    float dirZ = -cos(bodyRad);
    static bool lastShift = false;
    static bool lastFire = false;
    bool fire = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
        playerTank.selectedShell = shellType::APFSDS;
        playerTank.shellSpeed = 400;
        playerTank.finishReload = playerTank.reloadTime;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
        playerTank.selectedShell = shellType::HE;
        playerTank.shellSpeed = 100;
        playerTank.finishReload = playerTank.reloadTime;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        playerTank.selectedShell = shellType::SMOKE;
        playerTank.shellSpeed = 100;
        playerTank.finishReload = playerTank.reloadTime;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
       playerTank.bodyYaw += rotateSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        playerTank.bodyYaw -= rotateSpeed;
    }
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
        if (!playerTank.aimMode) cam.cameraYaw -= turretSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        playerTank.turretYaw -= turretSpeed;
        if (!playerTank.aimMode) cam.cameraYaw += turretSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        playerTank.gunPitch = std::max(playerTank.gunPitch - gunSpeed, -10.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        playerTank.gunPitch = std::min(playerTank.gunPitch + gunSpeed, 10.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        mnMap.setHeight(mnMap.getHeight() + mnMap.step * dt * 60);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        mnMap.setHeight(mnMap.getHeight() - mnMap.step * dt * 60);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        cam.cameraYaw += 45.0f * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        cam.cameraYaw -= 45.0f * dt;
    }
        
    if (shift && !lastShift) playerTank.aimMode = !playerTank.aimMode;
    lastShift = shift;

    if (fire && !lastFire && playerTank.finishReload <= 0.0f && playerTank.totalShells > 0) {
        float yaw = playerTank.bodyYaw + playerTank.turretYaw;

        projectileSystem.spawnShell(playerTank.x,playerTank.y + 1.6f,playerTank.z,yaw,playerTank.gunPitch,
            playerTank.selectedShell,playerTank.shellSpeed);
        
        sound.setSourcePosition(sound.shotSource, playerTank.x, playerTank.y + 1.6f, playerTank.z);
        alSourceStop(sound.shotSource);
        alSourcePlay(sound.shotSource);

        --playerTank.totalShells;

        playerTank.finishReload = playerTank.reloadTime; 
    }
    lastFire = fire;

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        float yaw = playerTank.bodyYaw + playerTank.turretYaw;

        projectileSystem.spawnBullet(playerTank.x,playerTank.y + 1.0f,playerTank.z,yaw);

        sound.setSourcePosition(sound.mgunSource, playerTank.x, playerTank.y + 1.6f, playerTank.z);
        alSourceStop(sound.mgunSource);
        alSourcePlay(sound.mgunSource);
    }
}
void countFps(double& deltaTime,double& lastTime,double& currentTime,int& frames,float& fps,float& fpsTimer) {
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    fpsTimer += deltaTime;
    frames++;

    if (fpsTimer >= 1.0f) {
        fps = frames / fpsTimer;
        frames = 0;
        fpsTimer = 0.0f;
    }
}
int main(){
    srand(time(NULL));

    sound.setupBuffers();
    sound.createSources();

    int width, height, channels;
    unsigned char* pixels = stbi_load("icon.png", &width, &height, &channels, 4);


    if (!glfwInit()){
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(ECRANW,ECRANH, "Tank", NULL, NULL);
    if (!window){
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

    light.initLighting();

    generateEnemyes(enemyes,COUNT);
    repl.setCoordinates(10.0f, static_cast<float>(rand() % 30),static_cast<float>((rand() % 50) - 50));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGui::StyleColorsDark();

    while (!glfwWindowShouldClose(window)){
        double currentTime = glfwGetTime();

        countFps(deltaTime,lastTime,currentTime,frames,fps,fpsTimer);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        gui.render(fps, playerTank,art,sound);

        ImGui::Render();

        float radYaw = cam.cameraYaw * 3.14159265f / 180.0f;
        float radPitch = cam.angle * 3.14159265f / 180.0f;
        float fx = cos(radPitch) * sin(radYaw);
        float fy = sin(radPitch);
        float fz = -cos(radPitch) * cos(radYaw);

        sound.setListener(cam.cameraX, cam.cameraY, cam.cameraZ, fx, fy, fz);

        if (playerTank.finishReload > 0.0f) playerTank.finishReload -= deltaTime;

        processTankInput(window, (float)deltaTime, projectileSystem);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawSky();
        cam.setupCamera(playerTank, playerTank.aimMode);
        drawGround(cam.cameraX, cam.cameraZ);

        playerTank.Draw();
        repl.drawReplCircle(30);

        Update(deltaTime,playerTank); // Update enemyes
        Render(smokes); // Render enemyes

        if (repl.isInCircle(playerTank.x, playerTank.z)) repl.startReplish(deltaTime,playerTank,ECRANH,ECRANW);

        art.updateShells(deltaTime);
        art.drawAllShells();
        art.deleteIfAlived();

        projectileSystem.update((float)deltaTime,sound,enemyes,healths, bounds,explosions,smokes,sound.explosionSource);
        projectileSystem.updateProjectiles(projectileSystem);
        projectileSystem.updateArtillery(art.shells,sound,enemyes,explosions,smokes,sound.artExplosionSource);
        
        updateExplosions(explosions, deltaTime);
        updateSmokes(smokes, deltaTime);
        
        hud.Draw3DAim(playerTank);

        mnMap.draw(ECRANW, ECRANH, playerTank, projectileSystem, explosions, smokes, cam, deltaTime);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
