//----------------------------------------------------------------------------------------
/**
 * \file    vertex.vert
 * \author  Jiøí Povolný
 * \date    2021/5/20
 * \brief   Default vertex shader
*/
//----------------------------------------------------------------------------------------

#version 140

// in
in vec3 position;
in vec3 normal;
in vec2 texCoord;
// uniforms
uniform mat4 normalMatrix;  ///< inverse transposed VMmatrix
uniform mat4 PVMmatrix;
uniform mat4 Vmatrix;       ///< View --> world to eye coordinates
uniform mat4 Mmatrix;       ///< Model --> model to world coordinates
// duck
uniform bool duckOn;        ///< is duck on
uniform bool isDuck;        ///< is this duck
uniform float duckFactor;   ///< size factor of duck
// out
smooth out vec2 texCoord_v;
smooth out vec3 normal_v;
smooth out vec3 position_v;

void main(){
    vec3 duck_position = position;
    if(isDuck && duckOn){
	    duck_position = mix(position * 2, position, duckFactor);
    }

    gl_Position = PVMmatrix * vec4(duck_position, 1); // out:v vertex in clip coordinates

    normal_v = normalize((Vmatrix * normalMatrix * vec4(normal, 0.0)).xyz); // normal in eye coordinates by NormalMatrix
    texCoord_v = texCoord;
    position_v = (Vmatrix * Mmatrix * vec4(position, 1)).xyz;
}
