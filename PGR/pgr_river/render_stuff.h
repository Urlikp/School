//----------------------------------------------------------------------------------------
/**
 * \file    render_stuff.h
 * \author  Jiøí Povolný
 * \date    2021/5/20
 * \brief   Rendering stuff - drawing functions for models, etc.
*/
//----------------------------------------------------------------------------------------

#ifndef __RENDER_STUFF_H
#define __RENDER_STUFF_H

#include "data.h"

/// defines geometry of object in the scene (space ship, ufo, asteroid, etc.)
/// geometry is shared among all instances of the same object type
typedef struct _MeshGeometry {
	GLuint        vertexBufferObject;   ///< identifier for the vertex buffer object
	GLuint        elementBufferObject;  ///< identifier for the element buffer object
	GLuint        vertexArrayObject;    ///< identifier for the vertex array object
	unsigned int  numTriangles;         ///< number of triangles in the mesh
	// material
	glm::vec3     ambient;		///< ambient
	glm::vec3     diffuse;		///< diffuse
	glm::vec3     specular;		///< specular
	float         shininess;	///< shininess
	GLuint        texture;		///< texture
} MeshGeometry;

/// struct for material parametrs
typedef struct _Material {
    glm::vec3     ambient;		///< ambient
    glm::vec3     diffuse;		///< diffuse
    glm::vec3     specular;		///< specular
    float         shininess;	///< shininess
    std::string   texture;		///< texture
} Material;

/// struct for light parametrs
typedef struct _Light {
	bool on;					///< is light on
	glm::vec3 ambient;			///< intensity & color of the ambient component
	glm::vec3 diffuse;			///< intensity & color of the diffuse component
	glm::vec3 specular;			///< intensity & color of the specular component
	glm::vec3 position;			///< light position
	glm::vec3 spotDirection;	///< spotlight direction
	float spotCosCutOff;		///< cosine of the spotlight's half angle
	float spotExponent;			///< distribution of the light energy within the reflector's cone (center->cone's edge)
	float quadraticAttenuation; ///< parameter for point light intensity scaled with distance
} Light;

/// parameters of individual objects in the scene (e.g. position, size, speed, etc.)
typedef struct _Object {
	glm::vec3 position;
	glm::vec3 direction;
	float     speed;
	float     size;

	bool destroyed;

	float startTime;
	float currentTime;
} Object;

/// parameters for fish and drone
typedef struct _MovingObject : public Object {
	glm::vec3 initPosition;
	bool      objectOn;
} MovingObject;

/// parametrs for fire
typedef struct _Fire : public Object {
	int textureFrames;
	float frameDuration;
} Fire;

typedef struct _commonShaderProgram {
	// identifier for the shader program
	GLuint program;          // = 0;
	// vertex attributes locations
	GLint posLocation;       // = -1;
	GLint colorLocation;     // = -1;
	GLint normalLocation;    // = -1;
	GLint texCoordLocation;  // = -1;
	// uniforms locations
	GLint PVMmatrixLocation;    // = -1;
	GLint VmatrixLocation;      // = -1;  view/camera matrix
	GLint MmatrixLocation;      // = -1;  modeling matrix
	GLint normalMatrixLocation; // = -1;  inverse transposed Mmatrix

	GLint timeLocation;         // = -1; elapsed time in seconds

	// material 
	GLint diffuseLocation;    // = -1;
	GLint ambientLocation;    // = -1;
	GLint specularLocation;   // = -1;
	GLint shininessLocation;  // = -1;
	// texture
	GLint useTextureLocation; // = -1; 
	GLint texSamplerLocation; // = -1;
	// fog
	GLint fogOn;
	GLint fogDensity;
	// water
	GLint isWater;
	// duck
	GLint duckOn;
	GLint isDuck;
	GLint duckFactor;
	// sunlight
	GLint sunLightOn;
	GLint sunLightAmbient;
	GLint sunLightDiffuse;
	GLint sunLightSpecular;
	GLint sunLightPosition;
	// reflectorlight
	GLint reflectorLightOn;
	GLint reflectorLightAmbient;
	GLint reflectorLightDiffuse;
	GLint reflectorLightSpecular;
	GLint reflectorLightPosition;
	GLint reflectorLightSpotDirection;
	GLint reflectorLightSpotCosCutOff;
	GLint reflectorLightSpotExponent;
	// firelight
	GLint fireLightOn;
	GLint fireLightAmbient;
	GLint fireLightDiffuse;
	GLint fireLightSpecular;
	GLint fireLightPosition;
	GLint fireLightQuadraticAtt;
} SCommonShaderProgram;

extern MeshGeometry* groundGeometry;
extern MeshGeometry* fishGeometry;
extern MeshGeometry* fireGeometry;
extern MeshGeometry* waterGeometry;
extern MeshGeometry* skyboxGeometry;
extern MeshGeometry* fireplaceGeometry;
extern MeshGeometry* benchGeometry;
extern MeshGeometry* droneGeometry;
extern MeshGeometry* kapradiGeometry;
extern MeshGeometry* duckGeometry;
extern MeshGeometry* woodtrunkGeometry;
extern MeshGeometry* stoneGeometry;
extern MeshGeometry* crateGeometry;
extern MeshGeometry* palmtreeGeometry;
extern MeshGeometry* crocusGeometry;
extern MeshGeometry* sunflowerGeometry;

extern std::vector<MeshGeometry*> woodpileGeometry;
extern std::vector<MeshGeometry*> tulipGeometry;
extern std::vector<MeshGeometry*> snowdropGeometry;

void enableFog();
void disableFog();
void changeFog();

/// Sets Light values
/**
	\param[in] viewMatrix           View (world to eye coordinates)
	\param[in] elapsedTime          Current time
	\param[in] sunOn                Is sun on
	\param[in] reflectorOn          Is reflector on
	\param[in] reflectorPosition    Position of reflector
	\param[in] reflectorDirection	Direction of reflector
	\param[in] fireOn               Is fire on
	\param[in] firePosition         Position of fire
*/
void setLightValues(glm::mat4 viewMatrix, float elapsedTime,
	bool sunOn,
	bool reflectorOn, glm::vec3 reflectorPosition, glm::vec3 reflectorDirection,
	bool fireOn, glm::vec3 firePosition);

/// Draw sinle mesh object
/**
	\param[in] object           Object
	\param[in] geometry         Geometry of object
	\param[in] viewMatrix       View (world to eye coordinates)
	\param[in] projectionMatrix Projection
	\param[in] rotationDegrees  Rotation degrees
	\param[in] rotationAxis     Rotation axis
*/
void drawObject(Object* object, MeshGeometry* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float rotationDegrees = 90.0f, glm::vec3 rotationAxis = glm::vec3(1, 0, 0));

/// Draw multi mesh object
/**
	\param[in] object           Object
	\param[in] geometry         Geometry of object
	\param[in] viewMatrix       View (world to eye coordinates)
	\param[in] projectionMatrix Projection
	\param[in] rotationDegrees  Rotation degrees
	\param[in] rotationAxis     Rotation axis
*/
void drawObject(Object* object, std::vector<MeshGeometry*> geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float rotationDegrees = 90.0f, glm::vec3 rotationAxis = glm::vec3(1, 0, 0));

/// Draw moving object
/**
	\param[in] object           Object
	\param[in] geometry         Geometry of object
	\param[in] viewMatrix       View (world to eye coordinates)
	\param[in] projectionMatrix Projection
*/
void drawObject(MovingObject* object, MeshGeometry* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

void drawFire(Fire* fire, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawDuck(Object* duck, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawSkybox(const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawWater(float elapsedTime, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

void initializeShaderPrograms();
void cleanupShaderPrograms();

/// Set light uniforms
void setLightUniforms();

void initializeModels();
void cleanupModels();

#endif // __RENDER_STUFF_H
