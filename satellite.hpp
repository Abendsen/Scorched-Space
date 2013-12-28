// satellite.hpp
#ifndef SATELLITE_HPP_
#define SATELLITE_HPP_
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "constants.hpp"
#include "draw.hpp"
#include <cstdio>

//------------//
// Base Class //
//------------//
class satellite
{
public:
    satellite() {/*Does nothing*/}
    virtual ~satellite() {/*Does nothing*/}
    virtual void draw() const {/*Does nothing*/}

    // Attributes:
    GLfloat mass;
    glm::vec2 pos;
    glm::vec2 vel;
    glm::vec3 color;
private:
};

//--------------//
// Planet Class //
//--------------//
class planet : public satellite
{
public:
    planet();
    planet(GLfloat imaxRad, glm::vec2 ipos);
    ~planet();
    void clean();
    GLfloat* getPlanetData() const;
    void changePlanetGraphic(GLfloat nmaxRad);
    void draw() const;

    // Public attributes:
    GLfloat orient;      // rotation about z-axis
    GLfloat rotSpeed; // rotation speed
    GLfloat maxRad;   // maximum radius from center
private:
    // Private attributes:
    GLuint planetVBO;
    GLfloat *planetData;
};

//--------------//
// Bullet Class //
//--------------//
class bullet : public satellite
{
public:
    bullet();
    bullet(glm::vec2 ipos, glm::vec2 ivel);
    ~bullet();
    void draw() const;

    // Attributes:
    GLfloat rad;
    GLfloat startTime;
    bool onPlanet;
private:

};

#endif
