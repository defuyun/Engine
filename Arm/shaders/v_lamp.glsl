#version 430

in vec3 position_;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
    gl_Position = project * view * model * vec4(position_, 1.0f);
}