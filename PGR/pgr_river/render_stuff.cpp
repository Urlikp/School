//----------------------------------------------------------------------------------------
/**
 * \file    render_stuff.cpp
 * \author  Jiøí Povolný
 * \date    2021/5/20
 * \brief   Rendering stuff - drawing functions for models, etc..
*/
//----------------------------------------------------------------------------------------

#include <iostream>
#include "pgr.h"
#include "render_stuff.h"
#include "data.h"
#include "spline.h"
#include "bird.h"

MeshGeometry* groundGeometry = NULL;
MeshGeometry* fishGeometry = NULL;
MeshGeometry* fireGeometry = NULL;
MeshGeometry* waterGeometry = NULL;
MeshGeometry* skyboxGeometry = NULL;
MeshGeometry* fireplaceGeometry = NULL;
MeshGeometry* benchGeometry = NULL;
MeshGeometry* droneGeometry = NULL;
MeshGeometry* kapradiGeometry = NULL;
MeshGeometry* duckGeometry = NULL;
MeshGeometry* woodtrunkGeometry = NULL;
MeshGeometry* stoneGeometry = NULL;
MeshGeometry* crateGeometry = NULL;
MeshGeometry* palmtreeGeometry = NULL;
MeshGeometry* crocusGeometry = NULL;
MeshGeometry* sunflowerGeometry = NULL;

std::vector<MeshGeometry*> woodpileGeometry;
std::vector<MeshGeometry*> tulipGeometry;
std::vector<MeshGeometry*> snowdropGeometry;

const char* FIREPLACE_MODEL_NAME = "data/Fireplace/fireplace.obj";
const char* WOODPILE_MODEL_NAME = "data/woodpile/WoodPile.obj";
const char* WOODTRUNK_MODEL_NAME = "data/WoodTrunk/WoodTrunk.obj";
const char* STONE_MODEL_NAME = "data/stone/Stone.obj";
const char* CRATE_MODEL_NAME = "data/crate/WoodenCrate.obj";
const char* TULIP_MODEL_NAME = "data/tulip/tulip.obj";
const char* PALMTREE_MODEL_NAME = "data/palmtree/PalmTree.obj";
const char* SNOWDROP_MODEL_NAME = "data/snowdrop/snowdrop.obj";
const char* CROCUS_MODEL_NAME = "data/crocus/crocus.obj";
const char* SUNFLOWER_MODEL_NAME = "data/Sunflower/Sunflower.obj";
const char* GROUND_MODEL_NAME = "data/ground/ground.obj";
const char* BENCH_MODEL_NAME = "data/bench/bench.obj";
const char* DRONE_MODEL_NAME = "data/drone/drone.obj";
const char* FISH_MODEL_NAME = "data/ryba/ryba.obj";
const char* KAPRADI_MODEL_NAME = "data/kapradi/kapradi.obj";
const char* FIRE_TEXTURE_NAME = "data/Fireplace/fire.png";
const char* WATER_TEXTURE_NAME = "data/water/water.png";
const char* DUCK_TEXTURE_NAME = "data/duck/Bird.jpg";
const char* SKYBOX_CUBE_TEXTURE_FILE_PREFIX = "data/skybox/skybox";

SCommonShaderProgram shaderProgram;

bool useLighting = false;
bool fogEnabled = false;

/// Struct for duck material
const Material duckMaterial = {
    glm::vec3(1.0f, 1.0f, 1.0f),        ///< ambient
    glm::vec3(1.0f, 1.0f, 1.0f),        ///< diffuse
    glm::vec3(0.288f, 0.288f, 0.288f),  ///< specular
    0.1f,                               ///< shinines
    DUCK_TEXTURE_NAME		            ///< texture name
};

/// Struct for water material
const Material waterMaterial = {
    glm::vec3(0.5f, 0.5f, 0.5f),    ///< ambient
    glm::vec3(0.5f, 0.5f, 0.5f),    ///< deffuse
    glm::vec3(0.8f, 0.8f, 0.8f),    ///< specular
    1.1f,                           ///< shinines
    WATER_TEXTURE_NAME		        ///< texture name
};

/// Struct for sunlight 
Light sunLight = {
    true,                           ///< on
    glm::vec3(0.0f),                ///< ambient
    glm::vec3(1.0f, 1.0f, 0.5f),    ///< diffuse
    glm::vec3(1.0f),                ///< specular
    glm::vec3(0.0f)                 ///< position
};

/// Struct for reflectorlight
Light reflectorLight = {
    false,              ///< on
    glm::vec3(0.2f),    ///< ambient
    glm::vec3(1.0f),    ///< diffuse
    glm::vec3(1.0f),    ///< specular
    glm::vec3(0.0f),    ///< position
    glm::vec3(0.0f),    ///< spotDirection
    0.95f,              ///< spotCosCutOff
    5.0                 ///< spotExponent
};

/// Struct for firelight
Light fireLight = {
    false,              ///< on
    glm::vec3(0.1f),    ///< ambient
    glm::vec3(0.2f),    ///< diffuse
    glm::vec3(0.1f),    ///< specular
    glm::vec3(0.0f),    ///< position
    glm::vec3(0.0f),    ///< spotDirection
    0.0f,               ///< spotCosCutOff
    0.0f,               ///< spotExponent
    2.0f                ///< quadraticAttenuation
};

/// Struct for fire shader
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

/// Struct for skybox shader
struct SkyboxFarPlaneShaderProgram {
    // identifier for the shader program
    GLuint program;                 // = 0;
    // vertex attributes locations
    GLint screenCoordLocation;      // = -1;
    // uniforms locations
    GLint inversePVmatrixLocation;  // = -1;
    GLint skyboxSamplerLocation;    // = -1;
    GLint fogOn;                    // = false;
} skyboxFarPlaneShaderProgram;

void enableFog() {
    fogEnabled = true;
}

void disableFog() {
    fogEnabled = false;
}

void changeFog() {
    fogEnabled = !fogEnabled;
}

/// Sets Light values
/**
    \param[in] viewMatrix           View (world to eye coordinates)
    \param[in] elapsedTime          Current time
    \param[in] sunOn                Is sun on
    \param[in] reflectorOn          Is reflector on
    \param[in] reflectorPosition    Position of reflector
    \param[in] reflectorDirection   Direction of reflector
    \param[in] fireOn               Is fire on
    \param[in] firePosition         Position of fire
*/
void setLightValues(glm::mat4 viewMatrix, float elapsedTime,
                    bool sunOn,
                    bool reflectorOn, glm::vec3 reflectorPosition, glm::vec3 reflectorDirection,
                    bool fireOn, glm::vec3 firePosition) {
    sunLight.on = sunOn;
    sunLight.position = glm::vec3(viewMatrix * glm::vec4(cos(elapsedTime * SUN_SPEED), 0.0f, sin(elapsedTime * SUN_SPEED), 0.0f));

    reflectorLight.on = reflectorOn;
    reflectorLight.position = glm::vec3(viewMatrix * glm::vec4(reflectorPosition, 1.0f));
    reflectorLight.spotDirection = normalize(glm::vec3(viewMatrix * glm::vec4(reflectorDirection, 0.0f)));

    fireLight.on = fireOn;
    fireLight.position = glm::vec3(viewMatrix * glm::vec4(firePosition, 1.0f));
}

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

/// Set light uniforms
void setLightUniforms() {
    glUniform1i(shaderProgram.sunLightOn, sunLight.on);
    glUniform3fv(shaderProgram.sunLightAmbient, 1, glm::value_ptr(sunLight.ambient));
    glUniform3fv(shaderProgram.sunLightDiffuse, 1, glm::value_ptr(sunLight.diffuse));
    glUniform3fv(shaderProgram.sunLightSpecular, 1, glm::value_ptr(sunLight.specular));
    glUniform3fv(shaderProgram.sunLightPosition, 1, glm::value_ptr(sunLight.position));

    glUniform1i(shaderProgram.reflectorLightOn, reflectorLight.on);
    glUniform3fv(shaderProgram.reflectorLightAmbient, 1, glm::value_ptr(reflectorLight.ambient));
    glUniform3fv(shaderProgram.reflectorLightDiffuse, 1, glm::value_ptr(reflectorLight.diffuse));
    glUniform3fv(shaderProgram.reflectorLightSpecular, 1, glm::value_ptr(reflectorLight.specular));
    glUniform3fv(shaderProgram.reflectorLightPosition, 1, glm::value_ptr(reflectorLight.position));
    glUniform3fv(shaderProgram.reflectorLightSpotDirection, 1, glm::value_ptr(reflectorLight.spotDirection));
    glUniform1f(shaderProgram.reflectorLightSpotCosCutOff, reflectorLight.spotCosCutOff);
    glUniform1f(shaderProgram.reflectorLightSpotExponent, reflectorLight.spotExponent);

    glUniform1i(shaderProgram.fireLightOn, fireLight.on);
    glUniform3fv(shaderProgram.fireLightAmbient, 1, glm::value_ptr(fireLight.ambient));
    glUniform3fv(shaderProgram.fireLightDiffuse, 1, glm::value_ptr(fireLight.diffuse));
    glUniform3fv(shaderProgram.fireLightSpecular, 1, glm::value_ptr(fireLight.specular));
    glUniform3fv(shaderProgram.fireLightPosition, 1, glm::value_ptr(fireLight.position));
    glUniform1f(shaderProgram.fireLightQuadraticAtt, fireLight.quadraticAttenuation);
}

/// Draw sinle mesh object
/**
    \param[in] object           Object
    \param[in] geometry         Geometry of object
    \param[in] viewMatrix       View (world to eye coordinates)
    \param[in] projectionMatrix Projection
    \param[in] rotationDegrees  Rotation degrees
    \param[in] rotationAxis     Rotation axis
*/
void drawObject(Object* object, MeshGeometry* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float rotationDegrees, glm::vec3 rotationAxis) {
    glUseProgram(shaderProgram.program);

    // prepare modelling transform matrix
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), object->position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(object->size));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationDegrees), rotationAxis);

    // send matrices to the vertex & fragment shader
    setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

    setMaterialUniforms(
        geometry->ambient,
        geometry->diffuse,
        geometry->specular,
        geometry->shininess,
        geometry->texture
    );

    // draw geometry
    glBindVertexArray(geometry->vertexArrayObject);
    glDrawElements(GL_TRIANGLES, geometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}

/// Draw multi mesh object
/**
    \param[in] object           Object
    \param[in] geometry         Geometry of object
    \param[in] viewMatrix       View (world to eye coordinates)
    \param[in] projectionMatrix Projection
    \param[in] rotationDegrees  Rotation degrees
    \param[in] rotationAxis     Rotation axis
*/
void drawObject(Object* object, std::vector<MeshGeometry*> geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float rotationDegrees, glm::vec3 rotationAxis) {
    glUseProgram(shaderProgram.program);

    // prepare modelling transform matrix
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), object->position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(object->size));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationDegrees), rotationAxis);

    // send matrices to the vertex & fragment shader
    setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

    for (int i = 0; i < geometry.size(); i++) {
        setMaterialUniforms(
            geometry.at(i)->ambient,
            geometry.at(i)->diffuse,
            geometry.at(i)->specular,
            geometry.at(i)->shininess,
            geometry.at(i)->texture
        );

        // draw geometry
        glBindVertexArray(geometry.at(i)->vertexArrayObject);
        glDrawElements(GL_TRIANGLES, geometry.at(i)->numTriangles * 3, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
    glUseProgram(0);

    CHECK_GL_ERROR();
}

/// Draw moving object
/**
    \param[in] object           Object
    \param[in] geometry         Geometry of object
    \param[in] viewMatrix       View (world to eye coordinates)
    \param[in] projectionMatrix Projection
*/
void drawObject(MovingObject* object, MeshGeometry* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    glUseProgram(shaderProgram.program);

    // align fish coordinate system to match its position and direction - see alignObject() function
    glm::mat4 modelMatrix = alignObject(object->position, object->direction, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(object->size));

    // send matrices to the vertex & fragment shader
    setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

    setMaterialUniforms(
        geometry->ambient,
        geometry->diffuse,
        geometry->specular,
        geometry->shininess,
        geometry->texture
    );

    // draw geometry
    glBindVertexArray(geometry->vertexArrayObject);
    glDrawElements(GL_TRIANGLES, geometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}

void drawDuck(Object* duck, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    glUseProgram(shaderProgram.program);

    glUniform1i(shaderProgram.isDuck, 1);

    // prepare modelling transform matrix
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), duck->position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(duck->size));

    // send matrices to the vertex & fragment shader
    setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

    const float change = cos(duck->currentTime - duck->startTime);
    glUniform1f(shaderProgram.duckFactor, change);

    setMaterialUniforms(
        duckGeometry->ambient,
        duckGeometry->diffuse,
        duckGeometry->specular,
        duckGeometry->shininess,
        duckGeometry->texture
    );

    // draw geometry
    glBindVertexArray(duckGeometry->vertexArrayObject);
    glDrawElements(GL_TRIANGLES, duckGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

    glUniform1i(shaderProgram.isDuck, 0);

    CHECK_GL_ERROR();

    glBindVertexArray(0);
    glUseProgram(0);
}

void drawFire(Fire* fire, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {   
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

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
}

void drawWater(float elapsedTime, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) { 
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glUseProgram(shaderProgram.program);
    glDepthMask(GL_FALSE);

    glUniform1i(shaderProgram.isWater, 1);

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1));
    setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
    glUniform1f(shaderProgram.timeLocation, elapsedTime);

    setMaterialUniforms(
        waterGeometry->ambient,
        waterGeometry->diffuse,
        waterGeometry->specular,
        waterGeometry->shininess,
        waterGeometry->texture
    );

    glBindVertexArray(waterGeometry->vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, 3 * waterGeometry->numTriangles);
    glDepthMask(GL_TRUE);

    glUniform1i(shaderProgram.isWater, 0);

    CHECK_GL_ERROR();

    glBindVertexArray(0);
    glUseProgram(0);

    glDisable(GL_BLEND);
}

void drawSkybox(const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
    glUseProgram(skyboxFarPlaneShaderProgram.program);

    // create view rotation matrix by using view matrix with cleared translation
    glm::mat4 viewRotation = viewMatrix;
    viewRotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // vertex shader will translate screen space coordinates (NDC) using inverse PV matrix
    glm::mat4 inversePVmatrix = glm::inverse(projectionMatrix * viewRotation);

    glUniformMatrix4fv(skyboxFarPlaneShaderProgram.inversePVmatrixLocation, 1, GL_FALSE, glm::value_ptr(inversePVmatrix));
    glUniform1i(skyboxFarPlaneShaderProgram.skyboxSamplerLocation, 0);
    glUniform1i(skyboxFarPlaneShaderProgram.fogOn, fogEnabled);

    // draw "skybox" rendering 2 triangles covering the far plane
    glBindVertexArray(skyboxGeometry->vertexArrayObject);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxGeometry->texture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, skyboxGeometry->numTriangles+2);

    glBindVertexArray(0);
    glUseProgram(0);
}

void cleanupShaderPrograms(void) {
    pgr::deleteProgramAndShaders(shaderProgram.program);
    pgr::deleteProgramAndShaders(fireShaderProgram.program);
    pgr::deleteProgramAndShaders(skyboxFarPlaneShaderProgram.program);
}

void initializeShaderPrograms(void) {
    std::vector<GLuint> shaderList;

    if(useLighting == true) {
        // load and compile shader for lighting (lights & materials)

        // push vertex shader and fragment shader
        shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "vertex.vert"));
        shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "fragment.frag"));

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
        // fog
        shaderProgram.fogOn = glGetUniformLocation(shaderProgram.program, "fogOn");
        shaderProgram.fogDensity = glGetUniformLocation(shaderProgram.program, "fogDensity");
        // water
        shaderProgram.isWater = glGetUniformLocation(shaderProgram.program, "isWater");
        // duck
        shaderProgram.duckOn = glGetUniformLocation(shaderProgram.program, "duckOn");
        shaderProgram.isDuck = glGetUniformLocation(shaderProgram.program, "isDuck");
        shaderProgram.duckFactor = glGetUniformLocation(shaderProgram.program, "duckFactor");
        // reflector
        shaderProgram.reflectorLightOn = glGetUniformLocation(shaderProgram.program, "reflector.on");
        shaderProgram.reflectorLightAmbient = glGetUniformLocation(shaderProgram.program, "reflector.ambient");
        shaderProgram.reflectorLightDiffuse = glGetUniformLocation(shaderProgram.program, "reflector.diffuse");
        shaderProgram.reflectorLightSpecular = glGetUniformLocation(shaderProgram.program, "reflector.specular");
        shaderProgram.reflectorLightPosition = glGetUniformLocation(shaderProgram.program, "reflector.position");
        shaderProgram.reflectorLightSpotDirection = glGetUniformLocation(shaderProgram.program, "reflector.spotDirection");
        shaderProgram.reflectorLightSpotCosCutOff = glGetUniformLocation(shaderProgram.program, "reflector.spotCosCutOff");
        shaderProgram.reflectorLightSpotExponent = glGetUniformLocation(shaderProgram.program, "reflector.spotExponent");
        // sun
        shaderProgram.sunLightOn = glGetUniformLocation(shaderProgram.program, "sun.on");
        shaderProgram.sunLightAmbient = glGetUniformLocation(shaderProgram.program, "sun.ambient");
        shaderProgram.sunLightDiffuse = glGetUniformLocation(shaderProgram.program, "sun.diffuse");
        shaderProgram.sunLightSpecular = glGetUniformLocation(shaderProgram.program, "sun.specular");
        shaderProgram.sunLightPosition = glGetUniformLocation(shaderProgram.program, "sun.position");
        // fire
        shaderProgram.fireLightOn = glGetUniformLocation(shaderProgram.program, "fire.on");
        shaderProgram.fireLightAmbient = glGetUniformLocation(shaderProgram.program, "fire.ambient");
        shaderProgram.fireLightDiffuse = glGetUniformLocation(shaderProgram.program, "fire.diffuse");
        shaderProgram.fireLightSpecular = glGetUniformLocation(shaderProgram.program, "fire.specular");
        shaderProgram.fireLightPosition = glGetUniformLocation(shaderProgram.program, "fire.position");
        shaderProgram.fireLightQuadraticAtt = glGetUniformLocation(shaderProgram.program, "fire.quadraticAttenuation");
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
    fireShaderProgram.posLocation      = glGetAttribLocation(fireShaderProgram.program, "position");
    fireShaderProgram.texCoordLocation = glGetAttribLocation(fireShaderProgram.program, "texCoord");
    // get uniforms locations
    fireShaderProgram.PVMmatrixLocation     = glGetUniformLocation(fireShaderProgram.program, "PVMmatrix");
    fireShaderProgram.VmatrixLocation       = glGetUniformLocation(fireShaderProgram.program, "Vmatrix");
    fireShaderProgram.timeLocation          = glGetUniformLocation(fireShaderProgram.program, "time");
    fireShaderProgram.texSamplerLocation    = glGetUniformLocation(fireShaderProgram.program, "texSampler");
    fireShaderProgram.frameDurationLocation = glGetUniformLocation(fireShaderProgram.program, "frameDuration");

    // load and compile shader for skybox (cube map)

    shaderList.clear();

    // push vertex shader and fragment shader
    shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "skybox.vert"));
    shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "skybox.frag"));
   
    // create the program with two shaders
    skyboxFarPlaneShaderProgram.program = pgr::createProgram(shaderList);

    // handles to vertex attributes locations
    skyboxFarPlaneShaderProgram.screenCoordLocation = glGetAttribLocation(skyboxFarPlaneShaderProgram.program, "screenCoord");
    // get uniforms locations
    skyboxFarPlaneShaderProgram.skyboxSamplerLocation   = glGetUniformLocation(skyboxFarPlaneShaderProgram.program, "skyboxSampler");
    skyboxFarPlaneShaderProgram.inversePVmatrixLocation = glGetUniformLocation(skyboxFarPlaneShaderProgram.program, "inversePVmatrix");
    skyboxFarPlaneShaderProgram.fogOn = glGetUniformLocation(skyboxFarPlaneShaderProgram.program, "fogOn");
}

/// Load mesh using assimp library
/**
    Vertex, normals and texture coordinates data are stored without interleaving |VVVVV...|NNNNN...|tttt

    \param[in] fileName file to open/load
    \param[in] shader   vao will connect loaded data to shader
    \param[in] geometry geometry of model
*/
bool loadMesh(const std::string& fileName, SCommonShaderProgram& shader, std::vector<MeshGeometry*>* fullGeometry) {
    Assimp::Importer importer;
    MeshGeometry* geometry;

    // Unitize object in size (scale the model to fit into (-1..1)^3)
    importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

    // Load asset from the file - you can play with various processing steps
    const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
        | aiProcess_Triangulate             // Triangulate polygons (if any).
        | aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
        | aiProcess_GenSmoothNormals        // Calculate normals per vertex.
        | aiProcess_JoinIdenticalVertices);

    // abort if the loader fails
    if (scn == NULL) {
        std::cerr << "assimp error: " << importer.GetErrorString() << std::endl;
        geometry = NULL;
        return false;
    }

    aiMesh* mesh;

    for (int i = 0; i < scn->mNumMeshes; i++) {
        mesh = scn->mMeshes[i];

        geometry = new MeshGeometry;

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

        if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
            color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

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

        if (useLighting == true) {
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

        (geometry)->numTriangles = mesh->mNumFaces;
        fullGeometry->push_back(geometry);
    }

    return true;
}

/// Load mesh using assimp library
/**
    Vertex, normals and texture coordinates data are stored without interleaving |VVVVV...|NNNNN...|tttt

    \param[in] fileName file to open/load
    \param[in] shader   vao will connect loaded data to shader
    \param[in] geometry geometry of model
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

    glEnableVertexAttribArray(shader.posLocation);
    // vertices of triangles - start at the beginning of the array
    glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

    glEnableVertexAttribArray(shader.normalLocation);
    // color of vertex starts after the position (interlaced arrays)
    glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(shader.texCoordLocation);
    glVertexAttribPointer(shader.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    (*geometry)->ambient = duckMaterial.ambient;
    (*geometry)->diffuse = duckMaterial.diffuse;
    (*geometry)->specular = duckMaterial.specular;
    (*geometry)->shininess = duckMaterial.shininess;
    (*geometry)->texture = pgr::createTexture(duckMaterial.texture);

    glBindVertexArray(0);

    (*geometry)->numTriangles = birdNTriangles;
    CHECK_GL_ERROR();
}

void initWaterGeometry(SCommonShaderProgram& shader, MeshGeometry** geometry) {
    *geometry = new MeshGeometry;
    
    glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
    glBindVertexArray((*geometry)->vertexArrayObject);

    glGenBuffers(1, &((*geometry)->vertexBufferObject));
    glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertexData), waterVertexData, GL_STATIC_DRAW);

    (*geometry)->ambient = waterMaterial.ambient;
    (*geometry)->diffuse = waterMaterial.diffuse;
    (*geometry)->specular = waterMaterial.specular;
    (*geometry)->shininess = waterMaterial.shininess;
    (*geometry)->texture = pgr::createTexture(waterMaterial.texture);

    glEnableVertexAttribArray(shader.posLocation);
    glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(shader.normalLocation);
    glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(shader.texCoordLocation);
    glVertexAttribPointer(shader.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

    (*geometry)->numTriangles = waterNumQuadVertices;
    CHECK_GL_ERROR();
}

void initfireGeometry(GLuint shader, MeshGeometry **geometry) {
    *geometry = new MeshGeometry;

    (*geometry)->texture = pgr::createTexture(FIRE_TEXTURE_NAME);

    glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
    glBindVertexArray((*geometry)->vertexArrayObject);

    glGenBuffers(1, &((*geometry)->vertexBufferObject));\
    glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fireVertexData), fireVertexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(fireShaderProgram.posLocation);
    // vertices of triangles - start at the beginning of the array (interlaced array)
    glVertexAttribPointer(fireShaderProgram.posLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

    glEnableVertexAttribArray(fireShaderProgram.texCoordLocation);
    // texture coordinates are placed just after the position of each vertex (interlaced array)
    glVertexAttribPointer(fireShaderProgram.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

    (*geometry)->numTriangles = fireNumQuadVertices;
    CHECK_GL_ERROR();
}

void initSkyboxGeometry(GLuint shader, MeshGeometry **geometry) {
    *geometry = new MeshGeometry;

    // 2D coordinates of 2 triangles covering the whole screen (NDC), draw using triangle strip
    static const float screenCoords[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
    glBindVertexArray((*geometry)->vertexArrayObject);

    // buffer for far plane rendering
    glGenBuffers(1, &((*geometry)->vertexBufferObject));\
    glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenCoords), screenCoords, GL_STATIC_DRAW);

    //glUseProgram(farplaneShaderProgram);

    glEnableVertexAttribArray(skyboxFarPlaneShaderProgram.screenCoordLocation);
    glVertexAttribPointer(skyboxFarPlaneShaderProgram.screenCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
    glUseProgram(0);
    CHECK_GL_ERROR();

    (*geometry)->numTriangles = 2;

    glActiveTexture(GL_TEXTURE0);

    glGenTextures(1, &((*geometry)->texture));
    glBindTexture(GL_TEXTURE_CUBE_MAP, (*geometry)->texture);

    const char * suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
    const GLuint targets[] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    for( int i = 0; i < 6; i++ ) {
        std::string texName = std::string(SKYBOX_CUBE_TEXTURE_FILE_PREFIX) + "_" + suffixes[i] + ".jpg";
        std::cout << "Loading cube map texture: " << texName << std::endl;
        if(!pgr::loadTexImage2D(texName, targets[i])) {
            pgr::dieWithError("Skybox cube map loading failed!");
        }
    }

    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // unbind the texture (just in case someone will mess up with texture calls later)
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    CHECK_GL_ERROR();
}

/// Initialize vertex buffers and vertex arrays for all objects. 
void initializeModels() {
    if (loadSingleMesh(GROUND_MODEL_NAME, shaderProgram, &groundGeometry) != true) {
        std::cerr << "initializeModels(): Ground model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadSingleMesh(FIREPLACE_MODEL_NAME, shaderProgram, &fireplaceGeometry) != true) {
        std::cerr << "initializeModels(): Fireplace model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadSingleMesh(BENCH_MODEL_NAME, shaderProgram, &benchGeometry) != true) {
        std::cerr << "initializeModels(): Bench model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadSingleMesh(DRONE_MODEL_NAME, shaderProgram, &droneGeometry) != true) {
        std::cerr << "initializeModels(): Bench model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadSingleMesh(KAPRADI_MODEL_NAME, shaderProgram, &kapradiGeometry) != true) {
        std::cerr << "initializeModels(): Bench model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadSingleMesh(FISH_MODEL_NAME, shaderProgram, &fishGeometry) != true) {
        std::cerr << "initializeModels(): Fish model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadSingleMesh(STONE_MODEL_NAME, shaderProgram, &stoneGeometry) != true) {
        std::cerr << "initializeModels(): Stone model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadSingleMesh(CRATE_MODEL_NAME, shaderProgram, &crateGeometry) != true) {
        std::cerr << "initializeModels(): Crate model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();    

    if (loadSingleMesh(PALMTREE_MODEL_NAME, shaderProgram, &palmtreeGeometry) != true) {
        std::cerr << "initializeModels(): Palmtree model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadSingleMesh(CROCUS_MODEL_NAME, shaderProgram, &crocusGeometry) != true) {
        std::cerr << "initializeModels(): Crocus model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadSingleMesh(SUNFLOWER_MODEL_NAME, shaderProgram, &sunflowerGeometry) != true) {
        std::cerr << "initializeModels(): Sunflower model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadSingleMesh(WOODTRUNK_MODEL_NAME, shaderProgram, &woodtrunkGeometry) != true) {
        std::cerr << "initializeModels(): Woodtrunk model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadMesh(WOODPILE_MODEL_NAME, shaderProgram, &woodpileGeometry) != true) {
        std::cerr << "initializeModels(): Woodpile model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadMesh(TULIP_MODEL_NAME, shaderProgram, &tulipGeometry) != true) {
        std::cerr << "initializeModels(): Tulip model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    if (loadMesh(SNOWDROP_MODEL_NAME, shaderProgram, &snowdropGeometry) != true) {
        std::cerr << "initializeModels(): Snowdrop model loading failed." << std::endl;
    }
    CHECK_GL_ERROR();

    // fill MeshGeometry structure for explosion object
    initfireGeometry(fireShaderProgram.program, &fireGeometry);

    // fill MeshGeometry structure for skybox object
    initSkyboxGeometry(skyboxFarPlaneShaderProgram.program, &skyboxGeometry);

    initDuckGeometry(shaderProgram, &duckGeometry);

    initWaterGeometry(shaderProgram, &waterGeometry);

    fogEnabled = false;
}

void cleanupGeometry(MeshGeometry *geometry) {
    glDeleteVertexArrays(1, &(geometry->vertexArrayObject));
    glDeleteBuffers(1, &(geometry->elementBufferObject));
    glDeleteBuffers(1, &(geometry->vertexBufferObject));

    if(geometry->texture != 0)
        glDeleteTextures(1, &(geometry->texture));
}

void cleanupModels() {
    cleanupGeometry(fishGeometry);
    cleanupGeometry(waterGeometry);
    cleanupGeometry(fireGeometry);
    cleanupGeometry(fireplaceGeometry);
    cleanupGeometry(groundGeometry);
    cleanupGeometry(benchGeometry);
    cleanupGeometry(droneGeometry);
    cleanupGeometry(kapradiGeometry);
    cleanupGeometry(duckGeometry);
    cleanupGeometry(skyboxGeometry);
    cleanupGeometry(woodtrunkGeometry);
    cleanupGeometry(stoneGeometry);
    cleanupGeometry(crateGeometry);
    cleanupGeometry(palmtreeGeometry);
    cleanupGeometry(crocusGeometry);
    cleanupGeometry(sunflowerGeometry);

    for (std::vector<MeshGeometry*>::iterator it = woodpileGeometry.begin(); it != woodpileGeometry.end(); ++it) {
        cleanupGeometry(*it);
    }

    for (std::vector<MeshGeometry*>::iterator it = tulipGeometry.begin(); it != tulipGeometry.end(); ++it) {
        cleanupGeometry(*it);
    }

    for (std::vector<MeshGeometry*>::iterator it = snowdropGeometry.begin(); it != snowdropGeometry.end(); ++it) {
        cleanupGeometry(*it);
    }
}
