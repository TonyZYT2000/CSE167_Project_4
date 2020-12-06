
#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 worldPos;
in vec3 worldNormal;

uniform vec3 eyePos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 kAmbient;
uniform vec3 kDiffuse;
uniform vec3 kSpecular;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
    // distance between light and object
    float dist = length(lightPos - worldPos);
    // linear attenuate the light color
    vec3 attLightColor = lightColor / (1 + (0.001 * dist));
    // vector pointing to light source
    vec3 lightVec = normalize(lightPos - worldPos);

    // ambient color
    vec3 ambient = attLightColor * kAmbient;

    // calculate the factor in diffuse model
    float diffuseFactor = max(dot(lightVec, worldNormal), 0);
    // cdiffuse color
    vec3 diffuse = attLightColor * kDiffuse * diffuseFactor;

    // viewing direction
    vec3 eyeVec = normalize(eyePos - worldPos);
    // reflected vector
    vec3 reflectVec = 2 * dot(lightVec, worldNormal) * worldNormal - lightVec;

    // calculate the factor in specular model
    float specularFactor = max(dot(reflectVec, eyeVec), 0);
    // specular color
    vec3 specular = attLightColor * kSpecular * pow(specularFactor, 6);

    float edge = max(0, dot(eyeVec, worldNormal));
    if (edge < 0.1) {
        fragColor = vec4(0, 0, 0, 1.0f);
    } else {
        float intensity = length(ambient + diffuse + specular);

        if (intensity > 0.95) {
            fragColor = vec4(vec3(1) * diffuse, 1.0);
        } else if (intensity > 0.5) {
            fragColor = vec4(vec3(0.7) * diffuse, 1.0);
        } else if (intensity > 0.05) {
            fragColor = vec4(vec3(0.35) * diffuse, 1.0);
        } else {
            fragColor = vec4(vec3(0.1) * diffuse, 1.0);
        }
    }
    /*
    fragColor = vec4(0);
    fragColor[0] = (worldNormal[0] + 1) / 2;
    fragColor[1] = (worldNormal[1] + 1) / 2;
    fragColor[2] = (worldNormal[2] + 1) / 2;
    fragColor[3] = 1.0f;
    */
}
