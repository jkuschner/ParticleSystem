#include "Spawner.hpp"
#include <stdlib.h>
#include <math.h>
#include <time.h>

ull Spawner::particlesCreated = 0;

void Spawner::addParticle() {
    // uniform randomness
    glm::vec3 randPos, randVel;
    GLuint randLife;

    float lo = initPosition.x - positionVar.x / 2;
    float hi = initPosition.x + positionVar.x / 2;
    
   // srand(time(0));
    randPos.x = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));

    lo = initPosition.y - positionVar.y / 2;
    hi = initPosition.y + positionVar.y / 2;
    randPos.y = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));
    // ensure that particle spawns above ground
    if (randPos.y <= 0.0f) {
        randPos.y = 0.1f;
    }

    lo = initPosition.z - positionVar.z / 2;
    hi = initPosition.z + positionVar.z / 2;
    randPos.z = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));

    lo = initVelocity.x - velocityVar.x / 2;
    hi = initVelocity.x + velocityVar.x / 2;
    randVel.x = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));

    lo = initVelocity.y - velocityVar.y / 2;
    hi = initVelocity.y + velocityVar.y / 2;
    randVel.y = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));

    lo = initVelocity.z - velocityVar.z / 2;
    hi = initVelocity.z + velocityVar.z / 2;
    randVel.z = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));

    lo = initLifespan - lifespanVar / 2;
    hi = initLifespan + lifespanVar / 2;
    randLife = int(lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo))));

    Particle* p = new Particle(randPos, randVel, MASS, randLife, particlesCreated);
    particles.push_back(p);
    particlesCreated++;

    std::cout << "Position x: " << randPos.x << "\nPosition y: " << randPos.y << "Position z:" << randPos.z << std::endl;
    std::cout << "Velocity x: " << randVel.x << "\nVelocity y: " << randVel.y << "Velocity z:" << randVel.z << std::endl;
}

void Spawner::update() {
    // delete dead particles
    std::vector<Particle*>::iterator it = particles.begin();
    std::vector<Particle*>::iterator tmpit;

    while(!particles.empty() && it != particles.end()) {
        tmpit = it + 1;
        if(tmpit != particles.end() && (*tmpit)->lifetime <= 0 ) {
            particles.erase(tmpit);
        }

        it++;
    }
    if(!particles.empty() && particles[0]->lifetime <= 0) {
        particles.erase(particles.begin());
    }

    // Apply all forces
    glm::vec3 drag;
    for(int i = 0; i < particles.size(); i++) {
        // zero out forces
        particles[i]->force = glm::vec3(0);
        //gravity
        particles[i]->force += gravity;
        //drag
        drag = 0.5f * air_density * DRAG_COFF * particle_radius * particle_radius * glm::length(particles[i]->velocity) * glm::length(particles[i]->velocity) * (-1.0f * glm::normalize(particles[i]->velocity));
        particles[i]->force += drag;
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

}

Spawner::Spawner() {
    initPosition = glm::vec3(0.0f);
    positionVar = glm::vec3(2.0f);
    initVelocity = glm::vec3(0.0f, 20.0f, 0.0f);
    velocityVar = glm::vec3(1.5f, 5.0f, 1.5f);
    gravity = glm::vec3(0.0f, -5.8f, 0.0f);
    creationRate = 10.0f;
    initLifespan = 200;
    lifespanVar = 10.0f;
    roundOff = 0.0f;
    air_density = AIR_DENSITY;
    drag_coff = DRAG_COFF;
    particle_radius = 0.1f;
    restitution = RESTITUTION;
    friction_coff = FRICTION_COFF;
}

std::vector<Cube*> Spawner::sprites() {
    std::vector<Cube*> sprites;
    sprites.reserve(particles.size());
    std::vector<Particle*>::iterator it;
    for (it = particles.begin(); it != particles.end(); ++it) {
        sprites.push_back((*it)->sprite);
    }
    
    return sprites;
}

void Spawner::changeParameters(float iposx, float iposy, float iposz,
                          float vposx, float vposy, float vposz,
                          float ivelx, float ively, float ivelz,
                          float vvelx, float vvely, float vvelz,
                          float gravi, float creat, GLuint ilife,
                          float vlife, float air_d, float dragc,
                          float psize, float bounc, float frict) {
    initPosition.x = iposx;
    initPosition.y = iposy;
    initPosition.z = iposz;
    positionVar.x = vposx;
    positionVar.y = vposy;
    positionVar.z = vposz;

    initVelocity.x = ivelx;
    initVelocity.y = ively;
    initVelocity.z = ivelz;
    velocityVar.x = vvelx;
    velocityVar.y = vvely;
    velocityVar.z = vvelz;

    gravity.y = gravi;
    creationRate = creat;
    initLifespan = ilife;
    lifespanVar = vlife;
    air_density = air_d;
    drag_coff = dragc;
    particle_radius = psize;
    restitution = bounc;
    friction_coff = frict;
}