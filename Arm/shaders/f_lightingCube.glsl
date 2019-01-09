#version 430

in vec3 o_normal_;
in vec3 o_fragPos_;

uniform vec3 lightColor_;
uniform vec3 objectColor_;
uniform vec3 lightPos_;
uniform vec3 cameraPos_;

out vec4 color;

void main()
{   
    vec3 normal = normalize(o_normal_);
    vec3 fragPos = o_fragPos_;
    float ambientStrength = 0.1;
    float specularStrength = 0.1;
    float diffuseStrength = 1.0;

    vec3 ambient = ambientStrength * lightColor_;
    vec3 lightDir = normalize(lightPos_ - fragPos);
    vec3 reflectDir = reflect(-lightDir,normal);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * diffuseStrength * lightColor_;

    vec3 cameraDir = normalize(fragPos - cameraPos_);
    float spec = pow(max(dot(cameraDir,reflectDir), 0.0), 32);
    vec3 specular = spec * specularStrength * lightColor_;

    vec3 result = (ambient + diffuse + specular) * objectColor_;
    color = vec4(result, 1.0);
}