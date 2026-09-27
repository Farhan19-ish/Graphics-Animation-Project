

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "pointLight.h"
#include "spotLight.h"

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model, float r, float g, float b);
static void classroom(unsigned int& cubeVAO, Shader& lightingShader);


// settings
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 800;

// modelling transform
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float doorSlide = 0.0f;
float scale_X = 1.0;
float scale_Y = 1.0;
glm::mat4 sceneTransform(1.0f);
float scale_Z = 1.0;

// camera
Camera camera(glm::vec3(0.0f, 2.4f, -9.5f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 1.0, eyeZ = 6.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);


// positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(2.5f,  3.7f,  2.0f),
    glm::vec3(2.5f,  3.7f, -2.0f),
    glm::vec3(-2.5f, 3.7f,  2.0f),
    glm::vec3(-2.5f, 3.7f, -2.0f)
};
glm::vec3 spotLightPositions[] = {
    glm::vec3(-1.65f, 3.38f, 3.15f),
    glm::vec3(1.65f, 3.38f, 3.15f)
};
PointLight pointlight1(

    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    1       // light number
);
PointLight pointlight2(

    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    2       // light number
);
PointLight pointlight3(

    pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    3       // light number
);
PointLight pointlight4(

    pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    4       // light number
);


// light settings
bool pointLightOn = true;
bool ambientToggle = true;
bool diffuseToggle = true;
bool specularToggle = true;


// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 444: Computer Graphics Laboratory", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

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

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float cube_vertices[] = {
        // positions      // normals
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
    };
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    unsigned int cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



    //ourShader.use();
    //lightingShader.use();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        // point light 1
        pointlight1.setUpPointLight(lightingShader);
        // point light 2
        pointlight2.setUpPointLight(lightingShader);
        // point light 3
        pointlight3.setUpPointLight(lightingShader);
        // point light 4
        pointlight4.setUpPointLight(lightingShader);

        lightingShader.setVec3("spotLights[0].position", spotLightPositions[0]);
        lightingShader.setVec3("spotLights[0].direction",
            glm::normalize(glm::vec3(1.0f, -0.83f, 0.61f)));
        lightingShader.setFloat("spotLights[0].cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLights[0].outerCutOff", glm::cos(glm::radians(20.0f)));
        lightingShader.setVec3("spotLights[0].ambient", glm::vec3(0.0f));
        lightingShader.setVec3("spotLights[0].diffuse", glm::vec3(3.0f, 2.2f, 0.35f));
        lightingShader.setVec3("spotLights[0].specular", glm::vec3(0.8f));
        lightingShader.setFloat("spotLights[0].k_c", 1.0f);
        lightingShader.setFloat("spotLights[0].k_l", 0.09f);
        lightingShader.setFloat("spotLights[0].k_q", 0.032f);

        lightingShader.setVec3("spotLights[1].position", spotLightPositions[1]);
        lightingShader.setVec3("spotLights[1].direction",
            glm::normalize(glm::vec3(-1.0f, -0.83f, 0.61f)));
        lightingShader.setFloat("spotLights[1].cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLights[1].outerCutOff", glm::cos(glm::radians(20.0f)));
        lightingShader.setVec3("spotLights[1].ambient", glm::vec3(0.0f));
        lightingShader.setVec3("spotLights[1].diffuse", glm::vec3(3.0f, 2.7f, 0.55f));
        lightingShader.setVec3("spotLights[1].specular", glm::vec3(0.8f));
        lightingShader.setFloat("spotLights[1].k_c", 1.0f);
        lightingShader.setFloat("spotLights[1].k_l", 0.09f);
        lightingShader.setFloat("spotLights[1].k_q", 0.032f);

        // activate shader
        lightingShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        lightingShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        //glm::mat4 view = basic_camera.createViewMatrix();
        lightingShader.setMat4("view", view);

        // Modelling Transformation
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        sceneTransform = model;

        //glBindVertexArray(cubeVAO);
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0, 36);


        const glm::vec3 spotLightMarkerColors[] = {
            glm::vec3(1.0f, 0.68f, 0.24f),
            glm::vec3(1.0f, 0.86f, 0.42f)
        };
        for (unsigned int i = 0; i < 2; i++)
        {
            const glm::vec3 markerSize(0.24f, 0.12f, 0.16f);
            model = glm::mat4(1.0f);
            model = glm::translate(model, spotLightPositions[i] - markerSize * 0.5f);
            model = glm::scale(model, markerSize);
            ourShader.setMat4("model", model);
            ourShader.setVec3("color", spotLightMarkerColors[i]);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        classroom(cubeVAO, lightingShader);

        // also draw the lamp object(s)
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            ourShader.setMat4("model", model);
            ourShader.setVec3("color", glm::vec3(0.8f, 0.8f, 0.8f));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            //glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f)
{
    lightingShader.use();

    lightingShader.setVec3("material.ambient", glm::vec3(r, g, b));
    lightingShader.setVec3("material.diffuse", glm::vec3(r, g, b));
    lightingShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    lightingShader.setFloat("material.shininess", 32.0f);

    model = sceneTransform * model;
    lightingShader.setMat4("model", model);

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

static void drawBlock(unsigned int& cubeVAO, Shader& lightingShader,
    const glm::vec3& center, const glm::vec3& size,
    const glm::vec3& color, float rotationDegrees = 0.0f)
{
    glm::mat4 model(1.0f);
    model = glm::translate(model, center);
    model = glm::rotate(model, glm::radians(rotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, size);
    model = glm::translate(model, glm::vec3(-0.5f));
    drawCube(cubeVAO, lightingShader, model, color.r, color.g, color.b);
}

static void drawDesk(unsigned int& cubeVAO, Shader& lightingShader,
    float x, float z, float width, float depth, float height)
{
    const glm::vec3 wood(0.48f, 0.25f, 0.12f);
    const glm::vec3 leg(0.30f, 0.16f, 0.08f);
    drawBlock(cubeVAO, lightingShader, glm::vec3(x, height, z),
        glm::vec3(width, 0.12f, depth), wood);

    const float legHeight = height - 0.06f;
    for (float sideX : {-1.0f, 1.0f})
    {
        for (float sideZ : {-1.0f, 1.0f})
        {
            drawBlock(cubeVAO, lightingShader,
                glm::vec3(x + sideX * (width * 0.42f), legHeight * 0.5f,
                    z + sideZ * (depth * 0.40f)),
                glm::vec3(0.09f, legHeight, 0.09f), leg);
        }
    }
}

static void drawChair(unsigned int& cubeVAO, Shader& lightingShader, float x, float z)
{
    const glm::vec3 wood(0.66f, 0.36f, 0.16f);
    const glm::vec3 leg(0.34f, 0.20f, 0.10f);
    drawBlock(cubeVAO, lightingShader, glm::vec3(x, 0.48f, z),
        glm::vec3(0.72f, 0.12f, 0.68f), wood);
    drawBlock(cubeVAO, lightingShader, glm::vec3(x, 0.86f, z - 0.28f),
        glm::vec3(0.72f, 0.66f, 0.12f), wood);

    for (float sideX : {-1.0f, 1.0f})
    {
        for (float sideZ : {-1.0f, 1.0f})
        {
            drawBlock(cubeVAO, lightingShader,
                glm::vec3(x + sideX * 0.28f, 0.23f, z + sideZ * 0.25f),
                glm::vec3(0.08f, 0.46f, 0.08f), leg);
        }
    }
}

static void classroom(unsigned int& cubeVAO, Shader& lightingShader)
{
    const glm::vec3 wall(0.78f, 0.78f, 0.70f);
    const glm::vec3 trim(0.38f, 0.19f, 0.09f);

    // Floor, three walls, and ceiling; the front remains open for viewing.
    drawBlock(cubeVAO, lightingShader, glm::vec3(0.0f, -0.08f, 0.0f),
        glm::vec3(8.0f, 0.16f, 8.0f), glm::vec3(0.56f, 0.47f, 0.34f));
    drawBlock(cubeVAO, lightingShader, glm::vec3(0.0f, 2.0f, 3.9f),
        glm::vec3(8.0f, 4.0f, 0.2f), wall);
    drawBlock(cubeVAO, lightingShader, glm::vec3(-3.9f, 2.0f, 0.0f),
        glm::vec3(0.2f, 4.0f, 8.0f), wall);
    drawBlock(cubeVAO, lightingShader, glm::vec3(3.9f, 2.0f, -3.65f),
        glm::vec3(0.2f, 4.0f, 0.7f), wall);
    drawBlock(cubeVAO, lightingShader, glm::vec3(3.9f, 2.0f, 1.4f),
        glm::vec3(0.2f, 4.0f, 5.2f), wall);
    drawBlock(cubeVAO, lightingShader, glm::vec3(3.9f, 3.4f, -2.25f),
        glm::vec3(0.2f, 1.2f, 2.1f), wall);
    drawBlock(cubeVAO, lightingShader, glm::vec3(0.0f, 4.08f, 0.0f),
        glm::vec3(8.0f, 0.16f, 8.0f), glm::vec3(0.84f, 0.85f, 0.82f));

    // Framed blackboard and its chalk rail.
    drawBlock(cubeVAO, lightingShader, glm::vec3(0.0f, 2.55f, 3.76f),
        glm::vec3(3.5f, 1.45f, 0.08f), glm::vec3(0.08f, 0.16f, 0.045f));
    drawBlock(cubeVAO, lightingShader, glm::vec3(0.0f, 3.32f, 3.70f),
        glm::vec3(3.72f, 0.12f, 0.12f), trim);
    drawBlock(cubeVAO, lightingShader, glm::vec3(0.0f, 1.78f, 3.70f),
        glm::vec3(3.72f, 0.12f, 0.12f), trim);
    drawBlock(cubeVAO, lightingShader, glm::vec3(-1.80f, 2.55f, 3.70f),
        glm::vec3(0.12f, 1.55f, 0.12f), trim);
    drawBlock(cubeVAO, lightingShader, glm::vec3(1.80f, 2.55f, 3.70f),
        glm::vec3(0.12f, 1.55f, 0.12f), trim);
    drawBlock(cubeVAO, lightingShader, glm::vec3(0.0f, 1.68f, 3.62f),
        glm::vec3(3.5f, 0.08f, 0.24f), glm::vec3(0.70f, 0.54f, 0.30f));

    // Sliding door panel, handle, and overhead track on the right wall.
    drawBlock(cubeVAO, lightingShader, glm::vec3(3.73f, 1.35f, -2.20f + doorSlide),
        glm::vec3(0.08f, 2.7f, 1.8f), glm::vec3(0.48f, 0.27f, 0.13f));
    drawBlock(cubeVAO, lightingShader, glm::vec3(3.66f, 1.35f, -1.55f + doorSlide),
        glm::vec3(0.06f, 0.28f, 0.06f), glm::vec3(0.82f, 0.72f, 0.48f));
    drawBlock(cubeVAO, lightingShader, glm::vec3(3.65f, 2.82f, -2.25f),
        glm::vec3(0.10f, 0.08f, 2.8f), glm::vec3(0.42f, 0.43f, 0.44f));

    // Rectangular ceiling lights.
    for (float x : {-2.3f, 2.3f})
    {
        drawBlock(cubeVAO, lightingShader, glm::vec3(x, 3.94f, 0.0f),
            glm::vec3(1.65f, 0.08f, 0.42f), glm::vec3(0.90f, 0.88f, 0.72f));
    }
    drawBlock(cubeVAO, lightingShader, glm::vec3(0.0f, 3.94f, -2.0f),
        glm::vec3(1.65f, 0.08f, 0.42f), glm::vec3(0.90f, 0.88f, 0.72f), 90.0f);

    // Teacher's desk and three student desks with block-built chairs.
    drawDesk(cubeVAO, lightingShader, 0.0f, 2.35f, 1.9f, 0.95f, 0.92f);
    drawChair(cubeVAO, lightingShader, 0.0f, 1.35f);
    drawDesk(cubeVAO, lightingShader, -1.75f, 0.0f, 1.25f, 0.82f, 0.78f);
    drawChair(cubeVAO, lightingShader, -1.75f, -0.88f);
    drawDesk(cubeVAO, lightingShader, 1.25f, 0.0f, 1.25f, 0.82f, 0.78f);
    drawChair(cubeVAO, lightingShader, 1.25f, -0.88f);
    drawDesk(cubeVAO, lightingShader, -0.25f, -2.25f, 1.25f, 0.82f, 0.78f);
    drawChair(cubeVAO, lightingShader, -0.25f, -3.13f);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float moveStep = 1.5f * deltaTime;
    const float scaleStep = 0.6f * deltaTime;
    const float rotateStep = 45.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        if (rotateAxis_X) rotateAngle_X -= rotateStep;
        else if (rotateAxis_Y) rotateAngle_Y -= rotateStep;
        else rotateAngle_Z -= rotateStep;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) translate_Y += moveStep;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) translate_Y -= moveStep;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) translate_X += moveStep;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) translate_X -= moveStep;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        doorSlide += 1.5f * deltaTime;
        if (doorSlide > 1.9f) doorSlide = 1.9f;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) translate_Z -= moveStep;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        doorSlide -= 1.5f * deltaTime;
        if (doorSlide < 0.0f) doorSlide = 0.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) scale_X -= scaleStep;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) scale_Y += scaleStep;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) scale_Y -= scaleStep;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) scale_Z += scaleStep;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) scale_Z -= scaleStep;
    if (scale_X < 0.1f) scale_X = 0.1f;
    if (scale_Y < 0.1f) scale_Y = 0.1f;
    if (scale_Z < 0.1f) scale_Z = 0.1f;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        rotateAngle_X += rotateStep;
        rotateAxis_X = 1.0;
        rotateAxis_Y = 0.0;
        rotateAxis_Z = 0.0;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        rotateAngle_Y += rotateStep;
        rotateAxis_X = 0.0;
        rotateAxis_Y = 1.0;
        rotateAxis_Z = 0.0;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        rotateAngle_Z += rotateStep;
        rotateAxis_X = 0.0;
        rotateAxis_Y = 0.0;
        rotateAxis_Z = 1.0;
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        eyeX += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        eyeX -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        eyeZ += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        eyeZ -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        eyeY += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        eyeY -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        if (pointLightOn)
        {
            pointlight1.turnOff();
            pointlight2.turnOff();
            pointlight3.turnOff();
            pointlight4.turnOff();
            pointLightOn = !pointLightOn;
        }
        else
        {
            pointlight1.turnOn();
            pointlight2.turnOn();
            pointlight3.turnOn();
            pointlight4.turnOn();
            pointLightOn = !pointLightOn;
        }
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        pointlight1.ambient += 0.1f;
        pointlight2.ambient += 0.1f;
        pointlight3.ambient += 0.1f;
        pointlight4.ambient += 0.1f;
    }

    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        pointlight1.ambient -= 0.1f;
        pointlight2.ambient -= 0.1f;
        pointlight3.ambient -= 0.1f;
        pointlight4.ambient -= 0.1f;
    }

    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        pointlight1.diffuse += 0.1f;
        pointlight2.diffuse += 0.1f;
        pointlight3.diffuse += 0.1f;
        pointlight4.diffuse += 0.1f;
    }

    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
    {
        pointlight1.diffuse -= 0.1f;
        pointlight2.diffuse -= 0.1f;
        pointlight3.diffuse -= 0.1f;
        pointlight4.diffuse -= 0.1f;
    }

    if (key == GLFW_KEY_6 && action == GLFW_PRESS)
    {
        pointlight1.specular += 0.1f;
        pointlight2.specular += 0.1f;
        pointlight3.specular += 0.1f;
        pointlight4.specular += 0.1f;
    }

    if (key == GLFW_KEY_7 && action == GLFW_PRESS)
    {
        pointlight1.specular -= 0.1f;
        pointlight2.specular -= 0.1f;
        pointlight3.specular -= 0.1f;
        pointlight4.specular -= 0.1f;
    }


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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

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

    //camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
