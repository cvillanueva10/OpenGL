#include <iostream>
#include <deque>

#if defined WIN32
#include <freeglut.h>
#elif defined __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif


using namespace std;

// Some global variables to maintain state
// Variables to store current color, initialize to black
float red = 0.0, green = 0.0, blue = 0.0;

// A point data structure
struct Point {
	// The coordinates of the point
	float x;
	float y;

	// The color of the point
	float r;
	float g;
	float b;

	//Default constructor
	Point() {
		x = 0.0;
		y = 0.0;
		r = 0.0;
		g = 0.0;
		b = 0.0;
	}
	// A constructor for point
	Point(float x, float y, float r, float g, float b) {
		this->x = x;
		this->y = y;
		this->r = r;
		this->g = g;
		this->b = b;
	}
};

//A square data structure containing 4 point variables (corners) 
struct Square {
	//Origin of square, top left
	Point topleft;
	Point topright;
	Point botleft;
	Point botright;

	//constructor for square
	Square(Point topleft, Point topright, Point botleft, Point botright){
		this->topleft = topleft;
		this->topright = topright;
		this->botleft = botleft;
		this->botright = botright;
	}

};

// A "Double Ended QUEue" to store points and squares
deque<Point> points;
deque<Square> squares;



// Store the width and height of the window
int width = 640, height = 640;

//Default size of squares
float length = (float)0.1;
float change = (float)0.0;

//-------------------------------------------------------
// A function to draw the scene
//-------------------------------------------------------
void appDrawScene() {
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set background color to white
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// Set up the transformations stack
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set the color to the current values of the global vars
	glColor3f(red, green, blue);

	// // Set point size
	glPointSize(6);

	// Draw a point at the bottom-right
	glBegin(GL_POINTS);

	glVertex2f(-0.8f, -0.8f);

	glEnd();



	// Draw all the points stored in the double-ended queue
	for (int i = 0; i < points.size(); i++) {

		// Set the vertex color to be whatever we stored in the point
		glColor3f(points[i].r, points[i].g, points[i].b);

		glBegin(GL_POINTS);

		// Draw the vertex in the right position
		glVertex2f(points[i].x, points[i].y);

		glEnd();
	}

	//Draw all the square stored in the double-ended queue using line loop
	for (int i = 0; i < squares.size(); i++) {
		glColor3f(squares[i].topleft.r, squares[i].topleft.g, squares[i].topleft.b);

		glBegin(GL_LINE_LOOP);

		glVertex2f(squares[i].topleft.x, squares[i].topleft.y);
		glVertex2f(squares[i].topright.x + change, squares[i].topright.y);
		glVertex2f(squares[i].botright.x + change, squares[i].botright.y - change);
		glVertex2f(squares[i].botleft.x, squares[i].botleft.y - change);

		glEnd();
		
	
	}

	// We have been drawing everything to the back buffer
	// Swap the buffers to see the result of what we drew
	glFlush();
	glutSwapBuffers();
}

//-------------------------------------------------------
// A function to convert window coordinates to scene
// We use it when a mouse event is handled
// Arguments: 	
//	x, y - the coordinates to be updated
//-------------------------------------------------------
void windowToScene(float& x, float& y) {
	x = (2.0f*(x / float(width))) - 1.0f;
	y = 1.0f - (2.0f*(y / float(height)));
}

//-------------------------------------------------------
// A function to handle window resizing
// Called every time the window is resized
// Arguments: 	
//	b    - mouse button that was clicked, left or right
//	s 	 - state, either mouse-up or mouse-down
//	x, y - coordinates of the mouse when click occured
//-------------------------------------------------------
void appReshapeFunc(int w, int h) {
	// Window size has changed
	width = w;
	height = h;

	double scale, center;
	double winXmin, winXmax, winYmin, winYmax;

	// Define x-axis and y-axis range
	const double appXmin = -1.0;
	const double appXmax = 1.0;
	const double appYmin = -1.0;
	const double appYmax = 1.0;

	// Define that OpenGL should use the whole window for rendering
	glViewport(0, 0, w, h);

	// Set up the projection matrix using a orthographic projection that will
	// maintain the aspect ratio of the scene no matter the aspect ratio of
	// the window, and also set the min/max coordinates to be the disered ones
	w = (w == 0) ? 1 : w;
	h = (h == 0) ? 1 : h;

	if ((appXmax - appXmin) / w < (appYmax - appYmin) / h) {
		scale = ((appYmax - appYmin) / h) / ((appXmax - appXmin) / w);
		center = (appXmax + appXmin) / 2;
		winXmin = center - (center - appXmin)*scale;
		winXmax = center + (appXmax - center)*scale;
		winYmin = appYmin;
		winYmax = appYmax;
	}
	else {
		scale = ((appXmax - appXmin) / w) / ((appYmax - appYmin) / h);
		center = (appYmax + appYmin) / 2;
		winYmin = center - (center - appYmin)*scale;
		winYmax = center + (appYmax - center)*scale;
		winXmin = appXmin;
		winXmax = appXmax;
	}

	// Now we use glOrtho to set up our viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(winXmin, winXmax, winYmin, winYmax, -1, 1);
}


//-------------------------------------------------------
// A function to handle mouse clicks
// Called every time the mouse button goes up or down
// Arguments: 	
//	b    - mouse button that was clicked, left or right
//	s 	 - state, either mouse-up or mouse-down
//	x, y - coordinates of the mouse when click occured
//-------------------------------------------------------
void appMouseFunc(int b, int s, int x, int y) {
	// Convert from Window to Scene coordinates
	float mx = (float)x;
	float my = (float)y;

	windowToScene(mx, my);

	//Use mouse location as top right point, use that info to get rest of points
	Point mtl = Point(mx, my, red, green, blue);
	Point mtr = Point(mx + length, my, red, green, blue);
	Point mbl = Point(mx, my - length, red, green, blue);
	Point mbr = Point(mx + length, my - length, red, green, blue);

	squares.push_front(Square(mtl, mtr, mbl, mbr));

	// Add a point with with coordinates matching the
	// current mouse position, and the current color values
    //points.push_front(Point(mx, my, red, green, blue));

	// Redraw the scene by calling appDrawScene above
	// so that the point we added above will get painted
	glutPostRedisplay();
}

//-------------------------------------------------------
// A function to handle mouse dragging
// Called every time mouse moves while button held down
// Arguments: 	
//	x, y - current coordinates of the mouse
//-------------------------------------------------------
void appMotionFunc(int x, int y) {
	// Convert from Window to Scene coordinates
	float mx = (float)x;
	float my = (float)y;

	windowToScene(mx, my);

	// Similar behavior to click handler. This function
	// allows us to paint free hand with the mouse.
	//points.push_front(Point(mx, my, red, green, blue));

	Point mtl = Point(mx, my, red, green, blue);
	Point mtr = Point(mx + length, my, red, green, blue);
	Point mbl = Point(mx, my - length, red, green, blue);
	Point mbr = Point(mx + length, my - length, red, green, blue);

	squares.push_front(Square(mtl, mtr, mbl, mbr));

	// Again, we redraw the scene
	glutPostRedisplay();
}

//-------------------------------------------------------
// A function to handle keyboard events
// Called every time a key is pressed on the keyboard
// Arguments: 	
//	key  - the key that was pressed
//	x, y - coordinates of the mouse when key is pressed
//-------------------------------------------------------
void appKeyboardFunc(unsigned char key, int x, int y) {
	// Define what should happen for a given key press 
	switch (key) {
		// Space was pressed. Erase all points and squares
	case ' ':
		points.clear();
		squares.clear();
		length = 0.1;
		change = 0.0;
		break;

		// Escape was pressed. Quit the program
	case 27:
		exit(0);
		break;

		// The "k" key was pressed. Set global color to black
	case 'k':
		for (int i = 0; i < squares.size(); i++) {
			squares[i].topleft.r = 0.0;
			squares[i].topleft.g = 0.0;
			squares[i].topleft.b = 0.0;
		}
		red = 0.0;
		green = 0.0;
		blue = 0.0;
		break;

		// The "w" key was pressed. Set global color and all squares to white
	case 'w':
		for (int i = 0; i < squares.size(); i++) {
			squares[i].topleft.r = 1.0;
			squares[i].topleft.g = 1.0;
			squares[i].topleft.b = 1.0;
		}
		red = 1.0;
		green = 1.0;
		blue = 1.0;
		break;

		// The "r" key was pressed. Set global color and all squares to red
	case 'r':		
		for (int i = 0; i < squares.size(); i++) {
			squares[i].topleft.r = 1.0;
			squares[i].topleft.g = 0.0;
			squares[i].topleft.b = 0.0;
		}
		red = 1.0;
		green = 0.0;
		blue = 0.0;
		break;

		// The "g" key was pressed. Set global color and all squares to green
	case 'g':		
		for (int i = 0; i < squares.size(); i++) {
			squares[i].topleft.r = 0.0;
			squares[i].topleft.g = 1.0;
			squares[i].topleft.b = 0.0;
		}
		red = 0.0;
		green = 1.0;
		blue = 0.0;
		break;

		// The "b" key was pressed. Set global color and all squares to red
	case 'b':		
		for (int i = 0; i < squares.size(); i++) {
			squares[i].topleft.r = 0.0;
			squares[i].topleft.g = 0.0;
			squares[i].topleft.b = 1.0;
		}
		red = 0.0;
		green = 0.0;
		blue = 1.0;
		break;

		//The "i" key was pressed. Increase the size of all squares
	case 'i':	
		for (int i = 0; i < squares.size(); i++) {
			change += 0.001;
		}
		break;

		//The "d" key was pressed. Decrease the size of all squares
	case 'd':		
		for (int i = 0; i < squares.size(); i++) {
			if (change > -0.09) {
				change -= 0.001;
			}
		}
		break;

	}
	// After all the state changes, redraw the scene
	glutPostRedisplay();
}


int main(int argc, char** argv) {
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);

	// Setup window position, size, and title
	glutInitWindowPosition(20, 60);
	glutInitWindowSize(width, height);
	glutCreateWindow("CSE165 OpenGL Demo");

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);


	// Set callback for drawing the scene
	glutDisplayFunc(appDrawScene);

	// Set callback for resizing th window
	glutReshapeFunc(appReshapeFunc);

	// Set callback to handle mouse clicks
	glutMouseFunc(appMouseFunc);

	// Set callback to handle mouse dragging
	glutMotionFunc(appMotionFunc);

	// Set callback to handle keyboad events
	glutKeyboardFunc(appKeyboardFunc);

	// Start the main loop
	glutMainLoop();
}