#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 cor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;
    
    if (aPos.y > 3)
        cor = vec3(0.6, 0.1, 0.0);
    else if (aPos.y > 0)
        cor = vec3(0.0, 0.5, 0.5);
    else if (aPos.y > -3)
        cor = vec3(0.5, 0.5, 0.0);
    else
        cor = vec3(0.0, 0.1, 0.6);

    // cor = color;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}