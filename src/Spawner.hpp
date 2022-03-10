#pragma once
#define Spawner_hpp

#include "core.hpp"
#include "Mesh.hpp"
#include "Cube.hpp"

#define SQRT2 1.41421356237f
#define DEFAULT_NORMAL glm::vec3(0, 1, 0)

#define PARTICLE_SPACING 0.2f
#define INITIAL_HEIGHT 1.7f

#define TIME_STEP 0.016666667f

#define GRAVITY -9.8f
#define MASS 0.5f

#define AIR_DENSITY 1.225f
#define DRAG_COFF 1.28f

#define RESTITUTION 0.05f
#define FRICTION_COFF 0.5f

typedef unsigned long long int ull;

struct Particle {
    glm::vec3 position;
    glm::vec3 position_prev;
    glm::vec3 velocity;
    glm::vec3 force;
    float     mass;
    GLuint lifetime;
    Cube* sprite;
    ull particleID;

    public:
        glm::vec3 acceleration() { return (1 / mass) * force; }
        glm::vec3 momentum()     { return mass * velocity; }
        //void applyForce(glm::vec3 &f) { force += f;}

        Particle(glm::vec3 pos, glm::vec3 v, float m, GLuint life, ull id) {
            position = pos;
            position_prev = pos;
            velocity = v;
            force = glm::vec3(0);
            mass = m;
            lifetime = life;
            particleID = id;
            sprite = new Cube(std::to_string(id), pos, pos + glm::vec3(CUBE_SIZE));
        }


        void updatePosition(float timestep) {
            /*
            glm::vec3 position_new = 2.0f * position - position_prev;
            position_new += acceleration() * timestep * timestep;
            position_prev = position;
            position = position_new;
            */

            // Euler integration
            velocity += acceleration() * timestep;
            position_prev = position;
            position += velocity * timestep;

            lifetime--;

            if (position.y < 0.0f) { // ground collision detection
                // collision handle
                glm::vec3 ground_normal = glm::vec3(0,1,0);
                float v_close = glm::dot(velocity, ground_normal);
                glm::vec3 impulse = -1.0f * (1.0f + RESTITUTION) * mass * v_close * ground_normal;

                // calculate impulse due to friction
                // start with finding v_tangent
                glm::vec3 fric_impulse = velocity - (v_close * ground_normal);
                fric_impulse = -1.0f * glm::normalize(fric_impulse);
                fric_impulse *= FRICTION_COFF * glm::length(impulse);

                // add to frictionless impulse for final impulse
                impulse += fric_impulse;
                // apply to velocity
                velocity += impulse / mass;

                // fix position
                glm::vec3 contact_point = (position_prev.y * position) - (position.y * position_prev);
                contact_point /= position_prev.y - position.y;

                position_prev = contact_point; //maybe not needed??
                position = contact_point + (velocity * timestep * 0.5f); // approx w/ half a time step
            } else {
                velocity += acceleration() * timestep;
            }

            sprite->update(position);
        }
};

class Spawner {
private:
    std::vector<Particle*> particles;
public:
    static ull particlesCreated;

    glm::vec3 initPosition;
    glm::vec3 positionVar;
    glm::vec3 initVelocity;
    glm::vec3 velocityVar;
    glm::vec3 gravity;
    GLuint creationRate; // particles per second
    GLuint initLifespan;
    float lifespanVar;
    float roundOff;
    float air_density;
    float drag_coff;
    float particle_radius;
    float restitution; //collision elasticity
    float friction_coff; //collision friction

    void addParticle();
    void update();
    std::vector<Cube*> sprites();

    Spawner();
};
