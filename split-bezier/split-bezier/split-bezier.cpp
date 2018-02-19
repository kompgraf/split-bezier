#include "stdafx.h"

#include <GLFW/glfw3.h>
#include <vector>


constexpr double CLICK_DETECTION_THRESHOLD = 100.0;
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

void split(std::vector<Point> &points, std::vector<Point> &left, std::vector<Point> &right, float t);
void drawSplit();

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
		const double diffx = clickPosition.x - point->x;
		const double diffy = clickPosition.y - point->y;

		const double distanceSquared = diffx * diffx + diffy * diffy;

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
 * A de Casteljau algoritmus rekurzív implementációja.
 *
 * FIGYELEM: Teljesítménykritikus alkalmazásokban kifejezetten ellenjavallott a rekurzív implementáció
 * használata, hiszen nem biztos, hogy a fordító a TCO segítségével el tudja kerülni a függvényhívásokat.
 * Emiatt bevgrafon kizárólag az iteratív változat az elfogadott.
 *
 * Jelen esetben azért alkalmazzuk a rekurzív megvalósítást, mert könnyebben olvasható kódot eredményez.
 */
Point deCasteljau(std::vector<Point> &points, float t)
{
	/*
	 * A rekurzió alapesete, ha csak egy pont maradt, az a t paraméter értékhez
	 * tartozó görbepont.
	 */
	if (points.size() == 1)
	{
		return points[0];
	}
	else
	{
		/*
		 * Vegyük észre, hogy a ciklus az utolsó elemig nem fut el, hiszen indextúllépés lenne.
		 */
		for (size_t i = 0; i < points.size() - 1; ++i)
		{
			points[i].x = (1 - t) * points[i].x + t * points[i + 1].x;
			points[i].y = (1 - t) * points[i].y + t * points[i + 1].y;
		}

		/*
		 * Az egymást követõ generációk mindig eggyel kevesebb pontot tartalmaznak,
		 * emiatt egy új generáció elõállítása után a vektor mérete eggyel csökkenthetõ.
		 */
		points.pop_back();

		return deCasteljau(points, t);
	}
}

void drawBezier()
{
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINE_STRIP);
	for (float t = 0; t <= 1.05; t += 0.05f)
	{
		/*
		 * Minden hívásnak egy újonnan feltöltött vektort kell átadnunk, mert
		 * az algoritmus helyben módosítja a vektor elemeit.
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

/*
 * A Bezier-görbét két részre osztó algoritmus rekurzív implementációja.
 *
 * FIGYELEM: Teljesítménykritikus alkalmazásokban kifejezetten ellenjavallott a rekurzív implementáció
 * használata, hiszen nem biztos, hogy a fordító a TCO segítségével el tudja kerülni a függvényhívásokat.
 * Emiatt bevgrafon kizárólag az iteratív változat az elfogadott.
 *
 * Jelen esetben azért alkalmazzuk a rekurzív megvalósítást, mert könnyebben olvasható kódot eredményez.
 *
 * Vegyük észre, hogy minden esetben referencia szerint adunk át vektorokat, így mindvégig ugyanazon
 * vektorokkal fogunk dolgozni, nem lesz másolás.
 */
void split(std::vector<Point> &points, std::vector<Point> &left, std::vector<Point> &right, float t)
{
	if (points.size() == 1)
	{
		/*
		 * Ha már csak egy pont maradt, az jobbszélsõ és balszélsõ is egyben, így
		 * mindkét félbe belekerül.
		 */
		left.push_back(points[0]);
		right.push_back(points[0]);
	}
	else
	{
		/*
		 * A generáció elsõ eleme a bal, utolsó eleme a jobb félbe kerül.
		 */
		left.push_back(points[0]);
		right.push_back(points[points.size() - 1]);

		/*
		 * Teljesen azonos a de Casteljau algoritmusban adott kóddal.
		 */
		for (size_t i = 0; i < points.size() - 1; ++i)
		{
			points[i].x = (1 - t) * points[i].x + t * points[i + 1].x;
			points[i].y = (1 - t) * points[i].y + t * points[i + 1].y;
		}

		/*
		 * A pontok számának szigorúan monoton csökkenése garantálja, hogy a 
		 * rekurzió nem lesz végtelen.
		 */
		points.pop_back();

		split(points, left, right, t);
	}
}

void drawSplit()
{
	std::vector<Point> left, right, points;

	for (auto ptr : controlPoints)
	{
		points.push_back(*ptr);
	}

	split(points, left, right, 0.5);

	glBegin(GL_POINTS);
	glColor3f(0.0, 0.0, 1.0);
	for (auto point : left)
	{
		glVertex2d(point.x, point.y);
	}
	glColor3f(0.0, 0.0, 0.5);
	for (auto point : right)
	{
		glVertex2d(point.x, point.y);
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

			drawSplit();
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
