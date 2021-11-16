//----------------------------------------------------------------------------------------
/**
 * \file    fragment.vert
 * \author  Jiøí Povolnı
 * \date    2021/5/20
 * \brief   Default fragment shader
*/
//----------------------------------------------------------------------------------------

#version 140

/// structure that describes currently used material
struct Material { 
    vec3 ambient;       ///< ambient component
    vec3 diffuse;       ///< diffuse component
    vec3 specular;      ///< specular component
    float shininess;    ///< sharpness of specular reflection
    bool useTexture;    ///< defines whether the texture is used or not
};

/// structure describing light parameters
struct Light {
    bool on;                    ///< is light on
    vec3 ambient;               ///< intensity & color of the ambient component
    vec3 diffuse;               ///< intensity & color of the diffuse component
    vec3 specular;              ///< intensity & color of the specular component
    vec3 position;              ///< light position
    vec3 spotDirection;         ///< spotlight direction
    float spotCosCutOff;        ///< cosine of the spotlight's half angle
    float spotExponent;         ///< distribution of the light energy within the reflector's cone (center->cone's edge)
    float quadraticAttenuation; ///< parameter for point light intensity scaled with distance
};

// in
smooth in vec2 texCoord_v;  ///< fragment texture coordinates
smooth in vec3 normal_v;    ///< camera space normal
smooth in vec3 position_v;  ///< camera space position
// unifoms
uniform sampler2D texSampler;   ///< sampler for the texture access
uniform Material material;      ///< current material
uniform float time;             ///< time used for simulation of moving lights (such as sun)
uniform mat4 PVMmatrix;         ///< Projection * View * Model (model to clip coordinates)
uniform mat4 Vmatrix;           ///< View (world to eye coordinates)
uniform mat4 Mmatrix;           ///< Model (model to world coordinates)
uniform mat4 normalMatrix;      ///< inverse transposed VMmatrix
// light
uniform Light sun;          ///< sunlight
uniform Light fire;         ///< firelight
uniform Light reflector;    ///< reflectorlight
// fog
uniform bool fogOn;         ///< is fog on
uniform float fogDensity;   ///< fog density
// water
uniform bool isWater;   ///< is this water
// out
out vec4 color_f;   ///< outgoing fragment color

float fogFactor;            ///< fog factor
vec3 fogColor = vec3(0.8);  ///< color of fog

/// set directional light
/**
    \param[in] light                light used
    \param[in] material             material used
    \param[in] fragmentPosition     position of fragment
    \param[in] fragmentNormal       normal of fragment
    \return color influenced by directional light
*/
vec4 directionalLight(Light light, Material material, vec3 fragmentPosition, vec3 fragmentNormal){
    vec3 ret = vec3(0.0f);

    vec3 L = normalize(light.position);
    vec3 R = reflect(-L, fragmentNormal);
    vec3 V = normalize(-fragmentPosition);
    float NdotL = max(0.0f, dot(fragmentNormal, L));
    float RdotV = max(0.0f, dot(R, V));

    ret += material.ambient * light.ambient;
    ret += material.diffuse * light.diffuse * NdotL;
    ret += material.specular * light.specular * pow(RdotV, material.shininess);

    return vec4(ret, 1.0f);
}

/// set point light
/**
    \param[in] light                light used
    \param[in] material             material used
    \param[in] fragmentPosition     position of fragment
    \param[in] fragmentNormal       normal of fragment
    \return color influenced by point light
*/
vec4 pointLight(Light light, Material material, vec3 fragmentPosition, vec3 fragmentNormal){
    vec3 ret = vec3(0.0f);

	float dist = length(light.position - fragmentPosition);
    
	vec3 L = normalize(light.position - fragmentPosition);
    vec3 R = reflect(-L, fragmentNormal);
    vec3 V = normalize(-fragmentPosition);
    float NdotL = max(0.0f, dot(fragmentNormal, L)/dist);
    float RdotV = max(0.0f, dot(R, V));

    ret += material.ambient * light.ambient;
    ret += material.diffuse * light.diffuse * NdotL;
    ret += material.specular * light.specular * pow(RdotV, material.shininess);

    float attenuationFactor = min(1.0f/(light.quadraticAttenuation*dist*dist), 1.0f);
	ret *= attenuationFactor;

    return vec4(ret, 1.0f);
}

/// set spot light
/**
    \param[in] light                light used
    \param[in] material             material used
    \param[in] fragmentPosition     position of fragment
    \param[in] fragmentNormal       normal of fragment
    \return color influenced by spot light
*/
vec4 spotLight(Light light, Material material, vec3 fragmentPosition, vec3 fragmentNormal){
    vec3 ret = vec3(0.0f);

    vec3 L = normalize(light.position - fragmentPosition);
    vec3 R = reflect(-L, fragmentNormal);
    vec3 V = normalize(-fragmentPosition);
    float NdotL = max(0.0f, dot(fragmentNormal, L));
    float RdotV = max(0.0f, dot(R, V));
    float spotCoef = max(0.0f, dot(-L, light.spotDirection));

    ret += material.ambient * light.ambient;
    ret += material.diffuse * light.diffuse * NdotL;
    ret += material.specular * light.specular * pow(RdotV, material.shininess);

    if (spotCoef < light.spotCosCutOff)
        ret *= 0.0f;
    else
        ret *= pow(spotCoef, light.spotExponent);

    return vec4(ret, 1.0f);
}

void main(){
    fogFactor = clamp(exp(-pow(length(position_v) * fogDensity, 2)), 0, 1);

    vec3 normal = normalize(normal_v);
    //basic ambient light
    vec3 globalAmbientLight = vec3(0.2f);
    vec4 outputColor = vec4(material.ambient * globalAmbientLight, 0.0f);

    // sun
    if (sun.on) {
        outputColor += directionalLight(sun, material, position_v, normal);
    }

    // flashlight
    if (reflector.on) {
        outputColor += spotLight(reflector, material, position_v, normal);
    }
    
    // fire
    if (fire.on) {
        outputColor += pointLight(fire, material, position_v, normal);
    }

    color_f = outputColor;

    // water
    if (isWater) {
        vec2 waterTexCoord_v = texCoord_v;
        waterTexCoord_v.y -= mod(time, 10)/10; 
        color_f = outputColor * texture(texSampler, waterTexCoord_v);
    } 
    else if (material.useTexture) {
        color_f = outputColor * texture(texSampler, texCoord_v);
    }

    // fog
    if (fogOn) {
        color_f = mix(vec4(fogColor, 1.0f), color_f, fogFactor);
    }
}
