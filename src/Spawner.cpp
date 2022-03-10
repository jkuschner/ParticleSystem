#include "Spawner.hpp"
#include <stdlib.h>

void Spawner::addParticle() {
    // uniform randomness
    glm::vec3 randPos, randVel;
    GLuint randLife;
    
    srand(time(0));
    randPos.x = float((rand() % int(positionVar.x))) + initPosition.x - (positionVar.x / 2);
    randPos.y = float((rand() % int(positionVar.y))) + initPosition.y - (positionVar.y / 2);
    randPos.z = float((rand() % int(positionVar.z))) + initPosition.z - (positionVar.z / 2);
    randVel.x = float((rand() % int(velocityVar.x))) + initVelocity.x - (velocityVar.x / 2);
    randVel.y = float((rand() % int(velocityVar.y))) + initVelocity.y - (velocityVar.y / 2);
    randVel.z = float((rand() % int(velocityVar.z))) + initVelocity.z - (velocityVar.z / 2);
    randLife = int(float(rand() % int(lifespanVar))) + initLifeSpan - (lifespanVar / 2));

    Particle* p = new Particle(randPos, randVel, MASS, randLife, particlesCreated);
    particles.push_back(p);
    particlesCreated++;
}

std::vector<std::string> Spawner::update() {
    // delete dead particles
    std::vector<Particle*>::iterator it = particles.begin();
    std::vector<Particle*>::iterator tmpit;

    std::vector<std::string> deleteIds;
    while(it != particles.end())
        tmpit = it;
        it++;
        if(tmpit->lifetime <= 0) {
            deleteIds.push_back(tmpit->sprite->name);
            particles.erase(tmpit);
        }
    }

    // zero out forces
    for(int i = 0; i < particles.size(); i++) {
        particles[i]->force = glm::vec3(0);
    }

    // Apply all forces
    // apply gravity to all particles
    for(int i = 0; i < particles.size(); i++) {
        particles[i]->force += gravity;
    }

    // apply drag force
    for(int i = 0; i < triangles.size(); i++) {
        // drag calculation based on spherical/cubic particles
    }
    
    // Integrate motion
    for(int i = 0; i < particles.size(); i++) {
        particles[i]->updatePosition(TIME_STEP);
    }

    // Create new particles
    float num = TIME_STEP * creationRate + roundOff;
    int newParticles = int(num);
    roundOff = num - float(newParticles);
    for(int i = 0; i < newParticles; i++) {
        addParticle();
    }

    return deleteIds;
}

Spawner::Spawner() {
    initPosition = glm::vec3(0f);
    positionVar = glm::vec3(1f);
    initVelcoity = glm::vec3(0f, 15f, 0f);
    velocityVar = glm::vec3(0.5f, 2f, 0.5f);
    gravity = glm::vec3(0f, -9.8f, 0f);
    creationRate = 10;
    initLifeSpan = 200;
    lifespanVar = 10f;
    roundOff = 0f;
    air_density = AIR_DENSITY;
    drag_coff = DRAG_COFF;
    particle_radius = 0.1f;
    restitution = RESTITUTION;
    friction_coff = FRICTION_COFF;
}

std::vector<Cube*> sprites() {
    std::vector<Cube*> sprites;
    sprites.reserve(particles.size());
    std::vector<Particle*>::iterator it;
    for (it = particles.begin(); it != particles.end(); ++it) {
        sprites.push_back(it->sprite);
    }
}