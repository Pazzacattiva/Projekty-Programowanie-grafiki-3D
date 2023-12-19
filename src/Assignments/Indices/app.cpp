//
// Created by pbialas on 25.09.2020.
//
#include <cstring>
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
            -0.5f, -0.5f, 0.0f, // Wierzchołek 4: pozycja
            0.0f, 1.0f, 0.0f,   // Wierzchołek 4: kolor zielony

            0.5f, 0.0f, 0.0f,   // Wierzchołek 5: pozycja
            0.0f, 1.0f, 0.0f,   // Wierzchołek 5: kolor zielony

            -0.5f, 0.0f, 0.0f,  // Wierzchołek 6: pozycja
            0.0f, 1.0f, 0.0f,   // Wierzchołek 6: kolor zielony

            // Triangle 3 vertices
            -0.5f, -0.5f, 0.0f, // Wierzchołek 7: pozycja
            0.0f, 1.0f, 0.0f,   // Wierzchołek 7: kolor zielony

            0.5f, -0.5f, 0.0f,  // Wierzchołek 8: pozycja
            0.0f, 1.0f, 0.0f,   // Wierzchołek 8: kolor zielony

            0.5f, 0.0f, 0.0f,   // Wierzchołek 9: pozycja
            0.0f, 1.0f, 0.0f    // Wierzchołek 9: kolor zielony
    };

    // Usuwanie powtarzających się wierzchołków
    std::vector<GLushort> indices = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint i_buffer_handle;
    glGenBuffers(1, &i_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);

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

    // Rysowanie za pomocą indeksów
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_SHORT, nullptr);

    glBindVertexArray(0);
}
