#include "Grid.h"

Grid::Grid(const int d, const int r, const int squarelen)
{

    this->r = r;
    this->d = (double)d;
    this->squarelen = squarelen;

    srand(static_cast <double> (time(0)));

    this->grid.resize(2);
    for (auto& v : this->grid)
    {
        v.resize(2);
        for (size_t i = 0; i < 2; i++)
            v[i] = PullRandom(d);

    }
}


vector<GLfloat> Grid::GenerateDiamondSquareTerrain(const int depth)
{
    vector<GLfloat> terrain{};

    if (depth < 1)
        return terrain;

    // Expand grid according to depth
    int to_expand = CalculateGridSize(depth);
    ExpandGrid(to_expand);

    // Generate heights with diamond square algorithm
    FillGrid(depth);

    // Extract vertices from terrain
    // Terrain is resized internally to hold as many vertices coordinates as it needs
    ExtractGridVertices(terrain);

    return terrain;
}


void Grid::ExpandGrid(const int n)
{
    // Store corners of initial 2x2 grid
    double ul = grid[0][0];
    double ur = grid[0][1];
    double dl = grid[1][0];
    double dr = grid[1][1];

    this->grid.resize(n);
    for (auto& v : this->grid)
    {
        // Resize grid
        v.resize(n);
        fill(v.begin(), v.end(), 0.0);
    }

    // Attrib old corners to new corners
    grid[0][0]         = ul;
    grid[0][n - 1]     = ur;
    grid[n - 1][0]     = dl;
    grid[n - 1][n - 1] = dr;

}


void Grid::FillGrid(const int depth)
{
    /*
    depth = 1
    *-----*-----*
    |     |     |
    *-----C-----*
    |     |     |
    *-----*-----*
    *
    depth = 2
    *-----*-----*-----*-----*
    |     |     |     |     |
    *-----X-----*-----X-----*
    |     |     |     |     |
    *-----*-----C-----*-----*
    |     |     |     |     |
    *-----X-----*-----X-----*
    |     |     |     |     |
    *-----*-----*-----*-----*
    *
    depth = 3
    *-----*-----*-----*-----*-----*-----*-----*-----*
    | 2 | ------------>     |     |     |     |     |
    *---|-O-----*-----O-----*-----*-----*-----*-----*
    |   | | 4 | ------------------------>     |     |
    *---|-*---|-X-----*-----*-----*-----X-----*-----*
    |   | |   | |     |     |     |     |     |     |
    |   | |   | |     |     |     |     |     |     |
    *---v-O---|-*-----*-----*-----*-----*-----*-----*
    |     |   | |     |     |     |     |     |     |
    *-----*---|-*-----*-----C-----*-----*-----*-----*
    |     |   | |     |     |     |     |     |     |
    *-----*---|-*-----*-----*-----*-----*-----*-----*
    |     |   | |     |     |     |     |     |     |
    |     |   | |     |     |     |     |     |     |
    *-----*---v-X-----*-----*-----*-----*-----*-----*
    |     |     |     |     |     |     |     |     |
    *-----*-----*-----*-----*-----*-----*-----*-----*
    |     |     |     |     |     |     |     |     |
    *-----*-----*-----*-----*-----*-----*-----*-----*
    * length of square for depth is 3 is ..., 8, 4, 2, 1.
    * stride for each centered square is 0, 2, 4, 8, ...
    */

    int lado = grid.size() - 1;

    // For each depth iteration
    for (int i = 0; i < depth; i++)
    {
        // Halve size of square length for each depth step
        lado = lado / 2;

        // Iterate vertically
        for (int j = 1; j <= pow(2, i); j++)
        {
            // Iterate Horizontally
            for (int k = 1; k <= pow(2, i); k++)
            {
                // Square center point
                int midx = lado + 2 * (j - 1) * lado;
                int midy = lado + 2 * (k - 1) * lado;
                double& mid = grid[midx][midy];

                // Corners of square have random values
                double ul = grid[midx - lado][midy - lado];
                double ur = grid[midx + lado][midy - lado];
                double dl = grid[midx - lado][midy + lado];
                double dr = grid[midx + lado][midy + lado];


                // Vertical and Horizontal points
                double& left  = grid[midx - lado][midy];
                double& up    = grid[midx][midy - lado];
                double& right = grid[midx + lado][midy];
                double& down  = grid[midx][midy + lado];


                // E = (A+B+C+D)/4 + Rand(d)
                mid = (ul + ur + dl + dr) / 4 + PullRandom(-d, d);

                // Wrapping
                
                left  = (ul + dl + mid + mid) / 4 + PullRandom(-d, d);
                up    = (ul + ur + mid + mid) / 4 + PullRandom(-d, d);
                right = (ur + dr + mid + mid) / 4 + PullRandom(-d, d);
                down  = (dl + dr + mid + mid) / 4 + PullRandom(-d, d);
                
                
                // Non Wrapping
                /*
                left  = (ul + dl + mid) / 3 + PullRandom(-d, d);
                up    = (ul + ur + mid) / 3 + PullRandom(-d, d);
                right = (ur + dr + mid) / 3 + PullRandom(-d, d);
                down  = (dl + dr + mid) / 3 + PullRandom(-d, d);
                */

            }
        }

        Reduce_d();
    }
}


void Grid::ExtractGridVertices(vector<GLfloat>& vertexbuf)
{
    unsigned int c = 0;

    for (int i = 0; i < (grid.size() - 1); i++)
    {
        for (int j = 0; j < (grid.size() - 1); j++)
        {
            // 2 Triangles
            /*
            glm::vec3 ul = { (float)i, grid[i][j], (float)j };
            glm::vec3 ur = { (float)i, grid[i][j + 1], (float)j + 1 };
            glm::vec3 dl = { (float)i + 1, grid[i + 1][j], (float)j };
            glm::vec3 dr = { (float)i + 1, grid[i + 1][j + 1], (float)j + 1 };
            */

            // Resizable
            // 2 Triangles
            glm::vec3 ul = { ResizeVertexCoords(i), grid[i][j], ResizeVertexCoords(j) };
            glm::vec3 ur = { ResizeVertexCoords(i), grid[i][j + 1], ResizeVertexCoords(j + 1) };
            glm::vec3 dl = { ResizeVertexCoords(i + 1), grid[i + 1][j], ResizeVertexCoords(j) };
            glm::vec3 dr = { ResizeVertexCoords(i + 1), grid[i + 1][j + 1], ResizeVertexCoords(j + 1) };
            

            GLfloat tmp[18] = {
                /*
                    ul-------ur
                    |      /
                    |   /
                    |/
                    dl
                */
                dl.x, dl.y, dl.z,
                ul.x, ul.y, ul.z,
                ur.x, ur.y, ur.z,
                /*
                               ur
                               /|
                            /   |
                         /      |
                      dl--------dr
                */
                ur.x, ur.y, ur.z,
                dr.x, dr.y, dr.z,
                dl.x, dl.y, dl.z,
            };

            auto st = vertexbuf.begin();
            advance(st, 18 * c);
            c++;

            vertexbuf.insert(st, begin(tmp), end(tmp));
        }
    }
}


// ----------------------------- //
// ---------- SUPPORT ---------- //
// ----------------------------- //

void Grid::Reduce_d()
{
    d *= pow(2, -r);
}

double Grid::PullRandom(const int max)
{
    return static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / max));
}

double Grid::PullRandom(const double min, const double max)
{
    // from https://stackoverflow.com/questions/686353/random-float-number-generation
    return (min + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (max - min))));
}

int Grid::CalculateGridSize(const int n, int acc)
{
    return n < 0 ? acc + grid.size() : CalculateGridSize(n - 1, acc + pow(2, n - 1));
}

float Grid::ResizeVertexCoords(const int i)
{
    return ((float)i / grid.size()) * squarelen;
}

void Grid::PrintGrid()
{
    for (auto& v : this->grid)
    {
        for (auto i : v)
            cout << i << " ";

        cout << endl;
    }

    cout << endl;
}

void Grid::PrintResultingVertexBuffer(const vector<GLfloat>& v)
{
    for (size_t i = 0; i < v.size(); i += 3)
    {
        cout << "x : " << v[i] << endl
            << "  y : " << v[i + 1] << endl
            << "   z : " << v[i + 2] << endl;
    }

    cout << endl;
}
