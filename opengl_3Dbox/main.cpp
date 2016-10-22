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

glm::mat4 view;
glm::mat4 view_origin;
float view_x = 0.0f;
float view_y = 0.0f;
float view_z = -3.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

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
	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	GLuint indices[] = { // 注意索引从0开始! 
		0, 1, 3, // 第一个三角形
		1, 2, 3  // 第二个三角形
	};
	//using namespace std;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	float screenWidth = 800;
	float screenHeight = 600;
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	glfwSetKeyCallback(window, key_callback);
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
	
	//3Dmatrix
	glm::mat4 model;
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 model_ori = model;
	// 注意，我们将矩阵向我们要进行移动场景的反向移动。
	//view_origin = glm::translate(view, glm::vec3(view_x, view_y, view_z));
	view = glm::translate(view, glm::vec3(view_x, view_y, view_z));
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), screenWidth / screenHeight, 0.1f, 100.0f);

	#pragma region Shader&Texture
	//Texture
	int img_width, img_height;
	unsigned char * image = SOIL_load_image("container.jpg", &img_width, &img_height, 0, SOIL_LOAD_RGB);
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	int img_width_2, img_height_2;
	unsigned char * image_2 = SOIL_load_image("awesomeface.png", &img_width_2, &img_height_2, 0, SOIL_LOAD_RGB);
	GLuint texture_2;
	glGenTextures(1, &texture_2);
	glBindTexture(GL_TEXTURE_2D, texture_2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width_2, img_height_2, 0, GL_RGB, GL_UNSIGNED_BYTE, image_2);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image_2);
	glBindTexture(GL_TEXTURE_2D, 0);
	//Triangle

	//Shader

	GLchar *tmp = nullptr;
	int tmplen;

	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	read_file_char(tmp, "vertexShader.txt");
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
	std::cout << "compilation1 success!" << std::endl;


	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	read_file_char(tmp, "fragmentShader.txt");
	glShaderSource(fragmentShader, 1, &tmp, NULL);
	glCompileShader(fragmentShader);
	delete tmp;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
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
	#pragma endregion

	//Points
	GLuint EBO;
	glGenBuffers(1, &EBO);
	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
		GLuint VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		//glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	/*
	// 0. 复制顶点数组到缓冲中供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 1. 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// 2. 当我们渲染一个物体时要使用着色器程序
	glUseProgram(shaderProgram);
	// 3. 绘制物体
	someOpenGLFunctionThatDrawsOurTriangle();
	*/
	glUseProgram(shaderProgram);
	GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		float r1 = rd() / 30 / (float)rd.max();
		float g1 = rd() / 30 / (float)rd.max();
		float b1 = rd() / 30 / (float)rd.max();
		r = (r + r1 > 1.0) ? (r + r1 - 1.0) : (r + r1);
		g = (g + g1 > 1.0) ? (g + g1 - 1.0) : (g + g1);
		b = (b + b1 > 1.0) ? (b + b1 - 1.0) : (b + b1);

		glClearColor(r, g, b, a);
		//glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//GLfloat timeValue = glfwGetTime();
		//GLfloat greenValue = (sin(timeValue) / 2) + 0.5;

		//GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		glUseProgram(shaderProgram);
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		//->
		//glBindTexture(GL_TEXTURE_2D, texture);
		//<-

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture1"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_2);
		glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture2"), 1);
		//model = glm::rotate(model_ori, (GLfloat)glfwGetTime() * glm::radians(50.0f),glm::vec3(0.5f, 1.0f, 0.0f));

		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));



		glBindVertexArray(VAO);
		////glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		
		for (GLuint i = 0; i < 10; i++)
		{
			glm::mat4 model_t;
			model_t = glm::translate(model_t, cubePositions[i]);
			GLfloat angle = glm::radians(20.0f * i);
			model_t = glm::rotate(model_t, (GLfloat)glfwGetTime() * glm::radians(30.0f) + angle, glm::vec3(1.0, 0.5, 0.2));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_t));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();

	return 0;
}

void change(float xc, float yc, float zc)
{
	view_x = view_x - xc;
	view_y = view_y - yc;
	view_z = view_z - zc;
	view = glm::translate(view_origin, glm::vec3(view_x, view_y, view_z));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// 当用户按下ESC键,我们设置window窗口的WindowShouldClose属性为true
	// 关闭应用程序
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	float move = 0.1;
	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
		change(0, 0, -move);
	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
		change(0, 0, move);
	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
		change(-move, 0, 0);
	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
		change(move, 0, 0);
	if (key == GLFW_KEY_LEFT_SHIFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
		change(0, -move, 0);
	if (key == GLFW_KEY_SPACE && (action == GLFW_REPEAT || action == GLFW_PRESS))
		change(0, move, 0);
}