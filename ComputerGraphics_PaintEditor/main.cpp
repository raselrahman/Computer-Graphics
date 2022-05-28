

/*
===============================================================================================
Name    : Rasel
Date    : 2021-06-12
Purpose : Simple 2D Paint Program using OpenGL/GLUT


Complex
===============================================================================================
*/

#include <gl/glut.h>
#include <iostream>
#include <cmath>
#include <string.h>
#include<time.h>
#include<vector>
#include<stdio.h>


// C++ program to find the max
// of Array using sort() in STL


using namespace std;
void printString(int x, int y, void* font, const char* str);
void translatePoint(float x, float y);
bool IsAbove(float x, float y);
void scalePoint(float x, float y);
void rotationPoint(float x, float y);
float NormX(float x);
float  NormY(float x);
float FracX(float x);
float FracY(float x);
// The following statement is to hide the console window
//#pragma comment (linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

// Maximum primitives
const int MAX = 50;
//Rotation
typedef struct {
	float x;
	float y;
} Point;


Point p1, p2, p3;
float ang = 5;
//float angEllip = 10;
bool First = FALSE;
// Window width and height
int window_w;
int window_h;
float NormCor_trans_start_x = 0, NormCor_trans_start_y = 0;


bool dragging = false, Scalling_special = false;
// Color structure
struct Color
{
	float r = 0.0;
	float g = 0.0;
	float b = 0.0;
};

// Shape structure
struct Shape
{
	string type;
	float startX, startY;
	float endX, endY;
	float pointSize;
	float lineWidth;
	bool isFilled = false;
	bool isActivated = false;
	Color color;
	float rotated = 0;
	float focusX = 0;
	float focusY = 0;
};
typedef struct {
	int shapeNo;
	Point Cordinate[10000];
} ShapeInfo;


Color color;
Shape shapeList[MAX];   // declaring primitives
ShapeInfo shapeinfo[MAX];// here we put all vertices of shapelist
vector<float> Vx[MAX];
vector<float> Vy[MAX];

int shapeCount = 0;



// Draw mode enum
enum DRAW_MODE   //8 modes
{
	NONE, POINT_MODE, LINE_MODE,
	W_TRIANGLE_MODE, F_TRIANGLE_MODE, W_RECTANGLE_MODE, F_RECTANGLE_MODE, W_CIRCLE_MODE, F_CIRCLE_MODE
};



bool TRANSLATION = FALSE, ROTATION = FALSE, SCALING = FALSE;


// Right-click menu enum
enum RIGHT_CLICK_MENU
{
	CLEAR, EXIT, Undo
};

// Color code enum
enum COLOR_CODE
{
	RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE, BLACK
};

DRAW_MODE mode = NONE;

float point_size = 1.0;
float line_width = 1.0;

const float PI = 3.142;

// Handle start drawing process
void handleStartDraw(float x, float y)
{
	if (mode == NONE)
	{
		return;
	}
	else
	{
		// Start drawing
		dragging = true;

		// Active current shape
		shapeList[shapeCount].isActivated = true;

		if (mode == POINT_MODE) // Point mode  //Ellipse
		{
			shapeList[shapeCount].type = "Point";
		}
		if (mode == LINE_MODE) // Line mode
		{
			shapeList[shapeCount].type = "Line";
		}
		if (mode == W_TRIANGLE_MODE || mode == F_TRIANGLE_MODE) // Triangle mode
		{
			if (mode == F_TRIANGLE_MODE) // Filled triangle(Bresenham line)
			{
				shapeList[shapeCount].isFilled = true;
			}
			else // Wireframe triangle
			{
				shapeList[shapeCount].isFilled = false;
			}

			shapeList[shapeCount].type = "Triangle";
		}
		if (mode == W_RECTANGLE_MODE || mode == F_RECTANGLE_MODE) // Rectangle mode
		{
			if (mode == F_RECTANGLE_MODE) // Filled rectangle
			{
				shapeList[shapeCount].isFilled = true;
			}
			else // Wireframe triangle
			{
				shapeList[shapeCount].isFilled = false;
			}

			shapeList[shapeCount].type = "Rectangle";
		}
		if (mode == W_CIRCLE_MODE || mode == F_CIRCLE_MODE) // Circle mode
		{
			if (mode == F_CIRCLE_MODE) // Filled circle
			{
				shapeList[shapeCount].isFilled = true;
			}
			else // Wireframe circle
			{
				shapeList[shapeCount].isFilled = false;
			}

			shapeList[shapeCount].type = "Circle";
		}

		// Set shape coordinates
		shapeList[shapeCount].startX = x;
		shapeList[shapeCount].startY = y;
		shapeList[shapeCount].endX = x;
		shapeList[shapeCount].endY = y;

		// Set shape point size and line width
		shapeList[shapeCount].pointSize = point_size;
		shapeList[shapeCount].lineWidth = line_width;

		// Set shape color
		shapeList[shapeCount].color.r = color.r;
		shapeList[shapeCount].color.g = color.g;
		shapeList[shapeCount].color.b = color.b;

		shapeCount++;
	}
}

// Handle continue drawing process

void handleContinueDraw(float x, float y)
{

	// Get current position of end X and Y
	int current = shapeCount - 1;

	if (!dragging)
	{
		return;
	}


	if (shapeCount >= 0 && (TRANSLATION == TRUE || ROTATION == TRUE || SCALING == TRUE))
	{
		if (TRANSLATION == TRUE)
		{
			translatePoint(x, y);
			printf("Trans---\n");
		}
		else if (SCALING == TRUE)
		{
			scalePoint(x, y);
			printf("scale---\n");
		}
		else if (ROTATION == TRUE)
		{
			printf("acess\n");
			rotationPoint(x, y);
		}
	}

	else
	{

		shapeList[current].endX = x;
		shapeList[current].endY = y;
	}


	glutPostRedisplay();
}

// Handle finish drawing process
void handleFinishDraw(float x, float y)
{
	if (!dragging)
	{
		return;
	}

	int current = shapeCount - 1;
	//if (ROTATION == TRUE && shapeList[current].type=="Point")
	//{

		//shapeList[current].rotated = angEllip;
	//}
	// Finish drawing
	dragging = false;
	if (Scalling_special)
	{
		TRANSLATION = FALSE;
		SCALING = TRUE;
		ROTATION = FALSE;
	}


	// Get current position of all coordinates



	if (!(shapeCount > 0 && (TRANSLATION == TRUE || ROTATION == TRUE || SCALING == TRUE)))
	{
		if (shapeList[current].startX == shapeList[current].endX && shapeList[current].startX == shapeList[current].endY)
		{
			shapeCount--;
		}
	}


	glutPostRedisplay();
}

// Clear all primivites on drawing area
void clearPrimitives()
{
	// Reset shape count to zero
	shapeCount = 0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();
	glutPostRedisplay();
}

// Color menu
void colorMenu(int code)
{
	if (code == RED) // Red
	{
		color.r = 1.0;
		color.g = 0.0;
		color.b = 0.0;
	}
	else if (code == GREEN) // Green
	{
		color.r = 0.0;
		color.g = 1.0;
		color.b = 0.0;
	}
	else if (code == BLUE) // Blue
	{
		color.r = 0.0;
		color.g = 0.0;
		color.b = 1.0;
	}
	else if (code == CYAN) // Cyan
	{
		color.r = 0.0;
		color.g = 1.0;
		color.b = 1.0;
	}
	else if (code == MAGENTA) // Magenta
	{
		color.r = 1.0;
		color.g = 0.0;
		color.b = 1.0;
	}
	else if (code == YELLOW) // Yellow
	{
		color.r = 1.0;
		color.g = 1.0;
		color.b = 0.0;
	}
	else if (code == WHITE) // White
	{
		color.r = 1.0;
		color.g = 1.0;
		color.b = 1.0;
	}
	else if (code == BLACK) // Black
	{
		color.r = 0.0;
		color.g = 0.0;
		color.b = 0.0;
	}

	int current = shapeCount - 1;

	// Change color of current shape
	if (shapeList[current].isActivated)
	{
		shapeList[current].color.r = color.r;
		shapeList[current].color.g = color.g;
		shapeList[current].color.b = color.b;
	}

	glutPostRedisplay();
}

// Point size menu
void pointSizeMenu(int size)
{
	if (size == 1.0) // 1.0
	{
		point_size = 1.0;
	}
	else if (size == 2.0) // 2.0
	{
		point_size = 2.0;
	}
	else if (size == 3.0) // 3.0
	{
		point_size = 3.0;
	}
	else if (size == 4.0) // 4.0
	{
		point_size = 4.0;
	}
	else if (size == 5.0) // 5.0
	{
		point_size = 5.0;
	}
	else if (size == 6.0) // 6.0
	{
		point_size = 6.0;
	}
}

// Line width menu
void lineWidthMenu(int width)
{
	if (width == 1.0) // 1.0
	{
		line_width = 1.0;
	}
	else if (width == 2.0) // 2.0
	{
		line_width = 2.0;
	}
	else if (width == 3.0) // 3.0
	{
		line_width = 3.0;
	}
	else if (width == 4.0) // 4.0
	{
		line_width = 4.0;
	}
	else if (width == 5.0) // 5.0
	{
		line_width = 5.0;
	}
	else if (width == 6.0) // 6.0
	{
		line_width = 6.0;
	}

	int current = shapeCount - 1;

	// Change line width of current shape
	if (shapeList[current].isActivated)
	{
		shapeList[current].lineWidth = line_width;
	}

	glutPostRedisplay();
}

// Line width menu
void TransformationType(int type)
{

	if (type == 1.0) // 1.0 translation
	{
		TRANSLATION = TRUE;
		SCALING = FALSE;
		ROTATION = FALSE;

	}
	else if (type == 2.0) // 2.0 scalling
	{
		SCALING = TRUE;
		TRANSLATION = FALSE;
		ROTATION = FALSE;
	}
	else if (type == 3.0) // 3.0 rotation
	{
		ROTATION = TRUE;
		SCALING = FALSE;
		TRANSLATION = FALSE;

		int current = shapeCount - 1;
		float shape_start_x = NormX(shapeList[current].startX);
		float shape_start_y = NormY(shapeList[current].startY);
		float shape_end_x = NormX(shapeList[current].endX);
		float shape_end_y = NormY(shapeList[current].endY);
		//printf("shape x:%f  shape y:%f\n", shape_start_x, shape_start_y);

		if (shapeList[current].type == "Point")  //Ellipse mainly 
		{
			//p1.x = (shape_end_x);
			//p1.y = (shape_end_y);
			//shapeList[current].rotated += 75;
			shapeList[current].focusX = (shape_end_x);
			shapeList[current].focusY = (shape_end_y);
			//angEllip = shapeList[current].rotated;

		}
		else if (shapeList[current].type == "Line" || shapeList[current].type == "Triangle" && shapeList[current].isFilled == TRUE)  //Line

		{
			shapeList[current].focusX = (shape_start_x + shape_end_x) / 2;
			shapeList[current].focusY = (shape_start_y + shape_end_y) / 2;

			p1.x = shapeList[current].focusX;
			p1.y = shapeList[current].focusY;
			//p1.x = (shape_start_x + shape_end_x) / 2;
			//p1.y = (shape_start_y + shape_end_y) / 2;



		}

		p2.x = shape_start_x;
		p2.y = shape_start_y;
		p3.x = shape_end_x;
		p3.y = shape_end_y;



	}
	else if (type == 4.0)
	{
		TRANSLATION = FALSE;
		SCALING = FALSE;
		ROTATION = FALSE;

	}

}

// Right-click menu
void rightClickMenu(int option)
{
	switch (option)
	{
	case CLEAR:	// Clear	
		clearPrimitives();
		Scalling_special = FALSE;
		break;

	case EXIT: // Exit
		exit(0);

	case Undo:	// Clear	
		if (shapeCount > 0)
		{
			shapeCount--;
		}

		break;
	}
}

// Create right-click mouse menu
void createMouseMenu()
{
	int c_menu, ps_menu, lw_menu, tf_menu;

	// Create color sub-menu
	c_menu = glutCreateMenu(colorMenu);
	glutAddMenuEntry("Red", RED);
	glutAddMenuEntry("Green", GREEN);
	glutAddMenuEntry("Blue", BLUE);
	glutAddMenuEntry("Cyan", CYAN);
	glutAddMenuEntry("Magenta", MAGENTA);
	glutAddMenuEntry("Yellow", YELLOW);
	glutAddMenuEntry("White", WHITE);
	glutAddMenuEntry("Black", BLACK);

	// Create point size sub-menu
	ps_menu = glutCreateMenu(pointSizeMenu);
	glutAddMenuEntry("1.0", 1.0);
	glutAddMenuEntry("2.0", 2.0);
	glutAddMenuEntry("3.0", 3.0);
	glutAddMenuEntry("4.0", 4.0);
	glutAddMenuEntry("5.0", 5.0);
	glutAddMenuEntry("6.0", 6.0);

	// Create point size sub-menu
	lw_menu = glutCreateMenu(lineWidthMenu);
	glutAddMenuEntry("1.0", 1.0);
	glutAddMenuEntry("2.0", 2.0);
	glutAddMenuEntry("3.0", 3.0);
	glutAddMenuEntry("4.0", 4.0);
	glutAddMenuEntry("5.0", 5.0);
	glutAddMenuEntry("6.0", 6.0);

	// Create point size sub-menu
	tf_menu = glutCreateMenu(TransformationType);
	glutAddMenuEntry("Translation", 1.0);
	glutAddMenuEntry("Scaling", 2.0);
	glutAddMenuEntry("Rotation", 3.0);
	glutAddMenuEntry("Nothing", 4.0);



	// Create main menu
	glutCreateMenu(rightClickMenu);
	glutAddSubMenu("Colors", c_menu);
	glutAddSubMenu("Point size", ps_menu);
	glutAddSubMenu("Line width", lw_menu);
	glutAddSubMenu("Transformation", tf_menu);
	glutAddMenuEntry("Clear", CLEAR);
	glutAddMenuEntry("Exit", EXIT);
	glutAddMenuEntry("Undo", Undo);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

}

// Select drawing mode from mouse click
void selectMode(int x, int y, int modifiers)
{
	if (y > window_h - 50) // Point mode
	{
		mode = POINT_MODE;
	}
	else if (y > window_h - 100) // Line mode
	{
		mode = LINE_MODE;
	}
	else if (y > window_h - 150) // Wireframe triangle mode
	{
		mode = W_TRIANGLE_MODE;
	}
	else if (y > window_h - 200) // Filled triangle mode
	{
		mode = F_TRIANGLE_MODE;
	}
	else if (y > window_h - 250) // Wireframe rectangle mode
	{
		mode = W_RECTANGLE_MODE;
	}
	else if (y > window_h - 300) // Filled rectangle mode
	{
		mode = F_RECTANGLE_MODE;
	}
	else if (y > window_h - 350) // Wireframe circle mode
	{
		mode = W_CIRCLE_MODE;
	}
	else if (y > window_h - 400) // Filled circle mode
	{
		mode = F_CIRCLE_MODE;
	}
}

// Draw outline around selected mode
void drawSelected()
{
	if (mode == NONE)
	{
		return;
	}
	if (mode == POINT_MODE) // Point mode
	{
		printString(15, window_h - 46, GLUT_BITMAP_TIMES_ROMAN_10, "Ellipse");
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINE_LOOP);
		glVertex2i(2, window_h - 2);
		glVertex2i(49, window_h - 2);
		glVertex2i(49, window_h - 49);
		glVertex2i(2, window_h - 49);
		glEnd();
	}
	else if (mode == LINE_MODE) // Line mode
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINE_LOOP);
		glVertex2i(2, window_h - 51);
		glVertex2i(49, window_h - 51);
		glVertex2i(49, window_h - 99);
		glVertex2i(2, window_h - 99);
		glEnd();
	}
	else if (mode == W_TRIANGLE_MODE) // Wireframe triangle mode
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINE_LOOP);
		glVertex2i(2, window_h - 101);
		glVertex2i(49, window_h - 101);
		glVertex2i(49, window_h - 149);
		glVertex2i(2, window_h - 149);
		glEnd();
	}
	else if (mode == F_TRIANGLE_MODE) // Filled triangle mode
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINE_LOOP);
		glVertex2i(2, window_h - 151);
		glVertex2i(49, window_h - 151);
		glVertex2i(49, window_h - 199);
		glVertex2i(2, window_h - 199);
		glEnd();
	}
	else if (mode == W_RECTANGLE_MODE) // Wireframe rectangle mode
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINE_LOOP);
		glVertex2i(2, window_h - 201);
		glVertex2i(49, window_h - 201);
		glVertex2i(49, window_h - 249);
		glVertex2i(2, window_h - 249);
		glEnd();
	}
	else if (mode == F_RECTANGLE_MODE) // Filled rectangle mode
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINE_LOOP);
		glVertex2i(2, window_h - 251);
		glVertex2i(49, window_h - 251);
		glVertex2i(49, window_h - 299);
		glVertex2i(2, window_h - 299);
		glEnd();
	}
	else if (mode == W_CIRCLE_MODE) // Wireframe circle mode
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINE_LOOP);
		glVertex2i(2, window_h - 301);
		glVertex2i(49, window_h - 301);
		glVertex2i(49, window_h - 349);
		glVertex2i(2, window_h - 349);
		glEnd();
	}
	else if (mode == F_CIRCLE_MODE) // Filled circle mode
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINE_LOOP);
		glVertex2i(2, window_h - 351);
		glVertex2i(49, window_h - 351);
		glVertex2i(49, window_h - 399);
		glVertex2i(2, window_h - 399);
		glEnd();
	}
}

void printString(int x, int y, void* font, const char* str)
{
	char* c;
	glColor3f(0.0f, 0.0f, 0.0f);
	glRasterPos2i(x, y);
	for (int i = 0; i < strlen(str); i++) {
		glutBitmapCharacter(font, str[i]);
	}
	glColor3f(1.0f, 1.0f, 1.0f);
}

// Draw menu icon
void drawMenuIcon()
{

	/*
	// Draw point icon
	glColor3f(1.0, 0.0, 0.0);
	glPointSize(1);
	glBegin(GL_POINTS);
	glVertex2i(26, window_h - 14);
	glVertex2i(25, window_h - 15);
	glVertex2i(25, window_h - 25);//
	glVertex2i(25, window_h - 35);


	glVertex2i(25, window_h - 25);
	glEnd();
	*/
	float cx = 25, cy = window_h - 25, rx = 15, ry = 10;
	int num_segments = 50;
	float theta = 2 * 3.1415926 / float(num_segments);
	float c = cosf(theta);//precalculate the sine and cosine
	float s = sinf(theta);
	float t;

	float x = 1;//we start at angle = 0 
	float y = 0;

	glBegin(GL_LINE_LOOP);
	for (int ii = 0; ii < num_segments; ii++)
	{
		//apply radius and offset
		glVertex2i(x * rx + cx, y * ry + cy);//output vertex 

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}
	glEnd();



	// Draw line icon
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2i(10, window_h - 90);
	glVertex2i(40, window_h - 60);
	glEnd();

	// Draw wireframe triangle icon
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex2i(25, window_h - 110);
	glVertex2i(10, window_h - 135);
	glVertex2i(40, window_h - 135);
	glEnd();

	// Draw filled triangle icon(Draw Bresenham Actually))
	/*
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
	glVertex2i(25, window_h - 160);
	glVertex2i(10, window_h - 185);
	glVertex2i(40, window_h - 185);
	glEnd();
	*/
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2i(10, window_h - 160);
	//glVertex2i(10, window_h - 185);
	glVertex2i(45, window_h - 185);
	glEnd();

	// Draw wireframe rectangle icon
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex2i(10, window_h - 210);
	glVertex2i(40, window_h - 210);
	glVertex2i(40, window_h - 240);
	glVertex2i(10, window_h - 240);
	glEnd();

	// Draw filled rectangle icon
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glVertex2i(10, window_h - 260);
	glVertex2i(40, window_h - 260);
	glVertex2i(40, window_h - 290);
	glVertex2i(10, window_h - 290);
	glEnd();

	int segment = 300;
	float twoPI = PI * 2.0;
	float radius = 15.0;

	// Draw wireframe circle icon
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);

	for (int i = 0; i < segment; i++)
	{
		glVertex2f(25 + (radius * cos(i * twoPI / segment)),
			(window_h - 325) + (radius * sin(i * twoPI / segment)));
	}

	glEnd();

	// Draw filled circle icon
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_TRIANGLE_FAN);

	for (int i = 0; i < segment; i++)
	{
		glVertex2f(25 + (radius * cos(i * twoPI / segment)),
			(window_h - 375) + (radius * sin(i * twoPI / segment)));
	}

	glEnd();

	// Draw color status box
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_QUADS);
	glVertex2i(2, window_h - 401);
	glVertex2i(49, window_h - 401);
	glVertex2i(49, window_h - 449);
	glVertex2i(2, window_h - 449);
	glEnd();
}

// Draw paint menu
void drawPaintMenu()
{
	// Draw menu bar
	glColor3f(0.4, 0.4, 0.4);
	glRectf(0, 0, 50, window_h);

	glColor3f(0.0, 0.0, 0.0);
	glLineWidth(2.0);
	glBegin(GL_LINES);

	// Draw bottom line
	glVertex2i(1, 1);
	glVertex2i(50, 1);

	// Draw left line
	glVertex2i(1, 1);
	glVertex2i(1, window_h);

	// Draw right line
	glVertex2i(50, 1);
	glVertex2i(50, window_h);

	// Draw top line
	glVertex2i(1, window_h - 1);
	glVertex2i(50, window_h - 1);

	int distance = 50;

	// Draw 9 lines for each icon
	for (int i = 1; i <= 9; i++)
	{
		glVertex2i(1, window_h - distance);
		glVertex2i(50, window_h - distance);

		distance = distance + 50;
	}

	glEnd();
}

// Draw menu interface
void drawInterface()
{
	// Draw paint menu
	drawPaintMenu();

	// Draw outline around selected mode
	drawSelected();

	// Draw menu icon
	drawMenuIcon();

	glutPostRedisplay();
}
float NormX(float x)
{
	x = x * (window_w - 50) + 50;
	return x;
}

float NormY(float y)
{
	y = (float)(window_h - y * window_h);
	return y;


}
float FracX(float x)
{
	x = (float)(x - 50) / (window_w - 50);
	return x;
}

float FracY(float y)
{

	y = (float)(window_h - y) / window_h;
	return y;
}

float X(float ang, float h, float k, float x, float y)
{
	//ang = (ang * 3.14) / 180.0;
	//float result =cos(ang) * x - sin(ang) * y + (-h * cos(ang) + k * sin(ang) + h);
	//ang = ang * 180 / 3.14;
	//return result;
	ang = (ang * 3.14) / 180.0;                 //angle in radians  
	float  result = 0;
	result = x * cos(ang) - y * sin(ang) + (h - cos(ang) * h + sin(ang) * k);
	ang = ang * 180 / 3.14;
	return result;

}
float Y(float ang, float h, float k, float x, float y)
{
	printf("angle\n");
	//ang = (ang * 3.14) / 180.0;
	//float result= sin(ang) * x + cos(ang) * y + (-h * sin(ang) - k * cos(ang) + k);  
	//ang = ang * 180 / 3.14;
	//return result;
	ang = (ang * 3.14) / 180.0;                 //angle in radians  
	float result = 0;
	result = x * sin(ang) + y * cos(ang) + (k - sin(ang) * h - cos(ang) * k);
	ang = ang * 180 / 3.14;
	return result;
}

Point rotate(Point p, Point c, float angle) {
	ang = (ang * 3.14) / 180.0;                 //angle in radians  
	Point ptemp;  ptemp.x = 0; ptemp.y = 0;
	ptemp.x = p.x * cos(ang) - p.y * sin(ang) + (c.x - cos(ang) * c.x + sin(ang) * c.y);
	ptemp.y = p.x * sin(ang) + p.y * cos(ang) + (c.y - sin(ang) * c.x - cos(ang) * c.y);
	ang = ang * 180 / 3.14;
	return ptemp;
}
Point rotateEllip(Point p, Point c, float ang) {
	ang = (ang * 3.14) / 180.0;                 //angle in radians  
	Point ptemp;  ptemp.x = 0; ptemp.y = 0;
	ptemp.x = p.x * cos(ang) - p.y * sin(ang) + (c.x - cos(ang) * c.x + sin(ang) * c.y);
	ptemp.y = p.x * sin(ang) + p.y * cos(ang) + (c.y - sin(ang) * c.x - cos(ang) * c.y);
	ang = ang * 180 / 3.14;
	return ptemp;
}

void rotationPoint(float x, float y)
{
	int current = shapeCount - 1;


	if (shapeList[current].type == "Line" || shapeList[current].type == "Triangle" && shapeList[current].isFilled == TRUE)

	{

		int current = shapeCount - 1;
		p1.x = shapeList[current].focusX;
		p1.y = shapeList[current].focusY;

		float shape_start_x = NormX(shapeList[current].startX);
		float shape_start_y = NormY(shapeList[current].startY);
		float shape_end_x = NormX(shapeList[current].endX);
		float shape_end_y = NormY(shapeList[current].endY);
		p3.x = shape_start_x;
		p3.y = shape_start_y;

		p2.x = shape_end_x;
		p2.y = shape_end_y;


		p3 = rotate(p3, p1, ang);
		p2 = rotate(p2, p1, ang);
		printf("p1:%f  %f  ; p2:%f  %f;  p3:%f   %f\n", p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);

		shapeList[current].startX = FracX(p2.x);
		shapeList[current].startY = FracY(p2.y);
		shapeList[current].endX = FracX(p3.x);
		shapeList[current].endY = FracY(p3.y);
	}
	else
	{
		int current = shapeCount - 1;
		if (ROTATION == TRUE && dragging == TRUE)
		{
			//angEllip += 1;
			//printf("Angle:%f\n", angEllip);

			shapeList[current].rotated += 1;
			//printf("%d  %d\n", shapeCount, i + 1);
		}


	}

}
// this function will translate the point
void translatePoint(float x, float y)
{
	if (IsAbove(x, y))
	{
		glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
	}

	int current = shapeCount - 1;

	float NormCor_trans_last_x = NormX(x);
	float NormCor_trans_last_y = NormY(y);


	float Tx = NormCor_trans_last_x - NormCor_trans_start_x;
	float Ty = NormCor_trans_last_y - NormCor_trans_start_y;

	NormCor_trans_start_x = NormCor_trans_last_x;
	NormCor_trans_start_y = NormCor_trans_last_y;




	float NormCor_shape_start_x = NormX(shapeList[current].startX);
	NormCor_shape_start_x += Tx;
	shapeList[current].startX = FracX(NormCor_shape_start_x);

	float NormCor_shape_start_y = NormY(shapeList[current].startY);
	NormCor_shape_start_y += Ty;
	shapeList[current].startY = FracY(NormCor_shape_start_y);


	float NormCor_shape_last_x = NormX(shapeList[current].endX);
	NormCor_shape_last_x += Tx;
	shapeList[current].endX = FracX(NormCor_shape_last_x);


	float NormCor_shape_last_y = NormY(shapeList[current].endY);
	NormCor_shape_last_y += Ty;
	shapeList[current].endY = FracY(NormCor_shape_last_y);


	//tsx = t1x;
	//tsy = t1y;

	//printf("X:%f  Y:%f\n", Tx, Ty);
	//printf("X:%f Y:%f\n", shapeList[current].startX, shapeList[current].startY);


}





bool IsAbove(float x, float y)
{
	int current;

	current = shapeCount - 1;
	if (shapeList[current].type == "Line" || shapeList[current].type == "Triangle" && shapeList[current].isFilled == TRUE)
	{


		float cursor_x;
		float cursor_y;
		cursor_x = NormX(x);
		cursor_y = NormY(y);
		float shape_start_x = NormX(shapeList[current].startX);
		float shape_start_y = NormY(shapeList[current].startY);
		float shape_end_x = NormX(shapeList[current].endX);
		float shape_end_y = NormY(shapeList[current].endY);
		float left_side = ((cursor_x - shape_start_x) / (shape_start_x - shape_end_x));
		float right_side = ((cursor_y - shape_start_y) / (shape_start_y - shape_end_y));
		//printf("left:%d  right:%d\n", left_side, right_side);
		float d = (right_side - left_side);

		if (d > -.04 && d < .04)
		{
			return TRUE;
			printf("%f\n", right_side - left_side);
		}
		else
		{
			return FALSE;
		}


		return TRUE;
	}


}

// this function draws
void scalePoint(float x, float y)
{

	int current = shapeCount - 1;
	float NormCor_trans_last_x = NormX(x);
	float NormCor_trans_last_y = NormY(y);

	float NormCor_shape_start_x = NormX(shapeList[current].startX);
	float NormCor_shape_start_y = NormY(shapeList[current].startY);
	float NormCor_shape_last_x = NormX(shapeList[current].endX);
	float NormCor_shape_last_y = NormY(shapeList[current].endY);


	float a = abs(NormCor_shape_start_x - NormCor_shape_last_x);
	float b = abs(NormCor_shape_start_y - NormCor_shape_last_y);
	float C1 = NormCor_shape_last_x, C2 = NormCor_shape_last_y;

	if (shapeList[current].type == "Line" || shapeList[current].type == "Triangle" && shapeList[current].isFilled == TRUE)
	{
		float S1 = sqrt(pow((NormCor_trans_start_x - NormCor_shape_start_x), 2) + pow((NormCor_trans_start_y - NormCor_shape_start_y), 2));
		float S2 = sqrt(pow((NormCor_trans_last_x - NormCor_shape_last_x), 2) + pow((NormCor_trans_last_y - NormCor_shape_last_y), 2));

		printf("s1:%f  s2:%f\n", S1, S2);

		if (S1 < S2) // clicked on first point 
		{

			if (S1 >= 0.0 && S1 <= 4)
			{

				std::swap(shapeList[current].startX, shapeList[current].endX);
				std::swap(shapeList[current].startY, shapeList[current].endY);

				TRANSLATION = FALSE;
				SCALING = FALSE;
				ROTATION = FALSE;
				handleContinueDraw(x, y);
				TRANSLATION = FALSE;
				SCALING = TRUE;
				ROTATION = FALSE;
			}

		}
		else
		{

			if (S2 >= 0.00 && S2 <= 4)
			{

				TRANSLATION = FALSE;
				SCALING = FALSE;
				ROTATION = FALSE;
				handleContinueDraw(x, y);
				Scalling_special = TRUE;
			}


		}
	}
	else if (shapeList[current].type == "Circle")
	{
		float r = sqrt(pow((NormCor_shape_start_x - NormCor_shape_last_x), 2) + pow((NormCor_shape_start_y - NormCor_shape_last_y), 2));
		float d = pow(r, 2) - (pow((NormCor_shape_start_x - NormCor_trans_start_x), 2) + pow((NormCor_shape_start_y - NormCor_trans_start_y), 2));
		float v = abs(sqrt(d));
		printf("%f\n", (sqrt(v)));
		if (v >= 0 && v < 30) //cursor on the boundary line 
		{
			TRANSLATION = FALSE;
			SCALING = FALSE;
			ROTATION = FALSE;
			handleContinueDraw(x, y);
			printf("I am here \n");
			Scalling_special = TRUE;
		}



	}
	else if (shapeList[current].type == "Point")//ellips
	{

		printf("hiiiii\n");
		float x_pos, x_neg, y_pos, y_neg;
		float x_diff1 = (NormCor_shape_last_x + a) - NormCor_trans_start_x;
		float y_diff = abs(NormCor_shape_last_y - NormCor_trans_start_y);
		float x_diff2 = (NormCor_shape_last_x - a) - NormCor_trans_start_x;


		if (abs(x_diff1) >= 0 && abs(x_diff1) <= 20 && y_diff >= 0 && y_diff <= 20)
		{
			TRANSLATION = FALSE;
			SCALING = TRUE;
			ROTATION = FALSE;

			shapeList[current].endX = FracX(NormCor_trans_start_x - a);
			NormCor_trans_start_x = (NormCor_trans_last_x);



			//handleContinueDraw(FracX(NormCor_trans_last_x-a), FracY(NormCor_trans_last_y));
			printf("I am here \n");
			//Scalling_special = TRUE;
		}

		if (abs(x_diff2) >= 0 && abs(x_diff2) <= 20 && y_diff >= 0 && y_diff <= 20)
		{
			TRANSLATION = FALSE;
			SCALING = TRUE;
			ROTATION = FALSE;

			shapeList[current].startX = FracX(NormCor_trans_start_x);
			NormCor_trans_start_x = (NormCor_trans_last_x);



			//handleContinueDraw(FracX(NormCor_trans_last_x-a), FracY(NormCor_trans_last_y));
			printf("I am here \n");
			//Scalling_special = TRUE;
		}
	}

}




void MidpointEllips(float xs, float ys, float xe, float ye, int i)
{

	if (Vx[i].size() > 0)
	{

		Vx[i].clear();
		Vy[i].clear();
	}
	glLineWidth(shapeList[i].lineWidth);
	xs = xs * (window_w - 50) + 50;
	xe = xe * (window_w - 50) + 50;

	ys = (float)(window_h - ys * window_h);
	ye = (float)(window_h - ye * window_h);

	float a = abs(xs - xe);
	float b = abs(ys - ye);
	float C1 = xe, C2 = ye;


	int x = 0, y = b; //start point

	int aa = a * a, bb = b * b, aa2 = aa * 2, bb2 = bb * 2;
	int fx = 0, fy = aa2 * b;
	int p = bb - aa * b + (int)0.25 * aa;

	while (fx < fy) //slope<1
	{


		float C1_plus_x = (float)(C1 + x - 50) / (window_w - 50);
		float	C1_minus_x = (float)(C1 - x - 50) / (window_w - 50);


		float C2_plus_y = (float)(window_h - (C2 + y)) / window_h;
		float C2_minus_y = (float)(window_h - (C2 - y)) / window_h;

		//glVertex2f(C1_plus_x, C2_plus_y);
		//glVertex2f(C1_plus_x, C2_minus_y);
		//glVertex2f(C1_minus_x, C2_plus_y);
		//glVertex2f(C1_minus_x, C2_minus_y);
		Vx[i].push_back(C1_plus_x);
		Vy[i].push_back(C2_plus_y);

		Vx[i].push_back(C1_plus_x);
		Vy[i].push_back(C2_minus_y);


		Vx[i].push_back(C1_minus_x);
		Vy[i].push_back(C2_plus_y);

		Vx[i].push_back(C1_minus_x);
		Vy[i].push_back(C2_minus_y);


		x++;
		fx = fx + bb2;
		if (p < 0)
		{
			p = p + fx + bb;

		}
		else {
			y--;
			fy = fy - aa2;
			p = p + fx + bb - fy;
		}
	}
	float C1_plus_x = (float)(C1 + x - 50) / (window_w - 50);
	float C2_plus_y = (float)(C2 + y - 50) / (window_w - 50);
	//glVertex2f(C1_plus_x, C2_plus_y);
	Vx[i].push_back(C1_plus_x);
	Vy[i].push_back(C2_plus_y);

	p = bb * (int)(x + 0.5) * (int)(x + 0.5) + aa * (y - 1) * (y - 1) - aa * bb;
	while (y > 0)
	{
		y--;
		fy = fy - aa2;
		if (p >= 0)
		{
			p = p - fy + aa;

		}
		else {
			x++;
			fx = fx + bb2;
			p = p + fx - fy + aa;
		}

		float C1_plus_x = (float)(C1 + x - 50) / (window_w - 50);
		float	C1_minus_x = (float)(C1 - x - 50) / (window_w - 50);


		float C2_plus_y = (float)(window_h - (C2 + y)) / window_h;
		float C2_minus_y = (float)(window_h - (C2 - y)) / window_h;

		//glVertex2f(C1_plus_x, C2_plus_y);
		//glVertex2f(C1_plus_x, C2_minus_y);
		//glVertex2f(C1_minus_x, C2_plus_y);
		//glVertex2f(C1_minus_x, C2_minus_y);

		Vx[i].push_back(C1_plus_x);
		Vy[i].push_back(C2_plus_y);

		Vx[i].push_back(C1_plus_x);
		Vy[i].push_back(C2_minus_y);


		Vx[i].push_back(C1_minus_x);
		Vy[i].push_back(C2_plus_y);

		Vx[i].push_back(C1_minus_x);
		Vy[i].push_back(C2_minus_y);


	}
	int current = shapeCount - 1;
	glBegin(GL_POINTS);
	for (int l = 0;l < Vx[i].size();l++)
	{

		// shapeList[i].rotated = ang;
		p3.x = NormX(Vx[i].at(l));
		p3.y = NormY(Vy[i].at(l));

		//shapeList[i].rotated =int(shapeList[i].rotated) % 360;
		//printf("P1X: %f  P1Y:%f\n", p1.x, p1.y);
		//if (shapeList[i].isActivated == FALSE)
		//{
		p1.x = shapeList[i].focusX;
		p1.y = shapeList[i].focusY;
		//}
		if (shapeList[i].rotated != 0 && i == current)
		{
			p3 = rotateEllip(p3, p1, shapeList[i].rotated);
			//printf(" Active%d\n", i);
		}
		else if (shapeList[i].rotated != 0 && i != current)
		{
			p3 = rotateEllip(p3, p1, shapeList[i].rotated);
			//printf("NOT Active%d\n", i);
		}

		glVertex2f(FracX(p3.x), FracY(p3.y));
	}
	//printf("Obj:%d Yes angle:%f\n", i, shapeList[i].rotated);
	glEnd();



}

/*





*/


void Bresenham_circle(float xx1, float yy1, float xx2, float yy2, int i)
{
	glLineWidth(shapeList[i].lineWidth);
	float x1 = xx1 * (window_w - 50) + 50;
	float x2 = xx2 * (window_w - 50) + 50;

	float y1 = (float)(window_h - yy1 * window_h);
	float y2 = (float)(window_h - yy2 * window_h);

	int r = floor(sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
	float d = float(3 - 2 * r);
	int x = 0;
	int y = r;

	while (x <= y)
	{
		//wx = (float)(x1 - x - 50) / (window_w - 50);

		//wy = (float)(window_h - (y1 -x) )/ window_h;

		float x1_plus_x = (float)(x1 + x - 50) / (window_w - 50);
		float x1_minus_y = (float)(x1 - y - 50) / (window_w - 50);
		float x1_plus_y = (float)(x1 + y - 50) / (window_w - 50);
		float x1_minus_x = (float)(x1 - x - 50) / (window_w - 50);

		float y1_plus_y = (float)(window_h - (y1 + y)) / window_h;
		float y1_minus_x = (float)(window_h - (y1 - x)) / window_h;
		float y1_plus_x = (float)(window_h - (y1 + x)) / window_h;
		float y1_minus_y = (float)(window_h - (y1 - y)) / window_h;

		glVertex2f(x1_plus_x, y1_plus_y);
		glVertex2f(x1_minus_y, y1_minus_x);
		glVertex2f(x1_plus_y, y1_minus_x);
		glVertex2f(x1_minus_y, y1_plus_x);
		glVertex2f(x1_plus_y, y1_plus_x);
		glVertex2f(x1_minus_x, y1_minus_y);
		glVertex2f(x1_plus_x, y1_minus_y);
		glVertex2f(x1_minus_x, y1_plus_y);

		if (d <= 0)
		{
			d = d + 4 * x + 6;

		}
		else
		{
			d = d + 4 * x - 4 * y + 10;
			y--;
		}
		x++;

	}

	glEnd();

}
void CircleMidpoint(float x1, float y1, float x2, float y2, int i)
{

	glLineWidth(shapeList[i].lineWidth);
	x1 = NormX(x1);
	x2 = NormX(x2);
	y1 = NormY(y1);
	y2 = NormY(y2);




	int r = floor(sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));


	float d = floor(1 - r);
	int x = 0;
	int y = r;
	while (x <= y)
	{


		float x1_plus_x = (float)(x1 + x - 50) / (window_w - 50);
		float x1_minus_y = (float)(x1 - y - 50) / (window_w - 50);
		float x1_plus_y = (float)(x1 + y - 50) / (window_w - 50);
		float x1_minus_x = (float)(x1 - x - 50) / (window_w - 50);

		float y1_plus_y = (float)(window_h - (y1 + y)) / window_h;
		float y1_minus_x = (float)(window_h - (y1 - x)) / window_h;
		float y1_plus_x = (float)(window_h - (y1 + x)) / window_h;
		float y1_minus_y = (float)(window_h - (y1 - y)) / window_h;
		glVertex2f(x1_plus_x, y1_plus_y);
		glVertex2f(x1_minus_y, y1_minus_x);
		glVertex2f(x1_plus_y, y1_minus_x);
		glVertex2f(x1_minus_y, y1_plus_x);
		glVertex2f(x1_plus_y, y1_plus_x);
		glVertex2f(x1_minus_x, y1_minus_y);
		glVertex2f(x1_plus_x, y1_minus_y);
		glVertex2f(x1_minus_x, y1_plus_y);


		if (d <= 0)
		{
			d = d + 2 * x + 3;

		}
		else
		{
			d = d + 2 * x - 2 * y + 6;
			y--;
		}
		x++;

	}
	glEnd();


}
void DDA_line(float xs, float ys, float xe, float ye, int i)
{

	if (shapeCount > 0 && (TRANSLATION == TRUE || ROTATION == TRUE || SCALING == TRUE))
	{

		//printf("X:%f  Y:%f\n", xs, ys);
		if (xs > 1 || ys > 1)
		{
			//printf("G-T");
		}
	}


	if (!(shapeCount > 0 && (TRANSLATION == TRUE || ROTATION == TRUE || SCALING == TRUE)))
	{

		//printf("X:%f  Y:%f\n", xs, ys);
		if (xs > 1 || ys > 1)
		{
			//printf("G-D");
		}
	}
	glLineWidth(shapeList[i].lineWidth);

	xs = xs * (window_w - 50) + 50;
	xe = xe * (window_w - 50) + 50;

	ys = (float)(window_h - ys * window_h);
	ye = (float)(window_h - ye * window_h);
	//printf("ValueX:%f  ValueY%f: \n", xs,ys);

	float dy, dx, step, x, y, k, Xin, Yin;
	dx = xe - xs;
	dy = ye - ys;

	if (abs(dx) > abs(dy))
	{
		step = abs(dx);
	}
	else
	{
		step = abs(dy);
	}


	Xin = (dx) / step;
	Yin = (dy) / step;

	x = xs;
	y = ys;
	float wx = (float)(x - 50) / (window_w - 50);
	float wy = (float)(window_h - y) / window_h;
	glBegin(GL_LINE_STRIP);
	glVertex2f(wx, wy);




	float X[1000], Y[1000];
	step = step;
	for (int k = 1; k <= int(step); k++)
	{
		x = x + Xin;
		y = y + Yin;

		//X[k] = x;
		//Y[k] = y;
		wx = (float)(x - 50) / (window_w - 50);
		wy = (float)(window_h - y) / window_h;


		glVertex2f(wx, wy);
	}
	glEnd();

}


void bresenham_line(float x1, float y1, float x2, float y2, int i)
{
	//printf("Using bresenham line");
	float x, y, dx, dy, dx1, dy1, px, py, xe, ye;

	glLineWidth(shapeList[i].lineWidth);

	x1 = NormX(x1);
	x2 = NormX(x2);

	y1 = NormY(y1);
	y2 = NormY(y2);

	dx = x2 - x1;
	dy = y2 - y1;
	dx1 = abs(dx);
	dy1 = abs(dy);
	px = 2 * dy1 - dx1;
	py = 2 * dx1 - dy1;
	glBegin(GL_LINE_STRIP);
	if (dy1 <= dx1)
	{
		if (dx >= 0)
		{
			x = x1;
			y = y1;
			xe = x2;
		}
		else
		{
			x = x2;
			y = y2;
			xe = x1;
		}

		float wx = (float)(x - 50) / (window_w - 50);
		float wy = (float)(window_h - y) / window_h;
		glVertex2f(wx, wy);

		printf(" x: %f  y:%f\n", wx, wy);
		for (int j = 0; x < xe; j++)
		{
			x = x + 1;
			if (px < 0)
			{
				px = px + 2 * dy1;
			}
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
				{
					y = y + 1;
				}
				else
				{
					y = y - 1;
				}
				px = px + 2 * (dy1 - dx1);
			}

			float wx = (float)(x - 50) / (window_w - 50);
			float wy = (float)(window_h - y) / window_h;
			glVertex2f(wx, wy);
		}
	}
	else
	{
		if (dy >= 0)
		{
			x = x1;
			y = y1;
			ye = y2;
		}
		else
		{
			x = x2;
			y = y2;
			ye = y1;
		}
		float wx = (float)(x - 50) / (window_w - 50);
		float wy = (float)(window_h - y) / window_h;
		glVertex2f(wx, wy);

		for (int j = 0; y < ye; j++)
		{
			y = y + 1;
			if (py <= 0)
			{
				py = py + 2 * dx1;
			}
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
				{
					x = x + 1;
				}
				else
				{
					x = x - 1;
				}
				py = py + 2 * (dx1 - dy1);
			}

			float wx = (float)(x - 50) / (window_w - 50);
			float wy = (float)(window_h - y) / window_h;
			glVertex2f(wx, wy);

		}
	}
	glEnd();

}


// Draw all primitives on drawing area
void drawPrimitives()
{
	for (int i = 0; i < shapeCount; i++)
	{
		// Set primitives color based on selected color
		glColor3f(shapeList[i].color.r, shapeList[i].color.g, shapeList[i].color.b);

		if (shapeList[i].type == "Point")  //Ellipse mainly 
		{
			// Draw point
			/*
			glPointSize(shapeList[i].pointSize);
			glBegin(GL_POINTS);
			glVertex2f(shapeList[i].startX, shapeList[i].startY);
			glEnd();
			*/
			MidpointEllips(shapeList[i].startX, shapeList[i].startY, shapeList[i].endX, shapeList[i].endY, i);
		}
		if (shapeList[i].type == "Line")  //Line
		{
			// Draw line
			glLineWidth(shapeList[i].lineWidth);
			/*
			 glBegin(GL_LINES);
			 glVertex2f(shapeList[i].startX, shapeList[i].startY);
			 glVertex2f(shapeList[i].endX, shapeList[i].endY);
			 glEnd();
			*/
			DDA_line(shapeList[i].startX, shapeList[i].startY, shapeList[i].endX, shapeList[i].endY, i);
		}

		if (shapeList[i].type == "Triangle") // Triangle
		{
			if (shapeList[i].isFilled) // Filled(Bresenhamline)
			{

				bresenham_line(shapeList[i].startX, shapeList[i].startY, shapeList[i].endX, shapeList[i].endY, i);
			}
			else // Wireframe
			{
				glLineWidth(shapeList[i].lineWidth);
				glBegin(GL_LINE_LOOP);
				// Draw triangle
				glVertex2f(shapeList[i].startX, shapeList[i].startY);
				glVertex2f(((shapeList[i].endX - shapeList[i].startX) / 2) +
					shapeList[i].startX, shapeList[i].endY);
				glVertex2f(shapeList[i].endX, shapeList[i].startY);
				glEnd();
			}


		}

		if (shapeList[i].type == "Rectangle") //Rectangle
		{
			if (shapeList[i].isFilled) // Filled
			{
				glBegin(GL_QUADS);
			}

			else // Wireframe
			{
				glLineWidth(shapeList[i].lineWidth);
				glBegin(GL_LINE_LOOP);
			}

			// Draw rectangle
			glVertex2f(shapeList[i].startX, shapeList[i].startY);
			glVertex2f(shapeList[i].endX, shapeList[i].startY);
			glVertex2f(shapeList[i].endX, shapeList[i].endY);
			glVertex2f(shapeList[i].startX, shapeList[i].endY);
			glEnd();
		}
		if (shapeList[i].type == "Circle")
		{
			glLineWidth(shapeList[i].lineWidth);
			glBegin(GL_POINTS);
			if (shapeList[i].isFilled) // Filled
			{
				CircleMidpoint(shapeList[i].startX, shapeList[i].startY, shapeList[i].endX, shapeList[i].endY, i);
			}
			else //( Wireframe )// Circle BresenhaM
			{
				Bresenham_circle(shapeList[i].startX, shapeList[i].startY, shapeList[i].endX, shapeList[i].endY, i);
			}




		}

		/// Create frame around primitives at the time of  drawing
		if (shapeList[i].isActivated)
		{
			glColor3f(1.0, 1.0, 1.0);
			glLineWidth(1.0);
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(1.0, 0xF0F0);
			glBegin(GL_LINE_LOOP);

			// Line frame
			if (shapeList[i].type == "Line" || (shapeList[i].type == "Triangle" && shapeList[i].isFilled == TRUE))
			{
				glVertex2f(shapeList[i].startX, shapeList[i].startY);
				glVertex2f(shapeList[i].endX, shapeList[i].endY);
			}

			// Triangle/Rectangle/Circle frame
			if (shapeList[i].type == "Triangle" && shapeList[i].isFilled == FALSE || shapeList[i].type == "Rectangle" ||
				shapeList[i].type == "Circle")
			{
				glVertex2f(shapeList[i].startX, shapeList[i].startY);
				glVertex2f(shapeList[i].endX, shapeList[i].startY);
				glVertex2f(shapeList[i].endX, shapeList[i].endY);
				glVertex2f(shapeList[i].startX, shapeList[i].endY);
			}
			if (shapeList[i].type == "Point")
			{  /*
				glVertex2f(shapeList[i].startX, shapeList[i].startY);
				glVertex2f(shapeList[i].endX, shapeList[i].startY);
				glVertex2f(shapeList[i].endX, shapeList[i].endY);
				glVertex2f(shapeList[i].startX, shapeList[i].endY);
				*/
				glVertex2f(shapeList[i].startX, shapeList[i].startY);
				glVertex2f(shapeList[i].endX + (shapeList[i].endX - shapeList[i].startX), shapeList[i].startY);
				glVertex2f(shapeList[i].endX + (shapeList[i].endX - shapeList[i].startX), shapeList[i].endY - (shapeList[i].startY - shapeList[i].endY));
				glVertex2f(shapeList[i].startX, shapeList[i].endY - (shapeList[i].startY - shapeList[i].endY));
			}
			glEnd();
			glDisable(GL_LINE_STIPPLE);
		}
	}
	//printf("%d",shapeCount);
}

// Initialize menu area and drawing area
void initArea(float x1, float x2, float y1, float y2)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(x1, x2, y1, y2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Initialize OpenGL/GLUT settings
void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, window_w, 0, window_h);
}

// Display callback
void display()
{
	glClearColor(0.5, 0.5, 0.5, 0.5);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(50, 0, window_w - 50, window_h);

	// Initialize drawing area
	initArea(0, 1, 0, 1);

	drawPrimitives();

	glViewport(0, 0, 50, window_h);

	// Initialize paint menu area
	initArea(0, 51, 0, window_h);

	drawInterface();

	glutSwapBuffers();
}

// Reshape callback
void reshape(int w, int h)
{
	// Adjust clipping
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Adjust viewport and clear
	glViewport(0, 0, w, h);
	glClearColor(0.8, 0.8, 0.8, 0.8);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	display();
	glFlush();

	// Set new window width and height value
	window_w = w;
	window_h = h;
}

// Mouse click callback
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (x < 50 && state == GLUT_DOWN)
		{
			// Select drawing mode
			selectMode(x, window_h - y, glutGetModifiers());
			//Unselect Transformation mode
			TRANSLATION = FALSE;
			SCALING = FALSE;
			ROTATION = FALSE;
			Scalling_special = false;
		}

		float wx, wy;

		wx = (float)(x - 50) / (window_w - 50);
		wy = (float)(window_h - y) / window_h;


		if (shapeCount > 0 && state == GLUT_DOWN && (TRANSLATION == TRUE || ROTATION == TRUE || SCALING == TRUE))
		{
			int current = shapeCount - 1;
			for (int i = 0;i <= current;i++)
			{
				//printf("OBJ NO:%d  Angle:%f\n", i, shapeList[i].rotated);
			}


			//shapeList[shapeCount - 1].isActivated = false;
			NormCor_trans_start_x = wx * (window_w - 50) + 50;
			NormCor_trans_start_y = (float)(window_h - wy * window_h);

			dragging = TRUE;

		}


		else if (state == GLUT_DOWN && !(TRANSLATION == TRUE || ROTATION == TRUE || SCALING == TRUE))
		{
			// Deactivate previus shape
			shapeList[shapeCount - 1].isActivated = false;
			glutSetCursor(GLUT_CURSOR_INHERIT);
			// Handle start draw
			handleStartDraw(wx, wy);
		}
		else
		{


			// Handle finish draw
			handleFinishDraw(wx, wy);


		}
	}
}

// Mouse motion callback
//call dhor
void motion(int x, int y)
{
	if (dragging)
	{
		float wx, wy;

		wx = (float)(x - 50) / (window_w - 50);
		wy = (float)(window_h - y) / window_h;

		// Handle continue draw
		//glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
		//glutSetCursor(GLUT_CURSOR_INHERIT);


		handleContinueDraw(wx, wy);
		//printf("check");
	}
}
void PassiveMove(int x, int y)
{

	float window_width = glutGet(GLUT_WINDOW_WIDTH);
	float window_height = glutGet(GLUT_WINDOW_HEIGHT);

	GLbyte color[4];
	GLfloat depth;
	GLuint index;

	float wx = (float)(x - 50) / (window_w - 50);
	float wy = (float)(window_h - y) / window_h;

	//printf("x:%f->%f y:%f->%f\n", wx, NormX(wx),wy,NormY(wy));

	//printf("Clicked on pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, depth %f, stencil index %u\n",
		//x, y, color[0], color[1], color[2], color[3], depth, index);
	if (IsAbove(FracX(x), FracY(y)))
	{

		//printf("True\n");
	}

	if (x > 50)
	{
		//glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR); GLUT_CURSOR_TOP_RIGHT_CORNER GLUT_CURSOR_FULL_CROSSHAIR GLUT_CURSOR_INFO

	}
}

// Main
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600); // Set window size
	glutInitWindowPosition(150, 50); // Set window position
	glutCreateWindow("Computer Graphics Lab(A K Z Rasel Rahman)");
	init();

	// Create right-click mouse menu
	createMouseMenu();
	// Callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(PassiveMove);
	glutMainLoop();
	return 0;
}
