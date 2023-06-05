#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoords;
in vec3 cor;

uniform sampler2D texture1;

void main()
{
    FragColor = vec4(cor, 1.0);
    //FragColor = texture(texture1, TexCoords) * vec4(cor, 1.0);
}