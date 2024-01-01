//
// Created by pbialas on 25.09.2020.
//

#include "app.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include <iostream>
#include <vector>
#include <tuple>
#include "glm/gtc/matrix_transform.hpp"
#include "Application/utils.h"

// Przewijanie myszką
void SimpleShapeApplication::scroll_callback(double xoffset, double yoffset) {
    Application::scroll_callback(xoffset, yoffset);
    camera()->zoom(yoffset / 30.0f);
}
// Camera Zoom
void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0,0,w,h);
    camera_->set_aspect((float) w / h);
}

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }
    // Usuwanie powtarzających się wierzchołków
    std::vector<GLushort> indices = {0,3,2 , 1,3,0 , 4,5,6 , 7,8,9 , 10,11,12 , 13,14,15};

    // Wierzchołki i kolory piramidy
    std::vector<GLfloat> vertices = {
            0.0f, 0.0f, 0.0f, 0.0f,1.0f,0.0f,//0
            1.0f, 0.0f, 0.0f, 0.0f,1.0f,0.0f,//1
            0.0f, 1.0f, 0.0f, 0.0f,1.0f,0.0f,//2
            1.0f, 1.0f, 0.0f, 0.0f,1.0f,0.0f,//3

            0.0f, 0.0f, 0.0f, 1.0f,0.0f,0.0f,
            0.0f, 1.0f, 0.0f, 1.0f,0.0f,0.0f,
            0.5f, 0.5f, 1.0f, 1.0f,0.0f,0.0f,//6

            0.0f, 1.0f, 0.0f, 0.0f,0.0f,1.0f,
            1.0f, 1.0f, 0.0f, 0.0f,0.0f,1.0f,
            0.5f, 0.5f, 1.0f, 0.0f,0.0f,1.0f,//9

            1.0f, 1.0f, 0.0f, 0.5f,0.0f,0.5f,
            1.0f, 0.0f, 0.0f, 0.5f,0.0f,0.5f,
            0.5f, 0.5f, 1.0f, 0.5f,0.0f,0.5f,//12

            1.0f, 0.0f, 0.0f, 0.0f,0.5f,0.5f,
            0.0f, 0.0f, 0.0f, 0.0f,0.5f,0.5f,
            0.5f, 0.5f, 1.0f, 0.0f,0.5f,0.5f}; //15

    // Modyfikowanie koloru pikseli
    float strength=0.6;
    float color[3]={0.7,0.6,0.7};

    //Resizing
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

    // Generowanie bufora i ładowanie do niego danych wierzchołków
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

    // Camera Zoom
    auto PVM = camera_->projection()*camera_->view()*M_;
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //Pyramid
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Binding the VAO - skonfiguruje wszystkie wymagane bufory wierzchołków
    glBindVertexArray(vao_);

    //Rysowanie za pomocą indeksów
    glDrawElements(GL_TRIANGLES , 18, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid *>(0));
    glBindVertexArray(0);
}