// main.cpp
#include <cstdio>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GL/freeglut.h>
#include "shaders/loadShaders.h"
#include "draw.hpp"
#include "CollisionDetector.hpp"
#include "satellite.hpp"

static planet planets[MAX_PLANET]; 
static bullet bullets[MAX_BULLET];
static int pIndex = 0;
static int bIndex = 0;

// To turn on shader program:
static GLuint shaderID = 0;

// Dimensions:
static int windowWidth  = 800;
static int windowHeight = 800;
static float gameWidth  = 150.0f;
static float gameHeight = 150.0f;

// Mouse coordinates:
static glm::vec2 mouse;

// Key state buffer:
static bool keyState[256] = {false};
void onKeyPress(unsigned char key, int mX, int mY) { keyState[key] = true; }
void onKeyRelease(unsigned char key, int mX, int mY) { keyState[key] = false; }

// Convert mouse coordinates to game coordinates:
glm::vec2 mouseToGame()
{
	using glm::vec2;
    // Mouse coordinates to game coordinates:
    float xScale = windowWidth/float(gameWidth);
    float yScale = windowHeight/float(gameHeight);
    return vec2(mouse[0]/float(xScale),(windowHeight-mouse[1])/float(yScale));
}

// Add a bullet to the scene:
void addBullet()
{
    // Initialize a bullet to be added to the scene:
    bullets[bIndex].pos = mouseToGame();
    bullets[bIndex].vel = glm::vec2(0.0f);
    bullets[bIndex].rad = 0.25f;
    bullets[bIndex].mass = 10.0f;
    bullets[bIndex].startTime = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
	bullets[bIndex].onPlanet = false;

	// Correct the bullet index:
    if (++bIndex >= MAX_BULLET) bIndex = 0;
}

// Add a planet to the scene:
void addPlanet()
{
    // Initialize a planet to be added to the scene:
	planets[pIndex].orient = (rand()%360)*(PI/180.0f);
	planets[pIndex].rotSpeed = 
		float((((rand()%MAX_ROTATION)+1)-MAX_ROTATION/2))/1000.0f;
    planets[pIndex].pos = mouseToGame();
    planets[pIndex].maxRad = (rand()%10)+10.0f;
	planets[pIndex].changePlanetGraphic(planets[pIndex].maxRad);
	planets[pIndex].color = glm::vec3(((rand()%100)+1)/100.0f,
									  ((rand()%100)+1)/100.0f,
									  ((rand()%100)+1)/100.0f);

	// Correct the planet index:
    if (++pIndex >= MAX_PLANET) pIndex = 0;

}

// Handle mouse events:
void processMousePassiveMotion(int xx, int yy) { mouse = glm::vec2(xx, yy); }
void processMouseActiveMotion(int button, int state, int xx, int yy) 
{
	static bool planetDrag = false;
	static int planetDragIndex = -1;
    mouse = glm::vec2(xx, yy); 
    // Left-button is pressed:
    if ((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)) addBullet();
    // Right-button is pressed:
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) addPlanet();

}

void updatePlanets()
{
	for (int p = 0; p < MAX_PLANET; ++p)
	{
		if (0.0f == planets[p].maxRad) continue;
		// Spin the planet a bit:
		planets[p].orient += planets[p].rotSpeed;
		if (planets[p].orient >= TAU) planets[p].orient -= TAU;
		else if (planets[p].orient <= 0.0f) planets[p].orient += TAU;
	}
}

void updateBullets()
{
	// namespace resolution
	using namespace glm;

    for (int b = 0; b < MAX_BULLET; ++b)
    {
        // Don't update bullets that don't exist:
        if (0.0f == bullets[b].rad || true == bullets[b].onPlanet) continue;

        // Sum of gravitational forces:
        vec2 sum = vec2(0.0f);
        for (int p = 0; p < MAX_PLANET; ++p)
        {   
			// Optimize distance check:
			GLfloat dx = planets[p].pos[0] - bullets[b].pos[0];
			GLfloat dy = planets[p].pos[1] - bullets[b].pos[1];
			GLfloat sqrDis = dx*dx + dy*dy;
			GLfloat sqrRadSum = planets[p].maxRad + bullets[b].rad;
			sqrRadSum *= sqrRadSum;

			// If bullet is in relative area of a planet:
            //float r = distance(planets[p].pos, bullets[b].pos);
            //if (r <= planets[p].maxRad+bullets[b].rad)
			if (sqrDis < sqrRadSum)
            {
				// Check if the bullet is colliding with the planet:
				if (CollisionDetector::checkCollision(planets[p], bullets[b]))
				{
					bullets[b].vel = vec2(0.0f);
					bullets[b].startTime = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
					bullets[b].onPlanet = true;
					bullets[b].rad = 0.0f;
					break;
				}
			}
            // Calculate the sum of all forces of gravity:
            vec2 ab = normalize(planets[p].pos-bullets[b].pos);
            float fg = (GRAVITATIONAL*planets[p].mass*bullets[b].mass)/(sqrDis);
            sum = sum+fg*ab;
        }
	
		if (true == bullets[b].onPlanet) continue;
        float t1 = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
        float t = t1 - bullets[b].startTime;
        bullets[b].vel = sum*t + bullets[b].vel;
        // Maximum velocity?
        if (length(bullets[b].vel) > MAX_BULLET_SPEED)
            bullets[b].vel = MAX_BULLET_SPEED*normalize(bullets[b].vel);
        bullets[b].pos = sum*t*t + bullets[b].vel*t+bullets[b].pos;
    }
}

void keyboardEvents()
{
	if (keyState['b']) addBullet();
}

// Returns true if time for a game tick.
bool timeForTick()
{
	static GLfloat t0 = 0.0f;
	GLfloat t1 = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
	if (t1-t0 >= 1.0f/60.0f)
	{
		t0 = t1;
		return true;
	}
	else return false;
}

// Prints the number of frames rendered in the last second.
void printRoughFPS()
{
	// frames per second
	static GLfloat t0 = 0.0f;
 	static int frameCount = 1;
	GLfloat t1 = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
 	if (t1-t0 >= 1.0f)
	{
 	 	// Print the number of frames counted in the last second.
 	 	fprintf(stdout, "FPS:%d\n", frameCount);
 	 	t0 = t1;
 	 	frameCount = 1;
 	}
 	else ++frameCount;
}

// Rendering and game logic.
void renderScene()
{
	if (!timeForTick()) return;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderID);

	printRoughFPS();
	keyboardEvents();
	updatePlanets();
	updateBullets();

	// Only draw planets that exist:
	for (int p = 0; p < MAX_PLANET; ++p)
		if (0.0f < planets[p].maxRad) planets[p].draw();
	// Only draw bullets that exist:
	for (int b = 0; b < MAX_BULLET; ++b)
		if (0.0f < bullets[b].rad) bullets[b].draw();

	glUseProgram(0);
	glutSwapBuffers();
}


// Initialize scene to be rendered.
void init()
{
	// OpenGL state settings:
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Link and compile shaders:
	GLuint shaders[2] = {
		createShader(GL_VERTEX_SHADER, "shaders/vertexShader"),
		createShader(GL_FRAGMENT_SHADER, "shaders/fragmentShader")
	};
	shaderID = createProgram(&shaders[0], 2);

	// Some drawing setup:
	glUseProgram(shaderID);
	createCircleVBO();
	createSquareVBO();
	setShaderHandles(shaderID);
	setCoordinateSystem(gameWidth, gameHeight);
	glUseProgram(0);
}

// TO DO: Maintain the aspect ratio when the window is resized.
void reshape(int w, int h)
{	
	windowWidth = w;
	windowHeight = h;
	glViewport(0.0f, 0.0f, (GLuint)w, (GLuint)h);
}

int main(int argc, char *argv[])
{
	printf("&argc : 0x%x\n", &argc);
	glutInit(&argc, argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);
	glutInitContextVersion(3, 0); // OpenGL 3.0
	glutInitContextFlags(GLUT_CORE_PROFILE); // set profile context
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Scorched Space");

    // initialize OpenGL Extension Wrangler (GLEW)
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {   
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(1);
    }   
    else fprintf(stdout, "GLEW initialized successfully\n");

	// initialize main program
	init();

	// glut program loop
	glutDisplayFunc(renderScene);
	glutReshapeFunc(reshape);
	glutIdleFunc(renderScene);
	glutPassiveMotionFunc(processMousePassiveMotion);
	glutMouseFunc(processMouseActiveMotion);
	glutKeyboardFunc(onKeyPress);
	glutKeyboardUpFunc(onKeyRelease);
	glutMainLoop();

	// Some OpenGL clean up:
	cleanBuffers();
	if (GL_TRUE == glIsProgram(shaderID)) glDeleteProgram(shaderID);

	return 0;
}
