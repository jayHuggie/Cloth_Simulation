#include "Particle.h"

// Constructor: Initializes the particle with given parameters.
Particle::Particle(glm::vec3* _position, glm::vec3* _normal, float* _mass, float* _gravityAcce, float* _groundPos)
{
    position = _position; // Pointer to the particle's position vector.
    normal = _normal; // Pointer to the particle's normal vector, used for surface interaction.
    isFixed = false; // Initially, the particle is not fixed and can move.

    mass = _mass; // Pointer to the particle's mass.
    gravityAcce = _gravityAcce; // Pointer to the acceleration due to gravity affecting the particle.
    groundPos = _groundPos; // Pointer to the ground's position (used for collision detection).

    // Initialize velocity and force vectors to zero.
    velocity = glm::vec3(0.0f);
    force = glm::vec3(0.0f);
    prevForce = glm::vec3(0.0f); // Stores the force from the previous frame.
}

// Apply an external force to the particle.
void Particle::ApplyForce(glm::vec3 f)
{
    force += f; // Add the external force to the current total force acting on the particle.
}

// Apply the force of gravity to the particle based on its mass.
void Particle::ApplyGravity()
{
    force += glm::vec3(0.0f, -(*mass) * (*gravityAcce), 0.0f); // Gravity acts downward.
}

// Update the particle's state by integrating its motion over time.
void Particle::Integrate(float deltaTime)
{
    if (isFixed) return; // Do nothing if the particle is fixed.

    ApplyGravity(); // Apply gravity force for this frame.

    glm::vec3 a = force / (*mass); // Calculate acceleration from force and mass.
    if (a.length() > EPSILON) // Check if the acceleration is significant.
    {
        velocity += a * deltaTime; // Update velocity based on acceleration.
        // If the velocity is very small, reset it to zero to avoid jitter.
        if (velocity.length() < EPSILON) velocity = glm::vec3(0.0f);
        // Update position based on velocity.
        *position += velocity * deltaTime;
        prevForce = force; // Remember the force for next frame.
    }
    GroundCollision(); // Handle collision with the ground.

    force = glm::vec3(0.0f); // Reset force after integration.
}

// Reset the accumulated force to zero. Can be used at the start of each simulation step.
void Particle::ResetForce()
{
    force = glm::vec3(0.0f);
}

// Getter for the particle's current velocity.
glm::vec3 Particle::GetVelocity()
{
    return velocity;
}

// Getter for the particle's current position.
glm::vec3 Particle::GetPosition()
{
    return *position;
}

// Reset the particle's normal vector to zero. Useful for beginning of a new simulation step.
void Particle::ResetNormal()
{
    *normal = glm::vec3(0.0f);
}

// Add a normal vector to the particle's current normal. Used for aggregating surface normals.
void Particle::AddNormal(glm::vec3 n)
{
    *normal += n; 
}

// Check for collision with the ground and adjust the particle's position and velocity accordingly.
void Particle::GroundCollision()
{
    // If the particle's position is below the ground, correct its position and zero out vertical velocity.
    if (position->y < (*groundPos) + EPSILON)
    {
        position->y = (*groundPos) + EPSILON; // Adjust position to sit on the ground.
        velocity.y = 0.0f; // Stop downward movement.
    }
}
