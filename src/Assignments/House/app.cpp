//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>

#include "Application/utils.h"

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them, and creates the program object
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER, std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    // A vector containing the x, y, z vertex coordinates for the triangles.
    std::vector<GLfloat> vertices = {
            // Triangle 1 vertices
            -0.5f, 0.0f, 0.0f,
            0.5f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f,

// Triangle 2 vertices
            -0.5f, -0.5f, 0.0f,
            0.5f, 0.0f, 0.0f,
            -0.5f, 0.0f, 0.0,

// Triangle 3 vertices
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.5f, 0.0f, 0.0f,

    };

    // Generating the buffer and loading the vertex data into it.
    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle));
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // This setups a Vertex Array Object (VAO) that encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);

    // Enable the attribute for vertex coordinates
    glEnableVertexAttribArray(0);
    // Configure how the data is laid out in the buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //end of vao "recording"

    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debugging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL viewport of the size of the whole rendering window.
    auto[w, h] = frame_buffer_size();
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

// This function is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    // Binding the VAO will set up all the required vertex buffers.
    glBindVertexArray(vao_);

    // Draw the first triangle
    glDrawArrays(GL_TRIANGLES, 0, 9);

    // Draw the second triangle
    glDrawArrays(GL_TRIANGLES, 3, 3);

    // Draw the third triangle
    glDrawArrays(GL_TRIANGLES, 6, 3);


    glBindVertexArray(0);
}
