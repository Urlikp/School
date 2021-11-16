//----------------------------------------------------------------------------------------
/**
 * \file    skybox.vert
 * \author  Jiøí Povolný
 * \date    2021/5/20
 * \brief   Skybox vertex shader
*/
//----------------------------------------------------------------------------------------

#version 140

// in
in vec2 screenCoord;
// uniforms  
uniform mat4 inversePVmatrix;
// out
smooth out vec3 texCoord_v;
smooth out float position_v; 
	
void main() {
    vec4 farCoordinates = vec4(screenCoord, 0.9999, 1.0);
    vec4 worldViewCoord = inversePVmatrix * farCoordinates;

    texCoord_v = worldViewCoord.xyz / worldViewCoord.w;
    gl_Position = farCoordinates;

	position_v = distance(vec4(0.0, 0.0, 0.0, 1.0), farCoordinates);
}