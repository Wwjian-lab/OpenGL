#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <ctime>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <irrKlang.h>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "car.h"
#include "FixedCamera.h"

#include <iostream>

using namespace irrklang;

#pragma region Function Declaration
//函数申明
GLFWwindow* windowInit();
bool gladInit();
void skyBoxInit();
void InitTreePosition();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void RenderFloor(Model& model, Shader& shader);
void RenderSkyBox(Shader& shader);
void RenderGameObj(Model& model, Shader& shader);
void RenderCar(Model& model, Shader& shader);
void RenderTree(Model& model, glm::mat4 modelMat, Shader& shader);
void DriveModeCameraFix();
GLuint loadCubemap(vector<const GLchar*> faces);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void Prompt();
void GetFPS();
#pragma endregion

#pragma region Global variable declaration
//常量，窗体长宽
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

GLuint skyboxVAO, skyboxVBO;
GLuint cubemapTexture;

// camera
Camera camera(glm::vec3(0.0f, 2.0f, 3.0f));
FixedCamera DrivaModeCamera(glm::vec3(0.0f, 2.0f, 3.0f));
Car car(glm::vec3(0.0f, 0.05f, 0.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool DriveCameramode = false;
bool PolyMode = false;
bool chTreePos = false;
bool CountDown = false;

bool playcarsound = false;

//时间，变化量
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//FPS计算
int FPS = 0;
int frames = 0;
float lastTime = 0.0f;

//SoundEngine
ISoundEngine *SoundEngine = createIrrKlangDevice();
#pragma endregion

vector<const GLchar*> faces;
#pragma region SkyboxVert
float skyboxVertices[] = {   
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

float floorVert[] = {
	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,
};
#pragma endregion
 
//随机数目数组
vector<glm::vec3> treePositions;


int main()
{
	Prompt();//打印操作提示
	SoundEngine->play2D("background.mp3", true);

	GLFWwindow* window = windowInit();
	bool gladinitOK = gladInit();
	if (window == NULL || !gladinitOK )
		return -1;

	//随机初始化生成树木位置
	InitTreePosition();

	// stbi加载图片，反转y轴
	stbi_set_flip_vertically_on_load(true);

	//深度测试
	glEnable(GL_DEPTH_TEST);

	//加载Shader
	Shader ourShader("vertexSource.vert", "fragmentSource.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");
	//加载模型
	Model ourModel("model/gun/Railgun_Prototype-Wavefront OBJ.obj");
	Model secdmodel("model/backpack/backpack.obj");
	Model floormodel("model/race-track/race-track.obj");
	//Model floormodel("model/road/road.blend");
	Model LanBoJini("model/car/Lamborghini.obj");
	//Model LanBoJini("Car Obj.obj");
	Model tree("model/tree/tree.obj");

	//设置天空盒VAO VBO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
	faces.push_back("skybox/right.jpg");
	faces.push_back("skybox/left.jpg");
	faces.push_back("skybox/bottom.jpg");
	faces.push_back("skybox/top.jpg");
	faces.push_back("skybox/back.jpg");
	faces.push_back("skybox/front.jpg");
	cubemapTexture = loadCubemap(faces);

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//处理键盘事件
		processInput(window);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderSkyBox(skyboxShader);

		car.UpdateLastPosition();
		car.UpdateLastYaw();


		if (DriveCameramode) DriveModeCameraFix();
		if (chTreePos) InitTreePosition();
		//开启Shader
		ourShader.use();
		RenderFloor(floormodel, ourShader);
		//RenderGameObj(secdmodel, ourShader);
		//RenderGameObj(ourModel, ourShader);
		//RenderGameObj(tree, ourShader);
		for (int i = 0; i < 2000; i++) {
			glm::mat4 modelMat(1.0f);
			modelMat = glm::translate(glm::mat4(1.0f),treePositions[i]);
			RenderTree(tree, modelMat, ourShader);
		}
		RenderCar(LanBoJini, ourShader);

		//交换缓冲（双缓冲），提交外部IO事件
		glfwSwapBuffers(window);
		glfwPollEvents();
		GetFPS();
		if (!CountDown) {
			SoundEngine->play2D("count3.mp3", false);
			CountDown = true;
		}
	}

	//结束，释放资源
	glfwTerminate();
	return 0;
}
//窗口初始化
GLFWwindow* windowInit() {
	// GLFW 初始化
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// GLFW window 初始化
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//捕获鼠标显示
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return window;
}

//glad初始化
bool gladInit() {
	// 加载glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	return true;
}

void skyBoxInit() {

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (!DriveCameramode) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, 2 * deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, 2 * deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, 2 * deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, 2 * deltaTime);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			camera.ProcessKeyboard(UP, 2 * deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			camera.ProcessKeyboard(DOWN, 2 * deltaTime);
	}
	else {
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			DrivaModeCamera.ProcessKeyboard(CAMERA_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			DrivaModeCamera.ProcessKeyboard(CAMERA_RIGHT, deltaTime);
	}

	//前景方向
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		car.ProcessKeyboardInput(CarForward, deltaTime);
		if (!playcarsound) {
			SoundEngine->play2D("car.mp3", false);
			playcarsound = true;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			car.ProcessKeyboardInput(CarLeft, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			car.ProcessKeyboardInput(CarRight, deltaTime);

		if(DriveCameramode) 
			camera.ZoomOut();
	}
	//后退方向
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		car.ProcessKeyboardInput(CarBackward, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			car.ProcessKeyboardInput(CarLeft, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			car.ProcessKeyboardInput(CarRight, deltaTime);
		}
		if(DriveCameramode)
			camera.ZoomIn();
	}
	glfwSetKeyCallback(window, key_callback);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_F10 && action == GLFW_PRESS) {
		DriveCameramode = !DriveCameramode;
		string info = DriveCameramode ? "驾驶视角" : "上帝视角";
		cout << info << endl;
	}
	if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
		PolyMode = !PolyMode;
		if (PolyMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		string info = PolyMode ? "线框图模式" : "正常渲染模式";
		cout << info << endl;
	}
	if (key == GLFW_KEY_F9 && action == GLFW_PRESS) {
		chTreePos = true;
		string info = "重新生成树木位置";
		cout << info << endl;

	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		string info = "FPS:";
		cout << info << FPS << endl;
	}
}
//窗口
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

//处理鼠标事件
// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//上帝视角才可用过鼠标滑动切换视角
	if (!DriveCameramode) {
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

//鼠标滚轮
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

GLuint loadCubemap(vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = stbi_load(faces[i], &width, &height, &nrComponents,0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		stbi_image_free(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

void RenderFloor(Model& model, Shader& shader) {
	glm::mat4 viewMat = camera.GetViewMatrix();
	//向下微调
	viewMat = glm::translate(viewMat, glm::vec3(0.0f, -3.0f, 0.0f));
	shader.setMat4("view", viewMat);
	glm::mat4 modelMat = glm::mat4(1.0f);
	shader.setMat4("model", modelMat);
	glm::mat4 projMat = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	shader.setMat4("proj", projMat);
	//modelMat = glm::scale(modelMat, glm::vec3(20.0f, 20.0f, 20.0f));
	//modelMat = glm::scale(modelMat, glm::vec3(0.004f, 0.004f, 0.004f));
	shader.setMat4("model", modelMat);
	model.Draw(shader);
}

//渲染天空盒
void RenderSkyBox(Shader& shader) {
	glDepthMask(GL_FALSE);// turn depth writing off
	shader.use();
	//使天空盒跟随camera移动
	glm::mat4 view_sky = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	glm::mat4 projection_sky = camera.GetProjMatrix((float)SCR_HEIGHT / (float)SCR_WIDTH);
	shader.setMat4("view", view_sky);
	shader.setMat4("projection", projection_sky);
	// skybox cube
	shader.setInt("skybox", 0);
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}



void RenderGameObj(Model& model, Shader& shader) {
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(0.0f,-1.0f, 0.0f));
	//缩小
	modelMat = glm::scale(modelMat, glm::vec3(1.0f, 1.0f, 1.0f));
	//旋转
	//modelMat = glm::rotate(modelMat, (float)glfwGetTime()*5.0f, glm::vec3(0, 1.0f, 0));
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", modelMat);
	model.Draw(shader);
}

void RenderTree(Model& model, glm::mat4 modelMat,Shader& shader) {
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	modelMat = glm::translate(modelMat, glm::vec3(0.0f, 1.0f, 0.0f));
	//放缩
	modelMat = glm::scale(modelMat, glm::vec3(3.0f, 3.0f, 3.0f));
	//旋转
	//modelMat = glm::rotate(modelMat, (float)glfwGetTime()*5.0f, glm::vec3(0, 1.0f, 0));
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", modelMat);
	model.Draw(shader);
}

void RenderCar(Model& model,Shader& shader) {
	glm::mat4 viewMat = camera.GetViewMatrix();
	shader.setMat4("view", viewMat);
	glm::mat4 projMat = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	shader.setMat4("projection", projMat);
	glm::mat4 modelMat = glm::mat4(1.0f);

	modelMat = glm::translate(modelMat, car.getMidPosition());
	modelMat = glm::translate(modelMat, glm::vec3(0 , -3.0f , 0));
	modelMat = glm::rotate(modelMat, glm::radians(car.getLastYaw() / 2), glm::vec3(0, 1.0f, 0));
	
	modelMat = glm::rotate(modelMat, glm::radians(car.getYaw() - car.getLastYaw() / 2), glm::vec3(0, 1.0f, 0));
	//抵消模型自带的旋转
	modelMat = glm::rotate(modelMat, glm::radians(-90.f), glm::vec3(0, 1.0f, 0));
	//调整大小
	//modelMat = glm::scale(modelMat, glm::vec3(1.0f, 1.0f, 1.0f));
	modelMat = glm::scale(modelMat, glm::vec3(0.004f, 0.004f, 0.004f));
	shader.setMat4("model", modelMat);
	model.Draw(shader);
}

//显示FPS
void GetFPS() {
	//将FPS打印在控制台右上角
	//POINT pt;
	//GetCursorPos(&pt);
	//COORD currentCursorPos = { pt.x,pt.y };
	//COORD showPos = { 110 ,0 };
	//HANDLE hOut;
	//hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	frames++;
	float currentTime = GetTickCount()*0.01f;
	if (currentTime - lastTime > 1.0f) {
		lastTime = currentTime;
		//SetConsoleCursorPosition(hOut,showPos);
		//printf("FPS:%d\n", FPS);
		FPS = frames;
		frames = 0;
		//SetConsoleCursorPosition(hOut, currentCursorPos);
	}
}

void DriveModeCameraFix() {
	camera.ZoomRecover();
	float angle = glm::radians(-car.getMidYaw());
	glm::mat4 rotateMatrix(
		cos(angle),		0.0,	sin(angle),		0.0,
		0.0,			1.0,		 0.0,		0.0,
		-sin(angle),	0.0,	cos(angle),		0.0,
		0.0,			0.0,		0.0,		1.0);
	glm::vec3 rotatedPosition = glm::vec3(rotateMatrix * glm::vec4(DrivaModeCamera.getPosition(), 1.0));
	//cout << rotatedPosition.x << "," << rotatedPosition.y << "," << rotatedPosition.z << endl;

	camera.FixView(rotatedPosition + car.getMidPosition(), DrivaModeCamera.getYaw() - car.getMidYaw());
}

void Prompt() {
	cout << "\t**** Race Game **** " << endl;
	cout << "\n\n";
	cout << "  Cursor:\tVision Move" << endl;
	cout << "  W:\t\tCamera Move Forward\n";
	cout << "  A:\t\tCamera Move Left\n";
	cout << "  S:\t\tCamera Move Backward\n";
	cout << "  D:\t\tCamera Move Right\n\n";
	cout << "  Left_Ctrl:\tCamera Move Down\n";
	cout << "  Space:\tCamera Move Up\n";

	cout << "  ↑:\t\tCar Move Forward\n";
	cout << "  ↓:\t\tCar Move Backward\n";
	cout << "  ←:\t\tCar Move Left\n";
	cout << "  →:\t\tCar Move Right\n\n";

	cout << "  F10:\t\tDrive mode Camera\n\n";
	cout << "\n  ESC:\t\tQuit Game";

}

void InitTreePosition() {
	for (int i = 0; i < 2000; i++) {
		glm::vec3 tmp((float)((rand() % 4001)-2000), 0, (float)((rand() % 4001)-2000));
		treePositions.push_back(tmp);
	}
	chTreePos = false;
}

/*
	vec3(x,z,y)
*/