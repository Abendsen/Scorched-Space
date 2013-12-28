// loadShaders.c
#include "loadShaders.h"

// Creates and returns a shader that is ready to be linked and compiled.
GLuint createShader(GLenum shaderType, const char *fileName)
{
	GLuint shader = glCreateShader(shaderType);

	// allocate memory for the file data	
	char *fileData; 
	int characters;
	if (characters = countChars(fileName))
		fileData = (char *)malloc(sizeof(char)*countChars(fileName));
	else
	{
		// no data
		fprintf(stderr, "Shader %s contains no data...\n", fileName);
		fprintf(stderr, "Closing program...\n");
		exit(EXIT_FAILURE);
	}

	// store data from fileName into fileData
	storeData(fileData, fileName);
	// fileData is now the source code for the shader	
	glShaderSource(shader, 1, (const GLchar**)&fileData, NULL);
	if (DEBUG) fprintf(stdout, "Compiling shader %s\n", fileName);
	glCompileShader(shader);

	// check compile status
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (GL_FALSE == status)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = 
			(GLchar *)malloc(sizeof(GLchar)*(infoLogLength+1));
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch (shaderType)
		{
			case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
			case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
			//case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		}
		fprintf(stderr, "Compile failure in %s shader:\n%s\n",
			strShaderType, strInfoLog);
		// free up dynamic memory
		free(strInfoLog);
	}

	// free up dynamic memory
	free(fileData);
	// return compiled shader value
	return shader;
}

// Links and compiles all shader source code to create main shader program.
GLuint createProgram(GLuint *shaderList, int size)
{
	// establish program id
	GLuint program = glCreateProgram();
	// link all shaders
	int i;
	for (i = 0; i < size; ++i)
		glAttachShader(program, shaderList[i]);

	if (DEBUG) fprintf(stdout, "Linking compiled shaders\n");
	glLinkProgram(program);

	// determine linking status
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (GL_FALSE == status) // failed to link shaders
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = 
			(GLchar *)malloc(sizeof(GLchar)*(infoLogLength+1));	
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		// print error message
		fprintf(stderr, "%s: Linker failure\n", strInfoLog);
		// free up dynamic memory
		free(strInfoLog);
	}

	for (i = 0; i < size; ++i)
		glDetachShader(program, shaderList[i]);

	// return the program id 
	return program;
}

// Returns the number of characters in fileName.
int countChars(const char *fileName)
{
	FILE *fp;

	// open file and check if it was successful
	if (NULL == (fp = fopen(fileName, "r")))
	{
		if (DEBUG) fprintf(stderr, "%s: No such file\n", fileName);
		exit(EXIT_FAILURE);
	}

	// count the number of characters in the 
	char ch;
	unsigned count = 0;
	// count all characters in the file
	while (EOF != (ch = fgetc(fp)))
		++count;

	// close file
	fclose(fp);

	// return the number of characters in the file
	return count;
}

// Stores contents of fileName into fileData.
void storeData(char *fileData, const char *fileName)
{
	FILE *fp;
	
	// open file and check if it was successful
	if (NULL == (fp = fopen(fileName, "r")))
	{
		if (DEBUG) fprintf(stderr, "%s: No such file\n", fileName);
		exit(EXIT_FAILURE);
	}

	int index = 0;
	char ch;
	// copy contents of file to fileData char array
	while (EOF != (ch = fgetc(fp)))
		fileData[index++] = ch;
	// set last character to the null character
	fileData[index-1] = '\0';
	 
	// close file
	fclose(fp);
}
