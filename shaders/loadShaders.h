// loadShaders.h
#ifndef LOADSHADERS_H_
#define LOADSHADERS_H_
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG 1 

GLuint createShader(GLenum shaderType, const char *fileName);
GLuint createProgram(GLuint *shaderList, int size);
int countChars(const char *fileName);
void storeData(char *fileData, const char *fileName);

#endif
