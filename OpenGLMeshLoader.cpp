#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <ctime>
#include <string>
#include <sstream>
#include <cmath>
#include <math.h>
#include <Windows.h>

#define DEG2RAD(a) (a * 0.0174532925)
#define _USE_MATH_DEFINES
#define M_PI 3.14159265358979323846
#define COLLISION_SOUND_PATH_CRUNCH "sound/crunchSound.wav"
#define COLLISION_SOUND_PATH_OUCH "sound/ouchSound.wav"
#define COLLISION_SOUND_PATH_SCREAM "sound/screamSound.wav"
#define COLLISION_SOUND_PATH_DAMNYOU "sound/damnYouSound.wav"



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

bool gameOverWon = false;


bool levelOneWon = false;
bool levelOne = true;
bool levelOneLost = false;

bool levelTwo = false;
bool levelTwoWon = false;
bool levelTwoLost = false;
bool justStartedLevelTwo = true;

float ScoreLevelOne = 0;
float ScoreLevelTwo = 0;

float playerHealth = 100.0;  // Assuming initial health is 100
float maxHealth = 100.0;     // Maximum health value
bool gameOver = false;

float babyX = 18.0;
float babyZ = 15.0;

//shark movement
float sharkX = 0;
float sharkY = 0;
float sharkZ = 0;
float sharkRotationAngle = 0.0;
float sharkScaleFactor = 1.65;

// Add these global variables to store the sun position
float sunRadius = 1.0;
float sunRotationAngle = 0.0;
float colorChangeSpeed = 0.01; // Adjust the speed of color change



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

Model_3DS model_human;
Model_3DS model_baby;
Model_3DS model_man;
Model_3DS model_man2;
Model_3DS model_man3;
Model_3DS model_man4;
Model_3DS model_man5;
Model_3DS model_man6;




//Model_3DS model_ChibiChick;



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
GLTexture tex_sky;
GLTexture tex_sea;









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



void drawSun() {

	// Save the current color state

	GLfloat currentColor[4];
	glGetFloatv(GL_CURRENT_COLOR, currentColor);

	// Update the rotation angle based on time
	sunRotationAngle += 0.03;

	// Use the current time to generate dynamic shades of yellow to orange
	float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	// Use the current time to generate dynamic colors
	float red = 1.0 + 1.0 * sin(currentTime);
	float green = 1.0 + 1.0 * sin(currentTime + 2.0);
	float blue = 0.2 + 0.2 * sin(currentTime + 4.0);

	glColor3f(red, green, blue);

	// Use sine function for brightness (yellow to orange)
	float brightness = 0.8 + 0.2 * sin(currentTime);

	// Use cosine function for hue variation (yellow to orange)
	float hue = 0.1 * cos(currentTime);

	// Calculate sun position in a circular path
	float sunZ = 20.0 * cos(sunRotationAngle);
	float sunY = 50.0 * cos(sunRotationAngle);
	float sunX = 20.0 * sin(sunRotationAngle);


	// Draw the rotating and color-changing sphere
	glPushMatrix();
	glTranslatef(sunX, 2, sunZ); // Set the sun's position
	//glRotatef(sunRotationAngle, 0, 0, 1);

	// Set the sun color
	glColor3f(red, green, blue);
	glutSolidSphere(sunRadius, 50, 50);

	glPopMatrix();

	// Restore the previous color state
	glColor4fv(currentColor);
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
	float score = ScoreLevelOne; // Replace with your actual score
	std::stringstream scoreString;
	scoreString << "Score: " << score;

	// Display time and score using larger and bold font
	drawText(timeString.str(), 10, 10, GLUT_BITMAP_TIMES_ROMAN_24); // Adjust the position as needed
	drawText(scoreString.str(), 10, 40, GLUT_BITMAP_TIMES_ROMAN_24); // Adjust the position as needed
}

void update(int value)
{
	if (!gameOver && !gameOverWon) {
		// Update the countdown timer
		countdownTimer -= 1.0;

		// Check if the timer has reached zero
		if (countdownTimer <= 0.0)
		{
			countdownTimer = 0.0; // Ensure timer doesn't go negative
			gameOver = true;
		}
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

	glEnable(GL_LIGHTING);


	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*****//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*****//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*****//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*****//

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

//fish

void PlayCollisionSoundFish() {
	// Use SND_ASYNC to play the sound asynchronously (non-blocking)
	PlaySound(COLLISION_SOUND_PATH_CRUNCH, NULL, SND_FILENAME | SND_ASYNC);
}

struct Fish {
	float posX, posY, posZ;
	float scale;
	float rotation;
	int modelType; // 0 for model_fish01, 1 for model_fish02
};

Fish fishes[] = {
	{0, 3, -5, 0.006, 0, 0},
	{-6, 3, -2, 0.006, 0, 0},
	{9, 3, 7, 0.006, 0, 0},
	{15, 3, 15, 0.006, 0, 0},
	{-5, 3, 6, 0.006, 90, 1},
	{-9, 3, -6, 0.006, 90, 1},
	{15, 3, -10, 0.006, 90, 1},
	{-6, 3, 15, 0.006, 90, 1}
};

bool FishCollisions(float sharkX, float sharkZ, float fishX, float fishZ) {
	// Set a threshold for collision detection
	float collisionThreshold = 2.0;

	// Check if the distance between shark and fish is less than the threshold
	float distance = sqrt(pow(sharkX - fishX, 2) + pow(sharkZ - fishZ, 2));

	if (distance < collisionThreshold) {
		PlayCollisionSoundFish();
		return true; // Collision detected
	}

	return false; // No collision
}

void UpdateFish() {
	// Loop through each fish and check for collisions with the shark
	for (int i = 0; i < sizeof(fishes) / sizeof(fishes[0]); i++) {
		if (FishCollisions(sharkX, sharkZ, fishes[i].posX, fishes[i].posZ)) {
			// If collision detected, make the fish disappear
			fishes[i].posX = -1000; // Move fish to a location outside the visible area
			fishes[i].posZ = -1000;
			// Increase the score by 10
			ScoreLevelOne += 10;
			sharkScaleFactor = sharkScaleFactor + 0.15;
		}
	}
}

void drawFish() {
	// Loop through each fish and draw them
	for (int i = 0; i < sizeof(fishes) / sizeof(fishes[0]); i++) {
		// Skip drawing if the fish has disappeared
		if (fishes[i].posX == -1000 && fishes[i].posZ == -1000) {
			continue;
		}

		glPushMatrix();
		glTranslatef(fishes[i].posX, fishes[i].posY, fishes[i].posZ);
		glScalef(fishes[i].scale, fishes[i].scale, fishes[i].scale);

		if (fishes[i].modelType == 1) {
			glRotatef(fishes[i].rotation, 90, 0, 1);
			model_fish02.Draw();
		}
		else {
			model_fish01.Draw();
		}

		glPopMatrix();
	}
}



//corals

void PlayCollisionSoundCoral() {
	// Use SND_ASYNC to play the sound asynchronously (non-blocking)
	PlaySound(COLLISION_SOUND_PATH_OUCH, NULL, SND_FILENAME | SND_ASYNC);
}

struct Coral {
	float posX, posY, posZ;
	float scale;
	int modelType; // Assuming you have multiple coral models
};

Coral corals[] = {
	{-7, 0, -0.5, 0.5, 0},
	{-4, 0, 12, 1.0, 1},
	{-15, 0, -15, 1.0, 1},
	{12, 0, 15, 1.0, 1},
	{-9, 0, 12, 1.0, 1},
	{-2, 0, 6, 1.0, 1},
	{13, 0, -9, 1.0, 1},
	{-15, 0, -14, 1.2, 2},
	{-9, 0, 10, 1.2, 2},
	{-2, 0, 4, 1.2, 2},
	{12, 0, 0, 1.2, 2},
	{14, 0, -8, 1.2, 2}
};

bool CoralCollisions(float sharkX, float sharkZ, float coralX, float coralZ) {
	// Set a threshold for collision detection
	float collisionThreshold = 2.2;

	// Check if the distance between shark and coral is less than the threshold
	float distance = sqrt(pow(sharkX - coralX, 2) + pow(sharkZ - coralZ, 2));

	if (distance < collisionThreshold) {
		PlayCollisionSoundCoral();

		return true; // Collision detected
	}

	return false; // No collision
}

void UpdateCorals() {
	// Loop through each coral and check for collisions with the shark
	for (int i = 0; i < sizeof(corals) / sizeof(corals[0]); i++) {
		if (CoralCollisions(sharkX, sharkZ, corals[i].posX, corals[i].posZ)) {
			// If collision detected, decrease the player's health by 20
			playerHealth -= 20;
			if (playerHealth <= 0) {
				gameOver = true;
			}
			// Make the coral disappear
			corals[i].posX = -1000; // Move coral to a location outside the visible area
			corals[i].posZ = -1000;
		}
	}
}
void drawCorals() {
	// Loop through each coral and draw them
	for (int i = 0; i < sizeof(corals) / sizeof(corals[0]); i++) {
		// Skip drawing if the coral has disappeared
		if (corals[i].posX == -1000 && corals[i].posZ == -1000) {
			continue;
		}

		glPushMatrix();
		glTranslatef(corals[i].posX, corals[i].posY, corals[i].posZ);
		glScalef(corals[i].scale, corals[i].scale, corals[i].scale);

		if (corals[i].modelType == 1) {
			// Assuming you have multiple coral models
			drawTexturedSphereCoral();
		}
		else if (corals[i].modelType == 2) {
			// Assuming you have multiple coral models
			drawTexturedSphereCoral2();
		}

		glPopMatrix();
	}
}

bool CrabCollisions(float sharkX, float sharkZ, float crabX, float crabZ) {
	// Set a threshold for collision detection
	float collisionThreshold = 3.0;

	// Check if the distance between shark and crab is less than the threshold
	float distance = sqrt(pow(sharkX - crabX, 2) + pow(sharkZ - crabZ, 2));

	if (distance < collisionThreshold) {
		return true; // Collision detected
	}

	return false; // No collision
}

void UpdateCrab() {
	// Check for collisions with the shark
	if (CrabCollisions(sharkX, sharkZ, 18, 15)) {
		// If collision detected, increase the score by 50
		ScoreLevelOne += 50;
		levelTwo = true;
		levelOne = false;
		levelOneWon = true;
		// Make the crab disappear
		// Assuming model_crab is an instance of a class that handles the crab model
		//model_crab.SetPosition(-1000, -1000, -1000);
	}
}

void drawCrab() {
	// Check for collisions and update score
	UpdateCrab();

	// Draw the crab
	glPushMatrix();
	glTranslatef(18, 1, 15);

	// Use a sine function to make the crab grow and shrink periodically
	float scaleFactor = 1.3 + 0.2 * sin(glutGet(GLUT_ELAPSED_TIME) * 0.001); // Adjust the frequency with the multiplier
	glScalef(scaleFactor, scaleFactor, scaleFactor);

	// Rotate and scale the crab
	glRotatef(180.f, 1, 1, 0);
	glRotatef(100.f, 1, 0, 0);
	glScalef(0.3, 0.3, 0.3);

	model_crab.Draw();
	glPopMatrix();
}


//humans
void PlayCollisionSoundMan() {
	// Use SND_ASYNC to play the sound asynchronously (non-blocking)
	PlaySound(COLLISION_SOUND_PATH_SCREAM, NULL, SND_FILENAME | SND_ASYNC);
}
struct Human {
	float posX, posY, posZ;
	float scale;
	int modelType; // Assuming you have multiple human models
};

Human humans[] = {
	{5, 0, 10, 0.04, 0},   // Human 1
	{0, 0, 0, 0.04, 0},    // Human 2
	{14, 0, 7, 0.04, 2},   // Human 3
	{-7, 0, -7, 0.04, 2},  // Human 4
	{-5, 0, 12, 0.04, 3},  // Human 5
	{0, 0, -15, 0.04, 4},  // Human 6
	{-15, 0, 15, 0.04, 5}, // Human 7
	{15, 0, -15, 0.04, 5}, // Human 8
	{19, 0, 1, 0.04, 6}    // Human 9
};

bool HumanCollisions(float sharkX, float sharkZ, float humanX, float humanZ) {
	// Set a threshold for collision detection
	float collisionThreshold = 3.0;

	// Check if the distance between shark and human is less than the threshold
	float distance = sqrt(pow(sharkX - humanX, 2) + pow(sharkZ - humanZ, 2));

	if (distance < collisionThreshold) {
		PlayCollisionSoundMan();
		return true; // Collision detected
	}

	return false; // No collision
}

void UpdateHumans() {
	// Loop through each human and check for collisions with the shark
	for (int i = 0; i < sizeof(humans) / sizeof(humans[0]); i++) {
		if (HumanCollisions(sharkX, sharkZ, humans[i].posX, humans[i].posZ)) {
			// If collision detected, make the human disappear
			humans[i].posX = -1000; // Move human to a location outside the visible area
			humans[i].posZ = -1000;
			// Increase the score by 10
			ScoreLevelOne += 10;
			sharkScaleFactor = sharkScaleFactor + 0.15;
			// Perform any other actions on collision (increase score, change shark scale, etc.)
		}
	}
}

void drawHumans() {
	// Loop through each human and draw them
	for (int i = 0; i < sizeof(humans) / sizeof(humans[0]); i++) {
		// Skip drawing if the human has disappeared
		if (humans[i].posX == -1000 && humans[i].posZ == -1000) {
			continue;
		}

		glPushMatrix();
		glTranslatef(humans[i].posX, humans[i].posY, humans[i].posZ);
		glScalef(humans[i].scale, humans[i].scale, humans[i].scale);

		// Draw the human model based on modelType
		switch (humans[i].modelType) {
		case 0:
			// Assuming you have multiple human models
			model_man.Draw();
			break;

		case 2:
			// Assuming you have multiple human models
			model_man2.Draw();
			break;
		case 3:
			// Assuming you have multiple human models
			model_man3.Draw();
			break;

		case 4:
			// Assuming you have multiple human models
			model_man4.Draw();
			break;
		case 5:
			// Assuming you have multiple human models
			model_man5.Draw();
			break;
		case 6:
			// Assuming you have multiple human models
			model_man6.Draw();
			break;


		default:
			model_man.Draw(); // Default human model
			break;
		}

		glPopMatrix();
	}
}

//BEACH BALLS beachball beachBall BeachBall

void PlayCollisionSoundBall() {
	// Use SND_ASYNC to play the sound asynchronously (non-blocking)
	PlaySound(COLLISION_SOUND_PATH_DAMNYOU, NULL, SND_FILENAME | SND_ASYNC);
}
struct BeachBall {
	float posX, posY, posZ;
	float scale;
};

BeachBall beachBalls[] = {
	{0, 2, -5, 1.0},
	{-4, 2, 6, 1.0},
	{-10, 2, 0, 1.0},
	{-15, 2, -15, 1.0},
	{12, 2, 15, 1.0},
	{-9, 2, 12, 1.0},
	{13, 2, -9, 1.0}
};

bool BeachBallCollisions(float sharkX, float sharkZ, float ballX, float ballZ) {
	// Set a threshold for collision detection
	float collisionThreshold = 2.0;

	// Check if the distance between shark and beach ball is less than the threshold
	float distance = sqrt(pow(sharkX - ballX, 2) + pow(sharkZ - ballZ, 2));

	if (distance < collisionThreshold) {
		PlayCollisionSoundBall();
		return true; // Collision detected
	}

	return false; // No collision
}

void UpdateBeachBalls() {
	// Loop through each beach ball and check for collisions with the shark
	for (int i = 0; i < sizeof(beachBalls) / sizeof(beachBalls[0]); i++) {
		if (BeachBallCollisions(sharkX, sharkZ, beachBalls[i].posX, beachBalls[i].posZ)) {
			// If collision detected, decrease the player's health by 10
			playerHealth -= 20;
			if (playerHealth <= 0) {
				gameOver = true;
			}
			// Make the beach ball disappear
			beachBalls[i].posX = -1000; // Move ball to a location outside the visible area
			beachBalls[i].posZ = -1000;
		}
	}
}

void drawBeachBalls() {
	// Loop through each beach ball and draw them
	for (int i = 0; i < sizeof(beachBalls) / sizeof(beachBalls[0]); i++) {
		// Skip drawing if the beach ball has disappeared
		if (beachBalls[i].posX == -1000 && beachBalls[i].posZ == -1000) {
			continue;
		}

		glPushMatrix();
		glTranslatef(beachBalls[i].posX, beachBalls[i].posY, beachBalls[i].posZ);
		glScalef(beachBalls[i].scale, beachBalls[i].scale, beachBalls[i].scale);

		drawTexturedSphere(); // Draw the textured sphere for beach ball

		glPopMatrix();
	}
}


bool BabyCollisions(float sharkX, float sharkZ, float babyX, float babyZ) {
	// Set a threshold for collision detection
	float collisionThreshold = 3.0;

	// Check if the distance between shark and crab is less than the threshold
	float distance = sqrt(pow(sharkX - babyX, 2) + pow(sharkZ - babyZ, 2));

	if (distance < collisionThreshold) {
		return true; // Collision detected
	}

	return false; // No collision
}

void UpdateBaby() {
	// Check for collisions with the shark
	if (BabyCollisions(sharkX, sharkZ, 18, 15)) {
		// If collision detected, increase the score by 50
		if (!gameOverWon && !gameOver) {
			ScoreLevelOne += 50;
		}

		gameOverWon = true;

		// Teleport the baby to a new position (-1000, -1000, -1000)
		babyX = -1000;
		babyZ = -1000;
	}
}

void drawBaby() {
	// Check for collisions and update score
	UpdateBaby();

	glPushMatrix();

	glTranslatef(babyX, 3, babyZ); // Use babyX and babyZ instead of fixed values
	// Use a sine function to make the baby grow and shrink periodically
	float scaleFactor = 1.3 + 0.2 * sin(glutGet(GLUT_ELAPSED_TIME) * 0.001); // Adjust the frequency with the multiplier
	glScalef(scaleFactor, scaleFactor, scaleFactor);

	// Rotate and scale the baby
	// glRotatef(180.f, 1, 1, 0);
	// glRotatef(100.f, 1, 0, 0);
	glScalef(0.03, 0.03, 0.03);
	model_baby.Draw();
	glPopMatrix();
}
void drawHealthBar()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, 0, HEIGHT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	float healthBarWidth = (playerHealth / maxHealth) * 190.0;
	glVertex2f(WIDTH - 195, 25);
	glVertex2f(WIDTH - 195 + healthBarWidth, 25);
	glVertex2f(WIDTH - 195 + healthBarWidth, 45);
	glVertex2f(WIDTH - 195, 45);
	glEnd();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

}

void resetHealthBar() {
	if (justStartedLevelTwo) {

		// Reset the health bar to 100
		playerHealth = maxHealth;
		sharkX = 0;
		sharkY = 2;
		sharkZ = 0;
	}
	justStartedLevelTwo = false;

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

	


	//shark
	glPushMatrix();
	//glTranslatef(12 + sharkX, 3 + sharkY, 0 + sharkZ);
	glTranslatef( sharkX, 2 + sharkY, 0 + sharkZ);
	glRotatef(sharkRotationAngle, 0, 1, 0);
	glScalef(sharkScaleFactor, sharkScaleFactor, sharkScaleFactor);
	model_shark.Draw();
	glPopMatrix();

	//sun
	glPushMatrix();
	drawSun();
	glPopMatrix();

	// sky box
	glPushMatrix();
	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);

	// Change the texture based on the levelTwo flag
	if (levelTwo) {
		glBindTexture(GL_TEXTURE_2D, tex_sky.texture[0]); // Assuming you have tex_sky loaded in LoadAssets
	}
	else {
		glBindTexture(GL_TEXTURE_2D, tex_sea.texture[0]); // Default sea texture
	}

	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);

	glPopMatrix();


	if (levelOne) {
		//draw and update corals
		drawCorals();
		UpdateCorals();

		// draw and update fish
		drawFish();
		UpdateFish();

		//draw scaling and descaling crab
		drawCrab();




	}else{

	if (levelTwo) {
		glBindTexture(GL_TEXTURE_2D, tex_sky.texture[0]); // Assuming you have tex_sky loaded in LoadAssets

		resetHealthBar();

		drawBeachBalls();
		UpdateBeachBalls();

		drawHumans();
		UpdateHumans();

		drawBaby();
		UpdateBaby();

		
	}
	}


	if (gameOver)
	{
		drawText("Game Over! You Lost!", WIDTH / 2 - 150, HEIGHT / 2, GLUT_BITMAP_TIMES_ROMAN_24);
	}

	if (gameOverWon)
	{
		drawText("Congratulations! You Win!", WIDTH / 2 - 150, HEIGHT / 2, GLUT_BITMAP_TIMES_ROMAN_24);
	}

	
	////drawsun
	//glPushMatrix();
	//glTranslatef(0, 2, 12);
	//glColor3f(1, 0.5, 0);
	//glutSolidSphere(1.0, 50, 50);
	//glPopMatrix();




	glPopMatrix();
	drawHealthBar();

	// Display text
	displayText();

	glutSwapBuffers();
}

//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char button, int x, int y)
{

	if (gameOver|| gameOverWon) {
		return;
	}
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
		if (sharkX <= 19) {
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
	
	model_crab.Load("Models/crab/crab.3ds");
	model_baby.Load("Models/Baby/baby.3ds");
	model_man.Load("Models/man/FinalTPose.3ds");
	model_man2.Load("Models/man2/FinalTPose.3ds");
	model_man3.Load("Models/man3/FinalTPose.3ds");
	model_man4.Load("Models/man4/FinalTPose.3ds");
	model_man5.Load("Models/man5/FinalTPose.3ds");
	model_man6.Load("Models/man6/FinalTPose.3ds");


	




	// Loading texture files
	tex_ball.Load("Textures/ball.bmp");
	tex_coraltex.Load("Textures/coraltex.bmp");
	tex_coraltexx.Load("Textures/coraltexx.bmp");





	tex_ground.Load("Textures/sand.bmp");
	tex_sky.Load("Textures/sky.bmp");
	tex_sea.Load("Textures/sea.bmp");
	
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
