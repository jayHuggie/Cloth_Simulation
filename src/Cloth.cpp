#include "Cloth.h"

// Constructor: Sets up the cloth simulation with given parameters.
Cloth::Cloth(float _size, float _mass, int N, glm::vec3 pos, glm::vec3 hori, glm::vec3 vert, GLuint ID)
{
    size = _size; // Total size of the cloth.
    mass = _mass; // Total mass of the cloth.
    numOfParticles = N; // Number of particles along one edge of the cloth.
    topLeftPos = pos; // Top-left position of the cloth in world space.
    horiDir = glm::normalize(hori); // Normalized horizontal direction of the cloth.
    vertDir = glm::normalize(vert); // Normalized vertical direction of the cloth.

    programID = ID; // OpenGL shader program ID for rendering.

    Initialize(); // Call initialization function to set up particles and connections.

    prevT = clock(); // Record the current time for future time step calculations.
}

// Destructor: Cleans up dynamically allocated memory and OpenGL resources.
Cloth::~Cloth()
{
    // Delete all particle, spring damper, and triangle objects.
    for (auto it : particles) { delete it; }
    for (auto it : connections) { delete it; }
    for (auto it : triangles) { delete it; }

    // Delete OpenGL buffers and VAO.
    glDeleteBuffers(2, vbos);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

// Initialize: Sets up the cloth simulation, creating particles, spring dampers, and triangles.
void Cloth::Initialize()
{
    // Calculate the length of each grid cell in the cloth.
    float gridLength = size / (numOfParticles - 1);

    // Reserve space for positions and normals.
    positions.resize(numOfParticles * numOfParticles);
    normals.resize(numOfParticles * numOfParticles);

    // Constants for aerodynamic force calculation.
    FluidDensity = 1.225;
    C_d = 1.0f;
    WindVelocity = glm::vec3(0.5f, 0.5f, -2.5f);

    // Constants for spring damper calculation.
    springConst = 1000.0f;
    dampingConst = 3.5f; 
    restLength = gridLength;
    restLengthDiag = gridLength * sqrt(2.0f); // Diagonal length for diagonal springs.

    // Particle properties.
    particleMass = mass / (numOfParticles * numOfParticles);
    gravityAcce = 2.0f;
    groundPos = 0.0f;

    // Ensure the ground position is below the lowest point of the cloth.
    if (groundPos > topLeftPos.y) groundPos = topLeftPos.y - EPSILON;

    // Create a ground object for collision detection.
    ground = new Ground(glm::vec3(-5.0f, groundPos, -5.0f), 10.0f, programID);

    // Oversampling factor for numerical integration stability.
    numOfOversamples = 20;

    // Calculate the direction normal to the cloth plane.
    glm::vec3 planeDir = glm::cross(horiDir, vertDir);
    planeDir.y = 0.0f; // Ensure it's horizontally aligned.
    planeDir = glm::normalize(planeDir);


    // For calculating particles
    glm::vec3 norm = glm::cross(vertDir, horiDir);
	norm = glm::normalize(norm);

    // Create particles.
    // Loop over each grid cell and initialize particles at grid points.
    // Particles below the ground level are adjusted to sit on an imaginary plane.
    for (int y = 0; y < numOfParticles; y++)
    {
        for (int x = 0; x < numOfParticles; x++)
        {
            // Calculate linear index for storage arrays.
            int i = x + y * numOfParticles;
            // Calculate particle position based on grid coordinates.
            positions[i] = topLeftPos + x * gridLength * horiDir + y * gridLength * vertDir;
            // Adjust positions below ground level.
            if (positions[i].y < groundPos)
            {
                int j = i - numOfParticles; // Previous row index.
                positions[i] = positions[j] + gridLength * planeDir; // Adjust position.
                normals[i] = glm::vec3(0.0f, -1.0f, 0.0f); // Normal pointing down.
            }
            else
            {
                normals[i] = norm; // Normal pointing up for particles above ground.
            }

            // Create a new particle at the calculated position.
            Particle* P = new Particle(&positions[i], &normals[i], &particleMass, &gravityAcce, &groundPos);
            particles.push_back(P); // Add to particle list.
        }
    }

    // Create triangles for aerodynamic force calculation.
    // Triangles are formed by connecting particles in a grid.
    for (int y = 0; y < numOfParticles - 1; y++)
    {
        for (int x = 0; x < numOfParticles - 1; x++)
        {
            // Calculate indices for the corners of each square cell.
            int topLeftIdx = x + y * numOfParticles;
            int topRightIdx = topLeftIdx + 1;
            int bottomLeftIdx = topLeftIdx + numOfParticles;
            int bottomRightIdx = bottomLeftIdx + 1;

            // Define triangles for the square cell.
            indices.push_back(glm::ivec3(topLeftIdx, bottomLeftIdx, bottomRightIdx));
            indices.push_back(glm::ivec3(topLeftIdx, bottomRightIdx, topRightIdx));

            // Create triangle objects for aerodynamic calculations.
            Triangle* tri;
            tri = new Triangle(particles[topLeftIdx], particles[bottomLeftIdx], particles[bottomRightIdx], &FluidDensity, &C_d, &WindVelocity);
            triangles.push_back(tri);
            tri = new Triangle(particles[topLeftIdx], particles[bottomRightIdx], particles[topRightIdx], &FluidDensity, &C_d, &WindVelocity);
            triangles.push_back(tri);
        }
    }

    // Create spring dampers for structural stability.
    // Connects particles with spring dampers along grid edges and diagonals.
    for (int y = 0; y < numOfParticles - 1; y++)
    {
        for (int x = 0; x < numOfParticles - 1; x++)
        {
            // Calculate indices for the corners of each square cell.
            int topLeftIdx = x + y * numOfParticles;
            int topRightIdx = topLeftIdx + 1;
            int bottomLeftIdx = topLeftIdx + numOfParticles;
            int bottomRightIdx = bottomLeftIdx + 1;

            // Create spring dampers for each edge and diagonal.
            SpringDamper* sp;
            sp = new SpringDamper(particles[topLeftIdx], particles[topRightIdx], &springConst, &dampingConst, &restLength);
            connections.push_back(sp);
            sp = new SpringDamper(particles[topLeftIdx], particles[bottomRightIdx], &springConst, &dampingConst, &restLengthDiag);
            connections.push_back(sp);
            sp = new SpringDamper(particles[topLeftIdx], particles[bottomLeftIdx], &springConst, &dampingConst, &restLength);
            connections.push_back(sp);
            sp = new SpringDamper(particles[bottomLeftIdx], particles[topRightIdx], &springConst, &dampingConst, &restLengthDiag);
            connections.push_back(sp);

            // Add additional dampers for the last column and row edges.
            if (x == numOfParticles - 2)
            {
                sp = new SpringDamper(particles[topRightIdx], particles[bottomRightIdx], &springConst, &dampingConst, &restLength);
                connections.push_back(sp);
            }
            if (y == numOfParticles - 2)
            {
                sp = new SpringDamper(particles[bottomLeftIdx], particles[bottomRightIdx], &springConst, &dampingConst, &restLength);
                connections.push_back(sp);
            }
        }
    }

    // Set fixed points along one edge of the cloth to simulate attachment.
    for (int i = 0; i < numOfParticles; i++)
    {
        fixedParticleIdx.push_back(i); // Store indices of fixed particles.
        particles[i]->SetFixed(true); // Mark particle as fixed, preventing it from moving.
    }
    

    // OpenGL setup for rendering the cloth.
    glGenVertexArrays(1, &vao); // Generate a VAO for the cloth.
    glGenBuffers(2, vbos); // Generate VBOs for positions and normals.
    glGenBuffers(1, &ebo); // Generate an EBO for indices.

    // Bind and configure the VAO and VBOs for position and normal data.
    // This includes uploading the initial positions and normals to the GPU.
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), nullptr, GL_STREAM_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), nullptr, GL_STREAM_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Bind and upload indices for the triangles to the EBO.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Unbind from the VAO to prevent accidental modifications.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Cloth::Update()
{
    // Measure the current time to calculate the frame rate and the time step.
    clock_t currT = clock();
    FPSCount++; // Increment the frame-per-second count.
    interval += currT - prevT; // Accumulate the time since the last frame.
    // Update the FPS counter once every second.
    if (interval >= 1000)
    {
        FPS = FPSCount; // Set the FPS value.
        interval = 0; // Reset the time interval.
        FPSCount = 0; // Reset the frame count.
    }
    float deltaT = (float)(currT - prevT) / 1000.0f; // Calculate the time step in seconds.
    prevT = currT; // Update the previous time marker.

    deltaT /= (float)numOfOversamples; // Adjust deltaT for oversampling.
    // Perform physics calculations with oversampling for increased stability.
    for (int i = 0; i < numOfOversamples; i++)
    {
        // Compute forces for all spring dampers.
        for (auto sp : connections)
        {
            sp->ComputeForce();
        }

        // Compute aerodynamic forces for all triangles.
        for (auto tri : triangles)
        {
            tri->ComputeAerodynamicForce();
        }

        // Integrate the motion of all particles.
        for (auto P : particles)
        {
            P->Integrate(deltaT);
        }
    }

    // Reset all normals to zero before recomputing them.
    for (auto& n : normals)
    {
        n = glm::vec3(0.0f);
    }

    // Compute normals for all triangles to reflect changes in the cloth's shape.
    for (auto tri : triangles)
    {
        tri->ComputeNormal();
    }

    // Normalize all normals after computation.
    for (auto& n : normals)
    {
        n = glm::normalize(n);
    }

    // Transfer updated positions and normals to their respective VBOs for rendering.
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, positions.data(), sizeof(glm::vec3) * positions.size());
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, normals.data(), sizeof(glm::vec3) * normals.size());
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Cloth::Draw(const glm::mat4& viewProjMtx)
{
    // Use the specified shader program for drawing.
    glUseProgram(programID);

    glm::mat4 model(1.0); // Model matrix for the cloth.
    glm::vec3 color(0.9f, 0.01f, 0.01f); // Color of the cloth.

    // Pass view projection matrix, model matrix, and color to the shader.
    glUniformMatrix4fv(glGetUniformLocation(programID, "viewProj"), 1, GL_FALSE, (float*)&viewProjMtx);
    glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, (float*)&model);
    glUniform3fv(glGetUniformLocation(programID, "DiffuseColor"), 1, &color[0]);

    // Bind the VAO for the cloth.
    glBindVertexArray(vao);

    // Draw the cloth using triangles, as specified by the indices in the EBO.
    glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, 0);

    // Unbind the VAO and the shader program.
    glBindVertexArray(0);
    glUseProgram(0);

    // Draw the ground for context.
    ground->Draw(viewProjMtx);
}
void Cloth::TranslateFixedParticles(int axis, float shift)
{
    // Create a translation vector along the specified axis.
    glm::vec3 delta(0.0f);
    delta[axis] = shift; // Apply the shift to the specified axis.

    // Apply the translation to all fixed particles.
    for (int i : fixedParticleIdx)
    {
        positions[i] += delta; // Update the position of each fixed particle.
    }
}

void Cloth::RotateFixedParticles(int axis, float shift)
{
    glm::mat4 rotateMat(1.0f); // Initialize a rotation matrix.

    // Determine the axis of rotation and apply the rotation angle.
    if (axis == 0)
    {
        rotateMat = glm::eulerAngleX(shift);
    }
    else if (axis == 1)
    {
        rotateMat = glm::eulerAngleY(shift);
    }
    else if (axis == 2)
    {
        rotateMat = glm::eulerAngleZ(shift);
    }

    // Calculate the midpoint between the first and last fixed particles.
    glm::vec3 midPoint = (positions[fixedParticleIdx.front()] + positions[fixedParticleIdx.back()]) / 2.0f;

    // Create a transformation matrix that rotates around the midpoint.
    glm::mat4 TransMat = glm::translate(midPoint) * rotateMat * glm::translate(-midPoint);

    // Apply the rotation to all fixed particles.
    for (int i : fixedParticleIdx)
    {
        positions[i] = TransMat * glm::vec4(positions[i], 1.0f); // Update the position of each fixed particle.
    }
}
