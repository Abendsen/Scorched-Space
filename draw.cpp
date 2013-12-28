// draw.c
// Authors: Ed Markowski, Joey Parker
#include "draw.hpp"

//----------------------//
// File-Scope Variables //
//----------------------//
static glm::mat4 globalTranslation = glm::mat4(1.0f);
static GLfloat drawLayer = 0.0f;
static GLuint shaderID = 0;
static GLuint circleVBO = GL_INVALID_VALUE;
static GLuint squareVBO = GL_INVALID_VALUE;
static GLuint a_position;
static GLuint u_modelview;
static GLuint u_viewport;
static GLuint u_projection;
static GLuint u_color;

//---------------//
// Draw Commands //
//---------------//
void drawPlanet(GLuint planetVBO, glm::vec2 pos, GLfloat rot)
{
	if (GL_FALSE == glIsBuffer(planetVBO))
	{
		// Display warning.
		return;
	}

	using namespace glm;
	mat4 mMat = translate(globalTranslation, vec3(pos, drawLayer));
	if (0.0f != rot) mMat = glm::rotate(mMat, rot, vec3(0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(u_modelview, 1, GL_FALSE, value_ptr(mMat));
	glBindBuffer(GL_ARRAY_BUFFER, planetVBO);
	glEnableVertexAttribArray(a_position);
	glVertexAttribPointer(a_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_PLANET_VERTS+2);
	glDisableVertexAttribArray(a_position);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void drawWirePlanet(GLuint planetVBO, glm::vec2 pos, GLfloat rot)
{
	if (GL_FALSE == glIsBuffer(planetVBO))
	{
		// Display warning.
		return;
	}

	using namespace glm;
	mat4 mMat = translate(globalTranslation, vec3(pos, drawLayer));
	if (0.0f != rot) mMat = rotate(mMat, rot, vec3(0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(u_modelview, 1, GL_FALSE, value_ptr(mMat));
	glBindBuffer(GL_ARRAY_BUFFER, planetVBO);
	glEnableVertexAttribArray(a_position);
	glVertexAttribPointer(a_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINE_LOOP, 1, NUM_PLANET_VERTS);
	glDisableVertexAttribArray(a_position);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void drawLine(glm::vec2 p0, glm::vec2 p1)
{
    using namespace glm;
	mat4 mMat = translate(globalTranslation, vec3(0.0f, 0.0f, drawLayer));
    glUniformMatrix4fv(u_modelview, 1, GL_FALSE, value_ptr(mMat));
    const GLfloat linePoints[] = {p0[0], p0[1], p1[0], p1[1]};
    glVertexAttribPointer(a_position, 2, GL_FLOAT, GL_FALSE, 0, linePoints);
    glEnableVertexAttribArray(a_position);
    glDrawArrays(GL_LINES, 0, 2);
    glDisableVertexAttribArray(a_position);
}

void drawSquare(GLfloat width, glm::vec2 pos)
{
	drawRectangle(width, width, pos);
}

void drawWireSquare(GLfloat width, glm::vec2 pos)
{
	drawWireRectangle(width, width, pos);
}

void drawRectangle(GLfloat width, GLfloat height, glm::vec2 pos)
{
	using namespace glm;
    mat4 mMat = translate(globalTranslation, vec3(pos, drawLayer));
    mMat = scale(mMat, vec3(width, height, 0.0f));
    glUniformMatrix4fv(u_modelview, 1, GL_FALSE, value_ptr(mMat));
    glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
    glEnableVertexAttribArray(a_position);
    glVertexAttribPointer(a_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(a_position);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void drawWireRectangle(GLfloat width, GLfloat height, glm::vec2 pos)
{
    using namespace glm;
    mat4 mMat = translate(globalTranslation, vec3(pos, drawLayer));
    mMat = scale(mMat, vec3(width, height, 0.0f));
    glUniformMatrix4fv(u_modelview, 1, GL_FALSE, glm::value_ptr(mMat));
    glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
    glEnableVertexAttribArray(a_position);
    glVertexAttribPointer(a_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_LINE_LOOP, 4, 4);
    glDisableVertexAttribArray(a_position);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void drawCircle(GLfloat rad, glm::vec2 pos)
{
    using namespace glm;
    mat4 mMat = translate(globalTranslation, vec3(pos, drawLayer));
    mMat = scale(mMat, vec3(rad, rad, 0.0f));
    glUniformMatrix4fv(u_modelview, 1, GL_FALSE, glm::value_ptr(mMat));
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glEnableVertexAttribArray(a_position);
    glVertexAttribPointer(a_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_CIRCLE_VERTS+2);
    glDisableVertexAttribArray(a_position);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void drawWireCircle(GLfloat rad, glm::vec2 pos)
{
    using namespace glm;
    mat4 mMat = translate(globalTranslation, vec3(pos, drawLayer));
    mMat = scale(mMat, vec3(rad, rad, 0.0f));
    glUniformMatrix4fv(u_modelview, 1, GL_FALSE, glm::value_ptr(mMat));
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glEnableVertexAttribArray(a_position);
    glVertexAttribPointer(a_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_LINE_LOOP, 1, NUM_CIRCLE_VERTS);
    glDisableVertexAttribArray(a_position);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void drawTriangleFan(GLfloat *vertData, GLuint vertCount, glm::vec2 pos)
{
	using namespace glm;
	mat4 mMat = translate(globalTranslation, vec3(pos, drawLayer));
	glUniformMatrix4fv(u_modelview, 1, GL_FALSE, value_ptr(mMat));
	const GLfloat *data = vertData;
    glVertexAttribPointer(a_position, 2, GL_FLOAT, GL_FALSE, 0, vertData);
    glEnableVertexAttribArray(a_position);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertCount);
    glDisableVertexAttribArray(a_position);
}

//---------------//
// State Setters //
//---------------//
void setDrawLayer(GLuint layer)
{
	if (layer >= NUM_DRAW_LAYERS) layer = NUM_DRAW_LAYERS-1;
	drawLayer = layer/float(NUM_DRAW_LAYERS);
}

void setDrawColor(glm::vec3 color)
{
	glUniform4fv(u_color, 1, glm::value_ptr(glm::vec4(color, 1.0f)));
}

void setDrawColor(glm::vec4 color)
{
	glUniform4fv(u_color, 1, glm::value_ptr(color));
}

void setShaderHandles(GLuint shaderID)
{
	a_position = glGetAttribLocation(shaderID, "position");
    u_modelview = glGetUniformLocation(shaderID, "modelview");
    u_viewport = glGetUniformLocation(shaderID, "viewport");
    u_projection = glGetUniformLocation(shaderID, "projection");
    u_color = glGetUniformLocation(shaderID, "color");
}

void setCoordinateSystem(GLfloat xVal, GLfloat yVal)
{
    glm::mat4 pMat = glm::ortho(0.0f, xVal, 0.0f, yVal);
    glUniformMatrix4fv(u_projection,
                       1, GL_FALSE,
                       glm::value_ptr(pMat));
}

//-------------------------------//
// Vertex Buffer Object Creation //
//-------------------------------//
void genRandomFractalMap(float range, int x0, int xn, float *map) 
{
    int xm = (xn+x0)/2;
    float val = range*(float(rand())/RAND_MAX-0.5); 
    
    if (xn - x0 <= 5)
	{
        for (int i = x0; i < xn; ++i)
		{
            map[i] = (map[x0] + map[xn])/2;
		}
	}
    else
    {   
        map[xm] = map[xm] + val + (map[x0]+map[xn])/2;
        genRandomFractalMap(range*0.5f,x0,xm,map);
        genRandomFractalMap(range*0.5f,xm,xn,map);
    }   
}

GLfloat *createPlanetData(GLfloat maxRad)
{
	GLfloat *ranMap = new GLfloat[NUM_PLANET_VERTS];
	for (int i = 0; i < NUM_PLANET_VERTS; ++i) ranMap[i] = 0;
	genRandomFractalMap(1.0f, 0, NUM_PLANET_VERTS-1, ranMap);

    double theta = 0.0;
    double radIncrement = 2.0*PI/double(NUM_PLANET_VERTS);

    GLfloat *planetData = new GLfloat[2*NUM_PLANET_VERTS+4];
    // Center of planet.
    planetData[0] = 0.0f;   // x
    planetData[1] = 0.0f;   // y
    GLfloat maxLen = 0.0f;
    // The rest of the points along the circumference.
    for (int i = 0; i < NUM_PLANET_VERTS; ++i)
    {   
		// i = NUM_PLANET_VERTS-1
		// planetData[2*NUM_PLANET_VERTS]
		// planetData[2*NUM_PLANET_VERTS+1]

       	//planetData[2*i+2] = cos(theta); // x
        //planetData[2*i+3] = sin(theta); // y

       	planetData[2*i+2] = cos(theta)*(1+ranMap[i]); // x
        planetData[2*i+3] = sin(theta)*(1+ranMap[i]); // y
        theta += radIncrement;

        // Find the length of this vector:
		using glm::distance;
		using glm::vec2;
		GLfloat len =  distance(vec2(planetData[2*i+2], planetData[2*i+3]), vec2(0.0f));
        if (len > maxLen) maxLen = len; 
    }       

    for (int i = 1; i <= NUM_PLANET_VERTS; ++i)
    {   
        planetData[2*i] *= (maxRad/maxLen);
        planetData[2*i+1] *= (maxRad/maxLen);
    }
    
    // Complete the circle.
    planetData[2*NUM_PLANET_VERTS+2] = planetData[2]; // x
    planetData[2*NUM_PLANET_VERTS+3] = planetData[3]; // y
    
    return planetData;
    
}

       
GLuint createPlanetVBO(GLfloat *planetData){

    GLuint planetBuffer;

    // Initialize circle vertex buffer object.
    glGenBuffers(1, &planetBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, planetBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat)*(2*NUM_PLANET_VERTS+4),
                 planetData,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //delete [] planetData;
    return planetBuffer;
}

void createCircleVBO()
{
    // Circle is common, check if it exists:
    if (GL_INVALID_VALUE != circleVBO) return;

    double theta = 0;
    double radIncrement = 2*PI/NUM_CIRCLE_VERTS;

    GLfloat *circleData = new GLfloat[2*(NUM_CIRCLE_VERTS+2)];
    // Center of circle.
    circleData[0] = 0.0f;   // x
    circleData[1] = 0.0f;   // y
    // The rest of the points along the circumference.
    for (int i = 1; i < NUM_CIRCLE_VERTS; ++i, theta += radIncrement)
    {
        circleData[2*i] = cos(theta);   // x
        circleData[2*i+1] = sin(theta); // y
    }
    // Complete the circle.
    circleData[2*NUM_CIRCLE_VERTS] = circleData[2];  // x
    circleData[2*NUM_CIRCLE_VERTS+1] = circleData[3];  // y

    // Initialize circle vertex buffer object.
    glGenBuffers(1, &circleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat)*(2*(NUM_CIRCLE_VERTS+2)),
                 circleData,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Local memory no longer needed.
    delete [] circleData;
}

void createSquareVBO()
{
    // Square is common, check if it exists:
    if (GL_INVALID_VALUE != squareVBO) return;

    GLfloat squareData[] = {
        // GL_TRIANGLE_STRIP
        -0.5f, -0.5f,   // bottom left corner
         0.5f, -0.5f,   // bottom right corner
        -0.5f,  0.5f,   // top left corner
         0.5f,  0.5f,   // top right corner 
        // GL_LINE_LOOP
        -0.5f, -0.5f,   // bottom left corner
         0.5f, -0.5f,   // bottom right corner
         0.5f,  0.5f,   // top right corner 
        -0.5f,  0.5f,   // top left corner
    };

    // create the square vbo
    glGenBuffers(1, &squareVBO);
    glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
    // Associate square_vertex_buffer with squareData.
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat)*16,
                 squareData,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//----------------------------------//
// Vertex Buffer Object Destruction //
//----------------------------------//
void cleanBuffers()
{
	if (GL_INVALID_VALUE != circleVBO) glDeleteBuffers(1, &circleVBO);
    if (GL_INVALID_VALUE != squareVBO) glDeleteBuffers(1, &squareVBO);
}

