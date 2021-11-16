//----------------------------------------------------------------------------------------
/**
 * \file    render_stuff.cpp
 * \author  Jaroslav Sloup, Tomas Barak, Petr Felkel
 * \date    2013
 * \brief   Rendering stuff - drawing functions for models, etc..
 */
//----------------------------------------------------------------------------------------

#include <iostream>
#include "pgr.h"
#include "render_stuff.h"
#include "data.h"
#include "spline.h"
#include "bird.h"

MeshGeometry* asteroidGeometry = NULL;
MeshGeometry* spaceShipGeometry = NULL;
MeshGeometry* ufoGeometry = NULL;
MeshGeometry* missileGeometry = NULL;

MeshGeometry* treeGeometry = NULL;
MeshGeometry* duckGeometry = NULL;
MeshGeometry* groundGeometry = NULL;
MeshGeometry* fireplaceGeometry = NULL;
MeshGeometry* fireGeometry = NULL;

std::vector<MeshGeometry*> benchGeometry; 
//std::vector<MeshGeometry*> fireplaceGeometry;

typedef struct Material {

    glm::vec3     ambient;
    glm::vec3     diffuse;
    glm::vec3     specular;
    float         shininess;
    std::string   texture;

} Material;

const Material duckMaterial = {
        glm::vec3(1.0f, 1.0f, 1.0f),   //ambient
        glm::vec3(1.0f, 1.0f, 1.0f),   //deffuse
        glm::vec3(0.3f, 0.3f, 0.3f),   //specular
        0.1f,                          //shinines
        "data/duck/Bird.jpg"		//texture name
};

void initMaterial(MeshGeometry** geometry, Material material) {
    (*geometry)->ambient = material.ambient;
    (*geometry)->diffuse = material.diffuse;
    (*geometry)->specular = material.specular;
    (*geometry)->shininess = material.shininess;
    (*geometry)->texture = pgr::createTexture(material.texture);
}



const char* ASTEROID_MODEL_NAME = "data/asteroid.obj";
const char* SPACESHIP_MODEL_NAME = "data/ghoul.obj";
const char* BENCH_MODEL_NAME = "data/bench/bench.obj";
const char* TREE_MODEL_NAME = "data/tree/tree.obj";
const char* DOG_MODEL_NAME = "data/beagle/beagle.obj";
const char* FIREPLACE_MODEL_NAME = "data/Fireplace/fireplace.obj";
const char* GROUND_MODEL_NAME = "data/ground/ground.obj";
const char* FIRE_TEXTURE_NAME = "data/Fireplace/fire.png";

SCommonShaderProgram shaderProgram;

bool useLighting = false;

struct fireShaderProgram {
    // identifier for the shader program
    GLuint program;              // = 0;
    // vertex attributes locations
    GLint posLocation;           // = -1;
    GLint texCoordLocation;      // = -1;
    // uniforms locations
    GLint PVMmatrixLocation;     // = -1;
    GLint VmatrixLocation;       // = -1;
    GLint timeLocation;          // = -1;
    GLint texSamplerLocation;    // = -1;
    GLint frameDurationLocation; // = -1;

} fireShaderProgram;

void setTransformUniforms(const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {

  glm::mat4 PVM = projectionMatrix * viewMatrix * modelMatrix;
  glUniformMatrix4fv(shaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVM));

  glUniformMatrix4fv(shaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
  glUniformMatrix4fv(shaderProgram.MmatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

  // just take 3x3 rotation part of the modelMatrix
  // we presume the last row contains 0,0,0,1
  const glm::mat4 modelRotationMatrix = glm::mat4(
    modelMatrix[0],
    modelMatrix[1],
    modelMatrix[2],
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
  );
  glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelRotationMatrix));

  //or an alternative single-line method: 
  //glm::mat4 normalMatrix = glm::transpose(glm::inverse(glm::mat4(glm::mat3(modelRotationMatrix))));

  glUniformMatrix4fv(shaderProgram.normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));  // correct matrix for non-rigid transform
}

void setMaterialUniforms(const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, float shininess, GLuint texture) {

  glUniform3fv(shaderProgram.diffuseLocation,  1, glm::value_ptr(diffuse));  // 2nd parameter must be 1 - it declares number of vectors in the vector array
  glUniform3fv(shaderProgram.ambientLocation,  1, glm::value_ptr(ambient));
  glUniform3fv(shaderProgram.specularLocation, 1, glm::value_ptr(specular));
  glUniform1f(shaderProgram.shininessLocation,    shininess);

  if(texture != 0) {
    glUniform1i(shaderProgram.useTextureLocation, 1);  // do texture sampling
    glUniform1i(shaderProgram.texSamplerLocation, 0);  // texturing unit 0 -> samplerID   [for the GPU linker]
    glActiveTexture(GL_TEXTURE0 + 0);                  // texturing unit 0 -> to be bound [for OpenGL BindTexture]
    glBindTexture(GL_TEXTURE_2D, texture);
  }
  else {
    glUniform1i(shaderProgram.useTextureLocation, 0);  // do not sample the texture
  }
}

void drawSpaceShip(SpaceShipObject *spaceShip, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {

  glUseProgram(shaderProgram.program);

  // prepare modeling transform matrix
  glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), spaceShip->position);
  modelMatrix = glm::rotate(modelMatrix, glm::radians(spaceShip->viewAngle), glm::vec3(0, 0, 1));
  modelMatrix = glm::scale(modelMatrix, glm::vec3(spaceShip->size, spaceShip->size, spaceShip->size));

  // send matrices to the vertex & fragment shader
  setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

  setMaterialUniforms(
    spaceShipGeometry->ambient,
    spaceShipGeometry->diffuse,
    spaceShipGeometry->specular,
    spaceShipGeometry->shininess,
    spaceShipGeometry->texture
  );

  // draw geometry
  glBindVertexArray(spaceShipGeometry->vertexArrayObject);
  glDrawElements(GL_TRIANGLES, spaceShipGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
  glUseProgram(0);

  return;
}

void drawAsteroid(AsteroidObject* asteroid, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
  float angle = asteroid->rotationSpeed * (asteroid->currentTime-asteroid->startTime); // angle in radians

  glUseProgram(shaderProgram.program);

  glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), asteroid->position);
  modelMatrix = glm::scale(modelMatrix, glm::vec3(asteroid->size));
  modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 0, 1));

  // send matrices to the vertex & fragment shader
  setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

  setMaterialUniforms(
    asteroidGeometry->ambient,
    asteroidGeometry->diffuse,
    asteroidGeometry->specular,
    asteroidGeometry->shininess,
    asteroidGeometry->texture
  );

  // draw geometry
  glBindVertexArray(asteroidGeometry->vertexArrayObject);
  glDrawElements(GL_TRIANGLES, asteroidGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
  glUseProgram(0);

  return;
}

void drawMissile(MissileObject* missile, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
  
  glUseProgram(shaderProgram.program);

  // align missile coordinate system to match its position and direction - see alignObject() function
  glm::mat4 modelMatrix = alignObject(missile->position, missile->direction, glm::vec3(0.0f, 0.0f, 1.0f));
  modelMatrix = glm::scale(modelMatrix, glm::vec3(missile->size));

  // angular speed = 2*pi*frequency => path = angular speed * time
  const float frequency = 2.0f; // per second
  const float angle = 2.0f*M_PI * frequency * (missile->currentTime-missile->startTime); // angle in radians
  modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));

  // send matrices to the vertex & fragment shader
  setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

  setMaterialUniforms(
    missileGeometry->ambient,
    missileGeometry->diffuse,
    missileGeometry->specular,
    missileGeometry->shininess,
    missileGeometry->texture
  );
  // draw the missile using glDrawArrays 
  glBindVertexArray(missileGeometry->vertexArrayObject);
  glDrawArrays(GL_TRIANGLES, 0, missileGeometry->numTriangles*3);

  glBindVertexArray(0);
  glUseProgram(0);

  return;
}

void drawUfo(UfoObject* ufo, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {

  glUseProgram(shaderProgram.program);

  // align ufo coordinate system to match its position and direction - see alignObject() function
  glm::mat4 modelMatrix = alignObject(ufo->position, ufo->direction, glm::vec3(0.0f, 0.0f, 1.0f));
  modelMatrix = glm::scale(modelMatrix, glm::vec3(ufo->size));

  // send matrices to the vertex & fragment shader
  setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

  // angular speed = 2*pi*frequency => path = angular speed * time
  const float frequency = 0.33f; // per second
  float angle = 6.28f * frequency * (ufo->currentTime-ufo->startTime); // angle in radians
  float scaleFactor = 0.5f*(cos(angle) + 1.0f);
  glm::vec3 yellowMat = glm::vec3(scaleFactor, scaleFactor, 0.0f);

  setMaterialUniforms(
    yellowMat,
    yellowMat,
    yellowMat,
    ufoGeometry->shininess,
    ufoGeometry->texture
  );

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_FRONT);
  // draw the first three (yellow) triangles of ufo top using glDrawArrays 
  glBindVertexArray(ufoGeometry->vertexArrayObject);
  glDrawArrays(GL_TRIANGLES, 0, 3*ufoGeometry->numTriangles/2);
  CHECK_GL_ERROR();

  setMaterialUniforms(
    ufoGeometry->ambient*(1.0f-scaleFactor),
    ufoGeometry->diffuse*(1.0f-scaleFactor),
    ufoGeometry->specular*(1.0f-scaleFactor),
    ufoGeometry->shininess,
    ufoGeometry->texture
  );

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_BACK);
  // draw the second three (magenta) triangles of ufo top using glDrawArrays 
  glBindVertexArray(ufoGeometry->vertexArrayObject);
  glDrawArrays(GL_TRIANGLES, 3*ufoGeometry->numTriangles/2, 3*ufoGeometry->numTriangles/2);
  CHECK_GL_ERROR();

  setMaterialUniforms(
    ufoGeometry->ambient,
    ufoGeometry->diffuse,
    ufoGeometry->specular,
    ufoGeometry->shininess,
    ufoGeometry->texture
  );

  // draw the six triangles of ufo bottom using glDrawElements 
  glDrawElements(GL_TRIANGLES, ufoGeometry->numTriangles*3, GL_UNSIGNED_INT, 0);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  CHECK_GL_ERROR();

  glBindVertexArray(0);
  glUseProgram(0);

  return;
}

void drawTree(Object* tree, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    glUseProgram(shaderProgram.program);
    setMaterialUniforms(
        treeGeometry->ambient,
        treeGeometry->diffuse,
        treeGeometry->specular,
        treeGeometry->shininess,
        treeGeometry->texture
    );

    glm::mat4 modelMatrix;
    modelMatrix = alignObject(tree->position, glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(tree->size, tree->size, tree->size));
    setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
    glBindVertexArray(treeGeometry->vertexArrayObject);
    glDrawElements(GL_TRIANGLES, treeGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    return;
}

void drawFireplace(Object* fireplace, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    glUseProgram(shaderProgram.program);

    // prepare modelling transform matrix
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), fireplace->position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(fireplace->size));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));

    // send matrices to the vertex & fragment shader
    setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
    /*
    for (size_t i = 0; i < fireplaceGeometry.size(); i++)
    {
        glBindVertexArray(fireplaceGeometry[i]->vertexArrayObject);
        setMaterialUniforms(
            fireplaceGeometry[i]->ambient,
            fireplaceGeometry[i]->diffuse,
            fireplaceGeometry[i]->specular,
            fireplaceGeometry[i]->shininess,
            fireplaceGeometry[i]->texture
        );

        // draw geometry
        glDrawElements(GL_TRIANGLES, fireplaceGeometry[i]->numTriangles * 3, GL_UNSIGNED_INT, 0);
    }*/

    setMaterialUniforms(
        fireplaceGeometry->ambient,
        fireplaceGeometry->diffuse,
        fireplaceGeometry->specular,
        fireplaceGeometry->shininess,
        fireplaceGeometry->texture
    );

    glBindVertexArray(fireplaceGeometry->vertexArrayObject);
    glDrawElements(GL_TRIANGLES, fireplaceGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
    return;
}

void drawFire(FireObject* fire, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

    // ======== BEGIN OF SOLUTION - TASK 4_2-2 ======== //
      // enable blending and set proper blending function
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    // ========  END OF SOLUTION - TASK 4_2-2  ======== //

    glUseProgram(fireShaderProgram.program);

    // just take 3x3 rotation part of the view transform
    glm::mat4 billboardRotationMatrix = glm::mat4(
        viewMatrix[0],
        viewMatrix[1],
        viewMatrix[2],
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    // inverse view rotation
    billboardRotationMatrix = glm::transpose(billboardRotationMatrix);

    glm::mat4 matrix = glm::translate(glm::mat4(1.0f), fire->position);
    matrix = glm::scale(matrix, glm::vec3(fire->size));
    matrix = matrix * billboardRotationMatrix; // make billboard to face the camera

    glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * matrix;
    glUniformMatrix4fv(fireShaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));  // model-view-projection
    glUniformMatrix4fv(fireShaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));   // view
    glUniform1f(fireShaderProgram.timeLocation, fire->currentTime - fire->startTime);
    glUniform1i(fireShaderProgram.texSamplerLocation, 0);
    glUniform1f(fireShaderProgram.frameDurationLocation, fire->frameDuration);

    glBindVertexArray(fireGeometry->vertexArrayObject);
    glBindTexture(GL_TEXTURE_2D, fireGeometry->texture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, fireGeometry->numTriangles);

    glBindVertexArray(0);
    glUseProgram(0);

    glDisable(GL_BLEND);

    return;
}

void drawGround(Object* ground, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    glUseProgram(shaderProgram.program);

    // prepare modelling transform matrix
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), ground->position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(ground->size));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));

    // send matrices to the vertex & fragment shader
    setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

    setMaterialUniforms(
        groundGeometry->ambient,
        groundGeometry->diffuse,
        groundGeometry->specular,
        groundGeometry->shininess,
        groundGeometry->texture
    );
    
    glBindVertexArray(groundGeometry->vertexArrayObject);
    glDrawElements(GL_TRIANGLES, groundGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    return;
}

void drawDuck(Object* duck, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    glUseProgram(shaderProgram.program);

    // prepare modelling transform matrix
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), duck->position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(duck->size, duck->size, duck->size));
    //modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, -1, -1));

    // send matrices to the vertex & fragment shader
    setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

    setMaterialUniforms(
        duckGeometry->ambient,
        duckGeometry->diffuse,
        duckGeometry->specular,
        duckGeometry->shininess,
        duckGeometry->texture
    );

    glBindVertexArray(duckGeometry->vertexArrayObject);
    glDrawElements(GL_TRIANGLES, duckGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

    CHECK_GL_ERROR();

    glBindVertexArray(0);
    glUseProgram(0);
}

void drawBench(Object* bench, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    glUseProgram(shaderProgram.program);
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), bench->position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(bench->size));
    modelMatrix = glm::rotate(modelMatrix, 0.1f, glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, 0, 1));

    setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
    for (size_t i = 0; i < benchGeometry.size(); i++)
    {
        
        setMaterialUniforms(
            benchGeometry[i]->ambient,
            benchGeometry[i]->diffuse,
            benchGeometry[i]->specular,
            benchGeometry[i]->shininess,
            benchGeometry[i]->texture
        );
        glBindVertexArray(benchGeometry[i]->vertexArrayObject);
        glUniform1i(shaderProgram.useTextureLocation, true);
        // draw geometry
        glDrawElements(GL_TRIANGLES, benchGeometry[i]->numTriangles * 3, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
    glUseProgram(0);
    return;
}

void cleanupShaderPrograms(void) {
  pgr::deleteProgramAndShaders(shaderProgram.program);
  pgr::deleteProgramAndShaders(fireShaderProgram.program);
}

void initializeShaderPrograms(void) {

  std::vector<GLuint> shaderList;

  if(useLighting == true) {
    // load and compile shader for lighting (lights & materials)

    // push vertex shader and fragment shader
    shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "lightingPerVertex.vert"));
    shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "lightingPerVertex.frag"));

    // create the shader program with two shaders
    shaderProgram.program = pgr::createProgram(shaderList);

    // get vertex attributes locations, if the shader does not have this uniform -> return -1
    shaderProgram.posLocation      = glGetAttribLocation(shaderProgram.program, "position");
    shaderProgram.normalLocation   = glGetAttribLocation(shaderProgram.program, "normal");
    shaderProgram.texCoordLocation = glGetAttribLocation(shaderProgram.program, "texCoord");
    // get uniforms locations
    shaderProgram.PVMmatrixLocation    = glGetUniformLocation(shaderProgram.program, "PVMmatrix");
    shaderProgram.VmatrixLocation      = glGetUniformLocation(shaderProgram.program, "Vmatrix");
    shaderProgram.MmatrixLocation      = glGetUniformLocation(shaderProgram.program, "Mmatrix");
    shaderProgram.normalMatrixLocation = glGetUniformLocation(shaderProgram.program, "normalMatrix");
    shaderProgram.timeLocation         = glGetUniformLocation(shaderProgram.program, "time");
    // material
    shaderProgram.ambientLocation      = glGetUniformLocation(shaderProgram.program, "material.ambient");
    shaderProgram.diffuseLocation      = glGetUniformLocation(shaderProgram.program, "material.diffuse");
    shaderProgram.specularLocation     = glGetUniformLocation(shaderProgram.program, "material.specular");
    shaderProgram.shininessLocation    = glGetUniformLocation(shaderProgram.program, "material.shininess");
    // texture
    shaderProgram.texSamplerLocation   = glGetUniformLocation(shaderProgram.program, "texSampler");
    shaderProgram.useTextureLocation   = glGetUniformLocation(shaderProgram.program, "material.useTexture");
    // reflector
    shaderProgram.reflectorPositionLocation  = glGetUniformLocation(shaderProgram.program, "reflectorPosition");
    shaderProgram.reflectorDirectionLocation = glGetUniformLocation(shaderProgram.program, "reflectorDirection");
  }
  else {
    // load and compile simple shader (colors only, no lights at all)

    // push vertex shader and fragment shader
    shaderList.push_back(pgr::createShaderFromSource(GL_VERTEX_SHADER, colorVertexShaderSrc));
    shaderList.push_back(pgr::createShaderFromSource(GL_FRAGMENT_SHADER, colorFragmentShaderSrc));

    // create the program with two shaders (fragment and vertex)
    shaderProgram.program = pgr::createProgram(shaderList);
    // get position and color attributes locations
    shaderProgram.posLocation   = glGetAttribLocation(shaderProgram.program, "position");
    shaderProgram.colorLocation = glGetAttribLocation(shaderProgram.program, "color");
    // get uniforms locations
    shaderProgram.PVMmatrixLocation = glGetUniformLocation(shaderProgram.program, "PVMmatrix");

  }

  // load and compile shader for explosions (dynamic texture)

  shaderList.clear();

  // push vertex shader and fragment shader
  shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "fire.vert"));
  shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "fire.frag"));

  // create the program with two shaders
  fireShaderProgram.program = pgr::createProgram(shaderList);

  // get position and texture coordinates attributes locations
  fireShaderProgram.posLocation = glGetAttribLocation(explosionShaderProgram.program, "position");
  fireShaderProgram.texCoordLocation = glGetAttribLocation(explosionShaderProgram.program, "texCoord");
  // get uniforms locations
  fireShaderProgram.PVMmatrixLocation = glGetUniformLocation(explosionShaderProgram.program, "PVMmatrix");
  fireShaderProgram.VmatrixLocation = glGetUniformLocation(explosionShaderProgram.program, "Vmatrix");
  fireShaderProgram.timeLocation = glGetUniformLocation(explosionShaderProgram.program, "time");
  fireShaderProgram.texSamplerLocation = glGetUniformLocation(explosionShaderProgram.program, "texSampler");
  fireShaderProgram.frameDurationLocation = glGetUniformLocation(explosionShaderProgram.program, "frameDuration");
}

/** Load mesh using assimp library
 *  Vertex, normals and texture coordinates data are stored without interleaving |VVVVV...|NNNNN...|tttt
 * \param fileName [in] file to open/load
 * \param shader [in] vao will connect loaded data to shader
 * \param geometry
 */
bool loadSingleMesh(const std::string &fileName, SCommonShaderProgram& shader, MeshGeometry** geometry) {
  Assimp::Importer importer;

  // Unitize object in size (scale the model to fit into (-1..1)^3)
  importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

  // Load asset from the file - you can play with various processing steps
  const aiScene * scn = importer.ReadFile(fileName.c_str(), 0
      | aiProcess_Triangulate             // Triangulate polygons (if any).
      | aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
      | aiProcess_GenSmoothNormals        // Calculate normals per vertex.
      | aiProcess_JoinIdenticalVertices);

  // abort if the loader fails
  if(scn == NULL) {
    std::cerr << "assimp error: " << importer.GetErrorString() << std::endl;
    *geometry = NULL;
    return false;
  }

  // some formats store whole scene (multiple meshes and materials, lights, cameras, ...) in one file, we cannot handle that in our simplified example
  if(scn->mNumMeshes != 1) {
    std::cerr << "this simplified loader can only process files with only one mesh" << std::endl;
    *geometry = NULL;
    return false;
  }

  // in this phase we know we have one mesh in our loaded scene, we can directly copy its data to OpenGL ...
  const aiMesh * mesh = scn->mMeshes[0];

  *geometry = new MeshGeometry;

  // vertex buffer object, store all vertex positions and normals
  glGenBuffers(1, &((*geometry)->vertexBufferObject));
  glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float)*mesh->mNumVertices, 0, GL_STATIC_DRAW); // allocate memory for vertices, normals, and texture coordinates
  // first store all vertices
  glBufferSubData(GL_ARRAY_BUFFER, 0, 3*sizeof(float)*mesh->mNumVertices, mesh->mVertices);
  // then store all normals
  glBufferSubData(GL_ARRAY_BUFFER, 3*sizeof(float)*mesh->mNumVertices, 3*sizeof(float)*mesh->mNumVertices, mesh->mNormals);
  
  // just texture 0 for now
  float *textureCoords = new float[2 * mesh->mNumVertices];  // 2 floats per vertex
  float *currentTextureCoord = textureCoords;

  // copy texture coordinates
  aiVector3D vect;
    
  if(mesh->HasTextureCoords(0) ) {
    // we use 2D textures with 2 coordinates and ignore the third coordinate
    for(unsigned int idx=0; idx<mesh->mNumVertices; idx++) {
      vect = (mesh->mTextureCoords[0])[idx];
      *currentTextureCoord++ = vect.x;
      *currentTextureCoord++ = vect.y;
    }
  }
    
  // finally store all texture coordinates
  glBufferSubData(GL_ARRAY_BUFFER, 6*sizeof(float)*mesh->mNumVertices, 2*sizeof(float)*mesh->mNumVertices, textureCoords);

  // copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
  unsigned int *indices = new unsigned int[mesh->mNumFaces * 3];
  for(unsigned int f = 0; f < mesh->mNumFaces; ++f) {
    indices[f*3 + 0] = mesh->mFaces[f].mIndices[0];
    indices[f*3 + 1] = mesh->mFaces[f].mIndices[1];
    indices[f*3 + 2] = mesh->mFaces[f].mIndices[2];
  }

  // copy our temporary index array to OpenGL and free the array
  glGenBuffers(1, &((*geometry)->elementBufferObject));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

  delete [] indices;

  // copy the material info to MeshGeometry structure
  const aiMaterial *mat  = scn->mMaterials[mesh->mMaterialIndex];
  aiColor4D color;
  aiString name;
  aiReturn retValue = AI_SUCCESS;

  // Get returns: aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
  mat->Get(AI_MATKEY_NAME, name); // may be "" after the input mesh processing. Must be aiString type!

  if((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
    color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

  (*geometry)->diffuse = glm::vec3(color.r, color.g, color.b);

  if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
    color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
  (*geometry)->ambient = glm::vec3(color.r, color.g, color.b);

  if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
    color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
  (*geometry)->specular = glm::vec3(color.r, color.g, color.b);

  ai_real shininess, strength;
  unsigned int max;	// changed: to unsigned

  max = 1;	
  if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max)) != AI_SUCCESS)
    shininess = 1.0f;
  max = 1;
  if((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
    strength = 1.0f;
  (*geometry)->shininess = shininess * strength;

  (*geometry)->texture = 0;

  // load texture image
  if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
    // get texture name 
    aiString path; // filename

    aiReturn texFound = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
    std::string textureName = path.data;

    size_t found = fileName.find_last_of("/\\");
    // insert correct texture file path 
    if(found != std::string::npos) { // not found
      //subMesh_p->textureName.insert(0, "/");
      textureName.insert(0, fileName.substr(0, found+1));
    }

    std::cout << "Loading texture file: " << textureName << std::endl;
    (*geometry)->texture = pgr::createTexture(textureName);
  }
  CHECK_GL_ERROR();

  glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
  glBindVertexArray((*geometry)->vertexArrayObject);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject); // bind our element array buffer (indices) to vao
  glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);

  glEnableVertexAttribArray(shader.posLocation);
  glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

  if(useLighting == true) {
    glEnableVertexAttribArray(shader.normalLocation);
    glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));
  }
  else {
	  glDisableVertexAttribArray(shader.colorLocation);
	  // following line is problematic on AMD/ATI graphic cards
	  // -> if you see black screen (no objects at all) than try to set color manually in vertex shader to see at least something
    glVertexAttrib3f(shader.colorLocation, color.r, color.g, color.b);
  }

  glEnableVertexAttribArray(shader.texCoordLocation);
  glVertexAttribPointer(shader.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));
  CHECK_GL_ERROR();

  glBindVertexArray(0);

  (*geometry)->numTriangles = mesh->mNumFaces;

  return true;
}

bool loadMultipleMesh(const std::string& fileName, SCommonShaderProgram& shader, std::vector<MeshGeometry*>& geometrys) {
    Assimp::Importer importer;

    importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1); // Unitize object in size (scale the model to fit into (-1..1)^3)
                                                                // Load asset from the file - you can play with various processing steps
    const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
        | aiProcess_Triangulate             // Triangulate polygons (if any).
        | aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
        | aiProcess_GenSmoothNormals        // Calculate normals per vertex.
        | aiProcess_JoinIdenticalVertices);
    // abort if the loader fails
    if (scn == NULL) {
        std::cerr << "assimp error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    // in this phase we know how many meshes in our loaded scene, we can directly copy its data to opengl ...
    for (size_t i = 0; i < scn->mNumMeshes; i++)
    {
        const aiMesh* mesh = scn->mMeshes[i];

        MeshGeometry* geometry = new MeshGeometry;

        // vertex buffer object, store all vertex positions and normals
        glGenBuffers(1, &((geometry)->vertexBufferObject));
        glBindBuffer(GL_ARRAY_BUFFER, (geometry)->vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW); // allocate memory for vertices, normals, and texture coordinates
                                                                                                // first store all vertices
        glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices);
        // then store all normals
        glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals);

        // just texture 0 for now
        float* textureCoords = new float[2 * mesh->mNumVertices];  // 2 floats per vertex
        float* currentTextureCoord = textureCoords;

        // copy texture coordinates
        aiVector3D vect;

        if (mesh->HasTextureCoords(0)) {
            // we use 2D textures with 2 coordinates and ignore the third coordinate
            for (unsigned int idx = 0; idx < mesh->mNumVertices; idx++) {
                vect = (mesh->mTextureCoords[0])[idx];
                *currentTextureCoord++ = vect.x;
                *currentTextureCoord++ = vect.y;
            }
        }

        // finally store all texture coordinates
        glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices, 2 * sizeof(float) * mesh->mNumVertices, textureCoords);

        // copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
        unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
            indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
            indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
            indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
        }

        // copy our temporary index array to OpenGL and free the array
        glGenBuffers(1, &((geometry)->elementBufferObject));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (geometry)->elementBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

        delete[] indices;

        // copy the material info to MeshGeometry structure
        const aiMaterial* mat = scn->mMaterials[mesh->mMaterialIndex];
        aiColor4D color;
        aiString name;
        aiReturn retValue = AI_SUCCESS;

        // Get returns: aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
        mat->Get(AI_MATKEY_NAME, name); // may be "" after the input mesh processing. Must be aiString type!

        if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS) {
            color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
            std::cout << "Material failed..." << std::endl;
        }

        (geometry)->diffuse = glm::vec3(color.r, color.g, color.b);

        if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
            color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
        (geometry)->ambient = glm::vec3(color.r, color.g, color.b);

        if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
            color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
        (geometry)->specular = glm::vec3(color.r, color.g, color.b);

        ai_real shininess, strength;
        unsigned int max;	// changed: to unsigned

        max = 1;
        if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max)) != AI_SUCCESS)
            shininess = 1.0f;
        max = 1;
        if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
            strength = 1.0f;
        (geometry)->shininess = shininess * strength;

        (geometry)->texture = 0;

        // load texture image
        if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            // get texture name 
            aiString path; // filename

            aiReturn texFound = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
            std::string textureName = path.data;

            size_t found = fileName.find_last_of("/\\");
            // insert correct texture file path 
            if (found != std::string::npos) { // not found
                                              //subMesh_p->textureName.insert(0, "/");
                textureName.insert(0, fileName.substr(0, found + 1));
            }

            std::cout << "Loading texture file: " << textureName << std::endl;
            (geometry)->texture = pgr::createTexture(textureName);
        }
        CHECK_GL_ERROR();

        glGenVertexArrays(1, &((geometry)->vertexArrayObject));
        glBindVertexArray((geometry)->vertexArrayObject);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (geometry)->elementBufferObject); // bind our element array buffer (indices) to vao
        glBindBuffer(GL_ARRAY_BUFFER, (geometry)->vertexBufferObject);

        glEnableVertexAttribArray(shader.posLocation);
        glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(shader.normalLocation);
        glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));

        glEnableVertexAttribArray(shader.texCoordLocation);
        glVertexAttribPointer(shader.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));
        CHECK_GL_ERROR();

        glBindVertexArray(0);

        (geometry)->numTriangles = mesh->mNumFaces;

        //push newGeometry to vector of geometies
        geometrys.push_back(geometry);
    }
    return true;
}

void initMissileGeometry(SCommonShaderProgram &shader, MeshGeometry **geometry) {

  *geometry = new MeshGeometry;

  glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
  glBindVertexArray((*geometry)->vertexArrayObject);

  glGenBuffers(1, &((*geometry)->vertexBufferObject));
  glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(missileVertices), missileVertices, GL_STATIC_DRAW);
  CHECK_GL_ERROR();

  glEnableVertexAttribArray(shader.posLocation);
  // vertices of triangles - start at the beginning of the array
  glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

  if(useLighting == false) {
    glEnableVertexAttribArray(shader.colorLocation);
    // colors of vertices start after the positions
    glVertexAttribPointer(shader.colorLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)(missileTrianglesCount * 3 * 3 * sizeof(float)));
  }
  else {
    glEnableVertexAttribArray(shader.normalLocation);
    // normals of vertices start after the colors
    glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)(2 * missileTrianglesCount * 3 * 3 * sizeof(float)));
  }

  (*geometry)->ambient = glm::vec3(0.0f, 1.0f, 1.0f);
  (*geometry)->diffuse = glm::vec3(0.0f, 1.0f, 1.0f);
  (*geometry)->specular = glm::vec3(0.0f, 1.0f, 1.0f);
  (*geometry)->shininess = 10.0f;
  (*geometry)->texture = 0;

  glBindVertexArray(0);

  (*geometry)->numTriangles = missileTrianglesCount;
}

void initUfoGeometry(SCommonShaderProgram &shader, MeshGeometry **geometry) {

  *geometry = new MeshGeometry;

  glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
  glBindVertexArray((*geometry)->vertexArrayObject);

  glGenBuffers(1, &((*geometry)->vertexBufferObject));
  glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ufoVertices), ufoVertices, GL_STATIC_DRAW);

  // copy our temporary index array to opengl and free the array
  glGenBuffers(1, &((*geometry)->elementBufferObject));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int) * ufoTrianglesCount, ufoIndices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(shader.posLocation);
  // vertices of triangles - start at the beginning of the array
  glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);

  if(useLighting == false) {
    glEnableVertexAttribArray(shader.colorLocation);
    // color of vertex starts after the position (interlaced arrays)
    glVertexAttribPointer(shader.colorLocation, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
  }
  else {
    glEnableVertexAttribArray(shader.normalLocation);
    // normal of vertex starts after the color (interlaced array)
    glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
  }

  (*geometry)->ambient = glm::vec3(1.0f, 0.0f, 1.0f);
  (*geometry)->diffuse = glm::vec3(1.0f, 0.0f, 1.0f);
  (*geometry)->specular = glm::vec3(1.0f, 0.0f, 1.0f);
  (*geometry)->shininess = 10.0f;
  (*geometry)->texture = 0;

  glBindVertexArray(0);

  (*geometry)->numTriangles = ufoTrianglesCount;
}

void initExplosionGeometry(GLuint shader, MeshGeometry** geometry) {

    *geometry = new MeshGeometry;

    (*geometry)->texture = pgr::createTexture(FIRE_TEXTURE_NAME);

    glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
    glBindVertexArray((*geometry)->vertexArrayObject);

    glGenBuffers(1, &((*geometry)->vertexBufferObject)); \
        glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(explosionVertexData), explosionVertexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(explosionShaderProgram.posLocation);
    // vertices of triangles - start at the beginning of the array (interlaced array)
    glVertexAttribPointer(explosionShaderProgram.posLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

    glEnableVertexAttribArray(explosionShaderProgram.texCoordLocation);
    // texture coordinates are placed just after the position of each vertex (interlaced array)
    glVertexAttribPointer(explosionShaderProgram.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

    (*geometry)->numTriangles = explosionNumQuadVertices;
}

void initDuckGeometry(SCommonShaderProgram& shader, MeshGeometry** geometry) {

    *geometry = new MeshGeometry;

    glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
    glBindVertexArray((*geometry)->vertexArrayObject);

    glGenBuffers(1, &((*geometry)->vertexBufferObject));
    glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(birdVertices), birdVertices, GL_STATIC_DRAW);

    // copy our temporary index array to opengl and free the array
    glGenBuffers(1, &((*geometry)->elementBufferObject));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int) * birdNTriangles, birdTriangles, GL_STATIC_DRAW);

    initMaterial(geometry, duckMaterial);

    glEnableVertexAttribArray(shader.posLocation);
    // vertices of triangles - start at the beginning of the array
    glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

    glEnableVertexAttribArray(shader.normalLocation);
    // color of vertex starts after the position (interlaced arrays)
    glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(shader.texCoordLocation);
    glVertexAttribPointer(shader.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

    (*geometry)->numTriangles = birdNTriangles;
}


/** Initialize vertex buffers and vertex arrays for all objects. 
 */
void initializeModels() {

    if (loadMultipleMesh(BENCH_MODEL_NAME, shaderProgram, benchGeometry) != true) {
        std::cerr << "initializeModels(): Bench model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadSingleMesh(TREE_MODEL_NAME, shaderProgram, &treeGeometry) != true) {
        std::cerr << "initializeModels(): Tree model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadSingleMesh(FIREPLACE_MODEL_NAME, shaderProgram, &fireplaceGeometry) != true) {
        std::cerr << "initializeModels(): Tree model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadSingleMesh(GROUND_MODEL_NAME, shaderProgram, &groundGeometry) != true) {
        std::cerr << "Ground model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    // load asteroid model from external file
    if(loadSingleMesh(ASTEROID_MODEL_NAME, shaderProgram, &asteroidGeometry) != true) {
        std::cerr << "initializeModels(): Asteroid model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    // load space ship model from external file
    if(loadSingleMesh(SPACESHIP_MODEL_NAME, shaderProgram, &spaceShipGeometry) != true) {
        std::cerr << "initializeModels(): Space ship model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    // fill MeshGeometry structure for missile object
    initMissileGeometry(shaderProgram, &missileGeometry);

    // fill MeshGeometry structure for ufo object
    initUfoGeometry(shaderProgram, &ufoGeometry);

    initDuckGeometry(shaderProgram, &duckGeometry);

}

void cleanupGeometry(MeshGeometry *geometry) {

  glDeleteVertexArrays(1, &(geometry->vertexArrayObject));
  glDeleteBuffers(1, &(geometry->elementBufferObject));
  glDeleteBuffers(1, &(geometry->vertexBufferObject));

  if(geometry->texture != 0)
    glDeleteTextures(1, &(geometry->texture));
}

void cleanupModels() {
  cleanupGeometry(treeGeometry);
  cleanupGeometry(spaceShipGeometry);
  cleanupGeometry(asteroidGeometry);
  cleanupGeometry(missileGeometry);
  cleanupGeometry(ufoGeometry);
  cleanupGeometry(duckGeometry);

  for (std::vector<MeshGeometry*>::iterator it = benchGeometry.begin(); it != benchGeometry.end(); ++it) {
      cleanupGeometry(*it);
  }
}
