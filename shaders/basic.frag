#version 410 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;
in vec3 FragPosEye;
out vec4 fColor;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;
uniform int useTexture;
uniform vec3 objectColor;
uniform int shadingMode;
uniform vec3 viewPos;

// Lumini
uniform vec3 lightDir;
uniform vec3 lightColor;

#define MAX_POINT 8
uniform int pointCount;
uniform vec3 pointPos[MAX_POINT];
uniform vec3 pointCol[MAX_POINT];
uniform float pointIntensity[MAX_POINT];

uniform float pointConstant;
uniform float pointLinear;
uniform float pointQuadratic;

uniform int isPortal;
uniform float timeSec;
uniform vec4 fogColor;
uniform float fogDensity;
uniform int fogEnabled;

float computeFog() {
    float fragmentDistance = length(FragPosEye);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2.0));
    return clamp(fogFactor, 0.0, 1.0);
}

float ShadowCalculation(vec3 N, vec3 L) {
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0) return 0.0;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(N, L)), 0.001);
    return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
}

void main() {
    
    vec4 texColor = texture(diffuseTexture, TexCoords);
    if(useTexture == 1 && texColor.a < 0.8) discard;

    vec3 albedo = (useTexture == 1) ? texColor.rgb : objectColor;
    vec3 N = normalize(Normal);
  
    if (shadingMode == 1) {
        N = normalize(cross(dFdy(FragPos), dFdx(FragPos)));
    }
    vec3 V = normalize(viewPos - FragPos);

    // 2. Dir Light (Lumina Lunii/Soarelui) cu Blinn-Phong [cite: 20, 32]
    vec3 L_dir = normalize(-lightDir);
    vec3 H_dir = normalize(L_dir + V); // Half-vector [cite: 29]
    float diff_dir = max(dot(N, L_dir), 0.0);
    float spec_dir = pow(max(dot(N, H_dir), 0.0), 32.0);
    
    float shadow = ShadowCalculation(N, L_dir);
    vec3 ambient = 0.2 * lightColor * albedo;
    vec3 color = ambient + (1.0 - shadow) * (diff_dir * albedo + spec_dir * 0.5) * lightColor;

    // 3. Point Lights cu Atenuare [cite: 46, 107, 112]
    for (int i = 0; i < pointCount && i < MAX_POINT; i++) {
        vec3 Lvec = pointPos[i] - FragPos;
        float dist = length(Lvec);
        vec3 L = normalize(Lvec);
        vec3 H = normalize(L + V);
        
        // Formula atenuare [cite: 46, 106]
        float att = 1.0 / (pointConstant + pointLinear * dist + pointQuadratic * (dist * dist));
        
        float diff = max(dot(N, L), 0.0);
        float spec = pow(max(dot(N, H), 0.0), 32.0);
        vec3 lc = pointCol[i] * pointIntensity[i];
        
        color += (diff * albedo + spec * 0.5) * lc * att;
    }

    // Portal FX
    if (isPortal == 1) {
        vec2 uv = TexCoords * 2.0 - 1.0;
        float r = length(uv);
        float swirl = sin(8.0 * atan(uv.y, uv.x) + 12.0 * r - timeSec * 4.0) * 0.5 + 0.5;
        color += mix(vec3(1.0, 0.8, 0.2), vec3(0.6, 0.2, 1.0), swirl * 0.35) * smoothstep(0.95, 0.25, r);
    }

    fColor = vec4(color, 1.0);
    if (fogEnabled == 1) {
        fColor = mix(fogColor, fColor, computeFog());
    }
}