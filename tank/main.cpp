#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <ctime>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>

#include "artillery.h"
#include "camera.h"
#include "tank.h"
#include "text.h"
#include "enemyes.h"
#include "environnement.h"
#include "effects.h"
#include "projectileSystem.h"
#include "HUD.h"
#include "sounds.h"
#include "smokeGranade.h"
#include "replenishmentAmmo.h"
#include "minimap.h"
#include "lightning.h"
#include "Logger.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GUI.h"
#include "weather.h"
#include "killchat.h"
#include "input.h"
#include "database.h"
#include "glfw_oglSetup.h"
#include "variables.h"
#include "cameraShake.h"
#include "gameBorder.h"

#define COUNT 55

Camera cam;
Tank tank;
Sound sound;
Replishment repl;
GUI gui;
Artillery art;
Weather weat;
SmokeGranade granades;
CameraShake camShake;

void windowCloseCallback(GLFWwindow* window) {
    if (!dbIsExists) createDb(TypeDb::Player,"playerInfo.db");
    saveDataForPlayer(tank.getKills(), tank.getDeath(), tank.getScore(), "+");
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent((float)xpos, (float)ypos);

    if (io.WantCaptureMouse || cursorVisibility) return;

    static bool first = true;
    static double lastX = 0.0;
    static double lastY = 0.0;

    if (first) {
        lastX = xpos;
        lastY = ypos;
        first = false;
        return;
    }

    double xoffset = xpos - lastX;
    double yoffset = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cam.cameraYaw += (float)xoffset;
    cam.cameraPitch -= (float)yoffset;

    tank.getTurretYaw() -= (float)xoffset;
    tank.getGunPitch() += (float)yoffset;

    cam.cameraYaw = fmod(cam.cameraYaw + 360.0f, 360.0f);

    tank.getTurretYaw() = fmod(tank.getTurretYaw() + 360.0f, 360.0f);

    tank.getGunPitch() = std::clamp(tank.getGunPitch(), -10.0f, 10.0f);
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

    srand((unsigned)time(nullptr));

    sound.setupBuffers();
    sound.createSources();

    int width, height, channels;
    unsigned char* pixels = stbi_load("icon.png", &width, &height, &channels, 4);

    if (!glfwInit()){
        LOG_ERROR("Failed to initialize GLFW");
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 16);
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

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    setMatrix(cam.left, cam.right, cam.bottom, cam.top, cam.nearPlane, cam.farPlane);

    std::unordered_map<int, Entity> enemyes;

    std::vector<ExplosionEffect*> explosions;
    std::vector<SmokeEffect*> smokes;

    EffectsContext context{ explosions, smokes };
    RayContext ray{ debugRay, drawDebugRay, lastHitID, lastHitDist };

    ECSCompenents components{ enemyes,healths,bounds };

    TrackBuffer leftTrack;
    TrackBuffer rightTrack;

    double lastTime = glfwGetTime();
    double deltaTime = 0.0;
    float fpsTimer = 0.0f, fps = 0.0f;
    int frames = 0;

    initLighting();
    LoadAllTextures();
    
    granades.reservePlace();
    reserveMessages();
    reserveProjectiles();

    granades.spawn(tank);
    generateEnemyes(enemyes,COUNT);
    repl.setCoordinates(10.0f, static_cast<float>(rand() % 30),static_cast<float>((rand() % 50) - 50));

    gui.setup(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetWindowCloseCallback(window, windowCloseCallback);

    while (!glfwWindowShouldClose(window)){
        double currentTime = glfwGetTime();

        if (fpsLimit) glfwSwapInterval(1);
        else glfwSwapInterval(0);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        gui.render(fps, tank, art, sound, weat.getWeather(), granades, badges, enemyes, tank.getTurretLocked());

        ImGui::Render();

        sound.setListener(cam.cameraPos, cam.returnForwardVector());

        weat.getWeather(sound, cam);
  
        countFps(deltaTime,lastTime,currentTime,frames,fps,fpsTimer);

        processTankInput(window, deltaTime, enemyes,tank,sound,cam,ray,granades,camShake);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cam.setupCamera(tank, camShake,tank.getAimMode());

        drawSky();
        drawGround(cam.cameraPos.x, cam.cameraPos.z,weat.getWeather());

        tank.Draw();
        tank.updatePosition(tank.getCurrentPos(), deltaTime);
        tank.updateDirrections(tank.getBodyRad(), tank.getBodyYaw());
        tank.UpdateTrack(deltaTime, tank.getCurrentPos(), leftTrack, rightTrack);
        tank.DrawTrack(leftTrack, rightTrack, 0.3f);

        if (tank.getFinishReload() > 0.0f) tank.getFinishReload() -= deltaTime;
        if (tank.getMoveSpeed() > 0.0f) tank.getMoveSpeed() *= tank.getReductionCoef();

        checkBorders(tank,deltaTime, ECRANW, ECRANH);

        repl.drawReplCircle(30);//replishement ammo

        Update(deltaTime,tank,sound); // Update enemyes
        Render(smokes); // Render enemyes

        weat.update(cam,deltaTime); //Update rain/snow
        weat.renderSnowPiles();
        weat.draw();

        updateKillChat(deltaTime); 
        showKillChat(ECRANW, ECRANH); 

        drawHUD(ECRANW,ECRANH,lastHitDist);

        showDestroyText(deltaTime);

        if (repl.isInCircle(tank.getCurrentPos().x, tank.getCurrentPos().z)) repl.startReplish(deltaTime, tank, ECRANH, ECRANW);

        art.updateShells(deltaTime);
        art.drawAllShells();
        art.deleteIfAlived();

        //Update projectiles
        update(deltaTime, sound, art.getShells(), components, context, tank, camShake);
        drawProjectiles();
        
        updateExplosions(explosions, deltaTime);
        updateSmokes(smokes, deltaTime);
        
        camShake.Update(deltaTime);

        granades.update(deltaTime,smokes,tank,sound);
        granades.drawAll(tank);

        auto tankCollision = checkCollisionWithTank(tank.getCurrentPos());

        //Collision with enemy
        if (tankCollision.checked) { 
            tank.getCurrentPos() = tank.getOldPos();

            if (healths.contains(tankCollision.id)) {
                healths[tankCollision.id].current -= tank.returnImpactImpulse();
            }

            sound.setSourcePosition(sound.sources["Collision"], tank.getCurrentPos());
            alSourcePlay(sound.sources["Collision"]);
        }

        Draw3DAim(tank);

        drawBorders();

        drawMiniMap(ECRANW, ECRANH, tank, context, cam, weat.getWeather(), badges, deltaTime);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    gui.destroy();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}