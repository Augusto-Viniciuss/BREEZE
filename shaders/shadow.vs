#version 460 core
// Entrada do shader: posição do vértice
layout (location = 0) in vec3 _position;

uniform mat4 light_space_matrix;   // Matriz de transformação da câmera de luz
uniform mat4 model;                // Matriz do modelo (transformação do objeto)

void main()
{
   // Calcula a posição do vértice no espaço da luz
   gl_Position = light_space_matrix * model * vec4(_position, 1.0);
}
