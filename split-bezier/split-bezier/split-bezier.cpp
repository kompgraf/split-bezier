#include "stdafx.h"

#include <GLFW/glfw3.h>
#include <vector>


constexpr float CLICK_DETECTION_THRESHOLD = 100.0f;
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;


struct Point
{
	double x, y;
};


Point *dragVector = nullptr;

std::vector<Point *> controlPoints;


Point* getClickedPoint(double xpos, double ypos);
void onMouseMove(GLFWwindow *window, double xpos, double ypos);
void onClick(GLFWwindow *window, int button, int action, int modifiers);

void initializeGL(GLFWwindow *window);
GLFWwindow *initializeGLFW();

void gameLoop(GLFWwindow *window);


int main(void)
{
	GLFWwindow *window = initializeGLFW();

	glfwMakeContextCurrent(window);

	initializeGL(window);

	gameLoop(window);

	glfwTerminate();

	for (auto point : controlPoints)
	{
		delete point;
	}

	return 0;
}

Point* getClickedPoint(const double xpos, const double ypos)
{
	const Point clickPosition{ xpos, ypos };

	for (auto point : controlPoints)
	{
		const float diffx = clickPosition.x - point->x;
		const float diffy = clickPosition.y - point->y;

		const float distanceSquared = diffx * diffx + diffy * diffy;

		if (distanceSquared <= CLICK_DETECTION_THRESHOLD)
		{
			return point;
		}
	}

	return nullptr;
}

void onMouseMove(GLFWwindow *window, double xpos, double ypos)
{
	if (dragVector != nullptr)
	{
		dragVector->x = xpos;
		dragVector->y = ypos;
	}
}

void onClick(GLFWwindow *window, int button, int action, int modifiers)
{
	if (button != GLFW_MOUSE_BUTTON_LEFT)
	{
		return;
	}

	if (action == GLFW_RELEASE)
	{
		dragVector = nullptr;

		return;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		double xpos, ypos;

		glfwGetCursorPos(window, &xpos, &ypos);

		dragVector = getClickedPoint(xpos, ypos);

		if (dragVector != nullptr)
		{
			return;
		}

		controlPoints.push_back(new Point{ xpos, ypos });
	}
}


void initializeGL(GLFWwindow *window)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glShadeModel(GL_FLAT);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(7.5);
	glLineWidth(1.0);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glOrtho(0.f, width, height, 0.f, 0.f, 1.f);
}

GLFWwindow *initializeGLFW()
{
	GLFWwindow* window;

	if (!glfwInit())
	{
		return nullptr;
	}

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Split Bezier", NULL, NULL);

	if (!window)
	{
		glfwTerminate();

		return nullptr;
	}

	glfwSetMouseButtonCallback(window, onClick);
	glfwSetCursorPosCallback(window, onMouseMove);

	return window;
}

void gameLoop(GLFWwindow *window)
{
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_POINTS);
		for (auto point : controlPoints)
		{
			glVertex2d(point->x, point->y);
		}
		glEnd();

		glfwSwapBuffers(window);

		glfwPollEvents();
	}
}
