//
// Created by pbialas on 25.09.2020.
//

#define STB_IMAGE_IMPLEMENTATION  1

#include "3rdParty/stb/stb_image.h"
#include "app.h"
#include "camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include <iostream>
#include <vector>
#include <tuple>
#include "glm/gtc/matrix_transform.hpp"
#include "Application/utils.h"
#include "Engine/mesh_loader.h"
#include "Engine/Mesh.h"
#include "Engine/Material.h"
#include "Engine/ColorMaterial.h"
#include "Engine/PhongMaterial.h"
#include "Engine/lights.h"


//Textures ModifierBlock
struct ModifierBlock {
    glm::vec4 Kd;
    GLboolean use_map_Kd;
};

//Phong
struct LightBlock {
    PointLight light[24];
    int num_lights;
};

struct MatricesBlock {
    glm::mat4 VM;
    glm::mat3 N;
};

// Przewijanie myszką
void SimpleShapeApplication::scroll_callback(double xoffset, double yoffset) {
    Application::scroll_callback(xoffset, yoffset);
    camera()->zoom(yoffset / 30.0f);
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0,0,w,h);
    camera_->set_aspect((float) w / h);
}

//Phong
void SimpleShapeApplication::init() {

    xe::ColorMaterial::init();
    xe::PhongMaterial::init();

    mesh = xe::load_mesh_from_obj(std::string(ROOT_DIR) + "/Models/square.obj", std::string(ROOT_DIR) + "/Models");

    add_submesh(mesh.get());

    auto light_1 = new PointLight({0.0, 0.5, 0.5}, {0.0, 1.0, 0.0}, {0.0, 0.0, 0.0});
    auto light_2 = new PointLight({-0.5, -0.5, 0.5}, {1.0, 0.0, 1.0}, {0.0, 0.0, 0.0});
    auto light_3 = new PointLight({0.5, -0.5, 0.5}, {0.0, 0.0, 1.0}, {0.0, 0.0, 0.0});

    add_light(*light_1);
    add_light(*light_2);
    add_light(*light_3);

    // Resizing
    auto [w, h] = frame_buffer_size();
    float aspect = (float) w / h;
    float fov = glm::pi<float>() / 4.0;
    float near = 0.1f;
    float far = 100.0f;

    //Zoom
    set_camera(new Camera);
    M_ = glm::mat4(1.0f);


    camera_ -> look_at(glm::vec3(10.6, -7.0, 6.6), glm::vec3(-0.5, 0.1, 0.0), glm::vec3(0.0, 0.3, -0.25));


    camera_->perspective(fov, aspect, near, far);

    //Camera Movement
    set_controler(new CameraControler(camera()));

    // PVM
    glGenBuffers(1, &u_pvm_buffer_);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_));
    glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(float), nullptr, GL_STATIC_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &Matrices);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, Matrices));
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);

    glGenBuffers(1, &Lights);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, Lights));
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBlock), nullptr, GL_STATIC_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);


    //Bind PVM
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_pvm_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);

    //Bind Matrices
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, Matrices);
    glBindBuffer(GL_UNIFORM_BUFFER, Matrices);

    //Bind Lights
    glBindBufferBase(GL_UNIFORM_BUFFER, 3, Lights);
    glBindBuffer(GL_UNIFORM_BUFFER, Lights);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Ustawienie Oświetlenia Otoczenia
    xe::PhongMaterial::set_ambient({0.3f, 0.3f, 0.3f});

    // Przezroczyste tło
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    glViewport(0, 0, w, h);

    // Testowanie głębokości i usuwanie twarzy.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void SimpleShapeApplication::frame() {

    auto PVM = camera_->projection()*camera_->view()*M_;
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    auto vm = camera_->view() * glm::mat4(1.0f);
    auto R = glm::mat3(vm);
    auto N = glm::mat3(glm::cross(R[1], R[2]), glm::cross(R[2], R[0]), glm::cross(R[0], R[1]));

    MatricesBlock matricesBlock{};

    matricesBlock.N = N;
    matricesBlock.VM = vm;

    glBindBuffer(GL_UNIFORM_BUFFER, Matrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MatricesBlock), &matricesBlock);

    glBindBuffer(GL_UNIFORM_BUFFER, Lights);

    LightBlock lightBlock{};

    lightBlock.num_lights = p_lights_.size();

    for(int i = 0; i < p_lights_.size(); i++) {
        lightBlock.light[i] = p_lights_[i];
    }
    int lightBlockSize = sizeof(LightBlock);

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBlock), &lightBlock);

    for (auto m: meshes_) {
        m->draw();
    }
}

//Camera Movement
void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    if (controler_) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            controler_->LMB_pressed(x, y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
            controler_->LMB_released(x, y);
    }

}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);
    if (controler_) {
        controler_->mouse_moved(x, y);
    }
}
