//
// Created by pbialas on 05.08.2020.
//

#pragma once

#include <vector>
#include <memory>
#include "Application/application.h"
#include "Application/utils.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glad/gl.h"
#include "camera.h"
#include "cameracontroler.h"
#include "Engine/Mesh.h"
#include "Engine/lights.h"


class SimpleShapeApplication : public xe::Application
{
public:

    SimpleShapeApplication(int width, int height, std::string title, bool debug) : Application(width, height, title, debug) {}

    void set_camera(Camera *camera) { camera_ = camera; }
    Camera *camera() const { return camera_; }

    void init() override;

    void frame() override;

    void framebuffer_resize_callback(int w, int h) override;

    void scroll_callback(double xoffset, double yoffset) override;

    void set_controler(CameraControler *controler) { controler_ = controler; }

    void cursor_position_callback(double x, double y);

    void mouse_button_callback(int button, int action, int mods);

    void add_submesh(xe::Mesh *mesh) {
        meshes_.push_back(mesh);
    }
    void add_light(const PointLight &p_light) {
        p_lights_.push_back(p_light);
    }

    void add_ambient(glm::vec3 ambient) {
        ambient_ = ambient;
    }
private:
    GLuint vao_;

    glm::mat4 M_;
    GLuint u_pvm_buffer_;

    Camera *camera_;
    CameraControler *controler_;

    GLuint color_uniform_buffer_ = 0u;
    GLint  uniform_map_Kd_location_ = 0;

    GLuint Matrices;
    GLuint Lights;

    std::vector<xe::Mesh*> meshes_;
    std::shared_ptr<xe::Mesh> mesh;

    glm::vec3 ambient_;
    std::vector<PointLight> p_lights_;
};