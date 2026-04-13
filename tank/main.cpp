#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
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
#include "smokeGranade.h"
#include "craters.h"
#include "rangefinder.h"
#include "killchat.h"

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
SmokeGranade granades;

int lastHitID = -1;
static float lastHitDistance = 0.0f;

bool firstMouse = true;
bool cursorVisibility = false;
bool fpsLimit = false;
bool badges = false;

static Ray debugRay;
static bool drawDebugRay = false;

double lastX = 800.0 / 2, lastY = 600.0 / 2;
float sensitivity = 0.1f;

std::string weather = "Clean";

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
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

    tank.turretYaw -= (float)xoffset;
    tank.gunPitch += (float)yoffset;

    cam.cameraYaw = fmod(cam.cameraYaw + 360.0f, 360.0f);

    tank.turretYaw = fmod(tank.turretYaw + 360.0f, 360.0f);

    tank.gunPitch = std::clamp(tank.gunPitch, -10.0f, 10.0f);
}
void processTankInput(GLFWwindow* window, float dt,ProjectileSystem& projectileSystem, std::unordered_map<int, Entity>& enemyes){
    static bool lastShift = false;
    static bool lastFire = false;
    static bool prevCtrl = false;
    static bool prevAlt = false;
    static bool prevG = false;
    static bool prevR = false;

    bool fire = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    bool currCtrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
    bool currAlt = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
    bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
    bool keyG = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;
    bool keyR = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
        tank.selectedShell = shellType::APFSDS;
        tank.shellSpeed = 400;
        tank.finishReload = tank.reloadTime;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
        tank.selectedShell = shellType::HE;
        tank.shellSpeed = 200;
        tank.finishReload = tank.reloadTime;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        tank.selectedShell = shellType::ATGM;
        tank.shellSpeed = 10;
        tank.finishReload = tank.reloadTime;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        tank.selectedShell = shellType::SMOKE;
        tank.shellSpeed = 200;
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
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        mnMap.setHeight(mnMap.getHeight() + mnMap.step * dt * 60);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        mnMap.setHeight(mnMap.getHeight() - mnMap.step * dt * 60);
    }
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        float yaw = tank.bodyYaw + tank.turretYaw;

        projectileSystem.spawnBullet({ tank.x, tank.y + 1.0f, tank.z }, yaw);

        sound.setSourcePosition(sound.sources["MGun"], tank.x, tank.y + 1.6f, tank.z);
        alSourceStop(sound.sources["MGun"]);
        alSourcePlay(sound.sources["MGun"]);
    }
    if (keyR && !prevR) {
        Ray ray;
        ray.origin = { tank.x, tank.y + 1.6f, tank.z };

        float yawRad = (tank.bodyYaw + tank.turretYaw) * 3.14159265f / 180.0f;
        float pitchRad = tank.gunPitch * 3.14159265f / 180.0f;

        ray.direction = { sin(yawRad) * cos(pitchRad), sin(pitchRad), cos(yawRad) * cos(pitchRad) };

        float maxDist = 300.0f;

        int hitID = -1;
        float hitDistance = Raycast(ray, enemyes, bounds, hitID, maxDist);

        debugRay = ray;
        drawDebugRay = true;

        if (hitID != -1) { 
            lastHitID = hitID;
            lastHitDistance = hitDistance;
        }
        else {
            lastHitID = -1; 
            lastHitDistance = 0.0f;
        }
    }
    if (keyG && !prevG) granades.strike();
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
        float yaw;

        if (tank.turretLocked) yaw = tank.bodyYaw + tank.turretYaw;
        else yaw = tank.turretYaw - 90.0f;

        projectileSystem.spawnShell({ tank.x,tank.y + 1.6f,tank.z }, yaw, tank.gunPitch,
            tank.selectedShell,tank.shellSpeed);
        
        sound.setSourcePosition(sound.sources["Shot"], tank.x, tank.y + 1.6f, tank.z);
        alSourceStop(sound.sources["Shot"]);
        alSourcePlay(sound.sources["Shot"]);

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
    prevG = keyG;
    prevR = keyR;
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

    EffectsContext context{ explosions, smokes };

    ProjectileSystem projectileSystem;
    
    TrackBuffer leftTrack;
    TrackBuffer rightTrack;

    granades.spawn(tank);

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
                alSourceStop(sound.sources["Rain"]);
                sound.rainPlayed = false;
                weat.particles.clear(); 
            } 
            weat.snowPiles.clear();
        }
        else if (weather == "Rainly") { 
            weat.snowPiles.clear();
            if (!sound.rainPlayed) { 
                alSourcePlay(sound.sources["Rain"]);
                sound.rainPlayed = true; 
            } 
            if (weat.particles.size() < weat.count) weat.generate(Type::rainly, cam); 
        }
        else if (weather == "Snowly") { 
            alSourceStop(sound.sources["Rain"]);
            sound.rainPlayed = false;
            if (weat.particles.size() < weat.count) weat.generate(Type::snowly, cam); 
            if (weat.snowPiles.size() == 0) weat.generateSnowPiles(50, 100.0f);
        }
  
        countFps(deltaTime,lastTime,currentTime,frames,fps,fpsTimer);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        gui.render(fps, tank,art,sound,weather,granades,badges,fpsLimit,enemyes,tank.turretLocked);

        ImGui::Render();

        float radYaw = cam.cameraYaw * 3.14159265f / 180.0f;
        float radPitch = cam.angle * 3.14159265f / 180.0f;
        float fx = cos(radPitch) * sin(radYaw);
        float fy = sin(radPitch);
        float fz = -cos(radPitch) * cos(radYaw);

        sound.setListener(cam.cameraX, cam.cameraY, cam.cameraZ, fx, fy, fz);

        if (tank.finishReload > 0.0f) tank.finishReload -= deltaTime;
        if (tank.moveSpeed > 0.0f) tank.moveSpeed *= tank.REDUCTION_COEF;

        processTankInput(window, (float)deltaTime, projectileSystem, enemyes);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawSky();
        cam.setupCamera(tank, tank.aimMode);
        drawGround(cam.cameraX, cam.cameraZ,weather);

        tank.Draw();
        tank.updatePosition(tank.x,tank.z,deltaTime);

        repl.drawReplCircle(30);//replishement ammo

        Update(deltaTime,tank,projectileSystem,sound); // Update enemyes
        Render(smokes); // Render enemyes

        weat.update(cam,deltaTime); //Update rain/snow
        weat.renderSnowPiles();
        weat.draw();

        updateKillChat(deltaTime); 
        showKillChat(ECRANW, ECRANH); 

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        std::string dist = std::format("Distance: {:.1f} m", lastHitDistance);
        RenderTextHUD(ECRANW / 2, ECRANH / 2, 1, 1, 1, dist.c_str(), ECRANW, ECRANH);

        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);

        showDestroyText(deltaTime);

        if (repl.isInCircle(tank.x, tank.z)) repl.startReplish(deltaTime,tank,ECRANH,ECRANW);

        svbmath::Vec3 forward = { tank.dirX, 0.0f, tank.dirZ };
        svbmath::Vec3 up = { 0, 1, 0 };
        svbmath::Vec3 tankRight = svbmath::Normalize(svbmath::Cross(forward, up));

        tank.updateDirrections(tank.bodyRad,tank.bodyYaw);
        tank.UpdateTrack(deltaTime,{tank.x,tank.y,tank.z},tankRight,leftTrack,rightTrack);
        tank.DrawTrack(leftTrack, 0.3f);
        tank.DrawTrack(rightTrack, 0.3f);

        art.updateShells(deltaTime);
        art.drawAllShells();
        art.deleteIfAlived();

        projectileSystem.update((float)deltaTime, sound, enemyes, healths, bounds, context, tank);
        projectileSystem.updateProjectiles(projectileSystem);
        projectileSystem.updateArtillery(art.shells,sound,enemyes, context);
        
        updateExplosions(explosions, deltaTime);
        updateSmokes(smokes, deltaTime);
        
        granades.update(deltaTime,smokes,tank,sound);
        granades.drawAll(tank);

        auto tankCollision = checkCollisionWithTank(tank.x, tank.y, tank.z);

        //Collision with enemy
        if (tankCollision.checked) { 
            tank.x = tank.oldX; tank.y = tank.oldY; tank.z = tank.oldZ;

            if (healths.contains(tankCollision.id)) {
                healths[tankCollision.id].current -= tank.returnImpactImpulse();
            }

            sound.setSourcePosition(sound.sources["Collision"], tank.x, tank.y, tank.z);
            alSourcePlay(sound.sources["Collision"]);
        }

        hud.Draw3DAim(tank);

        mnMap.draw(ECRANW, ECRANH, tank, projectileSystem, explosions, smokes, cam, weather,badges,deltaTime);

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
