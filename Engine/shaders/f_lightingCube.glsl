#version 430

in vec3 o_normal_;
in vec3 o_fragPos_;

uniform vec3 lightColor_;
uniform vec3 objectColor_;
uniform vec3 lightPos_;

out vec4 color;

void main()
{   
    vec3 normal = normalize(o_normal_);
    vec3 fragPos = o_fragPos_;
    float ambientStrength = 0.1;
    
    vec3 ambient = ambientStrength * lightColor_;
    vec3 lightDir = normalize(lightPos_ - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor_;

    vec3 result = (ambient + diffuse) * objectColor_;
    color = vec4(result, 1.0);
}