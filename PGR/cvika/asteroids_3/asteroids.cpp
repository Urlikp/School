//----------------------------------------------------------------------------------------
/**
 * \file    asteroids.cpp
 * \author  Jaroslav Sloup, Tomas Barak, Petr Felkel
 * \date    2011-2012
 * \brief   Simple implementaion of Asteroids game.
 */
//----------------------------------------------------------------------------------------

#include <time.h>
#include <list>
#include "pgr.h"
#include "render_stuff.h"
#include "spline.h"


extern SCommonShaderProgram shaderProgram;
extern bool useLighting;

typedef std::list<void *> GameObjectsList; 

enum class StaticCameras {Static_1, Static_2};

struct GameState {

  int windowWidth;    // set by reshape callback
  int windowHeight;   // set by reshape callback

  bool freeCameraMode;        // false;
  float cameraElevationAngle; // in degrees = initially 0.0f
  float cameraViewAngle;
  const float cameraSpeed = CAMERA_SPEED;
  glm::vec3 cameraPosition;
  glm::vec3 cameraDirection;
  StaticCameras staticCamera = StaticCameras::Static_1;


  bool gameOver;              // false;
  bool keyMap[KEYS_COUNT];    // false

  float elapsedTime;
  float missileLaunchTime;
  float ufoMissileLaunchTime;

} gameState;

struct GameObjects {

  SpaceShipObject *spaceShip; // NULL
  Object* bench;
  Object* tree;
  Object* fireplace;

  GameObjectsList ground;
  GameObjectsList duck;
  GameObjectsList asteroids;
  GameObjectsList missiles;
  GameObjectsList ufos;

} gameObjects;

glm::vec3 checkBoundaries(const glm::vec3& currentPosition) {
    glm::vec3 newPosition = currentPosition;

    if (newPosition.x > SCENE_WIDTH) {
        newPosition.x = SCENE_WIDTH;
    } else if (newPosition.x < -SCENE_WIDTH) {
        newPosition.x = -SCENE_WIDTH;
    }

    if (newPosition.y > SCENE_HEIGHT) {
        newPosition.y = SCENE_HEIGHT;
    } else if (newPosition.y < -SCENE_HEIGHT) {
        newPosition.y = -SCENE_HEIGHT;
    }

    return newPosition;
}


void moveCameraForward() {
    gameState.cameraPosition += gameState.cameraSpeed * gameState.cameraDirection;
}

void moveCameraBackwards() {
    gameState.cameraPosition -= gameState.cameraSpeed * gameState.cameraDirection;
}

void moveCameraLeft() {
    gameState.cameraPosition += gameState.cameraSpeed * glm::vec3(-gameState.cameraDirection.y, gameState.cameraDirection.x, gameState.cameraDirection.z);
}

void moveCameraRight() {
    gameState.cameraPosition -= gameState.cameraSpeed * glm::vec3(-gameState.cameraDirection.y, gameState.cameraDirection.x, gameState.cameraDirection.z);
}

void teleport(void) {

  // generate new space ship position randomly
  gameObjects.spaceShip->position = glm::vec3(
    (float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
    (float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
    0.0f
  );
}

void cleanUpObjects(void) {
  gameObjects.bench = NULL;
  gameObjects.tree = NULL;

  // delete asteroids
  while(!gameObjects.asteroids.empty()) {
    delete gameObjects.asteroids.back();
    gameObjects.asteroids.pop_back();
  } 

  // delete missiles
  while(!gameObjects.missiles.empty()) {
    delete gameObjects.missiles.back();
    gameObjects.missiles.pop_back();
  } 

  // delete ufos
  while(!gameObjects.ufos.empty()) {
    delete gameObjects.ufos.back();
    gameObjects.ufos.pop_back();
  } 

  while (!gameObjects.duck.empty()) {
      delete gameObjects.duck.back();
      gameObjects.duck.pop_back();
  }

  while (!gameObjects.ground.empty()) {
      delete gameObjects.ground.back();
      gameObjects.ground.pop_back();
  }
}

// generates random position that does not collide with the spaceship
glm::vec3 generateRandomPosition(void) {
 glm::vec3 newPosition;

    // position is generated randomly
    // coordinates are in range -1.0f ... 1.0f
    newPosition = glm::vec3(
      (float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
      (float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
      0.0f
    );

  return newPosition;
}

Object* createBench(glm::vec3 pos) {
    Object* bench = new Object;

    bench->destroyed = false;

    bench->startTime = gameState.elapsedTime;
    bench->currentTime = bench->startTime;

    bench->size = 0.2f;

    bench->direction = glm::vec3(0.0f, 0.0f, 0.0f);
    bench->direction = glm::normalize(bench->direction);

    bench->position = pos;

    return bench;
}

Object* createTree(glm::vec3 pos) {
    Object* tree = new Object;

    tree->destroyed = false;

    tree->startTime = gameState.elapsedTime;
    tree->currentTime = tree->startTime;

    tree->size = 0.4f;

    tree->direction = glm::vec3(0.0f, 0.0f, 0.0f);
    tree->direction = glm::normalize(tree->direction);

    tree->position = pos;

    return tree;
}

Object* createDuck() {
    Object* duck = new Object;
    duck->destroyed = false;
    duck->startTime = gameState.elapsedTime;
    duck->currentTime = duck->startTime;
    duck->size = 0.005f;
    duck->direction = glm::vec3(0.0f, 0.0f, 1.0f);
    duck->direction = glm::normalize(duck->direction);
    duck->position = glm::vec3(-1.0f, 0.0f, +0.0f);
    return duck;
}

Object* createGround(glm::vec3 pos) {
    Object* newGround = new Object;

    newGround->destroyed = false;

    newGround->startTime = gameState.elapsedTime;
    newGround->currentTime = newGround->startTime;

    newGround->size = 0.5f;

    newGround->direction = glm::vec3(0.0f, 0.0f, 0.0f);
    newGround->direction = glm::normalize(newGround->direction);

    newGround->position = pos;

    return newGround;
}

Object* createFireplace(glm::vec3 pos) {
    Object* newFireplace = new Object;

    newFireplace->destroyed = false;

    newFireplace->startTime = gameState.elapsedTime;
    newFireplace->currentTime = newFireplace->startTime;

    newFireplace->size = 0.05f;

    newFireplace->direction = glm::vec3(0.5f, 0.0f, 0.0f);
    newFireplace->direction = glm::normalize(newFireplace->direction);

    newFireplace->position = pos;

    return newFireplace;
}

AsteroidObject* createAsteroid(void) {
 AsteroidObject* newAsteroid = new AsteroidObject;

  newAsteroid->destroyed = false;

  newAsteroid->startTime = gameState.elapsedTime;
  newAsteroid->currentTime = newAsteroid->startTime;

  newAsteroid->size = ASTEROID_SIZE;

  // generate motion direction randomly in range -1.0f ... 1.0f
  newAsteroid->direction = glm::vec3(
    (float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
    (float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
    0.0f
  );
  newAsteroid->direction = glm::normalize(newAsteroid->direction);

  // position is generated randomly as well
  newAsteroid->position = generateRandomPosition();

  // motion speed 0.0f ... 1.0f
  newAsteroid->speed = ASTEROID_SPEED_MAX * (float)(rand() / (double)RAND_MAX);
  // rotation speed 0.0f ... 1.0f
  newAsteroid->rotationSpeed = ASTEROID_ROTATION_SPEED_MAX * (float)(rand() / (double)RAND_MAX);

  return newAsteroid;
}

UfoObject* createUfo(void) {
 UfoObject* newUfo = new UfoObject;

  newUfo->destroyed = false;

  newUfo->startTime = gameState.elapsedTime;
  newUfo->currentTime = newUfo->startTime;

  newUfo->size = UFO_SIZE;

  // generate initial position randomly
  newUfo->initPosition = generateRandomPosition();
  newUfo->position = newUfo->initPosition;
  // random speed in range 0.0f ... 1.0f
  newUfo->speed = (float)(rand() / (double)RAND_MAX);
  // random rotation speed in range 0.0f ... 1.0f
  newUfo->speed = UFO_ROTATION_SPEED_MAX * (float)(rand() / (double)RAND_MAX);

  // generate randomly in range -1.0f ... 1.0f
  newUfo->direction = glm::vec3(
    (float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
    (float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
    0.0f
  );
  newUfo->direction = glm::normalize(newUfo->direction);

  return newUfo;
}

void restartGame(void) {

  cleanUpObjects();

  gameState.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); // milliseconds => seconds

  // initialize space ship
  if(gameObjects.spaceShip == NULL)
    gameObjects.spaceShip = new SpaceShipObject;

  gameObjects.spaceShip->position = glm::vec3(0.0f, 0.0f, 0.0f);
  gameObjects.spaceShip->viewAngle = 90.0f; // degrees
  gameObjects.spaceShip->direction = glm::vec3(cos(glm::radians(gameObjects.spaceShip->viewAngle)), sin(glm::radians(gameObjects.spaceShip->viewAngle)), 0.0f);
  gameObjects.spaceShip->speed = 0.0f;
  gameObjects.spaceShip->size = SPACESHIP_SIZE;
  gameObjects.spaceShip->destroyed = false;
  gameObjects.spaceShip->startTime = gameState.elapsedTime;
  gameObjects.spaceShip->currentTime = gameObjects.spaceShip->startTime;

  Object* bench = createBench(glm::vec3(1.4f, 0.0f, -0.01f));
  gameObjects.bench = bench;

  Object* tree = createTree(glm::vec3(1.4f, 1.0f, 0.25f));
  gameObjects.tree = tree;

  Object* fireplace = createFireplace(glm::vec3(0.5f, 0.0f, -0.09f));
  gameObjects.fireplace = fireplace;

  Object* duck = createDuck();
  gameObjects.duck.push_back(duck);

  for (float i = -1.0f; i <= 3.0f; i += 1.0f) {
      for (float j = -1.0f; j <= 1.0f; j += 1.0f) {
          Object* newGround = createGround(glm::vec3(i, j, -0.1f));
          gameObjects.ground.push_back(newGround);
      }
  }

  // initialize asteroids
  for(int i=0; i<ASTEROIDS_COUNT_MIN; i++) {
    AsteroidObject* newAsteroid = createAsteroid();
    
    gameObjects.asteroids.push_back(newAsteroid); 
  }

  if(gameState.freeCameraMode == true) {
    gameState.freeCameraMode = false;
    glutPassiveMotionFunc(NULL);
  }

  gameState.staticCamera = StaticCameras::Static_1;
  gameState.cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
  gameState.cameraElevationAngle = 0.0f;
  gameState.cameraViewAngle = 90.0f;
  gameState.cameraDirection = glm::vec3(cos(glm::radians(gameState.cameraViewAngle)), sin(glm::radians(gameState.cameraViewAngle)), 0.0f);
  
  // reset key map
  for(int i=0; i<KEYS_COUNT; i++)
    gameState.keyMap[i] = false;

  gameState.gameOver = false;
  gameState.missileLaunchTime = -MISSILE_LAUNCH_TIME_DELAY;
  gameState.ufoMissileLaunchTime = -MISSILE_LAUNCH_TIME_DELAY;
}

void createMissile(const glm::vec3 &missilePosition, const glm::vec3 &missileDirection, float &missileLaunchTime) {

  float currentTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); // milliseconds => seconds
  if(currentTime-missileLaunchTime < MISSILE_LAUNCH_TIME_DELAY)
    return;

  missileLaunchTime = currentTime;

  MissileObject* newMissile = new MissileObject;

  newMissile->destroyed   = false;
  newMissile->startTime   = gameState.elapsedTime;
  newMissile->currentTime = newMissile->startTime;
  newMissile->size        = MISSILE_SIZE;
  newMissile->speed       = MISSILE_SPEED;
  newMissile->position    = missilePosition;
  newMissile->direction   = glm::normalize(missileDirection);
  
  gameObjects.missiles.push_back(newMissile); 
}

void drawWindowContents() {

  // setup parallel projection
  glm::mat4 orthoProjectionMatrix = glm::ortho(
    -SCENE_WIDTH, SCENE_WIDTH,
    -SCENE_HEIGHT, SCENE_HEIGHT,
    -10.0f*SCENE_DEPTH, 10.0f*SCENE_DEPTH
  );
  // static viewpoint - top view
  glm::mat4 orthoViewMatrix = glm::lookAt(
    gameState.cameraPosition,
    gameState.cameraPosition + gameState.cameraDirection,
    glm::vec3(0.0f, 0.0f, 1.0f)
  );

  // setup camera & projection transform
  glm::mat4 viewMatrix  = orthoViewMatrix;
  glm::mat4 projectionMatrix = orthoProjectionMatrix;

  projectionMatrix = glm::perspective(glm::radians(60.0f), gameState.windowWidth / (float)gameState.windowHeight, 0.1f, 10.0f);

  if(gameState.freeCameraMode == true) {

    glm::vec3 cameraPosition = gameState.cameraPosition;
    glm::vec3 cameraUpVector = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraCenter;

    glm::vec3 cameraViewDirection = gameState.cameraDirection;

    glm::vec3 rotationAxis = glm::cross(cameraViewDirection, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 cameraTransform = glm::rotate(glm::mat4(1.0f), glm::radians(gameState.cameraElevationAngle), rotationAxis);

    cameraUpVector = glm::vec3(cameraTransform * glm::vec4(cameraUpVector, 0.0f));
    cameraViewDirection = glm::vec3(cameraTransform * glm::vec4(cameraViewDirection, 0.0f));

    cameraCenter = cameraPosition + cameraViewDirection;

    viewMatrix = glm::lookAt(
      cameraPosition,
      cameraCenter,
      cameraUpVector
    );

    projectionMatrix = glm::perspective(glm::radians(60.0f), gameState.windowWidth/(float)gameState.windowHeight, 0.1f, 10.0f);
  } else {
      gameState.cameraElevationAngle = 0.0f;

      if (gameState.staticCamera == StaticCameras::Static_1) {
          viewMatrix = glm::lookAt(
              gameState.cameraPosition,
              gameState.cameraPosition + gameState.cameraDirection,
              glm::vec3(0.0f, 0.0f, 1.0f)
          );

          //projectionMatrix = glm::perspective(glm::radians(60.0f), gameState.windowWidth / (float)gameState.windowHeight, 0.1f, 10.0f);
      }
      else if (gameState.staticCamera == StaticCameras::Static_2) {
          viewMatrix = glm::lookAt(
              gameState.cameraPosition,
              gameState.cameraPosition + gameState.cameraDirection,
              glm::vec3(0.0f, 0.0f, 1.0f)
          );
      }
  }

  glUseProgram(shaderProgram.program);
  glUniform1f(shaderProgram.timeLocation, gameState.elapsedTime);

// ======== BEGIN OF SOLUTION - TASK 3_3-1 ======== //

  // set correct position and direction of the reflector to the shaders
  // use reflectorPosition and reflectorDirection uniforms

  glUniform3fv(shaderProgram.reflectorPositionLocation, 1, glm::value_ptr(gameState.cameraPosition));

  glUniform3fv(shaderProgram.reflectorDirectionLocation, 1, glm::value_ptr(gameState.cameraDirection));

// ========  END OF SOLUTION - TASK 3_3-1  ======== //
  glUseProgram(0);

  // draw space ship
  //drawSpaceShip(gameObjects.spaceShip, viewMatrix, projectionMatrix);

  CHECK_GL_ERROR(); 
  // draw asteroids
  /*for(GameObjectsList::iterator it = gameObjects.asteroids.begin(); it != gameObjects.asteroids.end(); ++it) {
    CHECK_GL_ERROR(); 

    AsteroidObject* asteroid = (AsteroidObject*)(*it);
    drawAsteroid(asteroid, viewMatrix, projectionMatrix);
  }*/

  // draw missiles
  /*for(GameObjectsList::iterator it = gameObjects.missiles.begin(); it != gameObjects.missiles.end(); ++it) {
    MissileObject* missile = (MissileObject *)(*it);
    drawMissile(missile, viewMatrix, projectionMatrix); 
  }*/

  // draw ufos
  /*for(GameObjectsList::iterator it = gameObjects.ufos.begin(); it != gameObjects.ufos.end(); ++it) {
    UfoObject* ufo = (UfoObject *)(*it);
    drawUfo(ufo, viewMatrix, projectionMatrix); 
  }*/

  for (GameObjectsList::iterator it = gameObjects.duck.begin(); it != gameObjects.duck.end(); ++it) {
      Object* duck = (Object*)(*it);
      drawDuck(duck, viewMatrix, projectionMatrix);
  }

  for (GameObjectsList::iterator it = gameObjects.ground.begin(); it != gameObjects.ground.end(); ++it) {
      Object* ground = (Object*)(*it);
      drawGround(ground, viewMatrix, projectionMatrix);
  }

  
  drawFireplace(gameObjects.fireplace, viewMatrix, projectionMatrix);
  drawBench(gameObjects.bench, viewMatrix, projectionMatrix);
  drawTree(gameObjects.tree, viewMatrix, projectionMatrix);

  if(gameState.gameOver == true) {
  }
}

// Called to update the display. You should call glutSwapBuffers after all of your
// rendering to display what you rendered.
void displayCallback() {
  GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

  glClear(mask);

  drawWindowContents();

  glutSwapBuffers();
}

// Called whenever the window is resized. The new window size is given, in pixels.
// This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshapeCallback(int newWidth, int newHeight) {

  gameState.windowWidth = newWidth;
  gameState.windowHeight = newHeight;

  glViewport(0, 0, (GLsizei) newWidth, (GLsizei) newHeight);
}

void updateObjects(float elapsedTime) {

    gameState.cameraPosition = checkBoundaries(gameState.cameraPosition);

  // update space ship 
  float timeDelta = elapsedTime - gameObjects.spaceShip->currentTime;
  gameObjects.spaceShip->currentTime = elapsedTime;
  gameObjects.spaceShip->position += timeDelta * gameObjects.spaceShip->speed * gameObjects.spaceShip->direction;

  // update asteroids
  GameObjectsList::iterator it = gameObjects.asteroids.begin();
  while(it != gameObjects.asteroids.end()) {
    AsteroidObject* asteroid = (AsteroidObject*)(*it);

    if(asteroid->destroyed == true) {
      it = gameObjects.asteroids.erase(it);
    }
    else {
      // update asteroid
      float timeDelta = elapsedTime - asteroid->currentTime;

      asteroid->currentTime = elapsedTime;
      asteroid->position += timeDelta * asteroid->speed * asteroid->direction;

      ++it;
    }
  }

  // update missiles
  it = gameObjects.missiles.begin();
  while(it != gameObjects.missiles.end()) {
    MissileObject* missile = (MissileObject*)(*it);

    // update missile
    float timeDelta = elapsedTime - missile->currentTime;

    missile->currentTime = elapsedTime;
    missile->position += timeDelta * missile->speed * missile->direction;

    if((missile->currentTime-missile->startTime)*missile->speed > MISSILE_MAX_DISTANCE) 
      missile->destroyed = true;

    if(missile->destroyed == true) {
      it = gameObjects.missiles.erase(it);
    }
    else {
      ++it;
    }
  }

  // update ufos
  it = gameObjects.ufos.begin();
  while(it != gameObjects.ufos.end()) {
    UfoObject* ufo = (UfoObject*)(*it);

    if(ufo->destroyed == true) {
      it = gameObjects.ufos.erase(it);
    }
    else {
      // update ufo
      ufo->currentTime = elapsedTime;

      ++it;
    }
  }
}

// Callback responsible for the scene update
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

  // space pressed -> launch missile
  if(gameState.keyMap[KEY_SPACE] == true) {
    // missile position and direction
    glm::vec3 missilePosition = gameObjects.spaceShip->position;
    glm::vec3 missileDirection = gameObjects.spaceShip->direction;

    missilePosition += missileDirection*1.5f*SPACESHIP_SIZE;

    createMissile(missilePosition, missileDirection, gameState.missileLaunchTime);
  }

  // generate new ufos randomly
  if(gameObjects.ufos.size() < UFOS_COUNT_MIN) {
    int howManyUfos = rand() % (UFOS_COUNT_MAX - UFOS_COUNT_MIN + 1);

    for(int i=0; i<howManyUfos; i++) {
      UfoObject* newUfo = createUfo();

      gameObjects.ufos.push_back(newUfo); 
    }
  }

  // generate new asteroids randomly
  if(gameObjects.asteroids.size() < ASTEROIDS_COUNT_MIN) {
    int howManyAsteroids = rand() % (ASTEROIDS_COUNT_MAX - ASTEROIDS_COUNT_MIN + 1);

    for(int i=0; i<howManyAsteroids; i++) {
      AsteroidObject* newAsteroid = createAsteroid();

      gameObjects.asteroids.push_back(newAsteroid); 
    }
  }

  // game over? -> create banner with scrolling text "game over"
  if(gameState.gameOver == true) {
    gameState.keyMap[KEY_SPACE] = false;
  }

  // set timeCallback next invocation
  glutTimerFunc(33, timerCallback, 0);

  glutPostRedisplay();
}

// Called when mouse is moving while no mouse buttons are pressed.
void passiveMouseMotionCallback(int mouseX, int mouseY) {
    if(mouseY != gameState.windowHeight/2) {
        float cameraElevationAngleDelta = 0.5f * (mouseY - gameState.windowHeight/2);

        if (fabs(gameState.cameraElevationAngle + cameraElevationAngleDelta) < CAMERA_ELEVATION_MAX) {
            gameState.cameraElevationAngle += cameraElevationAngleDelta;
        }

        // set mouse pointer to the window center
        glutWarpPointer(gameState.windowWidth/2, gameState.windowHeight/2);

        glutPostRedisplay();
    }

    if (mouseX != gameState.windowWidth / 2) {
        float cameraViewAngleDelta = 0.5f * (mouseX - gameState.windowWidth / 2);

        if (cameraViewAngleDelta < 0) {
            gameState.cameraViewAngle -= cameraViewAngleDelta;

            if (gameState.cameraViewAngle >= 360) {
                gameState.cameraViewAngle -= 360;
            }
        } else {
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

// Called whenever a key on the keyboard was pressed. The key is given by the "keyPressed"
// parameter, which is in ASCII. It's often a good idea to have the escape key (ASCII value 27)
// to call glutLeaveMainLoop() to exit the program.
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
    case ' ': // switch to static camera
        if (gameState.freeCameraMode == false) {
            if (gameState.staticCamera == StaticCameras::Static_1) {
                gameState.staticCamera = StaticCameras::Static_2;
                gameState.cameraPosition = glm::vec3(1.5f, 0.0f, 0.0f);
                gameState.cameraDirection = glm::vec3(-1.0f, 0.0f, 0.0f);
                gameState.cameraViewAngle = 180.0f;
            } else if (gameState.staticCamera == StaticCameras::Static_2) {
                gameState.staticCamera = StaticCameras::Static_1;
                gameState.cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
                gameState.cameraDirection = glm::vec3(1.0f, 0.0f, 0.0f);
                gameState.cameraViewAngle = 0.0f;
            }
        }
        break;
    case 't': // teleport space ship
        if(gameState.gameOver != true)
        teleport();
        break;
    case 'c': // switch to free camera
        gameState.freeCameraMode = !gameState.freeCameraMode;
        if(gameState.freeCameraMode == true) {
        glutPassiveMotionFunc(passiveMouseMotionCallback);
        glutWarpPointer(gameState.windowWidth/2, gameState.windowHeight/2);
        }
        else {
        glutPassiveMotionFunc(NULL);
        }
        break;
    default:
        ; // printf("Unrecognized key pressed\n");
    }
}

// Called whenever a key on the keyboard was released. The key is given by
// the "keyReleased" parameter, which is in ASCII. 
void keyboardUpCallback(unsigned char keyReleased, int mouseX, int mouseY) {

  switch(keyReleased) {
    case ' ':
      break;
    default:
      ; // printf("Unrecognized key released\n");
  }
}

// The special keyboard callback is triggered when keyboard function or directional
// keys are pressed.
void specialKeyboardCallback(int specKeyPressed, int mouseX, int mouseY) {

    if(gameState.gameOver == true)
        return;

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
            ; // printf("Unrecognized special key pressed\n");
        }
    }
}

// The special keyboard callback is triggered when keyboard function or directional
// keys are released.
void specialKeyboardUpCallback(int specKeyReleased, int mouseX, int mouseY) {

    if(gameState.gameOver == true)
        return;

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
            ; // printf("Unrecognized special key released\n");
        }
    }
}

// Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void initializeApplication() {

  // initialize random seed
  srand ((unsigned int)time(NULL));

  // initialize OpenGL
  glClearColor(0.06f, 0.08f, 0.16f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  useLighting = true;

  // initialize shaders
  initializeShaderPrograms();
  // create geometry for all models used
  initializeModels();

  gameObjects.spaceShip = NULL;

  restartGame();
}

void finalizeApplication(void) {

  cleanUpObjects();

  delete gameObjects.spaceShip;
  gameObjects.spaceShip = NULL;

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
