#version 460 core

// Saída de cor do fragmento (pixel)
out vec4 FragColor;
in vec3 normal;                     // Normal do vértice no espaço do mundo
in vec2 tex_coords;                 // Coordenadas de textura
in vec3 world_position;             // Posição do vértice no espaço do mundo
in vec4 frag_position_light_space;  // Posição do vértice no espaço da luz (usado para sombras)

float ambient_str = 0.25;  // Intensidade da luz ambiente
float diffuse_str = 0.4;   // Intensidade da luz difusa
float specular_str = 0.4;  // Intensidade da luz especular

uniform sampler2D current_texture;  // Textura atual para aplicar ao objeto
uniform sampler2D shadow_map;       // Mapa de sombras (shadow map) para sombras dinâmicas

uniform vec3 light_position;  // Posição da luz
uniform vec3 view_position;   // Posição do observador (câmera)

// Função que calcula a sombra do fragmento baseado no shadow map
float get_shadow(vec4 frag_pos_ls) {
    vec3 proj_coords = frag_position_light_space.xyz / frag_position_light_space.w;
    proj_coords = proj_coords * 0.5 + 0.5;  // Converte de [-1, 1] para [0, 1] (espaço de textura)

    // Se o fragmento estiver fora do intervalo de sombras, não há sombra
    if(proj_coords.z > 1.0) {
        return 0.0;
    }

    float bias = 0.005;  
    float shadow = 0.0;  
    float current_depth = proj_coords.z;  

    vec2 texel_size = 1.0 / textureSize(shadow_map, 0);

    // Algoritmo PCF (Percentage Closer Filtering) para suavizar as sombras
    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            float pcf_depth = texture(shadow_map, proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
        }
    }

    // Média do valor de sombra usando 9 amostras (3x3)
    shadow /= 9.0;

    return shadow;
}

void main() {
    // Cor da textura do fragmento (pixel) baseada nas coordenadas de textura
    vec3 tex_color = texture(current_texture, tex_coords).rgb;

    // Calcula o vetor de direção da luz
    vec3 light_direction = normalize(light_position - world_position);

    // Calcula o vetor de direção do observador (câmera)
    vec3 view_direction = normalize(view_position - world_position);

    // Calcula o vetor refletido da luz para cálculo da iluminação especular
    vec3 reflect_direction = reflect(-light_direction, normal);

    // Calcula a intensidade da luz difusa com base no ângulo entre a luz e a normal
    float light_str = max(0, dot(light_direction, normal));

    // Componente de luz ambiente (iluminação constante)
    vec3 ambient = ambient_str * tex_color;

    // Componente de luz difusa (iluminação dependente do ângulo)
    vec3 diffuse = diffuse_str * light_str * tex_color;

    // Cálculo da componente especular (brilho) usando o ângulo entre o observador e o reflexo
    float specular_value = pow(max(dot(view_direction, reflect_direction), 0.0), 32);  
    vec3 specular = specular_str * specular_value * vec3(1.0, 1.0, 1.0);  

    // Calcula a sombra usando o shadow map
    float shadow = get_shadow(frag_position_light_space);

    // Cor final do fragmento: soma luz ambiente, difusa e especular, reduzindo o efeito da luz difusa/especular nas áreas sombreadas
    FragColor = vec4(ambient + (1.0 - shadow) * (diffuse + specular), 1.0); 
}
