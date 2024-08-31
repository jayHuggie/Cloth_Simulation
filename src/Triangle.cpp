#include "Triangle.h"

// Constructor: Initializes a triangle with given particles, fluid density, drag coefficient, and wind velocity.
Triangle::Triangle(Particle* _P1, Particle* _P2, Particle* _P3, float* _FluidDensity, float* _C, glm::vec3* _WindVelocity)
{
    P1 = _P1; // First particle of the triangle
    P2 = _P2; // Second particle of the triangle
    P3 = _P3; // Third particle of the triangle

    FluidDensity = _FluidDensity; // Pointer to the fluid density (e.g., air density)
    C_d = _C; // Pointer to the drag coefficient (C_d)
    WindVelocity = _WindVelocity; // Pointer to the wind velocity vector
}

// Computes the aerodynamic force acting on the triangle due to wind.
void Triangle::ComputeAerodynamicForce()
{
    // Calculate the average velocity of the triangle's surface by averaging the velocities of its particles.
    glm::vec3 SurfaceVelocity = (
        P1->GetVelocity() + P2->GetVelocity() + P2->GetVelocity()) / 3.0f;

    // Calculate the relative velocity of the surface against the wind.
    glm::vec3 v_dir = SurfaceVelocity - *WindVelocity;
    float v_scale = glm::length(v_dir); // Magnitude of the relative velocity
    if (v_scale < EPSILON) return; // If velocity is negligible, no force is applied.

    v_dir /= v_scale; // Normalize the direction of the relative velocity.

    // Calculate the normal vector of the triangle using the cross product of two edges.
    glm::vec3 n = glm::cross(
        P2->GetPosition() - P1->GetPosition(), 
        P3->GetPosition() - P1->GetPosition());
    float area = glm::length(n); // The magnitude of this cross product is twice the area of the triangle.
    n /= area; // Normalize the normal vector.
    area /= 2; // Correct the area calculation.

    // Calculate the effective area exposed to the wind direction.
    float crossArea = area * glm::dot(v_dir, n);

    // Compute the aerodynamic force using the formula: F = -0.5 * rho * v^2 * C_d * A * n
    glm::vec3 f = -0.5f * (*FluidDensity) * v_scale * v_scale * (*C_d) * crossArea * n;

    // Distribute the force equally among the triangle's particles.
    f /= 3.0f;
    P1->ApplyForce(f);
    P2->ApplyForce(f);
    P3->ApplyForce(f);
}

// Computes and distributes the normal vector of the triangle to its particles.
void Triangle::ComputeNormal()
{
    // Calculate the normal vector of the triangle using the cross product of two edges.
    glm::vec3 n = glm::cross(
        P2->GetPosition() - P1->GetPosition(),
        P3->GetPosition() - P1->GetPosition());

    if (n.length() < EPSILON) return; // If the normal is negligible, skip normalization.

    n = glm::normalize(n); // Normalize the normal vector.

    // Add the computed normal to each particle. This is useful for lighting calculations.
    P1->AddNormal(n);
    P2->AddNormal(n);
    P3->AddNormal(n);
}
