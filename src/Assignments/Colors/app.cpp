//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>

#include "Application/utils.h"
void SimpleShapeApplication::init() {
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER, std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    std::vector<GLfloat> vertices = {
            // Triangle 1 vertices
            -0.5f, 0.0f, 0.0f,  // Wierzchołek 1: pozycja
            1.0f, 0.0f, 0.0f,   // Wierzchołek 1: kolor czerwony

            0.5f, 0.0f, 0.0f,   // Wierzchołek 2: pozycja
            1.0f, 0.0f, 0.0f,   // Wierzchołek 2: kolor czerwony

            0.0f, 0.5f, 0.0f,   // Wierzchołek 3: pozycja
            1.0f, 0.0f, 0.0f,   // Wierzchołek 3: kolor czerwony

            // Triangle 2 vertices
            -0.5f, -0.5f, 0.0f, // Wierzchołek 1: pozycja
            0.0f, 1.0f, 0.0f,   // Wierzchołek 1: kolor zielony

            0.5f, 0.0f, 0.0f,   // Wierzchołek 2: pozycja
            0.0f, 1.0f, 0.0f,   // Wierzchołek 2: kolor zielony

            -0.5f, 0.0f, 0.0f,  // Wierzchołek 3: pozycja
            0.0f, 1.0f, 0.0f,   // Wierzchołek 3: kolor zielony

            // Triangle 3 vertices
            -0.5f, -0.5f, 0.0f, // Wierzchołek 1: pozycja
            0.0f, 1.0f, 0.0f,   // Wierzchołek 1: kolor zielony

            0.5f, -0.5f, 0.0f,  // Wierzchołek 2: pozycja
            0.0f, 1.0f, 0.0f,   // Wierzchołek 2: kolor zielony

            0.5f, 0.0f, 0.0f,   // Wierzchołek 3: pozycja
            0.0f, 1.0f, 0.0f    // Wierzchołek 3: kolor zielony
    };

    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);

    // Konfiguracja atrybutów wierzchołków
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    auto [w, h] = frame_buffer_size();
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

void SimpleShapeApplication::frame() {
    glBindVertexArray(vao_);

    // Draw all triangles at once
    glDrawArrays(GL_TRIANGLES, 0, 9);

    glBindVertexArray(0);
}
