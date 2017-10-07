#version 430

in vec3 position_;
in vec3 normal_;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

out vec3 o_normal_;
out vec3 o_fragPos_;

void main()
{
    gl_Position = project * view * model * vec4(position_, 1.0f);
    o_normal_ = normal_;
    o_fragPos_ = vec3(model * vec4(position_, 1.0f));
}