#version 460 core
// Saída do fragment shader: cor do fragmento
out vec4 FragColor;
in vec2 tex_coords;    // Coordenadas de textura do vértice
in vec3 normal;        // Vetor normal do vértice (não utilizado neste shader)

// Contém a textura a ser aplicada
uniform sampler2D current_texture;
void main()
{
    // Aplica a textura usando as coordenadas de textura e define a cor do fragmento
    FragColor = vec4(texture(current_texture, tex_coords).rgb, 1.0);
}
