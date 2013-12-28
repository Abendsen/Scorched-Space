#ifndef COLLISIONDETECTOR_HPP_
#define COLLISIONDETECTOR_HPP_

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "satellite.hpp"
#include "constants.hpp"
#include "draw.hpp"
#include <vector>

class CollisionDetector
{
public:
    static bool checkCollision(const planet& p, const bullet& b);
private:
    // Separating axis theorem:
    static GLfloat* getTriangleList(const planet& p, const bullet& b,
                                    GLint* size);
    static bool polyCircleCheck(GLfloat* poly, int vCount,
                                GLfloat radius, const glm::vec2& pos);
    static bool polyPolyCheck(GLfloat* poly0, int vCount0,
                              GLfloat* poly1, int vCount1);
    static glm::vec2 getPolygonCenter(GLfloat* poly, int vCount);
    static glm::vec2 projection(const glm::vec2& v0, const glm::vec2& axis);
    static glm::vec2 midpoint(const glm::vec2& p0, const glm::vec2& p1);
    static void insertUniqueNormal(std::vector<glm::vec2>& nList,
                                   const glm::vec2& normal);
    static GLfloat* getMaxAlongAxis(GLfloat* convex, const int verts,
                                   const glm::vec2& axis);    
    static GLfloat* getMinAlongAxis(GLfloat* convex, const int verts,
                                   const glm::vec2& axis);    
    static GLfloat absf(GLfloat fval);
};

#endif
