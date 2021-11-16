//----------------------------------------------------------------------------------------
/**
 * \file    river.cpp
 * \author  Jiøí Povolný
 * \date    2021/5/20
 * \brief   Simple implementaion of River scene.
*/
//----------------------------------------------------------------------------------------

#include <time.h>
#include <list>
#include <iostream>
#include "pgr.h"
#include "render_stuff.h"
#include "spline.h"

extern SCommonShaderProgram shaderProgram;
extern bool useLighting;

typedef std::list<MovingObject*> MovingObjectsList;
typedef std::list<Object*> ObjectsList;

/// Enum for static cameras
enum class StaticCameras {
    Static_1,   ///< 1st static camera
    Static_2,   ///< 2nd static camera
    Dynamic     ///< Dynamic camera, 3rd person view of drone
};

/// Struct for current game/scene state
struct GameState {
    int windowWidth;                            ///< set by reshape callback
    int windowHeight;                           ///< set by reshape callback

    bool freeCameraMode;
    float cameraElevationAngle;                 ///< in degrees = initially 0.0f
    float cameraViewAngle;
    const float cameraSpeed = CAMERA_SPEED;
    glm::vec3 cameraPosition;
    glm::vec3 cameraDirection;
    StaticCameras staticCamera;

    bool keyMap[KEYS_COUNT];

    float elapsedTime;

    bool reflectorOn;
    bool sunOn;
    bool fogOn;
    bool fireOn;
    bool duckOn;
} gameState;

/// Struct for game/scene objects
struct GameObjects {
    MovingObjectsList fish;
    ObjectsList kapradi;

    MovingObject* drone;
    Object* ground;
    Object* fireplace;
    Object* bench;
    Object* duck;
    Object* woodpile;
    Object* woodtrunk;
    Object* stone;
    Object* crate;
    Object* tulip;
    Object* palmtree;
    Object* snowdrop;
    Object* crocus;
    Object* sunflower;
    Fire* fire;
} gameObjects;

/// Checks whether a given position is inside the scene
/**
    \param[in] currentPosition  Position to be tested
    \return Corrected position
*/
glm::vec3 checkBoundaries(const glm::vec3& currentPosition) {
    glm::vec3 newPosition = currentPosition;

    if (newPosition.x > SCENE_WIDTH) {
        newPosition.x = SCENE_WIDTH;
    }
    else if (newPosition.x < -SCENE_WIDTH) {
        newPosition.x = -SCENE_WIDTH;
    }

    if (newPosition.y > SCENE_HEIGHT) {
        newPosition.y = SCENE_HEIGHT;
    }
    else if (newPosition.y < -SCENE_HEIGHT) {
        newPosition.y = -SCENE_HEIGHT;
    }

    return newPosition;
}

/// Checks whether a given position is inside the bench
/**
    \param[in] currentPosition  Position to be tested
    \return True if position is inside the bench, otherwise false
*/
bool checkCollision(const glm::vec3& currentPosition) {
    if (currentPosition.x <= BENCH_MIDDLE_X + BENCH_WIDTH &&
        currentPosition.x >= BENCH_MIDDLE_X - BENCH_WIDTH &&
        currentPosition.y <= BENCH_MIDDLE_Y + BENCH_HEIGHT &&
        currentPosition.y >= BENCH_MIDDLE_Y - BENCH_HEIGHT) {
        return true;
    }

    return false;
}

void staticCamera_1() {
    gameState.staticCamera = StaticCameras::Static_1;
    gameState.cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f); 
    gameState.cameraDirection = glm::vec3(0.0f, 1.0f, 0.0f);
    gameState.cameraViewAngle = 90.0f;
}

void staticCamera_2() {
    gameState.staticCamera = StaticCameras::Static_2;
    gameState.cameraPosition = glm::vec3(1.5f, 0.0f, 0.0f);
    gameState.cameraDirection = glm::vec3(-1.0f, 0.0f, 0.0f);
    gameState.cameraViewAngle = 180.0f;
}

void dynamicCamera() {
    gameState.staticCamera = StaticCameras::Dynamic;
    gameState.cameraPosition = gameObjects.drone->position - gameObjects.drone->direction;
    gameState.cameraDirection = gameObjects.drone->direction;
    gameState.cameraViewAngle = 90.0f;
}

void leaveDynamicCamera() {
    gameState.cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    gameState.cameraDirection = glm::vec3(0.0f, 1.0f, 0.0f);
    gameState.cameraViewAngle = 90.0f;
}

Fire* insertFire(const glm::vec3& position) {
    Fire* newFire = new Fire;

    newFire->speed = 0.0f;

    newFire->startTime = gameState.elapsedTime;
    newFire->currentTime = newFire->startTime;

    newFire->size = FIRE_BILLBOARD_SIZE;
    newFire->direction = glm::vec3(0.0f, 0.0f, 1.0f);

    newFire->frameDuration = 0.1f;
    newFire->textureFrames = 16;

    newFire->position = position;

    return newFire;
}

void moveCameraForward() {
    const glm::vec3 newCameraPosition = gameState.cameraPosition + gameState.cameraSpeed * gameState.cameraDirection;

    if (!checkCollision(newCameraPosition)) {
        gameState.cameraPosition = newCameraPosition;
    }
}

void moveCameraBackwards() {
    const glm::vec3 newCameraPosition = gameState.cameraPosition - gameState.cameraSpeed * gameState.cameraDirection;

    if (!checkCollision(newCameraPosition)) {
        gameState.cameraPosition = newCameraPosition;
    }
}

void moveCameraLeft() {
    const glm::vec3 newCameraPosition = gameState.cameraPosition + gameState.cameraSpeed * glm::vec3(-gameState.cameraDirection.y, gameState.cameraDirection.x, gameState.cameraDirection.z);

    if (!checkCollision(newCameraPosition)) {
        gameState.cameraPosition = newCameraPosition;
    }
}

void moveCameraRight() {
    const glm::vec3 newCameraPosition = gameState.cameraPosition - gameState.cameraSpeed * glm::vec3(-gameState.cameraDirection.y, gameState.cameraDirection.x, gameState.cameraDirection.z);

    if (!checkCollision(newCameraPosition)) {
        gameState.cameraPosition = newCameraPosition;
    }
}

void cleanUpObjects(void) {
    while(!gameObjects.fish.empty()) {
        delete gameObjects.fish.back();
        gameObjects.fish.pop_back();
    } 

    while (!gameObjects.kapradi.empty()) {
        delete gameObjects.kapradi.back();
        gameObjects.kapradi.pop_back();
    }

    if (gameObjects.fire != NULL) {
        delete gameObjects.fire;
        gameObjects.fire = NULL;
    }

    if (gameObjects.fireplace != NULL) {
        delete gameObjects.fireplace;
        gameObjects.fireplace = NULL;
    }

    if (gameObjects.ground != NULL) {
        delete gameObjects.ground;
        gameObjects.ground = NULL;
    }

    if (gameObjects.bench != NULL) {
        delete gameObjects.bench;
        gameObjects.bench = NULL;
    }

    if (gameObjects.drone != NULL) {
        delete gameObjects.drone;
        gameObjects.drone = NULL;
    }

    if (gameObjects.duck != NULL) {
        delete gameObjects.duck;
        gameObjects.duck = NULL;
    }

    if (gameObjects.woodpile != NULL) {
        delete gameObjects.woodpile;
        gameObjects.woodpile = NULL;
    }

    if (gameObjects.woodtrunk != NULL) {
        delete gameObjects.woodtrunk;
        gameObjects.woodtrunk = NULL;
    }

    if (gameObjects.stone != NULL) {
        delete gameObjects.stone;
        gameObjects.stone = NULL;
    }

    if (gameObjects.crate != NULL) {
        delete gameObjects.crate;
        gameObjects.crate = NULL;
    }

    if (gameObjects.tulip != NULL) {
        delete gameObjects.tulip;
        gameObjects.tulip = NULL;
    }

    if (gameObjects.palmtree != NULL) {
        delete gameObjects.palmtree;
        gameObjects.palmtree = NULL;
    }

    if (gameObjects.snowdrop != NULL) {
        delete gameObjects.snowdrop;
        gameObjects.snowdrop = NULL;
    }

    if (gameObjects.crocus != NULL) {
        delete gameObjects.crocus;
        gameObjects.crocus = NULL;
    }

    if (gameObjects.sunflower != NULL) {
        delete gameObjects.sunflower;
        gameObjects.sunflower = NULL;
    }
}

MovingObject* createFish(glm::vec3 position, float size, float max_speed) {
    MovingObject* newFish = new MovingObject();

    newFish->destroyed = false;
    newFish->objectOn = true;

    newFish->startTime = gameState.elapsedTime;
    newFish->currentTime = newFish->startTime;

    newFish->size = size;

    newFish->initPosition = position;
    newFish->position = newFish->initPosition;

    newFish->speed = max_speed * (float)(rand() / (double)RAND_MAX);

    /// generate randomly in range -1.0f ... 1.0f
    newFish->direction = glm::vec3(
        (float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
        (float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
        0.0f
    );
    newFish->direction = glm::normalize(newFish->direction);

    return newFish;
}

MovingObject* createDrone(glm::vec3 position, float size, float speed) {
    MovingObject* newDrone = new MovingObject();

    newDrone->destroyed = false;
    newDrone->objectOn = true;

    newDrone->startTime = gameState.elapsedTime;
    newDrone->currentTime = newDrone->startTime;

    newDrone->size = size;

    newDrone->initPosition = position;
    newDrone->position = newDrone->initPosition;

    newDrone->speed = speed;

    newDrone->direction = glm::vec3(0.5f, 0.0f, 0.0f);
    newDrone->direction = glm::normalize(newDrone->direction);

    return newDrone;
}

/// Create and set parameters of object
/**
    \param[in] position Position of object
    \param[in] size     Size of object
*/
Object* createObject(glm::vec3 position, float size) {
    Object* newObject = new Object();

    newObject->destroyed = false;

    newObject->startTime = gameState.elapsedTime;
    newObject->currentTime = newObject->startTime;

    newObject->size = size;

    newObject->direction = glm::vec3(0.0f, 0.0f, 1.0f);
    newObject->direction = glm::normalize(newObject->direction);

    newObject->position = position;

    return newObject;
}

void restartGame(void) {
    cleanUpObjects();

    gameState.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); // milliseconds => seconds

    Object* ground = createObject(glm::vec3(GROUND_POSITION), GROUND_SIZE);
    gameObjects.ground = ground;

    Object* fireplace = createObject(glm::vec3(FIREPLACE_POSITION), FIREPLACE_SIZE);
    gameObjects.fireplace = fireplace;

    Object* bench = createObject(glm::vec3(BENCH_POSITION), BENCH_SIZE);
    gameObjects.bench = bench;

    Object* duck = createObject(glm::vec3(DUCK_POSITION), DUCK_SIZE);
    gameObjects.duck = duck;

    Object* woodpile = createObject(glm::vec3(WOODPILE_POSITION), WOODPILE_SIZE);
    gameObjects.woodpile = woodpile;

    Object* woodtrunk = createObject(glm::vec3(WOODTRUNK_POSITION), WOODTRUNK_SIZE);
    gameObjects.woodtrunk = woodtrunk;

    Object* stone = createObject(glm::vec3(STONE_POSITION), STONE_SIZE);
    gameObjects.stone = stone;

    Object* crate = createObject(glm::vec3(CRATE_POSITION), CRATE_SIZE);
    gameObjects.crate = crate;

    Object* tulip = createObject(glm::vec3(TULIP_POSITION), TULIP_SIZE);
    gameObjects.tulip = tulip;

    Object* palmtree = createObject(glm::vec3(PALMTREE_POSITION), PALMTREE_SIZE);
    gameObjects.palmtree = palmtree;

    Object* snowdrop = createObject(glm::vec3(SNOWDROP_POSITION), SNOWDROP_SIZE);
    gameObjects.snowdrop = snowdrop;

    Object* crocus = createObject(glm::vec3(CROCUS_POSITION), CROCUS_SIZE);
    gameObjects.crocus = crocus;

    Object* sunflower = createObject(glm::vec3(SUNFLOWER_POSITION), SUNFLOWER_SIZE);
    gameObjects.sunflower = sunflower;

    MovingObject* drone = createDrone(glm::vec3(DRONE_POSITION), DRONE_SIZE, DRONE_SPEED);
    gameObjects.drone = drone;

    for (int i = 0; i < FISH_COUNT; i++) {
        MovingObject* newFish = createFish(fishPositions[i], FISH_SIZE, FISH_SPEED_MAX);

        gameObjects.fish.push_back(newFish);
    }

    for (int i = 0; i < KAPRADI_COUNT; i++) {
        Object* newKapradi = createObject(kapradiPositions[i], KAPRADI_SIZE);

        gameObjects.kapradi.push_back(newKapradi);
    }

    if(gameState.freeCameraMode == true) {
        gameState.freeCameraMode = false;
        glutPassiveMotionFunc(NULL);
    }

    staticCamera_1();
    gameState.cameraElevationAngle = 0.0f;

    // reset key map
    for(int i=0; i<KEYS_COUNT; i++)
        gameState.keyMap[i] = false;

    if (gameObjects.fire == NULL) {
        glm::vec3 pos = gameObjects.fireplace->position;
        pos.z = 0.00f;
        gameObjects.fire = insertFire(pos);
    }

    gameState.fogOn = false;
    disableFog();
    gameState.fireOn = false;
    gameState.duckOn = false;
    gameState.sunOn = true;
    gameState.reflectorOn = false;
}

void drawWindowContents() {
    // setup parallel projection
    const glm::mat4 orthoProjectionMatrix = glm::ortho(
        -SCENE_WIDTH, SCENE_WIDTH,
        -SCENE_HEIGHT, SCENE_HEIGHT,
        -10.0f*SCENE_DEPTH, 10.0f*SCENE_DEPTH
    );
    // static viewpoint
    const glm::mat4 orthoViewMatrix = glm::lookAt(
        gameState.cameraPosition,
        gameState.cameraPosition + gameState.cameraDirection,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    // setup camera & projection transform
    glm::mat4 viewMatrix  = orthoViewMatrix;
    glm::mat4 projectionMatrix = orthoProjectionMatrix;

    projectionMatrix = glm::perspective(glm::radians(60.0f), gameState.windowWidth / (float)gameState.windowHeight, 0.1f, 10.0f);

    if(gameState.freeCameraMode == true) {

        const glm::vec3 cameraPosition = gameState.cameraPosition;
        glm::vec3 cameraUpVector = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 cameraCenter;

        glm::vec3 cameraViewDirection = gameState.cameraDirection;

        const glm::vec3 rotationAxis = glm::cross(cameraViewDirection, glm::vec3(0.0f, 0.0f, 1.0f));
        const glm::mat4 cameraTransform = glm::rotate(glm::mat4(1.0f), glm::radians(gameState.cameraElevationAngle), rotationAxis);

        cameraUpVector = glm::vec3(cameraTransform * glm::vec4(cameraUpVector, 0.0f));
        cameraViewDirection = glm::vec3(cameraTransform * glm::vec4(cameraViewDirection, 0.0f));

        cameraCenter = cameraPosition + cameraViewDirection;

        viewMatrix = glm::lookAt(
            cameraPosition,
            cameraCenter,
            cameraUpVector
        );

    } else {
        gameState.cameraElevationAngle = 0.0f;

        if (gameState.staticCamera == StaticCameras::Static_1) {
            viewMatrix = glm::lookAt(
                gameState.cameraPosition,
                gameState.cameraPosition + gameState.cameraDirection,
                glm::vec3(0.0f, 0.0f, 1.0f)
            );
        }
        else if (gameState.staticCamera == StaticCameras::Static_2) {
            viewMatrix = glm::lookAt(
                gameState.cameraPosition,
                gameState.cameraPosition + gameState.cameraDirection,
                glm::vec3(0.0f, 0.0f, 1.0f)
            );
        }
        else if (gameState.staticCamera == StaticCameras::Dynamic) {
            viewMatrix = glm::lookAt(
                gameState.cameraPosition,
                gameState.cameraPosition + gameState.cameraDirection,
                glm::vec3(0.0f, 0.0f, 1.0f)
            );
        }
    }

    glUseProgram(shaderProgram.program);
    glUniform1f(shaderProgram.timeLocation, gameState.elapsedTime);
    CHECK_GL_ERROR();

    setLightValues(
        viewMatrix, 
        gameState.elapsedTime, 
        gameState.sunOn, 
        gameState.reflectorOn, 
        gameState.cameraPosition, 
        gameState.cameraDirection, 
        gameState.fireOn, 
        gameObjects.fire->position
    );

    glUniform1i(shaderProgram.duckOn, gameState.duckOn);
    glUniform1i(shaderProgram.fogOn, gameState.fogOn);

    const float fogTimeFactor = abs(sin(gameState.elapsedTime * FOG_SPEED));
    const float fogDensity = glm::mix(1.0f, 0.1f, fogTimeFactor);

    glUniform1f(shaderProgram.fogDensity, fogDensity);
    setLightUniforms();
    CHECK_GL_ERROR();
    glUseProgram(0);

    drawObject(gameObjects.ground, groundGeometry, viewMatrix, projectionMatrix);

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    CHECK_GL_ERROR();
    glStencilFunc(GL_ALWAYS, 1, -1);
    CHECK_GL_ERROR();
    drawObject(gameObjects.fireplace, fireplaceGeometry, viewMatrix, projectionMatrix);
    glDisable(GL_STENCIL_TEST);

    drawObject(gameObjects.bench, benchGeometry, viewMatrix, projectionMatrix, BENCH_ROTATION_DEGREES, glm::vec3(BENCH_ROTATION_AXIS));

    drawObject(gameObjects.drone, droneGeometry, viewMatrix, projectionMatrix);

    drawObject(gameObjects.woodpile, woodpileGeometry, viewMatrix, projectionMatrix);

    drawObject(gameObjects.woodtrunk, woodtrunkGeometry, viewMatrix, projectionMatrix);

    drawObject(gameObjects.stone, stoneGeometry, viewMatrix, projectionMatrix);

    drawObject(gameObjects.crate, crateGeometry, viewMatrix, projectionMatrix);

    drawObject(gameObjects.tulip, tulipGeometry, viewMatrix, projectionMatrix, TULIP_ROTATION_DEGREES);

    drawObject(gameObjects.palmtree, palmtreeGeometry, viewMatrix, projectionMatrix, PALMTREE_ROTATION_DEGREES);

    drawObject(gameObjects.snowdrop, snowdropGeometry, viewMatrix, projectionMatrix, SNOWDROP_ROTATION_DEGREES);

    drawObject(gameObjects.crocus, crocusGeometry, viewMatrix, projectionMatrix, CROCUS_ROTATION_DEGREES);

    drawObject(gameObjects.sunflower, sunflowerGeometry, viewMatrix, projectionMatrix, SUNFLOWER_ROTATION_DEGREES);

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    CHECK_GL_ERROR();
    glStencilFunc(GL_ALWAYS, 2, -1);
    CHECK_GL_ERROR();
    drawDuck(gameObjects.duck, viewMatrix, projectionMatrix);
    glDisable(GL_STENCIL_TEST);

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    CHECK_GL_ERROR();
    int id = 3;
    // draw fish
    for(MovingObjectsList::iterator it = gameObjects.fish.begin(); it != gameObjects.fish.end(); ++it) {
        glStencilFunc(GL_ALWAYS, id, -1);
        CHECK_GL_ERROR();

        MovingObject* fish = (*it);
        drawObject(fish, fishGeometry, viewMatrix, projectionMatrix);

        id++;
    }
    glDisable(GL_STENCIL_TEST);

    for (ObjectsList::iterator it = gameObjects.kapradi.begin(); it != gameObjects.kapradi.end(); ++it) {
        Object* kapradi = (*it);
        drawObject(kapradi, kapradiGeometry, viewMatrix, projectionMatrix);
    }

    // draw skybox
    drawSkybox(viewMatrix, projectionMatrix); 

    drawWater(gameState.elapsedTime, viewMatrix, projectionMatrix);

    if (gameState.fireOn == true) {
        drawFire(gameObjects.fire, viewMatrix, projectionMatrix);
    }
}

/// Called to update the display. You should call glutSwapBuffers after all of your rendering to display what you rendered.
void displayCallback() {
    GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    mask |= GL_STENCIL_BUFFER_BIT;

    glClear(mask);

    drawWindowContents();

    glutSwapBuffers();
}

/// Called whenever the window is resized. The new window size is given, in pixels.
/// This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshapeCallback(int newWidth, int newHeight) {

    gameState.windowWidth = newWidth;
    gameState.windowHeight = newHeight;

    glViewport(0, 0, (GLsizei) newWidth, (GLsizei) newHeight);
}

void updateObjects(float elapsedTime) {
    if (gameState.freeCameraMode == true) {
        gameState.cameraPosition = checkBoundaries(gameState.cameraPosition);
    }

    for (MovingObjectsList::iterator it = gameObjects.fish.begin(); it != gameObjects.fish.end(); ++it) {
        MovingObject* fish = (*it);

        if (fish->objectOn) {
            fish->currentTime = elapsedTime;

            const float curveParamT = fish->speed * (fish->currentTime - fish->startTime);

            fish->position = fish->initPosition + evaluateClosedCurve(fishCurveData, fishCurveSize, curveParamT);
            fish->direction = glm::normalize(evaluateClosedCurve_1stDerivative(fishCurveData, fishCurveSize, curveParamT));
        }
    }

    if (gameObjects.drone != NULL) {
        MovingObject* drone = gameObjects.drone;

        drone->currentTime = elapsedTime;

        const float curveParamT = drone->speed * (drone->currentTime - drone->startTime);

        drone->position = drone->initPosition + evaluateClosedCurve(droneCurveData, droneCurveSize, curveParamT);
        drone->direction = glm::normalize(evaluateClosedCurve_1stDerivative(droneCurveData, droneCurveSize, curveParamT));

        if (gameState.freeCameraMode == false && gameState.staticCamera == StaticCameras::Dynamic) {
            gameState.cameraPosition = drone->position - drone->direction;
            gameState.cameraDirection = drone->direction;
        }
    }

    if (gameObjects.duck != NULL) {
        gameObjects.duck->currentTime = elapsedTime;
    }

    if (gameObjects.fire != NULL) {
        gameObjects.fire->currentTime = elapsedTime;
    }
}

/// Callback responsible for the scene update
void timerCallback(int) {
    // update scene time
    gameState.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); // milliseconds => seconds

    // call appropriate actions according to the currently pressed keys in key map
    // (combinations of keys are supported but not used in this implementation)
    if(gameState.keyMap[KEY_RIGHT_ARROW] == true)
        moveCameraRight();

    if(gameState.keyMap[KEY_LEFT_ARROW] == true)
        moveCameraLeft();

    if(gameState.keyMap[KEY_UP_ARROW] == true)
        moveCameraForward();

    if(gameState.keyMap[KEY_DOWN_ARROW] == true)
        moveCameraBackwards();

    // update objects in the scene
    updateObjects(gameState.elapsedTime);

    // set timeCallback next invocation
    glutTimerFunc(33, timerCallback, 0);

    glutPostRedisplay();
}

/// Called when mouse is moving while no mouse buttons are pressed.
void passiveMouseMotionCallback(int mouseX, int mouseY) {
    if(mouseY != gameState.windowHeight/2) {
        const float cameraElevationAngleDelta = 0.5f * (mouseY - gameState.windowHeight/2);

        if(fabs(gameState.cameraElevationAngle+cameraElevationAngleDelta) < CAMERA_ELEVATION_MAX)
            gameState.cameraElevationAngle += cameraElevationAngleDelta;

        // set mouse pointer to the window center
        glutWarpPointer(gameState.windowWidth/2, gameState.windowHeight/2);

        glutPostRedisplay();
    }

    if (mouseX != gameState.windowWidth / 2) {
        const float cameraViewAngleDelta = 0.5f * (mouseX - gameState.windowWidth/2);

        if (cameraViewAngleDelta < 0) {
            gameState.cameraViewAngle -= cameraViewAngleDelta;

            if (gameState.cameraViewAngle >= 360) {
                gameState.cameraViewAngle -= 360;
            }
        }
        else {
            gameState.cameraViewAngle -= cameraViewAngleDelta;

            if (gameState.cameraViewAngle < 0) {
                gameState.cameraViewAngle += 360;
            }
        }

        gameState.cameraDirection = glm::vec3(cos(glm::radians(gameState.cameraViewAngle)), sin(glm::radians(gameState.cameraViewAngle)), 0.0f);

        // set mouse pointer to the window center
        glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);

        glutPostRedisplay();
    }
}

/// Called whenever a key on the keyboard was pressed. The key is given by the "keyPressed"
/// parameter, which is in ASCII. It's often a good idea to have the escape key (ASCII value 27)
/// to call glutLeaveMainLoop() to exit the program.
void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY) {
    switch(keyPressed) {
    case 27: // escape
#ifndef __APPLE__
        glutLeaveMainLoop();
#else
        exit(0);
#endif
        break;
    case 'r': // restart game
        restartGame();
        break;
    case ' ':
        if (gameState.freeCameraMode == false) {
            if (gameState.staticCamera == StaticCameras::Static_1) {
                staticCamera_2();
            }
            else if (gameState.staticCamera == StaticCameras::Static_2) {
                dynamicCamera();
            }
            else if (gameState.staticCamera == StaticCameras::Dynamic) {
                staticCamera_1();
            }
        }
        break;
    case 'c': // switch camera
        gameState.freeCameraMode = !gameState.freeCameraMode;
        if(gameState.freeCameraMode == true) {
            glutPassiveMotionFunc(passiveMouseMotionCallback);
            glutWarpPointer(gameState.windowWidth/2, gameState.windowHeight/2);

            if (gameState.staticCamera == StaticCameras::Dynamic) {
                leaveDynamicCamera();
            }
        }
        else {
            glutPassiveMotionFunc(NULL);
        }
        break;
    case 'f':
        gameState.reflectorOn = !gameState.reflectorOn;
        break;
    case 'e':
        gameState.fogOn = !gameState.fogOn;
        changeFog();
        break;
    case 'q':
        gameState.sunOn = !gameState.sunOn;
        break;
    default:
        break; // printf("Unrecognized key pressed\n");
    }
}

/// Called whenever a key on the keyboard was released. The key is given by
/// the "keyReleased" parameter, which is in ASCII. 
void keyboardUpCallback(unsigned char keyReleased, int mouseX, int mouseY) {
    switch(keyReleased) {
    default:
        break; // printf("Unrecognized key released\n");
    }
}

/// The special keyboard callback is triggered when keyboard function or directional
/// keys are pressed.
void specialKeyboardCallback(int specKeyPressed, int mouseX, int mouseY) {
    if (gameState.freeCameraMode == true) {
        switch (specKeyPressed) {
        case GLUT_KEY_RIGHT:
            gameState.keyMap[KEY_RIGHT_ARROW] = true;
            break;
        case GLUT_KEY_LEFT:
            gameState.keyMap[KEY_LEFT_ARROW] = true;
            break;
        case GLUT_KEY_UP:
            gameState.keyMap[KEY_UP_ARROW] = true;
            break;
        case GLUT_KEY_DOWN:
            gameState.keyMap[KEY_DOWN_ARROW] = true;
            break;
        default:
            break; // printf("Unrecognized special key pressed\n");
        }
    }
}

/// The special keyboard callback is triggered when keyboard function or directional
/// keys are released.
void specialKeyboardUpCallback(int specKeyReleased, int mouseX, int mouseY) {
    if (gameState.freeCameraMode == true) {
        switch (specKeyReleased) {
        case GLUT_KEY_RIGHT:
            gameState.keyMap[KEY_RIGHT_ARROW] = false;
            break;
        case GLUT_KEY_LEFT:
            gameState.keyMap[KEY_LEFT_ARROW] = false;
            break;
        case GLUT_KEY_UP:
            gameState.keyMap[KEY_UP_ARROW] = false;
            break;
        case GLUT_KEY_DOWN:
            gameState.keyMap[KEY_DOWN_ARROW] = false;
            break;
        default:
            break; // printf("Unrecognized special key released\n");
        }
    }
}

/// When a user presses and releases mouse buttons in the window, each press
/// and each release generates a mouse callback.
void mouseCallback(int buttonPressed, int buttonState, int mouseX, int mouseY ) {
    // do picking only on mouse down
    if( ( buttonPressed == GLUT_LEFT_BUTTON ) && ( buttonState == GLUT_DOWN ) ) {

        // store value from the stencil buffer (byte)
        unsigned char objectID = 0; 

        glReadPixels(mouseX, gameState.windowHeight - 1 - mouseY, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &objectID);

        // the buffer was initially cleared to zeros
        if(objectID == 0) {
   		    // background was clicked
            printf("Clicked on background\n");
        }
        else {
            // object was clicked
            printf("Clicked on object with ID: %d\n", (int)objectID);

            if (objectID == 1) {
                gameState.fireOn = !gameState.fireOn;
            }
            else if (objectID == 2) {
                gameState.duckOn = !gameState.duckOn;
                gameObjects.duck->startTime = gameState.elapsedTime;
            }
            else if (objectID == 3) {
                MovingObjectsList::iterator it = gameObjects.fish.begin();
                MovingObject * fish = (*it);

                fish->objectOn = !fish->objectOn;
                fish->startTime = gameState.elapsedTime - fish->startTime;
            }
            else if (objectID == 4) {
                MovingObjectsList::iterator it = gameObjects.fish.begin();
                it++;
                MovingObject* fish = (*it);

                fish->objectOn = !fish->objectOn;
                fish->startTime = gameState.elapsedTime - fish->startTime;
            }
        }
    }
}

/// menu controls
/**
    \param[in] choice   chosen action
*/
void gameMenu(int choice) {
    glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
    switch (choice) {
    case 1:
        // restart the game
        restartGame();
        break;
    case 2:
        // static camera 1
        gameState.freeCameraMode = false;
        glutPassiveMotionFunc(NULL);
        staticCamera_1();
        break;
    case 3:
        // static camera 2
        gameState.freeCameraMode = false;
        glutPassiveMotionFunc(NULL); 
        staticCamera_2();
        break;
    case 5:
        // sun on
        gameState.sunOn = true;
        break;
    case 6:
        // sun off
        gameState.sunOn = false;
        break;
    case 7:
        // reflector on
        gameState.reflectorOn = true;
        break;
    case 8:
        // reflector off
        gameState.reflectorOn = false;
        break;
    case 9:
        // fog on
        gameState.fogOn = true;
        enableFog();
        break;
    case 10:
        // fog off
        gameState.fogOn = false;
        disableFog();
        break;
    case 12:
        // close
        exit(0);
        break;
    case 13:
        // free camera
        gameState.freeCameraMode = true;
        glutPassiveMotionFunc(passiveMouseMotionCallback);
        glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);

        if (gameState.staticCamera == StaticCameras::Dynamic) {
            leaveDynamicCamera();
        }
        break;
    case 14:
        // fire on
        gameState.fireOn = true;
        break;
    case 15:
        // fire off
        gameState.fireOn = false;
        break;
    case 16:
        // drone camera
        gameState.freeCameraMode = false;
        glutPassiveMotionFunc(NULL);
        dynamicCamera();
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

/// create menu
void createMenu(void) {
    // camera setup
    const int subMenuCamera = glutCreateMenu(gameMenu);
    glutAddMenuEntry("Static camera 1", 2);
    glutAddMenuEntry("Static camera 2", 3);
    glutAddMenuEntry("Free Camera", 13);
    glutAddMenuEntry("Drone Camera", 16);

    // sun on/off
    const int subMenuSun = glutCreateMenu(gameMenu);
    glutAddMenuEntry("On", 5);
    glutAddMenuEntry("Off", 6);

    // flashlight on/off
    const int subMenuReflector = glutCreateMenu(gameMenu);
    glutAddMenuEntry("On", 7);
    glutAddMenuEntry("Off", 8);

    // fire on/off
    const int subMenuFire = glutCreateMenu(gameMenu);
    glutAddMenuEntry("On", 14);
    glutAddMenuEntry("Off", 15);

    // liht menu
    const int subMenuLight = glutCreateMenu(gameMenu);
    glutAddSubMenu("Sun", subMenuSun);
    glutAddSubMenu("Flashlight", subMenuReflector);
    glutAddSubMenu("Fire", subMenuFire);

    // fog on/off
    const int subMenuFog = glutCreateMenu(gameMenu);
    glutAddMenuEntry("On", 9);
    glutAddMenuEntry("Off", 10);

    // create menu
    glutCreateMenu(gameMenu);
    glutAddSubMenu("Camera", subMenuCamera);
    glutAddSubMenu("Light", subMenuLight);
    glutAddSubMenu("Fog", subMenuFog);
    glutAddMenuEntry("Restart", 1);
    glutAddMenuEntry("Quit", 12);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/// Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void initializeApplication() {
    // initialize random seed
    srand ((unsigned int)time(NULL));

    // initialize OpenGL
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClearStencil(0);  // this is the default value
    glEnable(GL_DEPTH_TEST);

    useLighting = true;

    // initialize shaders
    initializeShaderPrograms();
  
    // create geometry for all models used
    initializeModels();

    restartGame();
}

void finalizeApplication(void) {
    cleanUpObjects();

    // delete buffers - space ship, asteroid, missile, ufo, banner, and explosion
    cleanupModels();

    // delete shaders
    cleanupShaderPrograms();
}

int main(int argc, char** argv) {
    // initialize windowing system
    glutInit(&argc, argv);

#ifndef __APPLE__
    glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
#else
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
#endif

    // initial window size
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow(WINDOW_TITLE);

    glutDisplayFunc(displayCallback);
    // register callback for change of window size
    glutReshapeFunc(reshapeCallback);
    // register callbacks for keyboard
    glutKeyboardFunc(keyboardCallback);
    glutKeyboardUpFunc(keyboardUpCallback);
    glutSpecialFunc(specialKeyboardCallback);     // key pressed
    glutSpecialUpFunc(specialKeyboardUpCallback); // key released

    glutMouseFunc(mouseCallback);

    createMenu();

    glutTimerFunc(33, timerCallback, 0);

    // initialize PGR framework (GL, DevIl, etc.)
    if(!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
        pgr::dieWithError("pgr init failed, required OpenGL not supported?");

    initializeApplication();

#ifndef __APPLE__
    glutCloseFunc(finalizeApplication);
#else
    glutWMCloseFunc(finalizeApplication);
#endif

    glutMainLoop();

    return 0;
}
