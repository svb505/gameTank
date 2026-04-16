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

#define COUNT 55
#define ECRANW 1600
#define ECRANH 1000

Camera cam;
Tank tank;
Sound sound;
Replishment repl;
GUI gui;
Artillery art;
Weather weat;
SmokeGranade granades;

int lastHitID = -1;
static float lastHitDist = 0.0f;

bool firstMouse = true;
bool cursorVisibility = false;
bool fpsLimit = false;

static Ray debugRay;
static bool drawDebugRay = false;

double lastX = 800.0 / 2, lastY = 600.0 / 2;
float sensitivity = 0.1f;

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

    tank.turretYaw -= (float)xoffset;
    tank.gunPitch += (float)yoffset;

    cam.cameraYaw = fmod(cam.cameraYaw + 360.0f, 360.0f);

    tank.turretYaw = fmod(tank.turretYaw + 360.0f, 360.0f);

    tank.gunPitch = std::clamp(tank.gunPitch, -10.0f, 10.0f);
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

    std::vector<ExplosionEffect*> explosions;
    std::vector<SmokeEffect*> smokes;

    EffectsContext context{ explosions, smokes };
    RayContext ray{ debugRay, drawDebugRay, lastHitID, lastHitDist };

    TrackBuffer leftTrack;
    TrackBuffer rightTrack;

    double lastTime = glfwGetTime();
    double deltaTime = 0.0;
    float fpsTimer = 0.0f,fps = 0.0f;
    int frames = 0;
    
    initLighting();
    granades.spawn(tank);
    generateEnemyes(enemyes,COUNT);
    repl.setCoordinates(10.0f, static_cast<float>(rand() % 30),static_cast<float>((rand() % 50) - 50));

    gui.setup(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    while (!glfwWindowShouldClose(window)){
        double currentTime = glfwGetTime();

        if (fpsLimit) glfwSwapInterval(1);
        else glfwSwapInterval(0);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        gui.render(fps, tank, art, sound, weat.weather, granades, badges, fpsLimit, enemyes, tank.turretLocked);

        ImGui::Render();

        sound.setListener(cam.cameraX, cam.cameraY, cam.cameraZ, cam.returnForwardVector());

        weat.getWeather(sound, cam);
  
        countFps(deltaTime,lastTime,currentTime,frames,fps,fpsTimer);

        processTankInput(window, deltaTime, enemyes,tank,sound,cam,ray,granades,cursorVisibility);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawSky();
        cam.setupCamera(tank, tank.aimMode);
        drawGround(cam.cameraX, cam.cameraZ,weat.weather);

        tank.Draw();
        tank.updatePosition(tank.pos,deltaTime);
        tank.updateDirrections(tank.bodyRad, tank.bodyYaw);
        tank.UpdateTrack(deltaTime, tank.pos, leftTrack, rightTrack);
        tank.DrawTrack(leftTrack, rightTrack, 0.3f);

        if (tank.finishReload > 0.0f) tank.finishReload -= deltaTime;
        if (tank.moveSpeed > 0.0f) tank.moveSpeed *= tank.REDUCTION_COEF;

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

        if (repl.isInCircle(tank.pos.x, tank.pos.z)) repl.startReplish(deltaTime,tank,ECRANH,ECRANW);

        art.updateShells(deltaTime);
        art.drawAllShells();
        art.deleteIfAlived();

        //Update projectiles
        update(deltaTime, sound, enemyes, healths, bounds, context, tank);
        updateProjectiles();
        updateArtillery(art.shells,sound,enemyes, context);
        
        updateExplosions(explosions, deltaTime);
        updateSmokes(smokes, deltaTime);
        
        granades.update(deltaTime,smokes,tank,sound);
        granades.drawAll(tank);

        auto tankCollision = checkCollisionWithTank(tank.pos);

        //Collision with enemy
        if (tankCollision.checked) { 
            tank.pos = tank.oldPos;

            if (healths.contains(tankCollision.id)) {
                healths[tankCollision.id].current -= tank.returnImpactImpulse();
            }

            sound.setSourcePosition(sound.sources["Collision"], tank.pos);
            alSourcePlay(sound.sources["Collision"]);
        }

        Draw3DAim(tank);

        drawMiniMap(ECRANW, ECRANH, tank, context, cam, weat.weather,badges,deltaTime);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    gui.destroy();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}