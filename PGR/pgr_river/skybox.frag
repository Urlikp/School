//----------------------------------------------------------------------------------------
/**
 * \file    skybox.frag
 * \author  Jiøí Povolný
 * \date    2021/5/20
 * \brief	Skybox fragment shader
*/
//----------------------------------------------------------------------------------------

#version 140
  
// in
smooth in vec3 texCoord_v;		///< fragment texture coordinates
smooth in float position_v;     ///< distance for fog
// uniforms
uniform samplerCube skyboxSampler;
uniform bool fogOn;		///< is fog on
// out
out vec4 color_f;	///< outgoing color

float fogFactor;			///< fog factor
vec3 fogColor = vec3(0.8);	///< color of fog

void main() {
	fogFactor = clamp(exp(-pow(length(position_v), 2)), 0, 1);
	color_f = texture(skyboxSampler, texCoord_v);

	if (fogOn) {
		color_f = mix(vec4(fogColor, 1.0f), color_f, fogFactor);
	}
}