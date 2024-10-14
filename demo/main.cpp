#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "BREEZE/shader.h"
#include "BREEZE/texture.h"
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include "BREEZE/shader_program.h"
#include "BREEZE/model.h"

// Estrutura de representação para cada planeta
struct planet_t
{
  BREEZE::Model* model;
  float          distance_to_sun;
  float          rotate_rate;
  float          rotate_value = 0;
};

// Função para rotacionar o planeta em seu próprio eixo
void planetRotateOwnAxis(BREEZE::Model* model) {
  model->model = glm::rotate(model->model, 0.01f, glm::vec3(0, 1, 0));
}
// Função para rotacionar o planeta em torno do sol
void planetRotateSunAxis(BREEZE::Model* model, float rotate_value, float distance_to_sun) {
  model->model[3].x = distance_to_sun * cos(rotate_value);
  model->model[3].z = distance_to_sun * sin(rotate_value);
}

int main(int argc, char** argv) {
  // Configuração da tella inicial
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Failed to initialize SDL" << std::endl;
    return -1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  SDL_Window* window =
      SDL_CreateWindow("Solar System - BREEZE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Failed to initialize window" << std::endl;
    return -1;
  }

  SDL_GLContext main_context = SDL_GL_CreateContext(window);

  SDL_GL_SetSwapInterval(1);
  if (glewInit() != GLEW_OK) {
    std::cerr << "Glew NOT ok" << std::endl;
    return -1;
  }

  glViewport(0, 0, 1280, 720);

  // Define a posição da câmera
  glm::vec3 camera_position  = glm::vec3(1.0f, 15.0f, 20.0f);
  glm::vec3 camera_target    = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 camera_direction = glm::normalize(camera_position - camera_target);

  glm::vec3 up           = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 camera_right = glm::normalize(glm::cross(up, camera_direction));

  glm::vec3 camera_up  = glm::normalize(glm::cross(camera_direction, camera_right));
  float     near_plane = 0.1f, far_plane = 100.f;

  // Cria a matriz de projeção
  glm::mat4 proj = glm::perspective(glm::radians(45.f), (float)1280.f / (float)720.f, 0.1f, 100.f);
  // Cria a matriz de visualização
  glm::mat4 view = glm::mat4(1.0f);
  view           = glm::lookAt(camera_position, camera_target, camera_up);

   // Define a posição da luz
  glm::vec3 light_pos        = glm::vec3(0.0, 0.0, 0.0);
  glm::mat4 light_projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);
  glm::mat4 light_view       = glm::lookAt(light_pos, glm::vec3(0.f, 0.0f, 0.f), glm::vec3(0.0f, 1.0f, 0.0f));

  glm::mat4 light_space_matrix = light_projection * light_view;

  // Carrega e compila shaders
  BREEZE::Shader* v_passthrough = new BREEZE::Shader("./shaders/passthrough.vs", BREEZE::shader_type::vertex);
  BREEZE::Shader* f_passthrough = new BREEZE::Shader("./shaders/passthrough.fs", BREEZE::shader_type::fragment);
  BREEZE::Shader* f_phong       = new BREEZE::Shader("./shaders/phong.fs", BREEZE::shader_type::fragment);
  BREEZE::Shader* v_shadow      = new BREEZE::Shader("./shaders/shadow.vs", BREEZE::shader_type::vertex);
  BREEZE::Shader* f_shadow      = new BREEZE::Shader("./shaders/shadow.fs", BREEZE::shader_type::fragment);

  if (!v_passthrough->compile())
    return -1;
  if (!f_passthrough->compile())
    return -1;
  if (!f_phong->compile())
    return -1;
  if (!v_shadow->compile())
    return -1;
  if (!f_shadow->compile())
    return -1;

  BREEZE::ShaderProgram* phong_shader   = new BREEZE::ShaderProgram(v_passthrough, f_phong);
  BREEZE::ShaderProgram* texture_shader = new BREEZE::ShaderProgram(v_passthrough, f_passthrough);
  BREEZE::ShaderProgram* shadow_shader  = new BREEZE::ShaderProgram(v_shadow, f_shadow);

  delete v_passthrough;
  delete f_passthrough;
  delete f_phong;
  delete v_shadow;
  delete f_shadow;

  // Carrega os modelos e define estados iniciais
  BREEZE::Texture sun_texture_diffuse("./assets/models/sun/13913_Sun_diff.jpg");
  BREEZE::Model*  sun_model = new BREEZE::Model("./assets/models/sun/sun.obj", &sun_texture_diffuse, nullptr);
  glm::vec3       sun_pos   = glm::vec3(0.0, 0.0, 0.0);
  sun_model->setShader(texture_shader);
  sun_model->model = glm::mat4(1);
  sun_model->model = glm::translate(sun_model->model, sun_pos);
  planet_t sun;
  sun.model           = sun_model;
  sun.distance_to_sun = 0;
  sun.rotate_rate     = 0.01f;

  BREEZE::Texture mercury_texture_diffuse("./assets/models/mercury/mercury-texture.jpeg");
  BREEZE::Model*  mercury_model = new BREEZE::Model("./assets/models/mercury/mercury.obj", &mercury_texture_diffuse, nullptr);
  glm::vec3       mercury_pos   = glm::vec3(4.0, 0.0, 0.0);
  mercury_model->setShader(phong_shader);
  mercury_model->model = glm::mat4(1);
  mercury_model->model = glm::translate(mercury_model->model, mercury_pos);
  mercury_model->model = glm::scale(mercury_model->model, glm::vec3(1 / 28.5, 1 / 28.5, 1 / 28.5));
  planet_t mercury;
  mercury.model           = mercury_model;
  mercury.distance_to_sun = 4.0;
  mercury.rotate_rate     = 0.027f;

  BREEZE::Texture venus_texture_diffuse("./assets/models/venus/venus-texture.jpg");
  BREEZE::Model*  venus_model = new BREEZE::Model("./assets/models/venus/venus.obj", &venus_texture_diffuse, nullptr);
  glm::vec3       venus_pos   = glm::vec3(6.0, 0.0, 0.0);
  venus_model->setShader(phong_shader);
  venus_model->model = glm::mat4(1);
  venus_model->model = glm::translate(venus_model->model, venus_pos);
  venus_model->model = glm::scale(venus_model->model, glm::vec3(1 / 11.5, 1 / 11.5, 1 / 11.5));
  planet_t venus;
  venus.model           = venus_model;
  venus.distance_to_sun = 6.0;
  venus.rotate_rate     = 0.020f;

  BREEZE::Texture earth_texture_diffuse("./assets/models/earth/Earth_TEXTURE_CM.tga");
  BREEZE::Model*  earth_model = new BREEZE::Model("./assets/models/earth/Earth.obj", &earth_texture_diffuse, nullptr);
  glm::vec3       earth_pos   = glm::vec3(8.0, 0.0, 0.0);
  earth_model->setShader(phong_shader);
  earth_model->model = glm::mat4(1);
  earth_model->model = glm::translate(earth_model->model, earth_pos);
  earth_model->model = glm::scale(earth_model->model, glm::vec3((1 / 10.9), 1 / 10.9, 1 / 10.9));
  planet_t earth;
  earth.model           = earth_model;
  earth.distance_to_sun = 8.0;
  earth.rotate_rate     = 0.016f;

  BREEZE::Texture mars_texture_diffuse("./assets/models/mars/mars-texture.jpg");
  BREEZE::Model*  mars_model = new BREEZE::Model("./assets/models/mars/mars.obj", &mars_texture_diffuse, nullptr);
  glm::vec3       mars_pos   = glm::vec3(11.0, 0.0, 0.0);
  mars_model->setShader(phong_shader);
  mars_model->model = glm::mat4(1);
  mars_model->model = glm::translate(mars_model->model, mars_pos);
  mars_model->model = glm::scale(mars_model->model, glm::vec3(1 / 20.5, 1 / 20.5, 1 / 20.5));
  planet_t mars;
  mars.model           = mars_model;
  mars.distance_to_sun = 11.0;
  mars.rotate_rate     = 0.010f;

  BREEZE::Texture jupiter_texture_diffuse("./assets/models/jupiter/jupiter-texture.jpg");
  BREEZE::Model*  jupiter_model = new BREEZE::Model("./assets/models/jupiter/jupiter.obj", &jupiter_texture_diffuse, nullptr);
  glm::vec3       jupiter_pos   = glm::vec3(15.0, 0.0, 0.0);
  jupiter_model->setShader(phong_shader);
  jupiter_model->model = glm::mat4(1);
  jupiter_model->model = glm::translate(jupiter_model->model, jupiter_pos);
  jupiter_model->model = glm::scale(jupiter_model->model, glm::vec3(0.4, 0.4, 0.4));
  planet_t jupiter;
  jupiter.model           = jupiter_model;
  jupiter.distance_to_sun = 15.0;
  jupiter.rotate_rate     = 0.008f;

  BREEZE::Texture saturn_texture_diffuse("./assets/models/saturn/Saturn_diff.jpg");
  BREEZE::Model*  saturn_model = new BREEZE::Model("./assets/models/saturn/13906_Saturn_v1_l3.obj", &saturn_texture_diffuse, nullptr);
  glm::vec3       saturn_pos   = glm::vec3(22.0, 0.0, 0.0);
  saturn_model->setShader(phong_shader);
  saturn_model->model = glm::mat4(1);
  saturn_model->model = glm::translate(saturn_model->model, saturn_pos);
  saturn_model->model = glm::scale(saturn_model->model, glm::vec3(0.003, 0.003, 0.003));
  saturn_model->model = glm::rotate(saturn_model->model, 1.4f, glm::vec3(1, 0, 0));
  planet_t saturn;
  saturn.model           = saturn_model;
  saturn.distance_to_sun = 22.0;
  saturn.rotate_rate     = 0.005f;

  BREEZE::Texture uranus_texture_diffuse("./assets/models/uranus/13907_Uranus_planet_diff.jpg");
  BREEZE::Model*  uranus_model = new BREEZE::Model("./assets/models/uranus/13907_Uranus_v2_l3.obj", &uranus_texture_diffuse, nullptr);
  glm::vec3       uranus_pos   = glm::vec3(27.0, 0.0, 0.0);
  uranus_model->setShader(phong_shader);
  uranus_model->model = glm::mat4(1);
  uranus_model->model = glm::translate(uranus_model->model, uranus_pos);
  uranus_model->model = glm::scale(uranus_model->model, glm::vec3(0.001, 0.001, 0.001));
  planet_t uranus;
  uranus.model           = uranus_model;
  uranus.distance_to_sun = 25.0;
  uranus.rotate_rate     = 0.0025f;

  BREEZE::Texture neptune_texture_diffuse("./assets/models/neptune/13908_Neptune_planet_diff.jpg");
  BREEZE::Model*  neptune_model = new BREEZE::Model("./assets/models/neptune/13908_Neptune_V2_l3.obj", &neptune_texture_diffuse, nullptr);
  glm::vec3       neptune_pos   = glm::vec3(27.0, 0.0, 0.0);
  neptune_model->setShader(phong_shader);
  neptune_model->model = glm::mat4(1);
  neptune_model->model = glm::translate(neptune_model->model, neptune_pos);
  neptune_model->model = glm::scale(neptune_model->model, glm::vec3(0.001, 0.001, 0.001));
  planet_t neptune;
  neptune.model           = neptune_model;
  neptune.distance_to_sun = 29.0;
  neptune.rotate_rate     = 0.001f;

  std::vector<planet_t> planets = {sun, venus, mercury, earth, mars, jupiter, saturn, uranus, neptune};

  // Variáveis de controle do estado do programa
  bool is_running     = true;
  bool wireframe_mode = false;
  bool cull_faces     = true;
  bool animation_mode = true;

  // Configurações iniciais da janela
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_MULTISAMPLE);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Criação do framebuffer para o mapa de sombras
  glActiveTexture(GL_TEXTURE1);
  uint32_t depth_map_fbo;
  glGenFramebuffers(1, &depth_map_fbo);

  const uint32_t shadow_width = 1280, shadow_height = 720;
  uint32_t       depth_map;
  glGenTextures(1, &depth_map);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  float border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

  // Configurações do framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Inicialização dos ângulos da câmera
  float camera_theta = 0;
  float camera_phi   = 0;

  // Loop principal do programa (altera modo, ajuste de ângulo e mudança na direção da câmera)
  while (is_running) {
    SDL_Event e;
    while (SDL_PollEvent(&e) > 0) {
      switch (e.type) {
        case SDL_QUIT:
          is_running = false;
          break;
        case SDL_KEYDOWN:
          switch (e.key.keysym.sym) {
            case SDL_KeyCode::SDLK_w:
              camera_target.x -= 0.08;
              break;
            case SDL_KeyCode::SDLK_s:
              camera_target.x += 0.08;
              break;
            case SDL_KeyCode::SDLK_a:
              camera_target.z += 0.08;
              break;
            case SDL_KeyCode::SDLK_d:
              camera_target.z -= 0.08;
              break;
            case SDL_KeyCode::SDLK_UP:
              camera_phi += 0.08;
              break;
            case SDL_KeyCode::SDLK_DOWN:
              camera_phi -= 0.08;
              break;
            case SDL_KeyCode::SDLK_LEFT:
              camera_theta += 0.08;
              break;
            case SDL_KeyCode::SDLK_RIGHT:
              camera_theta -= 0.08;
              break;
            case SDL_KeyCode::SDLK_i:
              wireframe_mode = !wireframe_mode;
              break;
            case SDL_KeyCode::SDLK_o:
              animation_mode = !animation_mode;
              break;
            case SDL_KeyCode::SDLK_p:
              cull_faces = !cull_faces;
              break;
          }
          break;
      }

      SDL_UpdateWindowSurface(window);
    }

    // Define o modo de renderização
    if (wireframe_mode)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Calcula a nova posição da câmera
    glm::mat4 rotation_m = glm::rotate(glm::mat4(1.0f), camera_theta, glm::vec3(0, 1, 0));
    camera_position      = glm::vec3(rotation_m * glm::vec4(camera_position, 1));
    camera_theta         = 0;

    rotation_m      = glm::rotate(glm::mat4(1.0f), camera_phi, glm::vec3(0, 0, 1));
    camera_position = glm::vec3(rotation_m * glm::vec4(camera_position, 1));
    camera_phi      = 0;

    camera_direction = glm::normalize(camera_position - camera_target);

    up           = glm::vec3(0.0f, 1.0f, 0.0f);
    camera_right = glm::normalize(glm::cross(up, camera_direction));

    camera_up = glm::normalize(glm::cross(camera_direction, camera_right));
    view      = glm::lookAt(camera_position, camera_target, camera_up);

    // Animação dos planetas
    if (animation_mode) {
      for (auto i = 0; i < planets.size(); i++) {
        planetRotateOwnAxis(planets[i].model);
        if (i != 0) {
          planetRotateSunAxis(planets[i].model, planets[i].rotate_value, planets[i].distance_to_sun);
        }
        planets[i].rotate_value += planets[i].rotate_rate;
      }
    }

    shadow_shader->use();

    int light_space_matrix_loc = glGetUniformLocation(shadow_shader->get_id(), "light_space_matrix");
    glUniformMatrix4fv(light_space_matrix_loc, 1, GL_FALSE, glm::value_ptr(light_space_matrix));

    glViewport(0, 0, shadow_width, shadow_height);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Renderiza os planetas para o mapa de sombras
    for (auto planet : planets) {
      planet.model->render(shadow_shader);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Configura o culling de faces
    if (cull_faces) {
      glEnable(GL_CULL_FACE);
    } else {
      glDisable(GL_CULL_FACE);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(0);

    glViewport(0, 0, 1280, 720);

    // Loop para renderizar cada planeta na cena
    for (auto planet : planets) {
      BREEZE::ShaderProgram* current_shader = planet.model->getShader();
      current_shader->use();

      light_space_matrix_loc = glGetUniformLocation(current_shader->get_id(), "light_space_matrix");
      glUniformMatrix4fv(light_space_matrix_loc, 1, GL_FALSE, glm::value_ptr(light_space_matrix));

      int texture_uniform_location = glGetUniformLocation(current_shader->get_id(), "current_texture");
      glUniform1i(texture_uniform_location, 0);

      int view_location = glGetUniformLocation(current_shader->get_id(), "view");
      glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));

      int projection_location = glGetUniformLocation(current_shader->get_id(), "projection");
      glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(proj));

      int light_pos_location = glGetUniformLocation(current_shader->get_id(), "light_position");
      glUniform3fv(light_pos_location, 1, &sun_pos[0]);

      int view_pos_location = glGetUniformLocation(current_shader->get_id(), "view_position");
      glUniform3fv(view_pos_location, 1, &camera_position[0]);

      int shadow_location = glGetUniformLocation(current_shader->get_id(), "shadow_map");
      glUniform1i(shadow_location, 1);

      planet.model->render();
    }

    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DeleteContext(main_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
