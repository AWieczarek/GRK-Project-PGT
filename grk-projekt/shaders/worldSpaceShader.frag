#version 430 core

float AMBIENT = 0.03;
float PI = 3.14;

uniform sampler2D shadowMap;
uniform sampler2D colorTexture;

uniform vec3 cameraPos;

//uniform vec3 color;

uniform vec3 sunDir;
uniform vec3 sunColor;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 lightPos2;
uniform vec3 lightColor2;

uniform vec3 spotlightPos;
uniform vec3 spotlightColor;
uniform vec3 spotlightConeDir;
uniform vec3 spotlightPhi;

uniform float shadowMapWidth;
uniform float shadowMapHeight;

uniform float metallic;
uniform float roughness;

uniform float exposition;
uniform float exposition2;

in vec3 vecNormal;
in vec3 worldPos;

out vec4 outColor;


in vec3 viewDirTS;
in vec3 lightDirTS;
in vec3 spotlightDirTS;
in vec3 sunDirTS;

in vec3 test;
in vec4 sunSpacePos;
in vec2 vertexTexCoordOut;

const int pcfCount = 8;
const float totalTexels = (pcfCount * 2.0 + 1.0) * (pcfCount * 2.0 + 1.0);

float DistributionGGX(vec3 normal, vec3 H, float roughness){
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(normal, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 normal, vec3 V, vec3 lightDir, float roughness){
    float NdotV = max(dot(normal, V), 0.0);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 

vec3 PBRLight(vec3 lightDir, vec3 radiance, vec3 normal, vec3 V){
	float diffuse = max(0,dot(normal,lightDir));
    vec3 color=texture(colorTexture, vertexTexCoordOut).xyz;

	//vec3 V = normalize(cameraPos-worldPos);
	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, color, metallic);

    vec3 H = normalize(V + lightDir);    
        
    // cook-torrance brdf
    float NDF = DistributionGGX(normal, H, roughness);        
    float G   = GeometrySmith(normal, V, lightDir, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
        
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
            
    // add to outgoing radiance Lo
    float NdotL = max(dot(normal, lightDir), 0.0);   
	return (kD * color / PI + specular) * radiance * NdotL;
}

float calculateShadow(vec4 lightPos, sampler2D Shadow_Map){
    vec3 _lightPos = lightPos.xyz/lightPos.w;
    _lightPos = _lightPos * 0.5 + 0.5;

    double TexelWidth = 1.0/shadowMapWidth;
    double TexelHeight = 1.0/shadowMapHeight;

    vec2 TexelSize = vec2(TexelWidth, TexelHeight);

    float shadowSum = 0.0;

    for (int y = -pcfCount ; y <= pcfCount ; y++) {
        for (int x = -pcfCount ; x <= pcfCount ; x++) {
            vec2 Offset = vec2(x, y) * TexelSize;
            float shadow = _lightPos.z > texture(Shadow_Map, _lightPos.xy + Offset).r + 0.0001 ? 1.0 : 0.0;
            shadowSum += shadow;
        }
    }

    return ((shadowSum/totalTexels)/lightPos.w);
}
 
void main()
{
	//vec3 normal = vec3(0,0,1);
    vec3 normal = normalize(vecNormal);

    //vec3 viewDir = normalize(viewDirTS);
    vec3 viewDir = normalize(cameraPos-worldPos);

	//vec3 lightDir = normalize(lightDirTS);
	vec3 lightDir = normalize(lightPos-worldPos);
    vec3 lightDir2 = normalize(lightPos2-worldPos);

    vec3 color=texture(colorTexture, vertexTexCoordOut).xyz;


	vec3 ambient = AMBIENT*color;
	vec3 attenuatedlightColor = lightColor/pow(length(lightPos-worldPos),2);
	vec3 ilumination;
	ilumination = ambient+PBRLight(lightDir,attenuatedlightColor,normal,viewDir);
	
    vec3 attenuatedlightColor2 = lightColor2/pow(length(lightPos2-worldPos),2);
	vec3 ilumination2;
	ilumination2 = ambient+PBRLight(lightDir,attenuatedlightColor2,normal,viewDir);

	//flashlight
	//vec3 spotlightDir= normalize(spotlightDirTS);
	vec3 spotlightDir= normalize(spotlightPos-worldPos);
	

    float angle_atenuation = clamp((dot(-normalize(spotlightPos-worldPos),spotlightConeDir)-0.5)*3,0,1);
	attenuatedlightColor = angle_atenuation*spotlightColor/pow(length(spotlightPos-worldPos),2);
	ilumination=ilumination+PBRLight(spotlightDir,attenuatedlightColor,normal,viewDir);

    attenuatedlightColor2 = angle_atenuation*spotlightColor/pow(length(spotlightPos-worldPos),2);
	ilumination2=ilumination2+PBRLight(spotlightDir,attenuatedlightColor2,normal,viewDir);

    // calculate shadow
    float shadow = calculateShadow(sunSpacePos, shadowMap);
    //float shadow2 = calculateShadow(sunSpacePos2, shadowMap2);

    //float shadow = (shadow1 + shadow2)/2.0;

    //float shadow = CalcShadowFactor();

	//sun
	ilumination = ilumination + PBRLight(sunDir,sunColor * (1.0 - shadow), normal, viewDir);

    //outColor = vec4(vec3(1.0) - exp(-ilumination*exposition),1);

    //outColor = (vec4(vec3(1.0) - exp(-ilumination*exposition),1) + vec4(vec3(1.0) - exp(-ilumination2*exposition2),1));

    if(exposition > 0.f)
        outColor = (vec4(vec3(1.0) - exp(-ilumination*exposition),1) + vec4(vec3(1.0) - exp(-ilumination2*exposition2),1));
    else
        outColor = vec4(vec3(1.0) - exp(-ilumination2*exposition2),1);


	//outColor = vec4(vec3(1.0) - exp(-ilumination*exposition),1);
	//outColor = vec4(roughness,metallic,0,1);
    //outColor = vec4(test;
}
