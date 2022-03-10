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
    std::cout << "print1" << std::endl;
    float hi = initPosition.x + positionVar.x / 2;
    std::cout << "print2" << std::endl;
    
   // srand(time(0));
    //randPos.x = fmod(double(rand()), double(positionVar.x)) + initPosition.x - (positionVar.x / 2);
    randPos.x = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));
    std::cout << "print3" << std::endl;

    lo = initPosition.y - positionVar.y / 2;
    std::cout << "print4" << std::endl;
    hi = initPosition.y + positionVar.y / 2;
    std::cout << "print5" << std::endl;
//    randPos.y = fmod(double(rand()), double(positionVar.y)) + initPosition.y - (positionVar.y / 2);
    randPos.y = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));
    std::cout << "print6" << std::endl;

    lo = initPosition.z - positionVar.z / 2;
    std::cout << "print7" << std::endl;
    hi = initPosition.z + positionVar.z / 2;
    std::cout << "print8" << std::endl;
//    randPos.z = fmod(double(rand()), double(positionVar.z)) + initPosition.z - (positionVar.z / 2);
    randPos.z = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));
    std::cout << "print9" << std::endl;

    lo = initVelocity.x - velocityVar.x / 2;
    std::cout << "print10" << std::endl;
    hi = initVelocity.x + velocityVar.x / 2;
    std::cout << "print11" << std::endl;
//    randVel.x = fmod(double(rand()), double(velocityVar.x)) + initVelocity.x - (velocityVar.x / 2);
    randVel.x = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));

    std::cout << "print12" << std::endl;
    lo = initVelocity.y - velocityVar.y / 2;
    std::cout << "print13" << std::endl;
    hi = initVelocity.y + velocityVar.y / 2;
    std::cout << "print14" << std::endl;
//    randVel.y = fmod(double(rand()), double(velocityVar.y)) + initVelocity.y - (velocityVar.y / 2);
    randVel.y = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));
    std::cout << "print15" << std::endl;

    lo = initVelocity.z - velocityVar.z / 2;
    std::cout << "print16" << std::endl;
    hi = initVelocity.z + velocityVar.z / 2;
    std::cout << "print17" << std::endl;
    //randVel.z = fmod(double(rand()), double(velocityVar.z)) + initVelocity.z - (velocityVar.z / 2);
    randVel.z = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));
    std::cout << "print18" << std::endl;

    lo = initLifespan - lifespanVar / 2;
    std::cout << "print19" << std::endl;
    hi = initLifespan + lifespanVar / 2;
    std::cout << "print20" << std::endl;
    //randLife = int(fmod(double(rand()), double(lifespanVar)) + initLifespan - (lifespanVar / 2));
    randLife = int(lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo))));
    std::cout << "print21" << std::endl;

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
    for(int i = 0; i < particles.size(); i++) {
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

}

Spawner::Spawner() {
    initPosition = glm::vec3(0.0f);
    positionVar = glm::vec3(2.0f);
    initVelocity = glm::vec3(0.0f, 20.0f, 0.0f);
    velocityVar = glm::vec3(1.5f, 5.0f, 1.5f);
    gravity = glm::vec3(0.0f, -5.8f, 0.0f);
    creationRate = 10;
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
