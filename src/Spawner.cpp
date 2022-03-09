#include "Spawner.hpp"

void Spawner::addParticle() {
    Particle* p = new Particle()
    // how to generate randomness?
}

void Cloth::update(glm::vec3 windSpeed) {
    // zero out forces
    for(int i = 0; i < particles.size(); i++) {
        for(int j = 0; j < particles[0].size(); j++) {
            particles[i][j]->force = glm::vec3(0);
        }
    }

    // Apply all forces
    // apply gravity to all particles
    glm::vec3 gravity = glm::vec3(0, GRAVITY, 0);
    for(int i = 0; i < particles.size(); i++) {
        for(int j = 0; j < particles[0].size(); j++) {
            particles[i][j]->force += gravity;
        }
    }

    // apply springdamper force
    for(int i = 0; i < springDampers.size(); i++) {
        springDampers[i]->computeForce();
    }

    // apply drag force
    for(int i = 0; i < triangles.size(); i++) {
        triangles[i]->calcVelocity(windSpeed);
        triangles[i]->computeForce();
    }
    
    // Integrate motion
    for(int i = 0; i < particles.size(); i++) {
        for(int j = 0; j < particles[0].size(); j++) {
            if(particles[i][j]->isFixed == false)
                particles[i][j]->updatePosition(TIME_STEP);
        }
    }

    // zero out particle normals
    for(int i = 0; i < particles.size(); i++) {
        for(int j = 0; j < particles[0].size(); j++) {
            if(particles[i][j]->isFixed == false)
                particles[i][j]->normal = glm::vec3(0);
        }
    }

    //Loop through all triangles and add the triangle normal to the normal of each of the three particles it connects
    for(int i = 0; i < triangles.size(); i++) {
        triangles[i]->calcNormal();
        triangles[i]->p1->normal += triangles[i]->normal;
        triangles[i]->p2->normal += triangles[i]->normal;
        triangles[i]->p3->normal += triangles[i]->normal;
    }

    //Loop through all the particles again and normalize the normal
    for(int i = 0; i < particles.size(); i++) {
        for(int j = 0; j < particles[0].size(); j++) {
            glm::normalize(particles[i][j]->normal);
        }
    }

    vec4s positions;
    vec4s normals;
    positions.reserve(verts.size());
    normals.reserve(verts.size());

    for(int i = 0; i < particles.size(); i++) {
        for(int j = 0; j < particles[0].size(); j++) {
            positions.push_back(glm::vec4(particles[i][j]->position, 1));
            normals.push_back(glm::vec4(particles[i][j]->normal, 0));
        }
    }

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec4), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);
}