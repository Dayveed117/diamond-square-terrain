﻿#include "glad/glad.h"
#include "shader_m.h"
#include "camera.h"
#include "Grid.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

int InitializeGlfwGlmGlad(bool& retflag);
void CleanBuffers();
void Render(Shader shader, unsigned int cubeTexture, int vertexnum);
void TransferDataToGPU(const vector<GLfloat> vbuf);

unsigned int loadTexture(const char* path);

// Settings
GLFWwindow* window = NULL;
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;


// Vertex array object   (VAO)
// Vertex buffer object  (VBO)
// Color buffer object   (CBO)
// Texture buffer object (TBO)
GLuint VertexArrayID;
GLuint vertexbuffer;
GLuint colorbuffer;
GLuint texturebuffer;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


int InitializeGlfwGlmGlad(bool& retflag)
{
    retflag = true;
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Diamond Square Terrain Generation", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    retflag = false;
    return {};
}


void TransferDataToGPU(const vector<GLfloat> vbuf)
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // cube VAO
    glGenVertexArrays(1, &VertexArrayID);
    glGenBuffers(1, &vertexbuffer);

    glBindVertexArray(VertexArrayID);

    // Vertexes
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vbuf.size() * sizeof(GLfloat), &vbuf[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}


void CleanBuffers()
{
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &texturebuffer);
    glDeleteBuffers(1, &colorbuffer);
}

void Render(Shader shader, unsigned int cubeTexture, int vertexnum)
{
    // Initialize MVP
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // Draw
    glBindVertexArray(VertexArrayID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    glDrawArrays(GL_TRIANGLES, 0, vertexnum);
    glBindVertexArray(0);
}

int main()
{
    bool retflag;
    int retval = InitializeGlfwGlmGlad(retflag);

    if (retflag)
        return retval;

    /*
     * d : standard discrepancy in heights
     * r : roughness
     * depth : diamond square step iterations
     * side_len : final square side length
     */
    const int d = 6;
    const int r = 3;
    const int depth = 9;
    const int side_len = 20;

    auto g = new Grid(d, r, side_len);
    const vector<GLfloat> v = g->GenerateDiamondSquareTerrain(depth);
    const int nc = v.size();

    TransferDataToGPU(v);

    // Shader object
    Shader shader("shaders/vertexshader.vert", "shaders/fragmentshader.frag");

    //  Invert image
    unsigned int cubeTexture = loadTexture("textures/na.png");

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("texture1", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Render(shader, cubeTexture, nc);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    CleanBuffers();

    glfwTerminate();
    return 0;
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{

    stbi_set_flip_vertically_on_load(true);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


// Test Main
/*
int main()
{
    const int d = 20;
    const int r = 2;
    const int depth = 2;
    auto g = new Grid(d, r);
    cout << "Depth : " << depth << endl;
    
    vector<GLfloat> v = g->GenerateDiamondTerrain(depth);
    g->PrintResultingVertexBuffer(v);

    

    return 0;
}
*/
