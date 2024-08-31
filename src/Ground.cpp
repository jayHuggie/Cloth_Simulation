#include "Ground.h"

#define EPSILON 1e-2f

// Constructor: Initializes the ground plane with specified top left corner, size, and shader program.
Ground::Ground(glm::vec3 topleft, float size, GLuint _programID)
{
    programID = _programID; // Shader program used for rendering.

    // Translate the model down by a small epsilon to avoid z-fighting with objects resting exactly on ground level.
    model = glm::translate(glm::vec3(0.0f, topleft.y - EPSILON, 0.0f));

    // Set up vertices for the ground plane. This forms a square ground plane.
    points.push_back(topleft);
    points.push_back(topleft + glm::vec3(size, 0.0f, 0.0f)); // Point to the right.
    points.push_back(topleft + glm::vec3(0.0f, 0.0f, size)); // Point forward.
    points.push_back(topleft + glm::vec3(size, 0.0f, size)); // Point diagonally opposite to the start.

    // Normals for each vertex pointing upwards, since this is a flat ground.
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // All normals are the same.
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

    // Define two triangles that make up the square ground plane using indices.
    indices.push_back(glm::ivec3(0, 2, 3)); // First triangle.
    indices.push_back(glm::ivec3(0, 3, 1)); // Second triangle.

    // OpenGL setup for rendering the ground.
    glGenVertexArrays(1, &vao); // Generate a VAO.
    glGenBuffers(2, vbos); // Generate VBOs for vertex positions and normals.
    glGenBuffers(1, &ebo); // Generate an EBO for indices.

    // Bind the VAO.
    glBindVertexArray(vao);

    // Setup the first VBO (vertex positions).
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]); // Bind the positions VBO.
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(), points.data(), GL_STATIC_DRAW); // Upload position data.
    glEnableVertexAttribArray(0); // Enable vertex attribute 0.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0); // Define vertex data layout.

    // Setup the second VBO (vertex normals).
    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]); // Bind the normals VBO.
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW); // Upload normal data.
    glEnableVertexAttribArray(1); // Enable vertex attribute 1.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0); // Define normal data layout.

    // Setup the EBO (element buffer object for indices).
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Bind the EBO.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(), indices.data(), GL_STATIC_DRAW); // Upload index data.

    // Unbind from the VBO and VAO to prevent accidental modification.
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the VBO.
    glBindVertexArray(0); // Unbind the VAO.
}

// Destructor: Cleans up OpenGL resources.
Ground::~Ground()
{
    // Delete OpenGL buffers and the VAO.
    glDeleteBuffers(2, vbos);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

// SetGroundLevel: Adjusts the ground plane's vertical position.
void Ground::SetGroundLevel(float level)
{
    // Adjust the model matrix to translate the ground to the new level, accounting for EPSILON.
    model = glm::translate(glm::vec3(0.0f, level - EPSILON, 0.0f));
}

// Draw: Renders the ground plane using the specified view-projection matrix.
void Ground::Draw(const glm::mat4& viewProjMtx)
{
    // Use the shader program for rendering.
    glUseProgram(programID);

    // Define a color for the ground plane.
    glm::vec3 color(1.0f, 0.2f, 0.2f);

    // Pass uniforms to the shader program.
    glUniformMatrix4fv(glGetUniformLocation(programID, "viewProj"), 1, GL_FALSE, (float*)&viewProjMtx);
    glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, (float*)&model);
    glUniform3fv(glGetUniformLocation(programID, "DiffuseColor"), 1, &color[0]);

    // Bind the VAO for rendering.
    glBindVertexArray(vao);

    // Draw the ground using triangles according to the indices defined in the EBO.
    glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, 0);

    // Unbind the VAO and shader program to cleanup.
    glBindVertexArray(0);
    glUseProgram(0);
}
