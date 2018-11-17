#include "Shader.h"

float Shader::deltaTime=0;
int Shader::SCR_WIDTH=0;
int Shader::SCR_HEIGHT=0;
float Shader::farSpace=0;
float Shader::nearSpace=0;
GLuint Shader::depthMapFBO=0;
GLuint Shader::depthMap=0;
Shader* Shader::shadowSh=nullptr;
// constructor generates the shader on the fly
// ------------------------------------------------------------------------

 Shader::Shader(const char * vertexPath, const char * fragmentPath, const char * geometryPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		vertexShaderName = vertexPath;
		fragmentShaderName = fragmentPath;
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// if geometry shader path is present, also load a geometry shader
		if (geometryPath != nullptr)
		{

			gShaderFile.open(geometryPath);
			geoShaderName = geometryPath;
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "vertexPath : " + std::string(vertexPath) << std::endl;
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char * fShaderCode = fragmentCode.c_str();
	// 2. compile shaders
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// if geometry shader is given, compile geometry shader
	unsigned int geometry;
	if (geometryPath != nullptr)
	{
		const char * gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
	}
	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geometryPath != nullptr)
		glAttachShader(ID, geometry);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != nullptr)
		glDeleteShader(geometry);

}

 // activate the shader
 // ------------------------------------------------------------------------

  void Shader::use()
 {
	 glUseProgram(ID);
 }

 // deactivate the shader
 // ------------------------------------------------------------------------

  void Shader::Stop()
 {
	 glUseProgram(0);
 }

 // utility uniform functions
 // ------------------------------------------------------------------------

  void Shader::setBool(const std::string & name, bool value) /*const*/
 {
	 glUniform1i(getLoc(name), (int)value);
 }

 // ------------------------------------------------------------------------

  void Shader::setInt(const std::string & name, int value) /*const*/
 {
	 glUniform1i(getLoc(name), value);
 }

 // ------------------------------------------------------------------------

  void Shader::setFloat(const std::string & name, float value) /*const*/
 {
	 glUniform1f(getLoc(name), value);
 }

 // ------------------------------------------------------------------------

  void Shader::setVec2(const std::string & name, const glm::vec2 & value) /*const*/
 {
	 glUniform2fv(getLoc(name), 1, &value[0]);
 }

  void Shader::setVec2(const std::string & name, float x, float y) /*const*/
 {
	 glUniform2f(getLoc(name), x, y);
 }

 // ------------------------------------------------------------------------

  void Shader::setVec3(const std::string & name, const glm::vec3 & value) /*const*/
 {
	 glUniform3fv(getLoc(name), 1, &value[0]);
 }

  void Shader::setVec3(const std::string & name, float x, float y, float z) /*const*/
 {
	 glUniform3f(getLoc(name), x, y, z);
 }

 // ------------------------------------------------------------------------

  void Shader::setVec4(const std::string & name, const glm::vec4 & value) /*const*/
 {
	 glUniform4fv(getLoc(name), 1, &value[0]);
 }

  void Shader::setVec4(const std::string & name, float x, float y, float z, float w)
 {
	 glUniform4f(getLoc(name), x, y, z, w);
 }

 // ------------------------------------------------------------------------

  void Shader::setMat2(const std::string & name, const glm::mat2 & mat) /*const*/
 {
	 glUniformMatrix2fv(getLoc(name), 1, GL_FALSE, &mat[0][0]);
 }

 // ------------------------------------------------------------------------

  void Shader::setMat3(const std::string & name, const glm::mat3 & mat) /*const*/
 {
	 glUniformMatrix3fv(getLoc(name), 1, GL_FALSE, &mat[0][0]);
 }

 // ------------------------------------------------------------------------

  void Shader::setMat4(const std::string & name, const glm::mat4 & mat) /*const*/
 {
	 glUniformMatrix4fv(getLoc(name), 1, GL_FALSE, &mat[0][0]);
 }

  void Shader::update(glm::mat4 proj, glm::mat4 view, glm::mat4 model,glm::vec3 camPos,std::vector<PointLight*> _pLights,DirLight _dirLight)
  {
	  //time in seconds
	  setFloat("Time",deltaTime/1000.0f);
	  setInt("SCR_WIDTH",SCR_WIDTH);
	  setInt(" SCR_HEIGHT", SCR_HEIGHT);
	  setFloat("nearSpace",nearSpace);
	  setFloat("farSpace", farSpace);

	  setVec3("viewPos", camPos);
	  setMat4("projection", proj);
	  setMat4("view", view);
	  setMat4("model", model);
	  //constant shininess
	  setFloat("material.shininess", 26.0);
	  //DIR LIGHT
	  setVec3("dirLight.direction",_dirLight.direction);
	  setVec3("dirLight.ambient", _dirLight.ambient);
	  setVec3("dirLight.diffuse", _dirLight.diffuse);
	  setVec3("dirLight.specular", _dirLight.specular);
	  //Point lights
	  int nr_pLights = _pLights.size();
	  if (nr_pLights > 100)nr_pLights = 100;//max is 100 point light
	  setInt("nr_active_plight", nr_pLights);
	  for (int i = 0; i < nr_pLights; i++) {
		  setVec3("pointLights["+std::to_string(i)+"].position", _pLights[i]->position);
		  setFloat("pointLights[" +std::to_string(i) + "].constant", _pLights[i]->constant);
		  setFloat("pointLights[" +std::to_string(i) + "].linear", _pLights[i]->linear);
		  setFloat("pointLights[" +std::to_string(i) + "].quadratic", _pLights[i]->quadratic);
		  setVec3("pointLights[" + std::to_string(i)+ "].ambient", _pLights[i]->ambient);
		  setVec3("pointLights[" + std::to_string(i)+ "].diffuse", _pLights[i]->diffuse);
		  setVec3("pointLights[" + std::to_string(i)+ "].specular", _pLights[i]->specular);
	  }

  }

  void Shader::destroy()
  {
	  locationsMap.clear();
	  glDeleteProgram(this->ID);
  }


Shader::~Shader()
{
}

GLuint Shader::getLoc(const std::string & name)
{
	std::map<std::string, GLuint>::iterator iter = locationsMap.find(name);
	if (iter != locationsMap.end())
		return iter->second;

	GLuint loc = glGetUniformLocation(ID, name.c_str());
	locationsMap.insert(std::pair<std::string, GLuint>(name, loc));
	return loc;

}

void Shader::checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}


