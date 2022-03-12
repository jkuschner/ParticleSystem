//
//  main.cpp
//  hw1
//
//  Created by Baichuan Wu on 12/16/21.
//

#include <Cube.hpp>
#include "Scene.hpp"
#include "Shader.hpp"
#include "Spawner.hpp"
#include "core.hpp"
#include "utils.hpp"
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl3.h"

const int width = 1200;
const int height = 900;

bool actPan = false, actPitchYaw = false, actZoom = false;
int mouseX = 0, mouseY = 0;

bool wireframe_mode = false;

Scene* scene;
Spawner* spawner;
std::map<std::string, Shader*> shaders;

// adjustable parameters
float iPos_x, iPos_y, iPos_z;
float vPos_x, vPos_y, vPos_z;
float iVel_x, iVel_y, iVel_z;
float vVel_x, vVel_y, vVel_z;
float grav;
float spawnRate;
int iLifespan;
float vLifespan;
float air, drag, rest, fric;


void initialize() {
    glClearColor(0.0f, 0.0f, 0.0f, 1);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    shaders.insert(std::make_pair("Workbench", new Shader("Workbench", "shaders/shader.vs", "shaders/shader.fs")));
    shaders.insert(std::make_pair("Grid", new Shader("Grid", "shaders/grid.vs", "shaders/grid.fs")));

    // MARK: Example code to load from obj file
    /*
        const char* objFile = "assets/cube.obj";
        scene = loadFromObj("Scene", objFile);
    */

    scene = new Scene("Scene");
    spawner = new Spawner();

    iPos_x = 0.0f; iPos_y = 0.0f; iPos_z = 0.0f;
    vPos_x = 2.0f; vPos_y = 2.0f; vPos_z = 2.0f;
    iVel_x = 0.0f; iVel_y = 20.0f; iVel_z = 0.0f;
    vVel_x = 1.5f; vVel_y = 5.0f; vVel_z = 1.5f;
    grav = -6.5f;
    spawnRate = 10.0f;
    iLifespan = 100;
    vLifespan = 10.0f;
    air = AIR_DENSITY;
    drag = DRAG_COFF;
    rest = RESTITUTION;
    fric = FRICTION_COFF;
}


/// This is the display call back.
void display_callback() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (wireframe_mode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    /*
     MARK: Draw Call
     For each frame, the draw() method of current scene is called once.
     */
    scene->draw(shaders["Workbench"]);

    // Draw the grid
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    scene->drawGrid(shaders["Grid"]);

    
    // set up and render ImGui window
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();

    //display code
    
    ImGui::Begin("System Parameters");
    ImGui::SliderFloat("spawn point x", &iPos_x, -10.0f, 10.0f);
    ImGui::SliderFloat("spawn point y", &iPos_y, 0.0f, 10.0f);
    ImGui::SliderFloat("spawn point z", &iPos_z, -10.0f, 10.0f);
    ImGui::SliderFloat("spawn variance x", &vPos_x, 0.0f, 10.0f);
    ImGui::SliderFloat("spawn variance y", &vPos_y, 0.0f, 10.0f);
    ImGui::SliderFloat("spawn variance z", &vPos_z, 0.0f, 10.0f);
    ImGui::SliderFloat("init velocity x", &iVel_x, -20.0f, 20.0f);
    ImGui::SliderFloat("init velocity y", &iVel_y, -20.0f, 20.0f);
    ImGui::SliderFloat("init velocity z", &iVel_z, -20.0f, 20.0f);
    ImGui::SliderFloat("velocity variance x", &vVel_x, 0.0f, 20.0f);
    ImGui::SliderFloat("velocity variance y", &vVel_y, 0.0f, 20.0f);
    ImGui::SliderFloat("velocity variance z", &vVel_z, 0.0f, 20.0f);
    ImGui::SliderFloat("gravity", &grav, -20.0f, 20.0f);
    ImGui::SliderFloat("spawn rate", &spawnRate, 0.0f, 100.0f);
    ImGui::SliderInt("lifetime", &iLifespan, 0, 100);
    ImGui::SliderFloat("lifetime variance", &vLifespan, 0.0f, 100.0f);
    ImGui::SliderFloat("air density", &air, 0.0f, 5.0f);
    ImGui::SliderFloat("drag coeficcient", &drag, 0.0f, 5.0f);
    ImGui::SliderFloat("restitution(bounciness)", &rest, 0.0f, 1.0f);
    ImGui::SliderFloat("friction coeficcient", &fric, 0.0f, 1.0f);
    ImGui::End();
    

    // Rendering
    ImGui::Render();
    //ImGuiIO& io = ImGui::GetIO();
    //glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    //glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    //glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    

    glutSwapBuffers();
}


/// The idle call back.
void idle_callback() {
    // MARK: Perform any background tasks here
    // CALL glutPostRedisplay() if the work changes what's displayed on screen
    spawner->update();
    spawner->changeParameters(iPos_x, iPos_y, iPos_z, vPos_x, vPos_y, vPos_z, iVel_x, iVel_y, iVel_z, vVel_x, vVel_y, vVel_z, grav, spawnRate, iLifespan, vLifespan, air, drag, rest, fric);
    scene->objects.clear();

    for (Cube* cube : spawner->sprites()) {
        scene->objects[cube->name] = cube;
    }

    /*
    for (const auto& object: scene->objects) {
        object.second->update();
    }
    */
    glutPostRedisplay();
}


/// Keystroke callback.
void handle_keypress(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // ESC
            exit(0);
            break;
        case 'r':
            scene->camera->reset();
            break;
        case 'w':
            wireframe_mode = !wireframe_mode;
            break;

        //
        // MARK: Add your custom keystroks here.
        //
        default:
            return;
    }
    glutPostRedisplay();
}


/// Mouse button callback
void handle_mouse_click(int button, int state, int x, int y) {
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);

    GLuint mod = glutGetModifiers();
    switch (button) {
        case GLUT_LEFT_BUTTON:
            actPan = (mod & GLUT_ACTIVE_SHIFT) && (state == GLUT_DOWN);
            actPitchYaw = !actPan && (state == GLUT_DOWN);
            break;
        case GLUT_RIGHT_BUTTON:
            actZoom = (state == GLUT_DOWN);
            break;
        default:
            break;
    }
    mouseX = x; mouseY = y;
}

/// Called when the mouse is moving with at least one button pressed
void handle_mouse_drag(int x, int y) {
    ImGui_ImplGLUT_MotionFunc(x, y);
    const int dmax = 100;
    const int dx = glm::clamp(x - mouseX, -dmax, dmax);
    const int dy = glm::clamp(y - mouseY, -dmax, dmax);
    
    mouseX = x; mouseY = y;
    
    if (actPan) {
        const float panRate = .0075f;
        scene->camera->offsetPivot(glm::vec2(-dx, dy) * panRate);
    }
    if (actPitchYaw) {
        const float pitchYawRate = .25f;
        scene->camera->setAzimuth(scene->camera->getAzimuth() + dx * pitchYawRate);
        scene->camera->setIncline(glm::clamp(scene->camera->getIncline() + dy * pitchYawRate, -90.f, 90.f));
    }
    if (actZoom) {
        const float zoomRate = .005f;
        float d = glm::clamp(scene->camera->getDistance() * (1.f - dx * zoomRate), .01f, 1000.f);
        scene->camera->setDistance(d);
    }
    
    scene->camera->update();
    glutPostRedisplay();
}


void cleanup() {
    if (scene) { delete scene; }
    shaders.clear();
}


int main(int argc, char * argv[]) {
    // Initialize glue window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Viewer");
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    initialize();

    /// see link: https://www.opengl.org/resources/libraries/glut/spec3/node46.html
    glutDisplayFunc(display_callback);
    glutIdleFunc(idle_callback);
    //ImGui stuff goes here
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGLUT_Init();
    ImGui_ImplGLUT_InstallFuncs();
    ImGui_ImplOpenGL3_Init();

    glutKeyboardFunc(handle_keypress);
    glutMouseFunc(handle_mouse_click);

    glutMotionFunc(handle_mouse_drag);
    glutMainLoop();

    //ImGui Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
    cleanup();
    return 0;
}
