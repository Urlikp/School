//----------------------------------------------------------------------------------------
/**
 * \file    data.h
 * \author  Jiøí Povolný
 * \date    2021/5/20
 * \brief   Basic defines and data structures.
*/
//----------------------------------------------------------------------------------------

#ifndef __DATA_H
#define __DATA_H

#define WINDOW_WIDTH   750
#define WINDOW_HEIGHT  750
#define WINDOW_TITLE   "PGR River"

/// keys used in the key map
enum { KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_SPACE, KEYS_COUNT };

// fire
#define FIRE_BILLBOARD_SIZE   0.1f

// water
#define WATER_POSITION_X    -1.0f
#define WATER_POSITION_Y    -2.5f
#define WATER_POSITION_Z    -0.5f
#define WATER_WIDTH         5.0f
#define WATER_HEIGHT        7.5f

// sun
#define SUN_SPEED   0.5f

// fog
#define FOG_SPEED   0.5f

// drone
#define DRONE_POSITION  0.0f, 0.0f, 1.0f
#define DRONE_SIZE      0.05f
#define DRONE_SPEED     1.0f

// ground
#define GROUND_POSITION 0.0f, 0.0f, -0.48f
#define GROUND_SIZE     5.0f

// fireplace
#define FIREPLACE_POSITION  -2.3f, 0.0f, -0.09f
#define FIREPLACE_SIZE      0.05f

// woodpile
#define WOODPILE_POSITION   -2.3f, 0.3f, -0.082f
#define WOODPILE_SIZE       0.04f

// woodtrunk
#define WOODTRUNK_POSITION  -2.3f, -0.3f, -0.052f
#define WOODTRUNK_SIZE      0.2f

// stone
#define STONE_POSITION  -0.55f, 0.3f, -0.45f
#define STONE_SIZE      0.2f

// crate
#define CRATE_POSITION  -2.8f, 0.3f, -0.073f
#define CRATE_SIZE      0.04f

// palmtree
#define PALMTREE_POSITION           -0.3f, -1.0f, 0.39f
#define PALMTREE_SIZE               0.8f
#define PALMTREE_ROTATION_DEGREES   0.0f

// crocus
#define CROCUS_POSITION         -2.0f, 0.8f, -0.105f
#define CROCUS_SIZE             0.04f
#define CROCUS_ROTATION_DEGREES 0.0f

// sunflower
#define SUNFLOWER_POSITION          -1.8f, 0.6f, -0.055f
#define SUNFLOWER_SIZE              0.1f
#define SUNFLOWER_ROTATION_DEGREES  0.0f

// tulip
#define TULIP_POSITION          -2.2f, 0.7f, -0.09f
#define TULIP_SIZE              0.04f
#define TULIP_ROTATION_DEGREES  0.0f

// snowdrop
#define SNOWDROP_POSITION           -1.9f, 1.0f, -0.106f
#define SNOWDROP_SIZE               0.04f
#define SNOWDROP_ROTATION_DEGREES   0.0f

// kapradi
#define KAPRADI_COUNT   5
#define KAPRADI_SIZE    0.07f

// bench
#define BENCH_POSITION          -2.8f, 0.0f, 0.02f
#define BENCH_MIDDLE_X          -2.8f
#define BENCH_MIDDLE_Y          0.0f
#define BENCH_SIZE              0.2f
#define BENCH_WIDTH             0.18f
#define BENCH_HEIGHT            0.3f
#define BENCH_ROTATION_DEGREES  -5.0f
#define BENCH_ROTATION_AXIS     0, 1, 0

// duck
#define DUCK_POSITION   -1.5f, 0.0f, -0.15f
#define DUCK_SIZE       0.002f

// fish
#define FISH_COUNT      2
#define FISH_SPEED_MAX  1.0f
#define FISH_SIZE       0.3f

// scene
#define SCENE_WIDTH  3.2f
#define SCENE_HEIGHT 2.3f
#define SCENE_DEPTH  1.0f

// camera
#define CAMERA_ELEVATION_MAX    45.0f
#define CAMERA_SPEED            0.05f

/// default shader - color per vertex and matrix multiplication
const std::string colorVertexShaderSrc(
    "#version 140\n"
    "uniform mat4 PVMmatrix;\n"
    "in vec3 position;\n"
    "in vec3 color;\n"
    "smooth out vec4 theColor;\n"
    "void main() {\n"
    "   gl_Position = PVMmatrix * vec4(position, 1.0);\n"
    "   theColor = vec4(color, 1.0);\n"
    "}\n"
);

/// default shader - color per fragment
const std::string colorFragmentShaderSrc(
    "#version 140\n"
    "smooth in vec4 theColor;\n"
    "out vec4 outputColor;\n"
    "void main() {\n"
    "   outputColor = theColor;\n"
    "}\n"
);

/// fish positions
const glm::vec3 fishPositions[FISH_COUNT] = {
    glm::vec3(1.2f, 2.0f, -0.55f),
    glm::vec3(1.6f, 1.5f, -0.55f)
};

/// kapradi positions
const glm::vec3 kapradiPositions[KAPRADI_COUNT] = {
    glm::vec3(-1.5f, 0.1f, -0.09f),
    glm::vec3(-1.5f, -0.15f, -0.07f),
    glm::vec3(-1.5f, 0.5f, -0.111f),
    glm::vec3(-1.5f, 0.3f, -0.102f),
    glm::vec3(-1.5f, 0.35f, -0.105f)
};

//
// fire billboard geometry definition 
//

const int fireNumQuadVertices = 4;
const float fireVertexData[fireNumQuadVertices * 5] = {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
};

//
// water billboard geometry definition 
//

const int waterNumQuadVertices = 2;
const float waterVertexData[] = {
    WATER_POSITION_X,               WATER_POSITION_Y,                   WATER_POSITION_Z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    WATER_POSITION_X + WATER_WIDTH, WATER_POSITION_Y,                   WATER_POSITION_Z, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f,
    WATER_POSITION_X,               WATER_POSITION_Y + WATER_HEIGHT,    WATER_POSITION_Z, 0.0f, 0.0f, 1.0f,	0.0f, 0.5f,
    WATER_POSITION_X + WATER_WIDTH, WATER_POSITION_Y + WATER_HEIGHT,    WATER_POSITION_Z, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f,
    WATER_POSITION_X + WATER_WIDTH, WATER_POSITION_Y,                   WATER_POSITION_Z, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f,
    WATER_POSITION_X,               WATER_POSITION_Y + WATER_HEIGHT,    WATER_POSITION_Z, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f,
};

#endif // __DATA_H
