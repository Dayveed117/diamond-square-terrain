#ifndef	SHADER_HPP
#define	SHADER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <fstream>
#include <iostream>
#include <sstream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

#endif