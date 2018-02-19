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

Point deCasteljau(std::vector<Point> &points, float t);
void drawBezier();

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

/*
 * A de Casteljau algoritmus rekurz�v implement�ci�ja.
 *
 * FIGYELEM: Teljes�tm�nykritikus alkalmaz�sokban kifejezetten ellenjavallott a rekurz�v implement�ci�
 * haszn�lata, hiszen nem biztos, hogy a ford�t� a TCO seg�ts�g�vel el tudja ker�lni a f�ggv�nyh�v�sokat.
 * Emiatt bevgrafon kiz�r�lag az iterat�v v�ltozat az elfogadott.
 *
 * Jelen esetben az�rt alkalmazzuk a rekurz�v megval�s�t�st, mert k�nnyebben olvashat� k�dot eredm�nyez.
 */
Point deCasteljau(std::vector<Point> &points, float t)
{
	/*
	 * A rekurzi� alapesete, ha csak egy pont maradt, az a t param�ter �rt�khez
	 * tartoz� g�rbepont.
	 */
	if (points.size() == 1)
	{
		return points[0];
	}
	else
	{
		/*
		 * Vegy�k �szre, hogy a ciklus az utols� elemig nem fut el, hiszen indext�ll�p�s lenne.
		 */
		for (int i = 0; i < points.size() - 1; ++i)
		{
			points[i].x = (1 - t) * points[i].x + t * points[i + 1].x;
			points[i].y = (1 - t) * points[i].y + t * points[i + 1].y;
		}

		/*
		 * Az egym�st k�vet� gener�ci�k mindig eggyel kevesebb pontot tartalmaznak,
		 * emiatt egy �j gener�ci� el��ll�t�sa ut�n a vektor m�rete eggyel cs�kkenthet�.
		 */
		points.pop_back();

		return deCasteljau(points, t);
	}
}

void drawBezier()
{
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINE_STRIP);
	for (float t = 0; t <= 1.05; t += 0.05)
	{
		/*
		 * Minden h�v�snak egy �jonnan felt�lt�tt vektort kell �tadnunk, mert
		 * az algoritmus helyben m�dos�tja a vektor elemeit.
		 */
		std::vector<Point> points;

		for (auto ptr : controlPoints)
		{
			points.push_back(*ptr);
		}

		auto p = deCasteljau(points, t);

		glVertex2d(p.x, p.y);
	}
	glEnd();
}

void gameLoop(GLFWwindow *window)
{
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		if (controlPoints.size() > 2)
		{
			drawBezier();
		}

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
