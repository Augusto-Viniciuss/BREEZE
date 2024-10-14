#version 460 core
layout (location = 0) in vec3 _position; // Posição do vértice (x, y, z)
layout (location = 1) in vec3 _normal;   // Normal do vértice (vetor normal para iluminação)
layout (location = 2) in vec2 _tex_coords; // Coordenadas de textura do vértice (u, v)

uniform mat4 model;               // Matriz de modelo (transforma de espaço local para o mundo)
uniform mat4 view;                // Matriz de visualização (transforma do espaço do mundo para o espaço da câmera)
uniform mat4 projection;          // Matriz de projeção (aplica a projeção perspectiva)
uniform mat4 light_space_matrix;  // Matriz de transformação para o espaço de luz (sombras)


out vec3 world_position;          // Posição do vértice no espaço do mundo
out vec3 normal;                  // Normal transformada para o espaço do mundo
out vec2 tex_coords;              // Coordenadas de textura que serão usadas (fragment shader)
out vec4 frag_position_light_space;  // Posição do vértice no espaço da luz (cálculo de sombras)

void main()
{
    gl_Position = projection * view * model * vec4(_position.xyz, 1.0);         // Calcula a posição final do vértice na tela
    world_position =  vec3(model * vec4(_position.xyz, 1.0));                   // Calcula a posição do vértice no espaço do mundo.
    normal = normalize(mat3(transpose(inverse(model))) * _normal);              // Calcula a normal transformada no espaço do mundo
    tex_coords = _tex_coords;                                                   // Passa as coordenadas de textura diretamente
    frag_position_light_space = light_space_matrix * vec4(world_position, 1.0); // Calcula a posição do vértice no espaço da luz
}
