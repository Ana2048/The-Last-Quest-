#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "Dialog.hpp"
#include "SkyBox.hpp"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
GLint viewPosLoc=-1;
GLint timeSecLoc=-1;

GLint useTextureLoc;
GLint objectColorLoc;
GLint isPortalLoc;

// point lights
GLint pointCountLoc;
GLint pointPosLoc;
GLint pointColLoc;
GLint pointIntensityLoc=-1;

GLint pointConstantLoc;
GLint pointLinearLoc;
GLint pointQuadraticLoc;

//skybox
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

// camera
gps::Camera myCamera(
    glm::vec3(-20.0f, 5.0f, 60.0f),
    glm::vec3(0.0f, 5.0f, 0.0f),
    glm::vec3(0.0f, 5.0f, 0.0f));

GLfloat cameraSpeed = 10.0f;
static float cameraPitch = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastTime = 0.0f;
float frameRate = 60.f;
float groundY = 3.0f;
float eyeHeight = 5.0f;



glm::vec3 portalPos(0.0f, 0.0f, 0.0f);
glm::vec3 yenPos = portalPos + glm::vec3(-7.5f, 0.0f, 0.0f);
glm::vec3 savedCamPos;
glm::vec3 savedCamTarget;

bool ePressed = false;
bool showYenMessage = false;
bool cinematicMode = false;
bool vPressed = false;
float cinematicAngle = 0.0f;

glm::vec3 nightCityPos(550.0f, 8.0f, 250.0f);
glm::vec3 nightCityTarget(550.0f, 8.0f, 240.0f);
glm::vec3 carBasePos(550.0f, 20.0f, 260.0f);
float carScale = 1.5f;

GLboolean pressedKeys[1024];

// models
gps::Model3D teapot, portalRing, portalDisc, Yennefer,forest, nightCity, flyingCar;
GLfloat angle;
enum SceneId{
    SCENE_FOREST=0,
    SCENE_NIGHTCITY=1
};
SceneId currentScene = SCENE_FOREST;

// shaders
gps::Shader myBasicShader,shadowShader,dialogShader;
gps::Dialog dialogUI;
GLuint shadowFBO = 0;
GLuint shadowMap = 0;
const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

glm::mat4 lightSpaceMatrix;
GLint lightSpaceLoc = -1;
GLint shadowMapLoc = -1;


enum RenderMode {
    MODE_SOLID=0,
	MODE_WIREFRAME,
    MODE_POINTS
};


RenderMode currentMode = MODE_SOLID;
bool nPressed = false;

const int TORCH_COUNT = 5;
glm::vec3 torchPos[TORCH_COUNT] = {
    {  -13.8f, 6.0f,  -5.5f },
    { 9.84f,  6.0f ,  -4.19f },
    {  2.16f, 6.0f, -7.04f },
    { 4.192f, 6.0f, -2.0f },
    {  1.76f, 6.0f,  5.0f }
};
glm::vec3 torchCol[TORCH_COUNT] = {
    {1.0f, 0.25f, 0.05f},
    {1.0f, 0.25f, 0.05f},
    {1.0f, 0.25f, 0.05f},
    {1.0f, 0.25f, 0.05f},
    {1.0f, 0.30f, 0.08f}
};
float torchBaseIntensity[TORCH_COUNT] = { 2.5f, 2.5f, 2.5f, 2.5f, 3.0f };

//luminile din oras point lights din laborator
const int CITY_LIGHT_COUNT = 6;

glm::vec3 cityLightPos[CITY_LIGHT_COUNT] = {
    { 540.0f, 15.0f, 240.0f }, 
    { 560.0f, 20.0f, 260.0f }, 
    { 550.0f, 10.0f, 230.0f }, 
    { 575.0f, 12.0f, 245.0f }, 
    { 535.0f, 18.0f, 255.0f }, 
    { 555.0f, 5.0f,  275.0f }  
};

glm::vec3 cityLightCol[CITY_LIGHT_COUNT] = {
    { 1.0f, 0.4f, 0.0f }, 
    { 0.0f, 0.3f, 1.0f }, 
    { 0.8f, 0.8f, 1.0f }, 
    { 0.9f, 0.0f, 0.9f }, 
    { 0.0f, 0.9f, 0.9f }, 
    { 1.0f, 0.0f, 0.0f }  
};
void applyCameraToShader()
{
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}


void initSkybox() {
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/posx.tga"); // Right
    faces.push_back("skybox/negx.tga"); // Left
    faces.push_back("skybox/posy.tga"); // Top (Cerul)
    faces.push_back("skybox/negy.tga"); // Bottom (Podeaua)
    faces.push_back("skybox/posz.tga"); // Back
    faces.push_back("skybox/negz.tga"); // Front

    mySkyBox.Load(faces);
}

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    glViewport(0, 0, width, height);

	dialogUI.resize(width, height);

    projection = glm::perspective(
        glm::radians(60.0f),
        (float)width / (float)height,
        0.5f,
        500.0f
    );

    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = 512.0f;
    static float lastY = 384.0f;
    static bool firstMouse = true;

    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xOffset = lastX - (float)xpos;
    float yOffset = lastY - (float)ypos;

    lastX = (float)xpos;
    lastY = (float)ypos;

    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;


    if (cameraPitch + yOffset > 89.0f)
        yOffset = 89.0f - cameraPitch;
    if (cameraPitch + yOffset < -89.0f)
        yOffset = -89.0f - cameraPitch;

    cameraPitch += yOffset;

    myCamera.rotate(yOffset, xOffset);


    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

   // normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

float distTo(const glm::vec3& a, const glm::vec3& b) {
    return glm::length(a - b);
}

bool dialogActive = false;

void processMovement() {

    float speed = cameraSpeed * deltaTime;

    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, speed);
        glm::vec3 p = myCamera.getPosition();
        float minY = groundY + eyeHeight;
        if (p.y < minY) {
            p.y = minY;
            myCamera.setPosition(p);
            applyCameraToShader();
        }

        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
      
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, speed);
        glm::vec3 p = myCamera.getPosition();
        float minY = groundY + eyeHeight;
        if (p.y < minY) {
            p.y = minY;
            myCamera.setPosition(p);
            applyCameraToShader();
        }

        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, speed);
        glm::vec3 p = myCamera.getPosition();
        float minY = groundY + eyeHeight;
        if (p.y < minY) {
            p.y = minY;
            myCamera.setPosition(p);
            applyCameraToShader();
        }

        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, speed);
        glm::vec3 p = myCamera.getPosition();
        float minY = groundY + eyeHeight;
        if (p.y < minY) {
            p.y = minY;
            myCamera.setPosition(p);
            applyCameraToShader();
        }

        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        
    }


    if (pressedKeys[GLFW_KEY_N] && !nPressed) {
        nPressed = true;
        currentMode = (RenderMode)((currentMode + 1) % 3);

        myBasicShader.useShaderProgram();

        if (currentMode == MODE_SOLID) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);   
        }
        else if (currentMode == MODE_WIREFRAME) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else if (currentMode == MODE_POINTS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        }
    }
    if (!pressedKeys[GLFW_KEY_N]) nPressed = false;

    static bool fPressed = false;
    static int fogOn = 1;
    static GLint fogEnabledLocCached = -1;
    if (fogEnabledLocCached == -1)
        fogEnabledLocCached = glGetUniformLocation(myBasicShader.shaderProgram, "fogEnabled");

    if (pressedKeys[GLFW_KEY_F] && !fPressed) {
        fPressed = true;
        fogOn = 1 - fogOn;

        myBasicShader.useShaderProgram();
        glUniform1i(fogEnabledLocCached, fogOn);

        std::cout << "FOG: " << (fogOn ? "ON" : "OFF") << "\n";
    }
    if (!pressedKeys[GLFW_KEY_F]) fPressed = false;

    static bool ePressed = false;
    if (pressedKeys[GLFW_KEY_E] && !ePressed) {
        ePressed = true;

        
        if (dialogActive) {
            dialogActive = false;
            dialogUI.hide();              
            std::cout << "Dialog closed\n";
        }
        else {
            glm::vec3 yenWorldPos = yenPos + glm::vec3(-4.0f, 8.0f, -4.0f);
			glm::vec3 portalWorldPos = portalPos + glm::vec3(0.0f, 6.0f, 0.0f);
            
            float dPortal = glm::length(myCamera.getPosition() - portalWorldPos);
            float dYen = glm::length(myCamera.getPosition() - yenWorldPos);

            
            if (dYen < 6.5f ) {
                dialogActive = true;
                stbi_set_flip_vertically_on_load(true);
                dialogUI.show("textures/dialog/yen_dialog1.png");
                std::cout << "Talked to Yennefer!\n";
            }
            
            else if (dPortal < 8.0f) {
                currentScene = SCENE_NIGHTCITY;  
                myCamera.reset(nightCityPos, nightCityTarget);
                applyCameraToShader();
                std::cout << "Teleport -> Night City!\n";
            }
        }
    }
    if (!pressedKeys[GLFW_KEY_E]) ePressed = false;

    bool cinematicPause = false;
    bool pPressed = false;
    if (pressedKeys[GLFW_KEY_V] && !vPressed) {
        vPressed = true;
        cinematicMode = !cinematicMode;
        if (cinematicMode) {
            cinematicPause = false; 
            std::cout << "Cinematic Mode: ON\n";
        }
        else {
            if (currentScene == SCENE_FOREST) myCamera.reset(glm::vec3(-20, 5, 60), glm::vec3(0, 5, 0));
            else myCamera.reset(nightCityPos, nightCityTarget);
            applyCameraToShader();
        }
    }
    if (!pressedKeys[GLFW_KEY_V]) vPressed = false;
    if (cinematicMode) {
        if (pressedKeys[GLFW_KEY_P] && !pPressed) {
            pPressed = true;
            cinematicPause = !cinematicPause;
            std::cout << "Cinematic Pause: " << (cinematicPause ? "ON" : "OFF") << "\n";
        }
        if (!pressedKeys[GLFW_KEY_P]) pPressed = false;

        if (!cinematicPause) {
            cinematicAngle += 0.5f * deltaTime;
        }

        float radius = 90.0f;
        float height = (currentScene == SCENE_FOREST) ? 45.0f : 60.0f;
        float cx = (currentScene == SCENE_FOREST) ? 0.0f : 550.0f;
        float cz = (currentScene == SCENE_FOREST) ? 0.0f : 250.0f;
        glm::vec3 target = (currentScene == SCENE_FOREST) ? glm::vec3(0, 5, 0) : glm::vec3(550, 10, 250);

        float camX = cx + sin(cinematicAngle) * radius;
        float camZ = cz + cos(cinematicAngle) * radius;

        myCamera.setPosition(glm::vec3(camX, height, camZ));
        myCamera.reset(glm::vec3(camX, height, camZ), target);
        applyCameraToShader();
    }
}


void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
	glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenFramebuffers(1, &shadowFBO);//stocheaza distanta de la sursa de lumina pentru fiecare fragment
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);//pentru coordonate in afara texturii(shadow acne)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDepthFunc(GL_LEQUAL);

}

void initModels() {
    teapot.LoadModel("models/teapot/teapot20segUT.obj");
	portalRing.LoadModel("models/portalRing/portalRing2.obj");
	portalDisc.LoadModel("models/portalDisc/portalRing.obj");
	Yennefer.LoadModel("models/yen/yen2.obj");
	forest.LoadModel("models/forest/forest1.obj");
	nightCity.LoadModel("models/nightCity/nightcity.obj");
	flyingCar.LoadModel("models/car/car2.obj");

}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
	shadowShader.loadShader(
        "shaders/shadow.vert",
		"shaders/shadow.frag");
    dialogShader.loadShader(
        "shaders/dialog.vert",
		"shaders/dialog.frag");
    skyboxShader.loadShader(
        "shaders/skyboxShader.vert",
		"shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
}

void initUniforms() {
    myBasicShader.useShaderProgram();

    dialogUI.init(myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

    //FOG
	GLint fogColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogColor");
	GLint fogDensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
	GLint fogEnabledLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogEnabled");

	glUniform4f(fogColorLoc, 0.5f, 0.5f, 0.5f, 1.0f);
	glUniform1f(fogDensityLoc, 0.02f);
    glUniform1i(fogEnabledLoc, 1);
	// shading mode

	GLint shadingModeLoc = glGetUniformLocation(myBasicShader.shaderProgram, "shadingMode");
	glUniform1i(shadingModeLoc, currentMode);

    
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "diffuseTexture"), 0);

    // ia locatiile 
    useTextureLoc = glGetUniformLocation(myBasicShader.shaderProgram, "useTexture");
    objectColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "objectColor");
    isPortalLoc = glGetUniformLocation(myBasicShader.shaderProgram, "isPortal");
    timeSecLoc = glGetUniformLocation(myBasicShader.shaderProgram, "timeSec");//pentru animatia portalului
    viewPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "viewPos");

    // point light uniforms
    pointCountLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointCount");
    pointPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointPos");
    pointColLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointCol");
    pointIntensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointIntensity");

    pointConstantLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointConstant");
    pointLinearLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLinear");
    pointQuadraticLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointQuadratic");

    
    glUniform1i(useTextureLoc, 1);
    glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform1i(isPortalLoc, 0);

    //trimit date pentru point lights spre shader
    glUniform1i(pointCountLoc, TORCH_COUNT);
    glUniform3fv(pointPosLoc, TORCH_COUNT, glm::value_ptr(torchPos[0]));
    glUniform3fv(pointColLoc, TORCH_COUNT, glm::value_ptr(torchCol[0]));
    glUniform1fv(pointIntensityLoc, TORCH_COUNT, torchBaseIntensity);

    // atenuare (raza medie)
    glUniform1f(pointConstantLoc, 1.0f);
    glUniform1f(pointLinearLoc, 0.09f);
    glUniform1f(pointQuadraticLoc, 0.032f);



    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(60.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.5f, 500.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDir = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f));
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.2f, 1.15f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
}
glm::mat4 computeLightSpaceMatrix() {
    glm::vec3 dir = glm::normalize(lightDir);

	//aici am calculat sa vad unde e lumina in functie de scena
    glm::vec3 sceneCenter = (currentScene == SCENE_FOREST) ? glm::vec3(0, 0, 0) : glm::vec3(550.0f, 8.0f, 250.0f);

    //pozitia luminii fictiva
    glm::vec3 lightPos = sceneCenter - (dir * 100.0f);

    glm::mat4 lightView = glm::lookAt(lightPos, sceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));

    //calculezcat trebuie sa fie spatiu ca sa se cuprinda obiectele si sa fie umbrite sau nu
    float orthoSize = (currentScene == SCENE_FOREST) ? 60.0f : 250.0f;
    glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 0.1f, 300.0f);

    return lightProjection * lightView;
}

void renderTeapot(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    teapot.Draw(shader);
}
void renderPortal(gps::Shader shader)
{
    shader.useShaderProgram();
    glm::mat4 portalBase = glm::mat4(1.0f);
    portalBase = glm::translate(portalBase, portalPos);
    glm::mat4 ringModel = portalBase;
    ringModel = glm::translate(ringModel, glm::vec3(-0.03f, 0.5f, -1.2f));
    ringModel = glm::scale(ringModel, glm::vec3(4.5f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ringModel));
    glm::mat3 ringNormalMatrix = glm::mat3(glm::inverseTranspose(view * ringModel));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(ringNormalMatrix));
    glUniform1i(glGetUniformLocation(shader.shaderProgram, "isPortal"), 0);
    glUniform1i(glGetUniformLocation(shader.shaderProgram, "useTexture"), 1);
    glUniform3f(glGetUniformLocation(shader.shaderProgram, "objectColor"), 1.0f, 1.0f, 1.0f);

    portalRing.Draw(shader);

    float t = (float)glfwGetTime();

    // puls + un mic "flicker" (schimbi valorile daca vrei mai agresiv)
    float pulse = 0.92f + 0.08f * sin(t * 3.0f);     // 0.84..1.00
    float wobble = 1.00f + 0.02f * sin(t * 7.0f);    // 0.98..1.02
    float discScale = 1.25f * pulse * wobble;        // baza * animatie
    glm::mat4 discModel = portalBase;
    discModel = glm::translate(discModel, glm::vec3(0.3f, -0.2f, 1.1f));
    discModel = glm::scale(discModel, glm::vec3(discScale));
	discModel = glm::scale(discModel, glm::vec3(1.7f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(discModel));
    glm::mat3 discNormalMatrix = glm::mat3(glm::inverseTranspose(view * discModel));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(discNormalMatrix));

    glUniform1i(glGetUniformLocation(shader.shaderProgram, "isPortal"), 1);
    glUniform1i(glGetUniformLocation(shader.shaderProgram, "useTexture"), 0);

    glUniform3f(glGetUniformLocation(shader.shaderProgram, "objectColor"), 1.0f, 0.9f, 0.2f);

    portalDisc.Draw(shader);

    glUniform1i(glGetUniformLocation(shader.shaderProgram, "isPortal"), 0);
    glUniform1i(glGetUniformLocation(shader.shaderProgram, "useTexture"), 1);
    glUniform3f(glGetUniformLocation(shader.shaderProgram, "objectColor"), 1.0f, 1.0f, 1.0f);
}


void renderYennefer(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 yenModel = glm::mat4(1.0f);
    yenModel = glm::translate(yenModel, yenPos);
	yenModel = glm::translate(yenModel, glm::vec3(-4.0f, -2.0f,-4.0f));
	yenModel = glm::rotate(yenModel, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    yenModel = glm::scale(yenModel, glm::vec3(2.5f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(yenModel));
    glm::mat3 yenNormalMatrix = glm::mat3(glm::inverseTranspose(view * yenModel));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(yenNormalMatrix));

    Yennefer.Draw(shader);
}

void renderForest(gps::Shader shader) {
    shader.useShaderProgram();
    glm::mat4 forestModel = glm::mat4(1.0f);
    forestModel = glm::translate(forestModel, glm::vec3(0.0f, -1.0f, 0.0f));
	forestModel = glm::rotate(forestModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    forestModel = glm::scale(forestModel, glm::vec3(2.7f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(forestModel));
    glm::mat3 forestNormalMatrix = glm::mat3(glm::inverseTranspose(view * forestModel));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(forestNormalMatrix));
    forest.Draw(shader);
}

void renderNightCity(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 cityModel = glm::mat4(1.0f);
    cityModel = glm::translate(cityModel, glm::vec3(550.0f, 6.5f, 250.0f));
    cityModel = glm::scale(cityModel, glm::vec3(1.2f)); 
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cityModel));
    glm::mat3 cityNormalMatrix = glm::mat3(glm::inverseTranspose(view * cityModel));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(cityNormalMatrix));

    nightCity.Draw(shader);
}


glm::vec3 currentCarPos;

void renderFlyingCar(gps::Shader shader)
{
    shader.useShaderProgram();
    float t = (float)glfwGetTime();

    float distance = 40.0f; 
    glm::vec3 center = carBasePos;

    //se misca drept 
    glm::vec3 pos = center;
    pos.z = center.z + sin(t * 0.5f) * distance;
	pos.y = center.y + 2.0f + sin(t * 2.0f) * 0.5f; //mica oscilatie pe Y
    currentCarPos = pos;

    float direction = cos(t * 0.5f);
    float yaw = (direction > 0) ? 0.0f : glm::radians(180.0f);

    glm::mat4 carModel = glm::mat4(1.0f);
    carModel = glm::translate(carModel, pos);
    carModel = glm::rotate(carModel, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    carModel = glm::scale(carModel, glm::vec3(carScale));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(carModel));
    glm::mat3 carNormalMatrix = glm::mat3(glm::inverseTranspose(view * carModel));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(carNormalMatrix));

    glUniform1i(useTextureLoc, 1);
    glUniform1i(isPortalLoc, 0);
	glDisable(GL_CULL_FACE);
    flyingCar.Draw(shader);
}



void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (currentScene == SCENE_FOREST) {
        renderPortal(myBasicShader);
        renderYennefer(myBasicShader);
        renderForest(myBasicShader);
    }
    else { // SCENE_NIGHTCITY
        glm::mat4 viewStatic = glm::mat4(glm::mat3(myCamera.getViewMatrix()));
		mySkyBox.Draw(skyboxShader, view, projection);
		myBasicShader.useShaderProgram();
        renderNightCity(myBasicShader);
		glDepthMask(GL_FALSE); 
		renderFlyingCar(myBasicShader);
		glDepthMask(GL_TRUE);
        
    }
}


void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    initSkybox(); 
    setWindowCallbacks();

    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processMovement();

        lightSpaceMatrix = computeLightSpaceMatrix();
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        shadowShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(shadowShader.shaderProgram, "lightSpaceMatrix"),
            1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        if (currentScene == SCENE_FOREST) {
            // Randare Forest pentru umbre
            glm::mat4 modelF = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)), glm::radians(90.0f), glm::vec3(0, 1, 0));
            modelF = glm::scale(modelF, glm::vec3(2.7f));
            glUniformMatrix4fv(glGetUniformLocation(shadowShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelF));
            forest.Draw(shadowShader);
			glm::mat4 yenModel = glm::mat4(1.0f);
			yenModel = glm::translate(yenModel, yenPos);
			yenModel = glm::translate(yenModel, glm::vec3(-4.0f, -2.0f, -4.0f));
			yenModel = glm::rotate(yenModel, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			yenModel = glm::scale(yenModel, glm::vec3(2.5f));
			glUniformMatrix4fv(glGetUniformLocation(shadowShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(yenModel));
			Yennefer.Draw(shadowShader);
        }
        else if (currentScene == SCENE_NIGHTCITY) {
            glm::mat4 cityModel = glm::translate(glm::mat4(1.0f), glm::vec3(550.0f, 6.5f, 250.0f));
            cityModel = glm::scale(cityModel, glm::vec3(1.2f));
            glUniformMatrix4fv(glGetUniformLocation(shadowShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(cityModel));
            nightCity.Draw(shadowShader);

            float t = (float)glfwGetTime();
            float direction = cos(t * 0.5f);
            float yaw = (direction > 0) ? 0.0f : glm::radians(180.0f);

            glm::mat4 carModel = glm::translate(glm::mat4(1.0f), currentCarPos);
            carModel = glm::rotate(carModel, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
            carModel = glm::scale(carModel, glm::vec3(carScale));
            glUniformMatrix4fv(glGetUniformLocation(shadowShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(carModel));
            flyingCar.Draw(shadowShader);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 
        if (currentScene == SCENE_NIGHTCITY) {
            glm::mat4 viewStatic = glm::mat4(glm::mat3(myCamera.getViewMatrix()));
            mySkyBox.Draw(skyboxShader, viewStatic, projection);
        }
        myBasicShader.useShaderProgram();

        if (currentScene == SCENE_NIGHTCITY) {
            //luminile colorate
            glUniform1i(pointCountLoc, CITY_LIGHT_COUNT);
            glUniform3fv(pointPosLoc, CITY_LIGHT_COUNT, glm::value_ptr(cityLightPos[0]));
            glUniform3fv(pointColLoc, CITY_LIGHT_COUNT, glm::value_ptr(cityLightCol[0]));

            float cityIntensities[CITY_LIGHT_COUNT] = { 5.0f, 6.0f, 4.0f, 7.0f, 5.0f, 4.0f };
            glUniform1fv(pointIntensityLoc, CITY_LIGHT_COUNT, cityIntensities);

            glUniform1f(pointConstantLoc, 1.0f);
            glUniform1f(pointLinearLoc, 0.045f);
            glUniform1f(pointQuadraticLoc, 0.0075f);

            glUniform3f(lightColorLoc, 0.4f, 0.4f, 0.4f);
        }
        else {
            glUniform1i(pointCountLoc, TORCH_COUNT);
            glUniform3fv(pointPosLoc, TORCH_COUNT, glm::value_ptr(torchPos[0]));
            glUniform3fv(pointColLoc, TORCH_COUNT, glm::value_ptr(torchCol[0]));

            glUniform1f(pointConstantLoc, 1.0f);
            glUniform1f(pointLinearLoc, 0.09f);
            glUniform1f(pointQuadraticLoc, 0.032f);
        }

        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(myCamera.getViewMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(myCamera.getPosition()));
        glUniform1f(timeSecLoc, currentFrame);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 1);

        float flick = 0.85f + 0.15f * sin(currentFrame * 10.0f);
        float tmpIntensity[TORCH_COUNT];
        for (int i = 0; i < TORCH_COUNT; i++) tmpIntensity[i] = torchBaseIntensity[i] * flick;
        glUniform1fv(pointIntensityLoc, TORCH_COUNT, tmpIntensity);

        if (currentScene == SCENE_FOREST) {
            renderForest(myBasicShader);
            renderYennefer(myBasicShader);
            renderPortal(myBasicShader);
        }
        else {
            renderNightCity(myBasicShader);
            renderFlyingCar(myBasicShader);
        }

        if (dialogActive) {
            dialogUI.render(dialogShader);
        }

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());
        glCheckError();
    }

    cleanup();
    return EXIT_SUCCESS;
}