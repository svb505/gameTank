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
#include "Logger.h"
#include "weather.h"

#define COUNT 55
#define ECRANW 1600
#define ECRANH 1000

Camera cam;
Tank tank;
Sound sound;
HUD hud;
Replishment repl;
MiniMap mnMap;
Light light;
GUI gui;
Artillery art;
Weather weat;

bool firstMouse = true;
double lastX = 800.0 / 2, lastY = 600.0 / 2;
float sensitivity = 0.2f;
bool cursorVisibility = false;
bool fpsLimit = false;
std::string weather = "Clean";

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent((float)xpos, (float)ypos); 

    if (io.WantCaptureMouse || cursorVisibility) return;

    if (firstMouse) {
        lastX = xpos;
        lastY = -ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cam.cameraYaw += static_cast<float>(xoffset);
    cam.cameraPitch += static_cast<float>(yoffset);

    if (cam.cameraYaw > 360.0f) cam.cameraYaw -= 360.0f;
    if (cam.cameraYaw < 0.0f)   cam.cameraYaw += 360.0f;
}
void processTankInput(GLFWwindow* window, float dt,ProjectileSystem& projectileSystem){
    static bool lastShift = false;
    static bool lastFire = false;
    static bool prevCtrl = false;
    static bool prevAlt = false;

    bool fire = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    bool currCtrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
    bool currAlt = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
    bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
        tank.selectedShell = shellType::APFSDS;
        tank.shellSpeed = 400;
        tank.finishReload = tank.reloadTime;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
        tank.selectedShell = shellType::HE;
        tank.shellSpeed = 100;
        tank.finishReload = tank.reloadTime;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        tank.selectedShell = shellType::SMOKE;
        tank.shellSpeed = 100;
        tank.finishReload = tank.reloadTime;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
       tank.bodyYaw += tank.rotateSpeed * dt;
       tank.moveSpeed *= tank.REDUCTION_COEF;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        tank.bodyYaw -= tank.rotateSpeed * dt;
        tank.moveSpeed *= tank.REDUCTION_COEF;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        tank.oldX = tank.x; tank.oldY = tank.y; tank.oldZ = tank.z;
        if (tank.moveSpeed <= tank.SPEED_LIMIT_FORWARD) tank.moveSpeed += tank.VELOCITY_COEF;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        tank.oldX = tank.x; tank.oldY = tank.y; tank.oldZ = tank.z;
        if (tank.moveSpeed >= tank.SPEED_LIMIT_BACK) tank.moveSpeed -= tank.VELOCITY_COEF / 2;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        tank.turretYaw += tank.turretSpeed * dt;
        if (!tank.aimMode) cam.cameraYaw -= tank.turretSpeed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        tank.turretYaw -= tank.turretSpeed * dt;
        if (!tank.aimMode) cam.cameraYaw += tank.turretSpeed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        tank.gunPitch = std::max(tank.gunPitch - tank.gunSpeed * dt, -10.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        tank.gunPitch = std::min(tank.gunPitch + tank.gunSpeed * dt, 10.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        mnMap.setHeight(mnMap.getHeight() + mnMap.step * dt * 60);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        mnMap.setHeight(mnMap.getHeight() - mnMap.step * dt * 60);
    }
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        float yaw = tank.bodyYaw + tank.turretYaw;

        projectileSystem.spawnBullet(tank.x, tank.y + 1.0f, tank.z, yaw);

        sound.setSourcePosition(sound.mgunSource, tank.x, tank.y + 1.6f, tank.z);
        alSourceStop(sound.mgunSource);
        alSourcePlay(sound.mgunSource);
    }
    if (currAlt && !prevAlt) {
        if (cursorVisibility) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            cursorVisibility = false;
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cursorVisibility = true;
        }
    }
    if (shift && !lastShift) tank.aimMode = !tank.aimMode;
    if (fire && !lastFire && tank.finishReload <= 0.0f && tank.totalShells > 0) {
        float yaw = tank.bodyYaw + tank.turretYaw;

        projectileSystem.spawnShell(tank.x,tank.y + 1.6f,tank.z,yaw,tank.gunPitch,
            tank.selectedShell,tank.shellSpeed);
        
        sound.setSourcePosition(sound.shotSource, tank.x, tank.y + 1.6f, tank.z);
        alSourceStop(sound.shotSource);
        alSourcePlay(sound.shotSource);

        --tank.totalShells;

        tank.finishReload = tank.reloadTime; 
    }
    if (currCtrl && !prevCtrl) {
        cam.zoomed = !cam.zoomed;
        cam.fov = cam.zoomed ? 40.0f : 70.0f;
    }

    lastFire = fire;
    prevCtrl = currCtrl;
    lastShift = shift;
    prevAlt = currAlt;
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
    Logger::initLogger();

    srand(time(NULL));

    sound.setupBuffers();
    sound.createSources();

    int width, height, channels;
    unsigned char* pixels = stbi_load("icon.png", &width, &height, &channels, 4);

    if (!glfwInit()){
        LOG_ERROR("Failed to initialize GLFW");
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(ECRANW,ECRANH, "Tank", NULL, NULL);
    if (!window){
        LOG_ERROR("Failed to create window");
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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    while (!glfwWindowShouldClose(window)){
        double currentTime = glfwGetTime();

        if (fpsLimit) glfwSwapInterval(1);
        else glfwSwapInterval(0);

        if (weather == "Clean") { 
            if (weat.particles.size() > 0) { 
                alSourceStop(sound.rainSource); 
                sound.rainPlayed = false;
                weat.particles.clear(); 
            } 
        }
        else if (weather == "Rainly") { 
            if (!sound.rainPlayed) { 
                alSourcePlay(sound.rainSource); 
                sound.rainPlayed = true; 
            } 
            if (weat.particles.size() < weat.count) weat.generate(Type::rainly, cam); 
        }
        else if (weather == "Snowly") { 
            alSourceStop(sound.rainSource); 
            sound.rainPlayed = false;
            if (weat.particles.size() < weat.count) weat.generate(Type::snowly, cam); }

        countFps(deltaTime,lastTime,currentTime,frames,fps,fpsTimer);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        gui.render(fps, tank,art,sound,weather,fpsLimit);

        ImGui::Render();

        float radYaw = cam.cameraYaw * 3.14159265f / 180.0f;
        float radPitch = cam.angle * 3.14159265f / 180.0f;
        float fx = cos(radPitch) * sin(radYaw);
        float fy = sin(radPitch);
        float fz = -cos(radPitch) * cos(radYaw);

        sound.setListener(cam.cameraX, cam.cameraY, cam.cameraZ, fx, fy, fz);

        if (tank.finishReload > 0.0f) tank.finishReload -= deltaTime;
        if (tank.moveSpeed > 0.0f) tank.moveSpeed *= tank.REDUCTION_COEF;

        processTankInput(window, (float)deltaTime, projectileSystem);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawSky();
        cam.setupCamera(tank, tank.aimMode);
        drawGround(cam.cameraX, cam.cameraZ,weather);

        tank.Draw();
        tank.updatePosition(tank.x,tank.z,deltaTime);

        repl.drawReplCircle(30);//replishement ammo

        Update(deltaTime,tank,projectileSystem,sound); // Update enemyes
        Render(smokes); // Render enemyes

        weat.update(cam,deltaTime);
        weat.draw();

        if (repl.isInCircle(tank.x, tank.z)) repl.startReplish(deltaTime,tank,ECRANH,ECRANW);

        tank.updateDirrections(tank.bodyRad,tank.bodyYaw);

        art.updateShells(deltaTime);
        art.drawAllShells();
        art.deleteIfAlived();

        projectileSystem.update((float)deltaTime,sound,enemyes,healths, bounds,explosions,smokes,tank);
        projectileSystem.updateProjectiles(projectileSystem);
        projectileSystem.updateArtillery(art.shells,sound,enemyes,explosions,smokes);
        
        updateExplosions(explosions, deltaTime);
        updateSmokes(smokes, deltaTime);
        
        auto tankCollision = checkCollisionWithTank(tank.x, tank.y, tank.z);

        if (tankCollision.checked) { //Collision with enemy
            tank.x = tank.oldX; tank.y = tank.oldY; tank.z = tank.oldZ;

            healths[tankCollision.id].current -= tank.returnImpactImpulse();

            sound.setSourcePosition(sound.collisionSource, tank.x, tank.y, tank.z);
            alSourcePlay(sound.collisionSource);
        }

        hud.Draw3DAim(tank);

        mnMap.draw(ECRANW, ECRANH, tank, projectileSystem, explosions, smokes, cam, weather,deltaTime);

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
