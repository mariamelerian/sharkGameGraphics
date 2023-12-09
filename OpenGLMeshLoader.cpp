#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <ctime>
#include <string>
#include <sstream>
#include <cmath>
#include <math.h>

#define DEG2RAD(a) (a * 0.0174532925)

int WIDTH = 1280;
int HEIGHT = 720;

GLuint tex;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.3;
GLdouble zFar = 100;

// Countdown timer variables
float countdownDuration = 90.0; // 1.5 minutes
float countdownTimer = countdownDuration;

bool levelOneWon = false;


//shark movement
float sharkX = 0;
float sharkY = 0;
float sharkZ = 0;
float sharkRotationAngle = 0.0;

// Add these global variables to store the last mouse position
int lastX = 0;
int lastY = 0;

// Add this global variable to control the camera rotation sensitivity
float rotationSpeed = 0.2;

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Vector Eye(20, 5, 20);
Vector At(0, 0, 0);
Vector Up(0, 1, 0);

int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_shark;
Model_3DS model_fish01;
Model_3DS model_fish02;
Model_3DS model_crab;
Model_3DS model_coral;
Model_3DS model_human;


class Vector3f {
public:
	float x, y, z;


	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;
	Camera(float eyeX = 20.0f, float eyeY = 10.0f, float eyeZ = 20.0f, float centerX = 4.0f, float centerY = 2.6f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.5f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;






//Model_3DS model_rock;
//Model_3DS model_seahorse;

// Textures
GLTexture tex_ground;
GLTexture tex_ball;
GLTexture tex_coraltex;
GLTexture tex_coraltexx;






//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void drawText(const std::string& text, GLfloat x, GLfloat y, void* font) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, 0, HEIGHT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Set the text color to black
	glColor3f(0.0, 0.0, 0.0);

	// Set the position to display the text
	glRasterPos2f(x, y);

	// Loop through each character in the string and display it
	for (char ch : text) {
		glutBitmapCharacter(font, ch);
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

// drawTexturedSphere function to draw a textured sphere
void drawTexturedSphere() {
	glPushMatrix();
	glScalef(0.25, 0.25, 0.25);
	glColor3f(1.0, 1.0, 1.0);  // Set the color to white
	//glBindTexture(GL_TEXTURE_2D, tex);
	glBindTexture(GL_TEXTURE_2D, tex_ball.texture[0]);

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 5, 50, 50);  // Adjust the radius and other parameters as needed
	gluDeleteQuadric(qobj);
	glPopMatrix();
}

// drawTexturedSphere function to draw a textured sphere
void drawTexturedSphereCoral() {
	glPushMatrix();
	glScalef(0.25, 0.25, 0.25);
	glColor3f(1.0, 1.0, 1.0);  // Set the color to white
	//glBindTexture(GL_TEXTURE_2D, tex);
	glBindTexture(GL_TEXTURE_2D, tex_coraltex.texture[0]);

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 5, 50, 50);  // Adjust the radius and other parameters as needed
	gluDeleteQuadric(qobj);
	glPopMatrix();
}

// drawTexturedSphere function to draw a textured sphere
void drawTexturedSphereCoral2() {
	glPushMatrix();
	glScalef(0.25, 0.25, 0.25);
	glColor3f(1.0, 1.0, 1.0);  // Set the color to white
	//glBindTexture(GL_TEXTURE_2D, tex);
	glBindTexture(GL_TEXTURE_2D, tex_coraltexx.texture[0]);

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 5, 50, 50);  // Adjust the radius and other parameters as needed
	gluDeleteQuadric(qobj);
	glPopMatrix();
}

void displayText() {
	// Get the current time
	 // Get the remaining minutes and seconds from the countdown timer
	int minutes = static_cast<int>(countdownTimer) / 60;
	int seconds = static_cast<int>(countdownTimer) % 60;

	// Convert time to a string
	std::stringstream timeString;
	timeString << "Time: " << minutes << ":" << (seconds < 10 ? "0" : "") << seconds;
	// Convert score to a string (replace with your actual score variable)
	int score = 100; // Replace with your actual score
	std::stringstream scoreString;
	scoreString << "Score: " << score;

	// Display time and score using larger and bold font
	drawText(timeString.str(), 10, 10, GLUT_BITMAP_TIMES_ROMAN_24); // Adjust the position as needed
	drawText(scoreString.str(), 10, 40, GLUT_BITMAP_TIMES_ROMAN_24); // Adjust the position as needed
}

void update(int value)
{
	// Update the countdown timer
	countdownTimer -= 1.0;

	// Check if the timer has reached zero
	if (countdownTimer <= 0.0)
	{
		// Perform actions when the timer reaches zero (game over, reset, etc.)
		countdownTimer = countdownDuration; // Reset the timer
	}

	// Schedule the next update after 1000 milliseconds (1 second)
	glutTimerFunc(1000, update, 0);

	// Request a redraw
	glutPostRedisplay();
}


//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

//=======================================================================
// Display Function
//=======================================================================

void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();

}


void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
	setupCamera();

	// Draw Ground
	RenderGround();

	// Draw Tree Model
	glPushMatrix();
	glTranslatef(10, 2, 0);
	glScalef(1, 1, 1);
	//model_fish.Draw();
	//model_seahorse.Draw();
	glPopMatrix();

	//shark
	glPushMatrix();
	glTranslatef(12 + sharkX, 2 + sharkY, 0 + sharkZ);
	glRotatef(sharkRotationAngle, 0, 1, 0);
	//glScalef(0.9, 0.9, 0.9);
	model_shark.Draw();
	glPopMatrix();



	// Draw Textured Sphere
	glPushMatrix();
	glTranslatef(0,2,-0.5);

	drawTexturedSphere();
	glPopMatrix();

	// Draw Textured coral sphere yellow
	glPushMatrix();
	glTranslatef(-7, 0, -0.5);
	glScalef(0.5, 0.5, 0.5);
	//model_human.Draw();
	glPopMatrix();

	// Draw Textured coral sphere yellow
	glPushMatrix();
	glTranslatef(-7, 0, -0.5);
	drawTexturedSphereCoral();
	glPopMatrix();

	// Draw Textured coral sphere green
	glPushMatrix();
	glTranslatef(-9, 0, -0.5);
	glScalef(1.2, 1.2, 1.2);
	drawTexturedSphereCoral2();
	glPopMatrix();
	//// Draw house Model
	//glPushMatrix();
	//glTranslatef(0, 1, 4);
	//glScalef(0.05, 0.05, 0.05);
	//glrotatef(90.f, 1, 0, 0);
	//model_beachBall.Draw();
	//glPopMatrix();

	//crab
	glPushMatrix();
	glTranslatef(10, 1, 4);
	glRotatef(180.f, 1, 1, 0);
	glRotatef(100.f, 1, 0, 0);
	glScalef(0.5, 0.3, 0.3);
	model_crab.Draw();
	glPopMatrix();
	// draw fish models
	glPushMatrix();
	glTranslatef(0, 2, -5);
	glScalef(0.004, 0.004, 0.004);
	model_fish01.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-6, 2, -2);
	glScalef(0.004, 0.004, 0.004);
	model_fish01.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(9, 2, 7);
	glScalef(0.004, 0.004, 0.004);
	model_fish01.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(15, 2, 15);
	glScalef(0.004, 0.004, 0.004);
	model_fish01.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-5, 2, 6);
	glScalef(0.004, 0.004, 0.004);
	glRotatef(90, 90, 0, 1);
	model_fish02.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-9, 2, -6);
	glScalef(0.004, 0.004, 0.004);
	glRotatef(90, 90, 0, 1);
	model_fish02.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(15, 2, -10);
	glScalef(0.004, 0.004, 0.004);
	glRotatef(90, 90, 0, 1);
	model_fish02.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-6, 2, 15);
	glScalef(0.004, 0.004, 0.004);
	glRotatef(90, 90, 0, 1);
	model_fish02.Draw();
	glPopMatrix();

	//coral
	glPushMatrix();
	glTranslatef(-6, 5, 15);
	//glScalef(0.004, 0.004, 0.004);
	glRotatef(90, 90, 0, 1);
	model_coral.Draw();
	glPopMatrix();

	//draw crab
	glPushMatrix();
	glTranslatef(0,0,0);
	glScalef(100,100,100);
	//glRotatef(90, 90, 0, 1);
	//model_urchin.Draw();
	glPopMatrix();


	////drawsun
	//glPushMatrix();
	//glTranslatef(0, 2, 12);
	//glColor3f(1, 0.5, 0);
	//glutSolidSphere(1.0, 50, 50);
	//glPopMatrix();


	//sky box
	glPushMatrix();

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);


	glPopMatrix();

	// Display text
	displayText();

	glutSwapBuffers();
}

//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char button, int x, int y)
{
	float d = 0.1;
	switch (button)
	{
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;

	case 'f':

		// Adjust the camera to a front view
		camera = Camera(20, 10, 20, 4, 2.6f, 0, 0, 1.5, 0);
		break;
	case 't':
		// Adjust the camera to a top view
		camera = Camera(0.5f, 2.8f, 1.3f, 0.5f, 0.5f, 0.45f, 0.0f, 1.4f, 0.5f);
		break;
	case 'g':
		// Adjust the camera to a side view
		camera = Camera(2.8f, 0.9f, 0.8f, 0.3f, 0.7f, 0.8f, 0.0f, 1.0f, 0.0f);
		break;



	case 'x':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'r':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 27:
		exit(0);
		break;

	case 'W':
		if (sharkX >= -23) {
			sharkX = sharkX - 1;
			sharkRotationAngle = 180;
		}
		break;
	case 'A':

		if (sharkZ <= 21) {
			sharkZ = sharkZ + 1;
			sharkRotationAngle = 270;
		}


		break;
	case 'S':
		if (sharkX <= 7) {
			sharkX = sharkX + 1;
			sharkRotationAngle = 0;
		}
		break;
	case 'D':
		if (sharkZ >= -12) {
			sharkZ = sharkZ - 1;
			sharkRotationAngle = 90;
		}



		break;

	default:
		break;


	}

	glutPostRedisplay();
}

void Special(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}


//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	// Calculate the change in mouse position
	int deltaX = x - lastX;
	int deltaY = y - lastY;

	// Update the last mouse position
	lastX = x;
	lastY = y;

	// Adjust the camera orientation based on mouse movement
	camera.rotateY(deltaX * rotationSpeed);
	camera.rotateX(deltaY * rotationSpeed);

	glutPostRedisplay();  // Re-draw scene
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	// Store the last mouse position when a button is pressed
	lastX = x;
	lastY = y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
	else if (state == GLUT_UP)
	{
		int deltaY = y - cameraZoom;
		cameraZoom = y;  // Update the stored zoom position

		// Adjust the camera position based on deltaY (you can adjust the multiplier)
		camera.moveZ(-deltaY * 0.05);  // Move along the negative z-axis with a smaller amount
	}
}




//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_shark.Load("Models/shark/SHARK.3ds");
	model_fish01.Load("Models/fish/fishs/TropicalFish15.3ds");
	model_fish02.Load("Models/fish/fish2/TropicalFish02.3ds");
	model_coral.Load("Models/coral/coral.3ds");
	model_crab.Load("Models/crab/crab.3ds");
	model_human.Load("Models/human/human.3ds");





	//model_fish02.Load("Models/fish/fish2/TropicalFish15.3ds");


	//model_seahorse.Load("Models/seahorse/seahorse.3ds");
	// Loading texture files
	tex_ball.Load("Textures/ball.bmp");
	tex_coraltex.Load("Textures/coraltex.bmp");
	tex_coraltexx.Load("Textures/coraltexx.bmp");





	tex_ground.Load("Textures/sand.bmp");
	loadBMP(&tex, "Textures/sea.bmp", true);
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);
	// Start the timer function
	glutTimerFunc(1000, update, 0);

	glutDisplayFunc(myDisplay);
	//glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);


	glutKeyboardFunc(myKeyboard);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}