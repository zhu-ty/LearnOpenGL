#pragma region old_include_&_define
#include<iostream>
#include<fstream>
#include<random>

#define GLEW_STATIC
#include<GL/glew.h>

#include<GLFW/glfw3.h>

#include<SOIL/SOIL.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"mesh.h"

glm::mat4 view;
glm::mat4 view_origin;
float view_x = 0.0f;
float view_y = 0.0f;
float view_z = -3.0f;

float aspect = 45.0f;

GLfloat pitch = 0.0f, yaw = -90.0f;

GLfloat deltaTime = 0.0f;   // 当前帧与上一帧的时间差
GLfloat lastFrame = 0.0f;   // 上一帧的时间

bool firstMouse = true;

bool keys[1024];
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat lastX = 400, lastY = 300;//鼠标在中心


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();
#pragma endregion

#pragma region light

glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};


#pragma endregion


void read_file_char(GLchar*& str, char* filename)
{
	std::ifstream t;
	t.open(filename);      // open input file  
	t.seekg(0, std::ios::end);    // go to the end  
	int length = t.tellg();           // report location (this is the length)  
	t.seekg(0, std::ios::beg);    // go back to the beginning  
	str = new GLchar[length + 1];    // allocate memory for a buffer of appropriate dimension
	str[length] = '\0';
	t.read(str, length);       // read the whole file into the buffer
	for (int i = length - 1; i >= 0; i--)//makesure there is a '\n' at the end.
	{
		if (str[i] == '\n')
		{
			str[i + 1] = '\0';
			break;
		}
	}
	t.close();
}

int main()
{
#pragma region Init
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	float screenWidth = 1366;
	float screenHeight = 768;
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	std::random_device rd;
#pragma endregion

#pragma region 3DMatrix
	//3Dmatrix
	glm::mat4 model;
	//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	view = glm::translate(view, glm::vec3(view_x, view_y, view_z));
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), screenWidth / screenHeight, 0.1f, 100.0f);

	glm::mat4 view;
	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
#pragma endregion

#pragma region Shader&Texture

	//Shader


	GLchar *tmp = nullptr;

	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	read_file_char(tmp, "vertexShader.glsl");
	glShaderSource(vertexShader, 1, &tmp, NULL);
	glCompileShader(vertexShader);
	delete tmp;
	//test if it's success
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	else
		std::cout << "compilation1 success!" << std::endl;


	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	read_file_char(tmp, "fragmentShader.glsl");
	glShaderSource(fragmentShader, 1, &tmp, NULL);
	glCompileShader(fragmentShader);
	delete tmp;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	else
		std::cout << "compilation2 success!" << std::endl;

	GLuint shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
	}
	//glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);
	GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // 依旧把光源设置为白色







	//GLuint vertexShader2;
	//vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
	//read_file_char(tmp, "lightningvertexShader.glsl");
	//glShaderSource(vertexShader2, 1, &tmp, NULL);
	//glCompileShader(vertexShader2);
	//delete tmp;
	//glGetShaderiv(vertexShader2, GL_COMPILE_STATUS, &success);
	//if (!success)
	//{
	//	glGetShaderInfoLog(vertexShader2, 512, NULL, infoLog);
	//	std::cout << "ERROR::SHADER::VERTEX2::COMPILATION_FAILED\n" << infoLog << std::endl;
	//}

	//GLuint fragmentShader2;
	//fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	//read_file_char(tmp, "lightningfragmentShader.glsl");
	//glShaderSource(fragmentShader2, 1, &tmp, NULL);
	//glCompileShader(fragmentShader2);
	//delete tmp;
	//glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
	//if (!success)
	//{
	//	glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
	//	std::cout << "ERROR::SHADER::FRAGMENT2::COMPILATION_FAILED\n" << infoLog << std::endl;
	//}

	//GLuint shaderProgram2;
	//shaderProgram2 = glCreateProgram();
	//glAttachShader(shaderProgram2, vertexShader2);
	//glAttachShader(shaderProgram2, fragmentShader2);
	//glLinkProgram(shaderProgram2);
	//glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
	//if (!success)
	//{
	//	glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
	//	std::cout << "ERROR::SHADER::SHADERPROGRAME2::COMPILATION_FAILED\n" << infoLog << std::endl;
	//}
	//glDeleteShader(vertexShader2);
	//glDeleteShader(fragmentShader2);
#pragma endregion

#pragma region model
	Model m("nanosuit/nanosuit.obj");
#pragma endregion



#pragma region Loops
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	float r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;
	//float r2 = 1.0f, g2 = 1.0f, b2 = 1.0f, a2 = 1.0f;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		do_movement();
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//float r1 = rd() / 200 / (float)rd.max();
		//float g1 = rd() / 200 / (float)rd.max();
		//float b1 = rd() / 200 / (float)rd.max();
		//r2 = (r2 + r1 > 2.0) ? (r2 + r1 - 2.0) : (r2 + r1);
		//g2 = (g2 + g1 > 2.0) ? (g2 + g1 - 2.0) : (g2 + g1);
		//b2 = (b2 + b1 > 2.0) ? (b2 + b1 - 2.0) : (b2 + b1);
		//r = (r2 < 1.0) ? r2 : (2.0 - r2);
		//g = (g2 < 1.0) ? g2 : (2.0 - g2);
		//b = (b2 < 1.0) ? b2 : (2.0 - b2);
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);


		

#pragma region object_loop
		glUseProgram(shaderProgram);



		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		GLfloat radius = 10.0f;
		GLfloat camX = sin(glfwGetTime()) * radius;
		GLfloat camZ = cos(glfwGetTime()) * radius;
		glm::mat4 view_new;
		view_new = view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_new));

		GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
		glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);


		projection = glm::perspective(glm::radians(aspect), (GLfloat)screenWidth / screenHeight, 0.1f, 100.0f);
		GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		//光照
		//GLint matAmbientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
		//GLint matDiffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
		//GLint matSpecularLoc = glGetUniformLocation(shaderProgram, "material.specular");
		GLint matShineLoc = glGetUniformLocation(shaderProgram, "material.shininess");

		//glUniform3f(matAmbientLoc, 1.0f, 0.5f, 0.31f);
		//glUniform3f(matDiffuseLoc, 1.0f, 0.9f, 0.91f);
		//glUniform3f(matSpecularLoc, 0.5f, 0.5f, 0.5f);
		glUniform1f(matShineLoc, 32.0f);


		//平行光
		GLint lightDirPos = glGetUniformLocation(shaderProgram, "dirLight.direction");
		glUniform3f(lightDirPos, -0.2f, -1.0f, -0.3f);


		GLint lightAmbientLoc = glGetUniformLocation(shaderProgram, "dirLight.ambient");
		GLint lightDiffuseLoc = glGetUniformLocation(shaderProgram, "dirLight.diffuse");
		GLint lightSpecularLoc = glGetUniformLocation(shaderProgram, "dirLight.specular");

		glUniform3f(lightAmbientLoc, 0.1f, 0.1f, 0.1f);
		glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);// 让我们把这个光调暗一点，这样会看起来更自然
		glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);


		//点光源
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].position"),
			pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[1].position"),
			pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[2].position"),
			pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[3].position"),
			pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);

		glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[0].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[0].quadratic"), 0.032);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[1].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[1].quadratic"), 0.032);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[2].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[2].quadratic"), 0.032);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[3].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[3].quadratic"), 0.032);

		//float dif__[3] = { 0.2,0.2,0.2 };

		//glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].ambient"), 0.1f, 0.1f, 0.1f);
		//glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].diffuse"), dif__[0], dif__[1], dif__[2]);
		//glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
		//glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[1].ambient"), 0.1f, 0.1f, 0.1f);
		//glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[1].diffuse"), dif__[0], dif__[1], dif__[2]);
		//glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
		//glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[2].ambient"), 0.1f, 0.1f, 0.1f);
		//glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[2].diffuse"), dif__[0], dif__[1], dif__[2]);
		//glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
		//glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[3].ambient"), 0.1f, 0.1f, 0.1f);
		//glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[3].diffuse"), dif__[0], dif__[1], dif__[2]);
		//glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);


		//手电筒

		glUniform3f(glGetUniformLocation(shaderProgram, "torchLight.position"),
			cameraPos.x, cameraPos.y, cameraPos.z);
		glUniform3f(glGetUniformLocation(shaderProgram, "torchLight.direction"),
			cameraFront.x, cameraFront.y, cameraFront.z);
		glUniform1f(glGetUniformLocation(shaderProgram,
			"torchLight.cutoff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(shaderProgram,
			"torchLight.outercutoff"), glm::cos(glm::radians(14.5f)));
		glUniform1f(glGetUniformLocation(shaderProgram, "torchLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "torchLight.linear"), 0.0014);
		glUniform1f(glGetUniformLocation(shaderProgram, "torchLight.quadratic"), 0.000007);

		glUniform3f(glGetUniformLocation(shaderProgram, "torchLight.ambient"), 0.2f, 0.2f, 0.1f);
		glUniform3f(glGetUniformLocation(shaderProgram, "torchLight.diffuse"), 1.9f, 1.9f, 1.9f);
		glUniform3f(glGetUniformLocation(shaderProgram, "torchLight.specular"), 5.0f, 5.0f, 5.0f);



		m.Draw(shaderProgram);
#pragma endregion

//#pragma region light_loop
//		glUseProgram(shaderProgram2);
//
//		for (int i = 0; i < 4; i++)
//		{
//			GLint viewLoc2 = glGetUniformLocation(shaderProgram2, "view");
//			glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view_new));
//
//			GLint projectionLoc2 = glGetUniformLocation(shaderProgram2, "projection");
//			glUniformMatrix4fv(projectionLoc2, 1, GL_FALSE, glm::value_ptr(projection));
//
//			glm::mat4 model2 = glm::mat4();
//			model2 = glm::translate(model2, pointLightPositions[i]);
//			model2 = glm::scale(model2, glm::vec3(0.2f));
//
//			GLint modelLoc2 = glGetUniformLocation(shaderProgram2, "model");
//			glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model2));
//
//			glBindVertexArray(lightVAO);
//			glDrawArrays(GL_TRIANGLES, 0, 36);
//			glBindVertexArray(0);
//		}
//#pragma endregion

		glfwSwapBuffers(window);
	}
#pragma endregion

#pragma region End
	glfwTerminate();
#pragma endregion

	return 0;
}

void change(float xc, float yc, float zc)
{
	view_x = view_x - xc;
	view_y = view_y - yc;
	view_z = view_z - zc;
	view = glm::translate(view_origin, glm::vec3(view_x, view_y, view_z));
}


void do_movement()
{
	// 摄像机控制
	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_W])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_SPACE])
		cameraPos += cameraSpeed * cameraUp;
	if (keys[GLFW_KEY_LEFT_SHIFT])
		cameraPos -= cameraSpeed * cameraUp;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// 当用户按下ESC键,我们设置window窗口的WindowShouldClose属性为true
	// 关闭应用程序
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
	//GLfloat cameraSpeed = 0.05f;
	//if (key == GLFW_KEY_W)
	//	cameraPos += cameraSpeed * cameraFront;
	//if (key == GLFW_KEY_S)
	//	cameraPos -= cameraSpeed * cameraFront;
	//if (key == GLFW_KEY_A)
	//	cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	//if (key == GLFW_KEY_D)
	//	cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
	if (firstMouse) // 这个bool变量一开始是设定为true的
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // 注意这里是相反的，因为y坐标的范围是从下往上的
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	float as_speed = 2.5f;
	if (aspect >= 1.0f && aspect <= 45.0f)
		aspect -= yoffset * as_speed;
	if (aspect <= 1.0f)
		aspect = 1.0f;
	if (aspect >= 45.0f)
		aspect = 45.0f;
}