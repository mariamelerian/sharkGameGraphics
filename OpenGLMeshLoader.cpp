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

bool s = true;  //from lab8



// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 1000;

float sunX, sunY, sunZ;

float countdownDuration = 120.0; // 1.5 minutes
float countdownTimer = countdownDuration;

bool gameOverWon = false;


bool levelOneWon = false;
bool levelOne = true;


bool levelTwo = false;
bool justStartedLevelTwo = true;

float ScoreLevelOne = 0;
float ScoreLevelTwo = 0;

float playerHealth = 100.0;
float maxHealth = 100.0;
bool gameOver = false;

float babyX = 18.0;
float babyZ = 15.0;

float sharkX = 0;
float sharkY = 0;
float sharkZ = 0;
float sharkRotationAngle = 0.0;
float sharkScaleFactor = 1.65;
char direction = 'd';

float sunRadius = 1.0;
float sunRotationAngle = 0.0;
float colorChangeSpeed = 0.01;


int lastX = 0;
int lastY = 0;



float rotationSpeed = 0.2;

bool firstPerson = false;
bool thirdPerson = false;
bool NormalView = true;


class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
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

// Textures
GLTexture tex_ground;
GLTexture tex_ball;
GLTexture tex_coraltex;
GLTexture tex_coraltexx;
GLTexture tex_sky;
GLTexture tex_sea;
GLTexture tex_sun;

void InitLightSource()
{
	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);

	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	GLfloat light_position[] = { sunX,sunY,sunZ, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}




void drawSun() {
	static float sunRotationAngle = 0.0;
	sunRotationAngle += 0.01;
	float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	sunZ = 2.0 * cos(sunRotationAngle);
	sunY = 9.0 * cos(sunRotationAngle);
	sunX = 2.0 * sin(sunRotationAngle);

	glPushMatrix();
	glTranslatef(0, sunY, sunZ);
	glRotatef(sunRotationAngle, 1, 0, 0);
	glScalef(0.25, 0.25, 0.25);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, tex_sun.texture[0]);

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 5, 50, 50);
	gluDeleteQuadric(qobj);
	glPopMatrix();

	sunRotationAngle += 0.2;
	if (sunRotationAngle > 360.0) {
		sunRotationAngle -= 360.0;
	}
}


void drawText(const std::string& text, GLfloat x, GLfloat y, void* font) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, 0, HEIGHT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(0.0, 0.0, 0.0);

	glRasterPos2f(x, y);

	for (char ch : text) {
		glutBitmapCharacter(font, ch);
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void drawTexturedSphere() {
	glPushMatrix();
	glScalef(0.25, 0.25, 0.25);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, tex_ball.texture[0]);

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 5, 50, 50);
	gluDeleteQuadric(qobj);
	glPopMatrix();
}

void drawTexturedSphereCoral() {
	glPushMatrix();
	glScalef(0.25, 0.25, 0.25);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, tex_coraltex.texture[0]);

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 5, 50, 50);
	gluDeleteQuadric(qobj);
	glPopMatrix();
}

void drawTexturedSphereCoral2() {
	glPushMatrix();
	glScalef(0.25, 0.25, 0.25);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, tex_coraltexx.texture[0]);

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 5, 50, 50);
	gluDeleteQuadric(qobj);
	glPopMatrix();
}

void displayText() {
	int minutes = static_cast<int>(countdownTimer) / 60;
	int seconds = static_cast<int>(countdownTimer) % 60;

	std::stringstream timeString;
	timeString << "Time: " << minutes << ":" << (seconds < 10 ? "0" : "") << seconds;
	float score = ScoreLevelOne;
	std::stringstream scoreString;
	scoreString << "Score: " << score;

	drawText(timeString.str(), 10, 10, GLUT_BITMAP_TIMES_ROMAN_24);
	drawText(scoreString.str(), 10, 40, GLUT_BITMAP_TIMES_ROMAN_24);
}

void update(int value)
{
	if (!gameOver && !gameOverWon) {
		countdownTimer -= 1.0;

		if (countdownTimer <= 0.0)
		{
			countdownTimer = 0.0;
			gameOver = true;
		}
	}
	glutTimerFunc(1000, update, 0);

	glutPostRedisplay();
}

void InitMaterial()
{
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	glEnable(GL_LIGHTING);


	gluPerspective(fovy, aspectRatio, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

void RenderGround()
{
	glDisable(GL_LIGHTING);

	glColor3f(0.6, 0.6, 0.6);

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);

	glColor3f(1, 1, 1);
}

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
	float collisionThreshold = 2.0;

	// check if the distance between shark and fish is less than the threshold
	float distance = sqrt(pow(sharkX - fishX, 2) + pow(sharkZ - fishZ, 2));

	if (distance < collisionThreshold) {
		PlayCollisionSoundFish();
		return true;
	}

	return false;
}

void UpdateFish() {
	for (int i = 0; i < sizeof(fishes) / sizeof(fishes[0]); i++) {
		if (FishCollisions(sharkX, sharkZ, fishes[i].posX, fishes[i].posZ)) {
			fishes[i].posX = -1000;
			fishes[i].posZ = -1000;
			ScoreLevelOne += 10;
			sharkScaleFactor = sharkScaleFactor + 0.15;
		}
	}
}

void drawFish() {
	for (int i = 0; i < sizeof(fishes) / sizeof(fishes[0]); i++) {
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
	PlaySound(COLLISION_SOUND_PATH_OUCH, NULL, SND_FILENAME | SND_ASYNC);
}

struct Coral {
	float posX, posY, posZ;
	float scale;
	int modelType;
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
	float collisionThreshold = 2.2;

	// check if the distance between shark and coral is less than the threshold
	float distance = sqrt(pow(sharkX - coralX, 2) + pow(sharkZ - coralZ, 2));

	if (distance < collisionThreshold) {
		PlayCollisionSoundCoral();

		return true;
	}

	return false;
}

void UpdateCorals() {
	for (int i = 0; i < sizeof(corals) / sizeof(corals[0]); i++) {
		if (CoralCollisions(sharkX, sharkZ, corals[i].posX, corals[i].posZ)) {
			playerHealth -= 20;
			if (playerHealth <= 0) {
				gameOver = true;
			}
			corals[i].posX = -1000;
			corals[i].posZ = -1000;
		}
	}
}
void drawCorals() {
	for (int i = 0; i < sizeof(corals) / sizeof(corals[0]); i++) {
		if (corals[i].posX == -1000 && corals[i].posZ == -1000) {
			continue;
		}

		glPushMatrix();
		glTranslatef(corals[i].posX, corals[i].posY, corals[i].posZ);
		glScalef(corals[i].scale, corals[i].scale, corals[i].scale);

		if (corals[i].modelType == 1) {
			drawTexturedSphereCoral();
		}
		else if (corals[i].modelType == 2) {
			drawTexturedSphereCoral2();
		}

		glPopMatrix();
	}
}

bool CrabCollisions(float sharkX, float sharkZ, float crabX, float crabZ) {
	float collisionThreshold = 3.0;

	// check if the distance between shark and crab is less than the threshold
	float distance = sqrt(pow(sharkX - crabX, 2) + pow(sharkZ - crabZ, 2));

	if (distance < collisionThreshold) {
		return true;
	}

	return false;
}

void UpdateCrab() {
	// check for collisions with the shark
	if (CrabCollisions(sharkX, sharkZ, 18, 15)) {
		ScoreLevelOne += 50;
		levelTwo = true;
		levelOne = false;
		levelOneWon = true;
	}
}

void drawCrab() {
	UpdateCrab();

	glPushMatrix();
	glTranslatef(18, 1, 15);

	float scaleFactor = 1.3 + 0.2 * sin(glutGet(GLUT_ELAPSED_TIME) * 0.001);
	glScalef(scaleFactor, scaleFactor, scaleFactor);

	glRotatef(180.f, 1, 1, 0);
	glRotatef(100.f, 1, 0, 0);
	glScalef(0.3, 0.3, 0.3);

	model_crab.Draw();
	glPopMatrix();
}


//humans
void PlayCollisionSoundMan() {
	PlaySound(COLLISION_SOUND_PATH_SCREAM, NULL, SND_FILENAME | SND_ASYNC);
}
struct Human {
	float posX, posY, posZ;
	float scale;
	int modelType;
};

Human humans[] = {
	{5, 0, 10, 0.04, 0},
	{0, 0, 0, 0.04, 0},
	{14, 0, 7, 0.04, 2},
	{-7, 0, -7, 0.04, 2},
	{-5, 0, 12, 0.04, 3},
	{0, 0, -15, 0.04, 4},
	{-15, 0, 15, 0.04, 5},
	{15, 0, -15, 0.04, 5},
	{19, 0, 1, 0.04, 6}
};

bool HumanCollisions(float sharkX, float sharkZ, float humanX, float humanZ) {
	float collisionThreshold = 3.0;

	// check if the distance between shark and human is less than the threshold
	float distance = sqrt(pow(sharkX - humanX, 2) + pow(sharkZ - humanZ, 2));

	if (distance < collisionThreshold) {
		PlayCollisionSoundMan();
		return true;
	}

	return false;
}

void UpdateHumans() {
	for (int i = 0; i < sizeof(humans) / sizeof(humans[0]); i++) {
		if (HumanCollisions(sharkX, sharkZ, humans[i].posX, humans[i].posZ)) {
			humans[i].posX = -1000;
			humans[i].posZ = -1000;
			ScoreLevelOne += 10;
			sharkScaleFactor = sharkScaleFactor + 0.15;
		}
	}
}

void drawHumans() {
	for (int i = 0; i < sizeof(humans) / sizeof(humans[0]); i++) {
		if (humans[i].posX == -1000 && humans[i].posZ == -1000) {
			continue;
		}

		glPushMatrix();
		glTranslatef(humans[i].posX, humans[i].posY, humans[i].posZ);
		glScalef(humans[i].scale, humans[i].scale, humans[i].scale);

		switch (humans[i].modelType) {
		case 0:
			model_man.Draw();
			break;

		case 2:
			model_man2.Draw();
			break;
		case 3:
			model_man3.Draw();
			break;

		case 4:
			model_man4.Draw();
			break;
		case 5:
			model_man5.Draw();
			break;
		case 6:
			model_man6.Draw();
			break;


		default:
			model_man.Draw();
			break;
		}

		glPopMatrix();
	}
}

//beachballs

void PlayCollisionSoundBall() {
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
	float collisionThreshold = 2.0;

	// check if the distance between shark and beach ball is less than the threshold
	float distance = sqrt(pow(sharkX - ballX, 2) + pow(sharkZ - ballZ, 2));

	if (distance < collisionThreshold) {
		PlayCollisionSoundBall();
		return true;
	}

	return false;
}

void UpdateBeachBalls() {
	for (int i = 0; i < sizeof(beachBalls) / sizeof(beachBalls[0]); i++) {
		if (BeachBallCollisions(sharkX, sharkZ, beachBalls[i].posX, beachBalls[i].posZ)) {
			playerHealth -= 20;
			if (playerHealth <= 0) {
				gameOver = true;
			}
			beachBalls[i].posX = -1000;
			beachBalls[i].posZ = -1000;
		}
	}
}

void drawBeachBalls() {
	for (int i = 0; i < sizeof(beachBalls) / sizeof(beachBalls[0]); i++) {
		if (beachBalls[i].posX == -1000 && beachBalls[i].posZ == -1000) {
			continue;
		}

		glPushMatrix();
		glTranslatef(beachBalls[i].posX, beachBalls[i].posY, beachBalls[i].posZ);
		glScalef(beachBalls[i].scale, beachBalls[i].scale, beachBalls[i].scale);

		drawTexturedSphere();

		glPopMatrix();
	}
}


bool BabyCollisions(float sharkX, float sharkZ, float babyX, float babyZ) {
	float collisionThreshold = 3.0;

	// check if the distance between shark and baby is less than the threshold
	float distance = sqrt(pow(sharkX - babyX, 2) + pow(sharkZ - babyZ, 2));

	if (distance < collisionThreshold) {
		return true;
	}

	return false;
}

void UpdateBaby() {
	if (BabyCollisions(sharkX, sharkZ, 18, 15)) {
		if (!gameOverWon && !gameOver) {
			ScoreLevelOne += 50;
		}

		gameOverWon = true;

		babyX = -1000;
		babyZ = -1000;
	}
}

void drawBaby() {
	UpdateBaby();

	glPushMatrix();

	glTranslatef(babyX, 3, babyZ);

	float scaleFactor = 1.3 + 0.2 * sin(glutGet(GLUT_ELAPSED_TIME) * 0.001);
	glScalef(scaleFactor, scaleFactor, scaleFactor);

	glScalef(0.03, 0.03, 0.03);
	model_baby.Draw();
	glPopMatrix();
}

void setupLights() {
	int elapsedTime = glutGet(GLUT_ELAPSED_TIME);



	GLfloat lmodel_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	GLfloat l0Diffuse[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	GLfloat l0Spec[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	GLfloat l0Ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat l0Position[] = { 10.0f, 0.0f, 0.0f, s };
	GLfloat l0Direction[] = { -1.0, 0.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0Diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0Ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, l0Position);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 90.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, l0Direction);

	GLfloat l1Diffuse[] = { 1.0f, 1.0f, 0.0f, 0.0f };
	GLfloat l1Ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat l1Position[] = { 0.0f, 10.0f, 0.0f, s };
	GLfloat l1Direction[] = { 0.0, 1.0, 0.0 };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1Diffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, l1Ambient);
	glLightfv(GL_LIGHT1, GL_POSITION, l1Position);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 90.0);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l1Direction);

	GLfloat l2Diffuse[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	GLfloat l2Ambient[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat l2Position[] = { 2, 3, 15, s };
	GLfloat l2Direction[] = { 2, 1, 15 };

	glLightfv(GL_LIGHT2, GL_DIFFUSE, l2Diffuse);
	glLightfv(GL_LIGHT2, GL_AMBIENT, l2Ambient);
	glLightfv(GL_LIGHT2, GL_POSITION, l2Position);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 90.0);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, l2Direction);


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
		sharkY = sharkY;
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
	setupLights();
	setupCamera();


	// Draw Ground
	RenderGround();




	//shark
	glPushMatrix();
	glTranslatef(sharkX, 2 + sharkY, 0 + sharkZ);
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
		glBindTexture(GL_TEXTURE_2D, tex_sky.texture[0]);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, tex_sea.texture[0]);
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

	}
	else {

		if (levelTwo) {
			glBindTexture(GL_TEXTURE_2D, tex_sky.texture[0]);

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


	glPopMatrix();
	drawHealthBar();

	displayText();

	glutSwapBuffers();
}

void myKeyboard(unsigned char button, int x, int y)
{

	if (gameOver || gameOverWon) {
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

	case 'x':
		// default view
		camera = Camera(20, 10, 20, 4, 2.6f, 0, 0, 1.5, 0);
		break;

	case 't':
		if (firstPerson) { !firstPerson; }

		thirdPerson = !thirdPerson;
		switch (direction) {
		case 'd':
			camera.eye.x = sharkX + 10.0f; // Increase the distance behind the shark
			camera.eye.y = 10.0f;           // Increase the height
			camera.eye.z = sharkZ + 10.0f;  // Increase the distance above the shark
			camera.center.x = sharkX;
			camera.center.y = 4.0f;
			camera.center.z = sharkZ;
			break;
		case 's':
			camera.eye.x = sharkX;      // Adjust the distance behind the shark
			camera.eye.y = 10.0f;        // Increase the height
			camera.eye.z = sharkZ + 10.0f; // Increase the distance above the shark
			camera.center.x = sharkX;
			camera.center.y = 4.0f;
			camera.center.z = sharkZ;
			break;
		case 'a':
			camera.eye.x = sharkX + 10.0f; // Increase the distance behind the shark
			camera.eye.y = 10.0f;           // Increase the height
			camera.eye.z = sharkZ + 10.0f;  // Increase the distance above the shark
			camera.center.x = sharkX;
			camera.center.y = 4.0f;
			camera.center.z = sharkZ;
			break;
		case 'w':
			//MAKE IT ROTATE
			camera.eye.x = sharkX;      // Adjust the distance behind the shark
			camera.eye.y = 10.0f;        // Increase the height
			camera.eye.z = sharkZ - 10.0f; // Increase the distance above the shark
			camera.center.x = sharkX;
			camera.center.y = 4.0f;
			camera.center.z = sharkZ;
			break;
		}

		camera.look();
		break;

	case 'f':
		if (thirdPerson) { !thirdPerson; }

		firstPerson = !firstPerson;
		switch (direction) {
		case 'd':
			camera.eye.x = sharkX;
			camera.eye.y = 4;
			camera.eye.z = sharkZ;
			camera.center.x = 400;
			camera.center.y = 4;
			camera.center.z = sharkZ;
			break;
		case 's':
			camera.eye.x = sharkX;
			camera.eye.y = 4;
			camera.eye.z = sharkZ;
			camera.center.x = sharkX;
			camera.center.y = 4;
			camera.center.z = 400;
			break;

		case 'a':
			camera.eye.x = sharkX;
			camera.eye.y = 4;
			camera.eye.z = sharkZ;
			camera.center.x = -400;
			camera.center.y = 4;
			camera.center.z = sharkZ;
			break;
		case 'w':
			camera.eye.x = sharkX;
			camera.eye.y = 4;
			camera.eye.z = sharkZ;
			camera.center.x = sharkX;
			camera.center.y = 4;
			camera.center.z = -400;
			break;
		}

		camera.look();
		break;
	case 'g':
		camera = Camera(2.8f, 0.9f, 0.8f, 0.3f, 0.7f, 0.8f, 0.0f, 1.0f, 0.0f);
		break;

	case 'r':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 27:
		exit(0);
		break;

	case 'A':
		if (sharkX >= -16) {
			sharkX = sharkX - 1;
			sharkRotationAngle = 180;
			if (firstPerson) {
				direction = 'a';
				camera.eye.x--;
				camera.eye.x = sharkX;
				camera.eye.y = 4;
				camera.eye.z = sharkZ;
				camera.center.x = -400;
				camera.center.y = 4;
				camera.center.z = sharkZ;
			}
			else if (thirdPerson) {


				direction = 'a';
				camera.eye.x--;
				camera.eye.x = sharkX + 10;
				camera.eye.y = 10;
				camera.eye.z = sharkZ + 10;
				camera.center.x = sharkX;

				camera.center.y = 4;
				camera.center.z = sharkZ;

			}


		}


		break;
	case 'S':

		if (sharkZ <= 20) {
			sharkZ = sharkZ + 1;
			sharkRotationAngle = 270;

			if (firstPerson) {
				direction = 's';
				camera.eye.z++;
				camera.eye.x = sharkX;
				camera.eye.y = 4;
				camera.eye.z = sharkZ;
				camera.center.x = sharkX;
				camera.center.y = 4;
				camera.center.z = 400;
			}
			else if (thirdPerson) {
				direction = 's';
				camera.eye.z++;
				camera.eye.x = sharkX;
				camera.eye.y = 10;
				camera.eye.z = sharkZ + 10;
				camera.center.x = sharkX;
				camera.center.y = 4;
				camera.center.z = sharkZ;
			}


		}


		break;

	case 'D':
		if (sharkX <= 18) {
			sharkX = sharkX + 1;
			sharkRotationAngle = 0;

			if (firstPerson) {
				direction = 'd';
				camera.eye.x++;
				camera.eye.x = sharkX + 5;
				camera.eye.y = 4;
				camera.eye.z = sharkZ;
				camera.center.x = 400;
				camera.center.y = 4;
				camera.center.z = sharkZ;
			}
			else if (thirdPerson) {
				direction = 'd';
				camera.eye.x++;
				camera.eye.x = sharkX + 10; // Adjust the distance in front of the shark
				camera.eye.y = 10;
				camera.eye.z = sharkZ + 10;
				camera.center.x = sharkX;
				camera.center.y = 4;
				camera.center.z = sharkZ;
			}
		}
		break;

	case 'W':
		if (sharkZ >= -12) {
			sharkZ = sharkZ - 1;
			sharkRotationAngle = 90;

			if (firstPerson) {
				direction = 'w';
				camera.eye.z--;
				camera.eye.x = sharkX;
				camera.eye.y = 4;
				camera.eye.z = sharkZ;
				camera.center.x = sharkX;
				camera.center.y = 4;
				camera.center.z = -400;
			}
			else if (thirdPerson) {
				direction = 'w';
				camera.eye.z--;
				camera.eye.x = sharkX + 10; // Adjust the distance in front of the shark
				camera.eye.y = 10;
				camera.eye.z = sharkZ + 10;
				camera.center.x = sharkX;
				camera.center.y = 4;
				camera.center.z = sharkZ;
			}


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

void LoadAssets()
{
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


	tex_ball.Load("Textures/ball.bmp");
	tex_coraltex.Load("Textures/coraltex.bmp");
	tex_coraltexx.Load("Textures/coraltexx.bmp");


	tex_ground.Load("Textures/sand.bmp");
	tex_sky.Load("Textures/sky.bmp");
	tex_sea.Load("Textures/sea.bmp");
	tex_sun.Load("Textures/sun.bmp");


	loadBMP(&tex, "Textures/sea.bmp", true);
}
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutTimerFunc(1000, update, 0);

	glutDisplayFunc(myDisplay);

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