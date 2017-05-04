#include "GameAssetManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.cc"
#include "GameAsset.cc"

auto player=std::make_shared<BoundingBox>(Vector3(0.0f,0.0f,0.0f),0.5f,0.5f,0.5f);

/**
 * Creates a GameAssetManager to load the correct shaders based on the
 * ApplicationMode.
 */
GameAssetManager::GameAssetManager(ApplicationMode mode) {
  std::string vertex_shader("shaders/translate.vs");
  std::string fragment_shader("shaders/fragment.fs");

  switch(mode) {
  case ROTATE:
    vertex_shader = "shaders/rotate.vs";
    break;
  case SCALE:
    vertex_shader = "shaders/scale.vs";
    break;
  case TRANSFORM:
  default:
    break;
  };

  program_token = CreateGLProgram(vertex_shader, fragment_shader);
}

/**
 * Deletes a GameAssetManager, in particular it will clean up any modifications
 * to the OpenGL state.
 */
GameAssetManager::~GameAssetManager() {
  glDeleteProgram(program_token);
}

/**
 * Unimplemented copy constructor -- this means that the GameAssetManager
 * may not work as you'd expect when being copied.
 */
GameAssetManager::GameAssetManager(GameAssetManager const& the_manager) {
  // TODO: implement this
}

/**
 * Unimplemented move constructor -- this unimplemented method violates the
 * C++11 move semantics for GameAssetManager.
 */
GameAssetManager::GameAssetManager(GameAssetManager const&& the_manager) {
  // TODO: implement this
}

/**
 * Unimplemented assisgnment operator -- violates the expected semantics for
 * assignment in C++11.
 */
void GameAssetManager::operator=(GameAssetManager const& the_manager) {
  // TODO: implement this
}
/**
 * Moves the Camera.
 */
void GameAssetManager::translateCamera(GLfloat x, GLfloat y, GLfloat z){
camera.translateCamera(x,y,z);
Vector3 vec(x,y,z);
player->SetCentre(vec);
}
/**
 * Adds a GameAsset to the scene graph.
 */
void GameAssetManager::AddAsset(std::shared_ptr<GameAsset> the_asset) {
  draw_list.push_back(the_asset);
}

/**
 * Draws each GameAsset in the scene graph.
 */
void GameAssetManager::Draw() {



  auto view=camera.getView();
  GLint ViewLocation = glGetUniformLocation(program_token,"view");
  glUniformMatrix4fv(ViewLocation, 1, GL_FALSE, glm::value_ptr(view));

  glUseProgram(program_token);
   SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_KEYDOWN:
	switch(event.key.keysym.sym){
	  /**
 	  * Move camera to the left.
 	  */
	  case SDLK_a:
	translateCamera(-0.5f,0.0f,0.0f);
  	for(auto c : draw_list){
	if(player->CollidesWith(c->getbbox())){
		translateCamera(0.5f,0.0f,0.0f);
}
}
	    

	  break;
	  /**
 	  * Move camera to the right.
 	  */	
	  case SDLK_d:
  	translateCamera(0.5f,0.0f,0.0f);
  	for(auto c : draw_list){
	if(player->CollidesWith(c->getbbox())){
		translateCamera(-0.5f,0.0f,0.0f);
}
}
	  break;
	  /**
 	  * Move camera backwards.
 	  */
	  case SDLK_s:
	translateCamera(0.0f,0.0f,-5.0f);
  	for(auto c : draw_list){
	if(player->CollidesWith(c->getbbox())){
		translateCamera(0.0f,0.0f,5.0f);
}
}
	  break;
	  /**
 	  * Move camera forwards.
 	  */
	  case SDLK_w:
	translateCamera(0.0f,0.0f,5.0f);
  	for(auto c : draw_list){
	if(player->CollidesWith(c->getbbox())){
		translateCamera(0.0f,0.0f,-5.0f);
}
}
	  break;
	  /**
 	  * Move camera downwards.
 	  */
	  case SDLK_z:
	  translateCamera(0.0f,0.5f,0.0f);
	  break;
	  /**
 	  * Move camera upwards.
 	  */
	  case SDLK_x:
	  translateCamera(0.0f,-0.5f,0.0f);
	  break;
  default:
  break;
}
    default:
      break;
    }
  }
  for(auto ga: draw_list) {
   ga->Draw(program_token);

   glm::mat4 model=ga->getModelMat();
   GLint ModelLocation = glGetUniformLocation(program_token,"model");
   glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, glm::value_ptr(model));
  
   glm::vec3 colour=ga->getModelColour();
   GLint ColourLocation = glGetUniformLocation(program_token,"colour");
   glUniform3f(ColourLocation, colour[0], colour[1], colour[2]);

  }




}


/**
 * When given the contents of a vertex shader and fragment shader
 * GameAssetManager::CreateGLProgram will compile and link them.  This
 * implementation will exit with -1 error if an error is detected.
 *
 * @return the GL "token" referring to the gl program.
 */
GLuint GameAssetManager::CreateGLProgram(std::string & vertex_shader
                                         , std::string & fragment_shader) {
  auto v_shader_token = CreateGLESShader(GL_VERTEX_SHADER, vertex_shader);
  auto f_shader_token = CreateGLESShader(GL_FRAGMENT_SHADER, fragment_shader);

  GLint program_ok;

  GLuint program = glCreateProgram();

  glAttachShader(program, v_shader_token);
  glAttachShader(program, f_shader_token);
  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
  if (!program_ok) {
    std::cerr << "Failed to link shader program:" << std::endl;
    glDeleteProgram(program);
    ::exit(-1);
  }
  return program;
}

/**
 * When given a type of shader to construct and the contents of a shader,
 * GameAssetManager::CreateGLESShader will create the shader or exit with
 * error -1.
 *
 * @return the GL "token" for the requested shader.
 */
GLuint GameAssetManager::CreateGLESShader(GLenum type, std::string & shader) {
  GLuint shader_token;
  GLint shader_ok;
  auto source = ReadShader(shader);

  if (!source.first)
    return 0;

  shader_token = glCreateShader(type);
  glShaderSource(shader_token, 1, (const GLchar**)&source.first, &source.second);
  glCompileShader(shader_token);
  delete(source.first);

  glGetShaderiv(shader_token, GL_COMPILE_STATUS, &shader_ok);
  if (!shader_ok) {
    GLint maxLength = 0;
    glGetShaderiv(shader_token, GL_INFO_LOG_LENGTH, &maxLength);

    //The maxLength includes the NULL character
    std::vector<char> errorLog(maxLength);
    glGetShaderInfoLog(shader_token, maxLength, &maxLength, &errorLog[0]);

    //Provide the infolog in whatever manor you deem best.
    std::cerr << "Failed to compile " << shader << " with error code " << shader_ok << std::endl;
    for(auto c: errorLog) {
      std::cerr << c;
    }

    glDeleteShader(shader_token); //Don't leak the shader.
    ::exit(-1);
  }
  return shader_token;
}

/**
 * ReadShader reads the contents of a file and packs it into a null termintated
 * GLchar * which is suitable for sending to OpenGL.
 *
 * @return a pair consisting of the shader file contents and a holder for the
 *         OpenGL "token".
 */
std::pair<GLchar *, GLint> GameAssetManager::ReadShader(std::string & shader) {
  std::ifstream input_file;
  GLint length;
  input_file.open(shader, std::ios::in);

  input_file.seekg(0, std::ios::end);  // go to the end of the file
  length = input_file.tellg();    // get length of the file
  input_file.seekg(0, std::ios::beg);  // go to beginning of the file

  GLchar * buffer = new GLchar[length+1];
  input_file.read(buffer, length);
  buffer[length+1]='\0';  // Ensure null terminated

  input_file.close();
  return std::make_pair(buffer, length);
}
