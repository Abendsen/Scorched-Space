// draw.hpp
#ifndef DRAW_HPP_
#define DRAW_HPP_
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "constants.hpp"
#include "satellite.hpp"

// Planet specific:
void drawPlanet(GLuint planetVBO, glm::vec2 pos, GLfloat rot);
void drawWirePlanet(GLuint planetVBO, glm::vec2 pos, GLfloat rot);
GLuint createPlanetVBO(GLfloat *planetData);
GLfloat *createPlanetData(GLfloat maxRad);
void genRandomFractalMap(float range, int x0, int xn, float *map);

// Draw commands:
void drawLine(glm::vec2 p0, glm::vec2 p1);
void drawSquare(GLfloat width, glm::vec2 pos);
void drawWireSquare(GLfloat width, glm::vec2 pos);
void drawRectangle(GLfloat width, GLfloat height, glm::vec2 pos);
void drawWireRectangle(GLfloat width, GLfloat height, glm::vec2 pos);
void drawCircle(GLfloat rad, glm::vec2 pos);
void drawWireCircle(GLfloat rad, glm::vec2 pos);
void drawTriangleFan(GLfloat *vertData, GLuint vertCount, glm::vec2 pos);

// State settings:
void setDrawLayer(GLuint layer);
void setDrawColor(glm::vec3 color);
void setDrawColor(glm::vec4 color);
void setShaderHandles(GLuint shaderID);
void setCoordinateSystem(GLfloat xVal, GLfloat yVal);

// Common buffer creation:
void createCircleVBO();
void createSquareVBO();

// Buffer cleanup -- Must be called at end of program!
void cleanBuffers();

#endif
