//
// Created by pbialas on 25.09.2020.
//

#define STB_IMAGE_IMPLEMENTATION  1

#include "3rdParty/stb/stb_image.h"
#include "app.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include <iostream>
#include <vector>
#include <tuple>
#include "glm/gtc/matrix_transform.hpp"
#include "Application/utils.h"

#include "Engine/Mesh.h"
#include "Engine/ColorMaterial.h"
#include "Engine/texture.h"

//Textures ModifierBlock
struct ModifierBlock {
    glm::vec4 Kd;
    GLboolean use_map_Kd;
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

void SimpleShapeApplication::init() {
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }
    // Usuwanie powtarzających się wierzchołków
    std::vector<GLushort> indices = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};

    // Wierzchołki i kolory piramidy
    std::vector<GLfloat> vertices = {

            // Czerwień
            -0.5f, -0.5f, -0.5f,    0.191f, 0.5f,
            0.0f, 0.5f, 0.0f,       0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,     0.5f, 0.809f,

            // Kobalt
            -0.5f, -0.5f, 0.5f,     0.5f, 0.809f,
            0.0f, 0.5f, 0.0f,       1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,    0.809f, 0.5f,

            // Zieleń
            0.5f, -0.5f, -0.5f,     0.5f, 0.191f,
            0.0f, 0.5f, 0.0f,       0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,      0.191f, 0.5f,

            // Żółty
            0.5f, -0.5f, 0.5f,      0.809f, 0.5f,
            0.0f, 0.5f, 0.0f,       1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,     0.5f, 0.191f,

            // Szary_1
            -0.5f, -0.5f, -0.5f,    0.191f, 0.5f,
            0.5f, -0.5f, -0.5f,     0.5f, 0.809f,
            -0.5f, -0.5f, 0.5f,     0.5f, 0.191f,

            // Szary_2
            0.5f, -0.5f, 0.5f,      0.809f, 0.5f,
            -0.5f, -0.5f, 0.5f,     0.5f, 0.809f,
            0.5f, -0.5f, -0.5f,     0.5f, 0.191f,
    };

    //MeshesMaterials
    auto texture = create_texture(std::string(ROOT_DIR) + "/Models/multicolor.png");

    xe::ColorMaterial::init();

    auto pyramid = new xe::Mesh;

    pyramid->allocate_vertex_buffer(vertices.size() * sizeof(GLfloat), GL_STATIC_DRAW);
    pyramid->load_vertices(0, vertices.size() * sizeof(GLfloat), vertices.data());
    pyramid->vertex_attrib_pointer(0, 3, GL_FLOAT, 5 * sizeof(GLfloat), 0);
    pyramid->vertex_attrib_pointer(1, 2, GL_FLOAT, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));

    pyramid->allocate_index_buffer(indices.size() * sizeof(GLfloat), GL_STATIC_DRAW);
    pyramid->load_indices(0, indices.size() * sizeof(GLfloat), indices.data());

    pyramid->add_submesh(0, 3, new xe::ColorMaterial({1.0f, 1.0f, 1.0f, 1.0f}, texture, 1) );
    pyramid->add_submesh(3, 6, new xe::ColorMaterial({1.0f, 1.0f, 1.0f, 1.0f}, texture, 2) );
    pyramid->add_submesh(6, 9, new xe::ColorMaterial({1.0f, 1.0f, 1.0f, 1.0f}, texture, 3) );
    pyramid->add_submesh(9, 12, new xe::ColorMaterial({1.0f, 1.0f, 1.0f, 1.0f}, texture, 4) );
    pyramid->add_submesh(12, 18, new xe::ColorMaterial({1.0f, 1.0f, 1.0f, 1.0f}, texture, 5) );
    add_submesh(pyramid);

    //Textures
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(program);

    uniform_map_Kd_location_ = glGetUniformLocation(program, "map_Kd");

    ModifierBlock modifierData;
    modifierData.Kd = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    modifierData.use_map_Kd = GL_TRUE;

    if (texture > 0) {
        OGL_CALL(glUniform1i(uniform_map_Kd_location_, 0));
        modifierData.use_map_Kd = GL_TRUE;
    }

    //Modyfikowanie koloru pikseli
    float strength=0.6;
    float color[3]={0.7,0.6,0.7};

    // Resizing
    auto[w, h] = frame_buffer_size();
    float aspect = (float)w/h;
    float fov = glm::pi<float>()/4.0;
    float near = 0.1f;
    float far = 100.0f;

    //Zoom
    set_camera(new Camera);
    M_ =  glm::mat4(1.0f);
    camera_ -> look_at(glm::vec3(10.6, -7.0, 6.6), glm::vec3(-0.5, 0.1, 0.0), glm::vec3(0.0, 0.3, -0.25));
    camera_ -> perspective(fov, aspect,near,far);

    //Camera Movement
    set_controler(new CameraControler(camera()));

    GLuint v_buffer_handle_uniforms;
    glGenBuffers(1, &v_buffer_handle_uniforms);
    glBindBuffer(GL_UNIFORM_BUFFER, v_buffer_handle_uniforms);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(GLfloat), nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_UNIFORM_BUFFER, 0,sizeof(float),&strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 4*sizeof(float),3*sizeof(float),color);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // PVM
    glGenBuffers(1, &u_pvm_buffer_);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_));
    glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindVertexArray(0);

    //Generowanie bufora i ładowanie do niego danych wierzchołków
    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    //Bufor indeksów
    GLuint v_buffer_handle_indices;
    glGenBuffers(1, &v_buffer_handle_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v_buffer_handle_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLfloat), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3*sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBufferBase(GL_UNIFORM_BUFFER,0,v_buffer_handle_uniforms);
    glBindBufferBase(GL_UNIFORM_BUFFER,1,u_pvm_buffer_);

    // Przezroczyste tło
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    glViewport(0, 0, w, h);

    glUseProgram(program);
}

void SimpleShapeApplication::frame() {

    //Pyramid
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    //MeshesMaterials
    for (auto m: meshes_) {
        m->draw();
    }
    // Binding the VAO - skonfiguruje wszystkie wymagane bufory wierzchołków
    auto PVM = camera_->projection()*camera_->view()*M_;
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //Rysowanie za pomocą indeksów
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES , 18, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid *>(0));
    glBindVertexArray(0);
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
