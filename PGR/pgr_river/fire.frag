//----------------------------------------------------------------------------------------
/**
 * \file    fire.frag
 * \author  Ji�� Povoln�
 * \date    2021/5/20
 * \brief   Fire fragment shader
*/
//----------------------------------------------------------------------------------------

#version 140

// in
smooth in vec3 position_v;    ///< camera space fragment position
smooth in vec2 texCoord_v;    ///< fragment texture coordinates
// uniforms
uniform float time;						///< used for simulation of moving lights (such as sun) and to select proper animation frame
uniform mat4 Vmatrix;					///< view (camera) transform --> world to eye coordinates
uniform sampler2D texSampler;			///< sampler for texture access
uniform ivec2 pattern = ivec2(8, 2);	///< there are 8 frames in the row, two rows total
uniform float frameDuration = 0.1f;		///< one frame lasts 0.1s
// out
out vec4 color_f;	///< outgoing fragment color

/// texture movement
vec4 sampleTexture(int frame) {
  vec2 offset = vec2(1.0) / vec2(pattern);

  vec2 texCoordBase = texCoord_v / vec2(pattern);
  vec2 texCoord = texCoordBase + vec2(frame % pattern.x, (frame / pattern.x)) * offset;

  return texture(texSampler, texCoord);
}

void main() {
  // frame of the texture to be used for explosion drawing 
  int frame = int(time / frameDuration);

  // sample proper frame of the texture to get a fragment color  
  color_f = sampleTexture(frame);
}
