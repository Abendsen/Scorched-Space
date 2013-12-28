#include "satellite.hpp"

//-----------------------------//
// Planet Class Implementation //
//-----------------------------//

// Default constructor:
planet::planet()
{
	//fprintf(stdout, "Constructing satellite::planet: 0x%x...\n", this);
	this->orient = 0.0f;
	this->rotSpeed = 0.0f;
	this->mass = 0.0f;
	this->maxRad = 0.0f;
	this->pos = glm::vec2(0.0f);
	this->vel = glm::vec2(0.0f);
	this->color = glm::vec3(1.0f);
	this->planetVBO = GL_INVALID_VALUE;
	this->planetData = NULL;
}

// 2-parameter constructor:
planet::planet(GLfloat imaxRad, glm::vec2 ipos)
{
	//fprintf(stdout, "Constructing satellite::planet: 0x%x...\n", this);
	this->orient = 0.0f;
	this->rotSpeed = 0.0f;
	this->maxRad = imaxRad;
	this->mass = this->maxRad*PLANET_MASS;
	this->pos = ipos;
	this->vel = glm::vec2(0.0f);
	this->color = glm::vec3(1.0f);
	this->planetData = createPlanetData(this->maxRad);
	this->planetVBO = createPlanetVBO(this->planetData);
}

// Destructor:
planet::~planet()
{
	//fprintf(stdout, "Destructing satellite::planet: 0x%x...\n", this);
	this->clean();
}

GLfloat* planet::getPlanetData() const
{
	return this->planetData;
}

// Clean up memory:
void planet::clean()
{
	// Delete the planetVBO if it exists:
	if (GL_TRUE == glIsBuffer(planetVBO))
	{
		// This replaces destroyPlanetVBO(...) in draw.hpp:
		glDeleteBuffers(1, &planetVBO);
		planetVBO = GL_INVALID_VALUE;
	}
	// planetData memory no longer needed:
	if (NULL != this->planetData)
	{
		delete [] this->planetData;
		this->planetData = NULL;
	}
}

// Set a different planet graphic:
void planet::changePlanetGraphic(GLfloat nmaxRad)
{
	this->maxRad = nmaxRad;
	this->mass = this->maxRad*PLANET_MASS;

	this->clean();
	this->planetData = createPlanetData(this->maxRad);
	this->planetVBO = createPlanetVBO(this->planetData);
}

// Draw call:
void planet::draw() const
{
	// Don't draw a planet that doesn't exist:
	if (GL_FALSE == glIsBuffer(this->planetVBO)) return;

	// Convert radians to degrees for drawing:
	GLfloat deg = (this->orient)*(180.0f/PI);

	// We'll have to come up with layer constants later.
	setDrawLayer(0);
	setDrawColor(glm::vec4(this->color, 0.5f));
	drawPlanet(this->planetVBO, this->pos, deg);

	setDrawLayer(1);
	setDrawColor(this->color);
	drawWirePlanet(this->planetVBO, this->pos, deg);
}

//-----------------------------//
// Bullet Class Implementation //
//-----------------------------//

// Default constructor:
bullet::bullet()
{
	//fprintf(stdout, "Constructing satellite::bullet: 0x%x...\n", this);
	this->onPlanet = false;
	this->pos = glm::vec2(0.0f);
	this->vel = glm::vec2(0.0f);
	this->rad = 0.0f;
	this->mass = 0.0f;
	this->startTime = 0.0f;
	this->color = glm::vec3(1.0f);
}

// 2-parameter constructor:
bullet::bullet(glm::vec2 ipos, glm::vec2 ivel)
{
	//fprintf(stdout, "Constructing satellite::bullet: 0x%x...\n", this);
	this->onPlanet = false;
	this->pos = ipos;
	this->vel = ivel;
	this->rad = 0.0f;
	this->mass = 0.0f;
	this->startTime = 0.0f;
	this->color = glm::vec3(1.0f);
}

// Destructor for testing:
bullet::~bullet()
{
	//fprintf(stdout, "Destructing satellite::bullet: 0x%x...\n", this);
}

// Draw call:
void bullet::draw() const
{
	// Don't draw a circle that doesn't exist:
	if (0.0f >= this->rad) return;	

	setDrawLayer(2);
	setDrawColor(this->color);
	drawCircle(this->rad, this->pos);
}
