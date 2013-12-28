//CollisionDetector.cpp
#include "CollisionDetector.hpp"

// Check for a collision between a planet and bullet type object.
bool CollisionDetector::checkCollision(const planet &p, const bullet &b)
{
    bool collision = false;

    int size = 0;
    GLfloat* triangles = getTriangleList(p, b, &size);
    if (-1 == size)
    {
        // core was hit
        fprintf(stdout, "!!!CORE HIT!!!\n");
        return true;
    }
    else if (NULL == triangles) return collision;

    // Determine the number of triangles from size:
    int tCount = size/2-2;
    for (int i = 0; i < tCount; ++i)
    {
        GLfloat tri[6] = {
            triangles[0],     triangles[1],
            triangles[2*i+2], triangles[2*i+3],
            triangles[2*i+4], triangles[2*i+5]
        };

        // If the bullet intersects a triangle, draw the triangle.
        if (polyCircleCheck(tri, 3, b.rad, b.pos))
        {
            #ifdef DEBUG_CD
            fprintf(stdout,
                    "DRAWING: <%f, %f>, <%f, %f>, <%f, %f>\n\n",
                    tri[0], tri[1], tri[2], tri[3], tri[4], tri[5]);
            #endif
            glm::vec2 p0 = glm::vec2(tri[0], tri[1]);
            glm::vec2 p1 = glm::vec2(tri[2], tri[3]);
            glm::vec2 p2 = glm::vec2(tri[4], tri[5]);
            // Red triangle:
            setDrawLayer(1);
            setDrawColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            drawTriangleFan(tri, 3, glm::vec2(0.0f));
            // White lines:
            setDrawLayer(2);
            setDrawColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            drawLine(p0, p1);
            drawLine(p1, p2);
            drawLine(p2, p0);
            collision = true;
        }
    }

    delete [] triangles;
    return collision;
}

// --PURPOSE--
// Determine the minimum set of triangles such that the object lies
// within them. This is pretty specific to the planet class.
// --PARAMETERS--
// pl: A planet object.
// bu: A bullet object.
// --RETURNS--
// A pointer to a set of triangles such that the object is in them. User
// must manage the memory of the retrieved triangle list. 
GLfloat* CollisionDetector::getTriangleList(const planet& pl,
                                            const bullet& bu,
                                            GLint* size)
{
    using namespace glm;

    GLfloat* planetData = pl.getPlanetData();
    if (bu.rad >= distance(pl.pos, bu.pos))
    {
        // Hit planet center!
        // This would be a good place to crack the planet
        // into smaller chunks.
        *size = -1;     // signal that core was hit
        return NULL;
    }

    // glm uses degrees -- cmath uses radians
    GLfloat orientDeg = GLfloat(pl.orient*(180.0f/PI));

    // Vectors:
    vec2 xAxis = vec2(cos(pl.orient), sin(pl.orient));
    vec2 p = bu.pos-pl.pos;
    vec2 q = p+bu.rad*normalize(vec2(-p[1], p[0]));
    // Signed distance from p to xAxis:
    GLfloat sDis = (p[0]*xAxis[1]-p[1]*xAxis[0])/sqrt(p[0]*p[0]+p[1]*p[1]);

    // Angles:
    GLfloat radIncrement = TAU/GLfloat(NUM_PLANET_VERTS);
    GLfloat tmp = dot(p, xAxis)/(length(p)*length(xAxis));
    GLfloat theta = (tmp >= 1.0f) ? 0.0f : (tmp <= -1.0f) ? PI : acos(tmp);
    if (0.0f < sDis) theta = TAU-theta;
    tmp = dot(p, q)/(length(p)*length(q));
    GLfloat alpha = (tmp >= 1.0f) ? 0.0f : (tmp <= -1.0f) ? PI : acos(tmp);

    GLfloat beta0 = theta-alpha;
    GLfloat beta1 = theta+alpha;
    if (beta0 > beta1)
    {
        GLfloat swp = beta0;
        beta0 = beta1;
        beta1 = swp;
    }
    else if (0.0f > beta0)
    {
        beta0 += TAU;
        beta1 += TAU;
    }

    // Find the upper and lower indices that contain the arc:
    GLfloat rem0 = GLfloat(GLint(1E6*beta0)%GLint(1E6*radIncrement))/1E6;
    GLfloat rem1 = GLfloat(GLint(1E6*beta1)%GLint(1E6*radIncrement))/1E6;
    beta0 -= rem0; beta1 -= rem1-radIncrement;
    GLint lowerIndex = GLint(round(beta0/radIncrement));
    GLint upperIndex = GLint(round(beta1/radIncrement));
    if (lowerIndex == upperIndex)
    {
        lowerIndex -= 1;
        upperIndex += 1;
    }

    // Number of vertices that enclose the bullet:
    GLint vCount = (lowerIndex < upperIndex)
                    ? upperIndex-lowerIndex+1
                    : NUM_PLANET_VERTS-(lowerIndex-upperIndex);

    #ifdef DEBUG_CD
    // Print this data:
    fprintf(stdout,
            "-----------------------------------------\n"
            "DEBUG: CollisionDetector::getTriangleList\n"
            "-----------------------------------------\n"
            "planet: 0x%x\tbullet: 0x%x\n"
            "radIncrement: %f\n"
            "theta:        %f\n"
            "alpha:        %f\n"
            "orientation:  %f\n"
            "triangles:    %d\n"
            "lower index:  %d\tupper index: %d\n\n",
            &pl, &bu,
            radIncrement,
            theta,
            alpha,
            pl.orient,
            vCount-1,
            lowerIndex, upperIndex);
    #endif
    
    // Joey: Here's an explanation of the list returned!
    // Generate the list to be returned. It's first point
    // is the origin of the planet and the rest of the points
    // are consecutive vertices going around the planet:
    *size = 2*vCount+2;
    GLfloat* list = new GLfloat[*size];
    list[0] = pl.pos[0]; list[1] = pl.pos[1];
    GLint pIndex = GLint(round(lowerIndex));
    for (GLint i = 1; i <= vCount; ++i)
    {
        vec2 rP = rotate(vec2(planetData[2*pIndex+2],
                              planetData[2*pIndex+3]), orientDeg);
        list[2*i] = rP[0]+pl.pos[0];
        list[2*i+1] = rP[1]+pl.pos[1];
        if (++pIndex >= NUM_PLANET_VERTS) pIndex = 0;
    }

    return list;
}

glm::vec2 CollisionDetector::midpoint(const glm::vec2& p0,
                                      const glm::vec2& p1)
{
    glm::vec2 foo = p1-p0;
    GLfloat len = glm::length(foo);
    return (len/2.0f)*glm::normalize(foo)+p0;
        
}

// projection
glm::vec2 CollisionDetector::projection(const glm::vec2& v0,
                                        const glm::vec2& axis)
{
    GLfloat scalar = glm::dot(v0, axis)/(axis[0]*axis[0]+axis[1]*axis[1]);
    return scalar*axis;    

}

// --PURPOSE--
// Determine the point that has the greatest projection length along
// the given axis.
// --PARAMETERS--
// convex:    A pointer to an array of points of a convex shape.
// verts:    The number of vertices in the shape.
// axis:    The axis to project along.
// --RETURNS--
// Pointer to the point that is the maximum projection on the axis.
GLfloat* CollisionDetector::getMaxAlongAxis(GLfloat* convex,
    const int verts, const glm::vec2& axis)
{
    GLfloat* maxAddress = NULL;
    // Greatest projection length:
    GLfloat gPL = 0.0f;
    // For all vertices:
    for (int v = 0; v < verts; ++v)
    {
        glm::vec2 point = glm::vec2(convex[2*v], convex[2*v+1]);
        // projection length of point <convex[2*v], convex[2*v+1]>:    
        GLfloat pLen = glm::length(projection(point, axis));
        if (pLen > gPL)
        {
            maxAddress = convex+2*v;    
            gPL = pLen;
        }
    }
    return maxAddress;
}

// --PURPOSE--
// Determine the point that has the smallest projection length along
// the given axis.
// --PARAMETERS--
// convex:    A pointer to an array of points of a convex shape.
// verts:    The number of vertices in the shape.
// axis:    The axis to project along.
// --RETURNS--
// Pointer to the point that is the minimum projection on the axis.
GLfloat* CollisionDetector::getMinAlongAxis(GLfloat* convex,
    const int verts, const glm::vec2& axis)
{
    GLfloat* minAddress = NULL;
    // Smallest projection length:
    GLfloat sPL;
    // For all vertices:
    for (int v = 0; v < verts; ++v)
    {
        glm::vec2 point = glm::vec2(convex[2*v], convex[2*v+1]);
        // projection length of point <convex[2*v], convex[2*v+1]>:    
        GLfloat pLen = glm::length(projection(point, axis));
        if (pLen < sPL || 0 == v)
        {
            minAddress = convex+2*v;    
            sPL = pLen;
        }
    }
    return minAddress;
}

GLfloat CollisionDetector::absf(GLfloat fval)
{
    return (fval < 0.0f) ? -fval : fval;
}

// Only insert a normal vector that isn't in the normal list.
void CollisionDetector::insertUniqueNormal(std::vector<glm::vec2>& nList,
   const glm::vec2& normal)
{
    for (int n = 0; n < nList.size(); ++n)
    {
        // For first check:
        GLfloat diff0 = absf(nList[n][0]-normal[0]);
        GLfloat diff1 = absf(nList[n][1]-normal[1]);
        if ((diff0 <= TOL) && (diff1 <= TOL))
        {
            #ifdef DEBUG_CD
            fprintf(stdout,
                    "diff0: %f\tdiff1: %f\n"
                    "Normal <%f, %f> will not be inserted.\n"
                    "It is a duplicate of <%f, %f>.\n",
                    diff0, diff1,
                    normal[0], normal[1],
                    nList[n][0], nList[n][1]);
            #endif
            return;    
        }

        // For second check:
        GLfloat diff2 = absf(nList[n][0]+normal[0]);
        GLfloat diff3 = absf(nList[n][1]+normal[1]);
        // Try multiplying normal by -1!
        if ((diff2 <= TOL) && (diff3 <= TOL))
        {
            #ifdef DEBUG_CD
            fprintf(stdout,
                    "diff2: %f\tdiff3: %f\n"
                    "Normal <%f, %f> will not be inserted.\n"
                    "It is a duplicate of <%f, %f>.\n",
                    diff2, diff3,
                    normal[0], normal[1],
                    nList[n][0], nList[n][1]);
            #endif
            return;
        }
    }
    nList.push_back(glm::normalize(normal));
}

glm::vec2 CollisionDetector::getPolygonCenter(GLfloat *poly, int vCount)
{
    glm::vec2 sum = glm::vec2(0.0f);
    for (int i = 0; i < vCount; ++i)
    {
        sum = sum + glm::vec2(poly[2*i], poly[2*i+1]);
    }

    glm::vec2 center = sum/GLfloat(vCount);

    // Draw the point:
    #ifdef DEBUG_CD
    setDrawLayer(6);
    setDrawColor(glm::vec4(1.0f));
    drawWireCircle(1.0f, center);    
    #endif

    return center;
}

bool CollisionDetector::polyCircleCheck(GLfloat* poly, int vCount,
                                        GLfloat radius, const glm::vec2& pos)
{
    using glm::vec2;
    std::vector<vec2> normals;
    vec2 tmpNormal;

    // Insert normal vectors formed by poly into normal list: 
    for (int i = 0; i < vCount-1; ++i)
    {
        // In 2D, normal can be formed easily as so:
        tmpNormal = vec2(-poly[2*i+3]+poly[2*i+1],
                          poly[2*i+2]-poly[2*i]);
        insertUniqueNormal(normals, tmpNormal);
    }
    tmpNormal = vec2(-poly[2*(vCount-1)+1]+poly[1],
                      poly[2*(vCount-1)]-poly[0]);
    insertUniqueNormal(normals, tmpNormal);

    // For each unique normal vector:
    GLfloat* p0min = NULL;
    GLfloat* p0max = NULL;
    for (int i = 0; i < normals.size(); ++i)
    {
        // Current normal vector we're working with:
        vec2 cNormal = normals[i];
        // Find the max and min pointers of each polygon on the normal vector.
        p0min = getMinAlongAxis(poly, vCount, cNormal);
        p0max = getMaxAlongAxis(poly, vCount, cNormal);
        if (NULL == p0min || NULL == p0max) return false;
        //fprintf(stdout, "p0min: 0x%x\tp0max: 0x%x\n", p0min, p0max);
        // Form the four vectors formed by the max-min projections:
        vec2 p0min_v = projection(vec2(p0min[0], p0min[1]), cNormal);
        vec2 p0max_v = projection(vec2(p0max[0], p0max[1]), cNormal);
        // Circle is a little different.
        // First project the circle position onto cNormal.
        vec2 cMid_p = projection(pos, cNormal);
        vec2 a = midpoint(p0min_v, p0max_v);
        vec2 b = cMid_p;
        // If no overlap, there is no collision, return false.
        if (glm::distance(a, b) > glm::length(p0max_v-p0min_v)/2.0f+radius)
            return false;
        // Otherwise, continue.
    }

    #ifdef DEBUG_CD
    fprintf(stdout,
            "-----------------------------------------\n"
            "DEBUG: CollisionDetector::polyCircleCheck\n"
            "-----------------------------------------\n"
            "!!!COLLISION DETECTED!!!\n"
            "normal vectors: %d\n"
            "<p0min[0], p0min[1]>: <%f, %f>\n"
            "<p0max[0], p0max[1]>: <%f, %f>\n"
            "VERTICES: <%f, %f>, <%f, %f>, <%f, %f>\n",
            normals.size(),
            p0min[0], p0min[1], p0max[0], p0max[1],
            poly[0], poly[1], poly[2], poly[3], poly[4], poly[5]);
    #endif

    return true;
}

// Not tested or functional yet.
bool CollisionDetector::polyPolyCheck(GLfloat* poly0, int vCount0,
    GLfloat* poly1, int vCount1)
{
    using glm::vec2;
    std::vector<vec2> normals;
    vec2 tmpNormal;

    // Insert normal vectors formed by poly0 into normal list: 
    for (int i = 0; i < vCount0-1; ++i)
    {
        // In 2D, normal can be formed easily as so:
        tmpNormal = vec2(-poly0[2*i+3]+poly0[2*i+1],
                          poly0[2*i+2]-poly0[2*i]);
        insertUniqueNormal(normals, tmpNormal);
    }
    tmpNormal = vec2(-poly0[2*(vCount0-1)+1]+poly0[1],
                      poly0[2*(vCount0-1)]-poly0[0]);
    insertUniqueNormal(normals, tmpNormal);

    // Insert normal vectors formed from poly1 into normal list: 
    for (int i = 0; i < vCount1-1; ++i)
    {
        // In 2D, normal can be formed easily as so:
        tmpNormal = vec2(-poly1[2*i+3]+poly1[2*i+1],
                          poly1[2*i+2]-poly1[2*i]);
        insertUniqueNormal(normals, tmpNormal);
    }
    tmpNormal = vec2(-poly1[2*(vCount0-1)+1]+poly1[1],
                      poly1[2*(vCount0-1)]-poly1[0]);
    insertUniqueNormal(normals, tmpNormal);

    // For each unique normal vector:
    for (int i = 0; i < normals.size(); ++i)
    {
        // Current normal vector we're working with:
        vec2 cNormal = normals[i];
        // Find the max and min pointers of each polygon on the normal vector.
        GLfloat* p0min = getMinAlongAxis(poly0, vCount0, cNormal);
        GLfloat* p0max = getMaxAlongAxis(poly0, vCount0, cNormal);
        GLfloat* p1min = getMinAlongAxis(poly1, vCount1, cNormal);
        GLfloat* p1max = getMaxAlongAxis(poly1, vCount1, cNormal);
        // Form the four vectors formed by the max-min projections:
        vec2 p0min_v = projection(vec2(p0min[0], p0min[1]), cNormal);
        vec2 p0max_v = projection(vec2(p0max[0], p0max[1]), cNormal);
        vec2 p1min_v = projection(vec2(p1min[0], p1min[1]), cNormal);
        vec2 p1max_v = projection(vec2(p1max[0], p1max[1]), cNormal);
        vec2 a = midpoint(p0min_v, p0max_v);
        vec2 b = midpoint(p1min_v, p1max_v);
        // If no overlap, there is no collision, return false.
        if (2.0f*glm::distance(a, b)
            <= glm::length(p0max_v-p0min_v)+glm::length(p1max_v-p1min_v))
            return false;
        // Otherwise, continue.
    }
    
    return true;
}
