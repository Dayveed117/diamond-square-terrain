#pragma once

#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

class Grid
{
public:

    vector<vector<double>> grid;
    Grid(const int d, const int r, const int squarelen);

    vector<GLfloat> GenerateDiamondSquareTerrain(const int depth);
    
    void PrintGrid();
    void PrintResultingVertexBuffer(const vector<GLfloat>& v);

private:

    int squarelen;
    int r;
    double d;

    double PullRandom(const int max);
    double PullRandom(const double min, const double max);
    void Reduce_d();
    
    int CalculateGridSize(const int n, int acc = 0);

    void ExpandGrid(const int n);
    void FillGrid(const int depth);

    vector<GLfloat> ExtractGridVertices();
    float ResizeVertexCoords(const int i);
    void ExtractGridVertices(vector<GLfloat>& vertexbuf);
};

