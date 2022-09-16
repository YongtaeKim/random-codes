// OpenGL Project

// Author: KIM, YONGTAE

/*
Featuring Effects:

1. Lighting
2. Shadows
3. Blending
4. Reflections
5. Particle System
6. Animation
7. Textures
8. Skybox
9. Camera controls

*/


#include "../shared/gltools.h"    // OpenGL toolkit

#include <iostream>
#include <time.h> 
#include <math.h>
#include <string.h>

using namespace std;

#define PI 3.14159265
#define MOVE_DELTA 2.0f // Move Speed
#define ANGLE_DELTA 2.0f
#define MOUSE_ANGLE_DELTA 0.1f

// Texture enums
#define NUM_TEXTURES 7
#define WALL_T 0
#define FLOOR_T 1
#define CUBE_T 2
#define PYRAMID_T 3
#define CEILING_T 4
#define PARTICLE_T 5
#define TEAPOT_T 6

GLuint  textureObjects[NUM_TEXTURES];

// Object Textures
const char *szTextureFiles[] = { "txKWall.tga", "txKFloor.tga",
"txKCube.tga", "txKPyramid.tga", "RT02_50.tga", "particle.tga", "txKTeapot.tga" };


// Skybox Textures
const char *szCubeFaces[6] = { "GalaxyRt.tga",
"GalaxyLf.tga",
"GalaxyUp.tga",
"GalaxyDn.tga",
"GalaxyBk.tga",
"GalaxyFt.tga" };


// Skybox enums
GLenum  cube[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };


// Particles
#define MAX_PARTICLES 4000
#define SPEED_SCALE 5

struct particles				// Create A Structure For Particle
{
	float   speed;				    //speed along the movement vector
	float	life;					// Particle Life
	float   fade;				    // fade delta
	float	r;						// Red Value
	float	g;						// Green Value
	float	b;						// Blue Value
	float	x;						// X Position
	float	y;						// Y Position
	float	z;						// Z Position
	float	dx;						// X Direction
	float	dy;						// Y Direction
	float	dz;						// Z Direction
};							// Particles Structure

particles particle[MAX_PARTICLES];	// Particle Array (Room For Particle Info)

float randomFloat(float min, float max)
{
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}




float spin = 0;
bool lookUp;
bool lookDown;
bool lookLeft;
bool lookRight;
bool walkForward;
bool walkBackward;
bool strafeLeft;
bool strafeRight;
float objectYRotation = 0;
float xCameraLocation;
float yCameraLocation;
float zCameraLocation;
float yRotationAngle;
float zRotationAngle;
float xRotationAngle;
int mouseLastx;
int mouseLasty;
float sunRotation = 0;


// Light values and coordinates
struct Color {
	float r = 0;
	float g = 0;
	float b = 0;
	Color(float red, float green, float blue) : r(red), g(green), b(blue) {}
} userLightColor(0.7, 0.7, 0.7);

GLfloat	lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };

// Addtional Light Positions for rotation
GLfloat	mylightPos1[] = { 7.0f, 0.0f, 0.0f, 1.0f };
GLfloat	mylightPos2[] = { -7.0f, 0.0f, 0.0f, 1.0f };

GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat  diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat  specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat  ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat  ambientNoLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };

GLfloat	none[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat full[] = { 1.0f, 1.0f, 1.0f, 1.0f };

GLfloat rotateThings; // Rotation

bool specularFlag = true;
bool diffuseFlag = true;
bool ambientFlag = true;
bool smoothFlag = true;

bool warped = false;

void mouseMovement(int x, int y)
{
	if (!warped)
	{
		int mouseDiffx = x - glutGet(GLUT_WINDOW_WIDTH) / 2;
		int mouseDiffy = y - glutGet(GLUT_WINDOW_HEIGHT) / 2;
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
		xRotationAngle -= ((GLfloat)mouseDiffy)*0.2;
		yRotationAngle += ((GLfloat)mouseDiffx)*0.2;
		if (xRotationAngle >= 90)
			xRotationAngle = 90;
		if (xRotationAngle <= -90)
			xRotationAngle = -90;
		warped = true; //don't allow glutWarpPoint to cause glut to call mouseMovement again
	}

	//cout << "x:" << x << "y:" << y << endl;
}





void adjustLight()
{
	diffuse[0] = userLightColor.r;
	diffuse[1] = userLightColor.g;
	diffuse[2] = userLightColor.b;

	specular[0] = userLightColor.r;
	specular[1] = userLightColor.g;
	specular[2] = userLightColor.b;

	ambientLight[0] = userLightColor.r*.1;
	ambientLight[1] = userLightColor.g*.1;
	ambientLight[2] = userLightColor.b*.1;


	// Additional
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT2, GL_POSITION, lightPos);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	if (ambientFlag)
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	else
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, none);

	if (smoothFlag)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);

}



// Particles being poured from a rotating Teapot.
void initParticle(int index)
{
	// Rotating Radius.
	float radius = 12;

	particle[index].r = 1;
	particle[index].g = 0.5;
	particle[index].b = 0.5;

	// Rotation is synced with the rotating Teapot.
	particle[index].x = cos(-rotateThings * PI / 180.0) * radius;
	particle[index].y = sin(-rotateThings * PI / 180.0) * radius;
	particle[index].z = 0;

	particle[index].dx = 0;
	particle[index].dy = 0;
	particle[index].dz = -2; // Only falling downward.

	particle[index].life = 1;

	particle[index].fade = randomFloat(0.1, 1.1);

	// 0.5 ~ 1.0 for looking heavy.
	particle[index].speed = randomFloat(0.5, 1) * SPEED_SCALE;

}

void initParticles()
{
	srand(time(NULL));
	for (int i = 0; i<MAX_PARTICLES; i++)
		initParticle(i);
}

void drawParticles()
{
	float deltaTime = 1.f / 60.f;
	glDisable(GL_LIGHTING);
	//glDisable (GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);									// Enable Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// Type Of Blending To Perform
														//glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, textureObjects[PARTICLE_T]);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE); //sets coordinates from 0 to 1 in each direction for the quad
														   //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_TEXTURE_2D);
	glPointSize(30.0f);      // 5.5
	glBegin(GL_POINTS);
	for (int i = 0; i<MAX_PARTICLES; i++)
	{
		if (particle[i].life < 0)
			initParticle(i);


		particle[i].x += particle[i].dx * deltaTime * particle[i].speed;
		particle[i].y += particle[i].dy * deltaTime * particle[i].speed;
		particle[i].z += particle[i].dz * deltaTime * particle[i].speed;
		if (particle[i].life > 0.5)
		{
			particle[i].b -= 2 * particle[i].fade * deltaTime;
			particle[i].r += 2 * particle[i].fade * deltaTime;
		}
		else
		{
			particle[i].g -= 2 * particle[i].fade * deltaTime;
			particle[i].r -= 2 * particle[i].fade * deltaTime;
		}


		particle[i].life -= particle[i].fade * deltaTime;

		// Draw The Particle Using Our RGB Values, Fade The Particle Based On Its Life
		glColor4f(particle[i].r, particle[i].g, particle[i].b, particle[i].life);

		glVertex3f(particle[i].x, particle[i].y, particle[i].z); // Top Righ

	}
	glEnd();
	glDisable(GL_BLEND); // Enable Blending
	glDisable(GL_POINT_SPRITE);
	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

// Shadow
//http://www.opengl.org/archives/resources/features/StencilTalk/tsld021.htm

void shadowMatrix(GLfloat shadowMat[4][4], GLfloat groundplane[4], GLfloat lightpos[4])

{
	GLfloat dot;
	// Find dot product between light position vector and ground plane normal.
	dot = groundplane[0] * lightpos[0] +
		groundplane[1] * lightpos[1] +
		groundplane[2] * lightpos[2] +
		groundplane[3] * lightpos[3];
	shadowMat[0][0] = dot - lightpos[0] * groundplane[0];
	shadowMat[1][0] = 0.f - lightpos[0] * groundplane[1];
	shadowMat[2][0] = 0.f - lightpos[0] * groundplane[2];
	shadowMat[3][0] = 0.f - lightpos[0] * groundplane[3];
	shadowMat[0][1] = 0.f - lightpos[1] * groundplane[0];
	shadowMat[1][1] = dot - lightpos[1] * groundplane[1];
	shadowMat[2][1] = 0.f - lightpos[1] * groundplane[2];
	shadowMat[3][1] = 0.f - lightpos[1] * groundplane[3];
	shadowMat[0][2] = 0.f - lightpos[2] * groundplane[0];
	shadowMat[1][2] = 0.f - lightpos[2] * groundplane[1];
	shadowMat[2][2] = dot - lightpos[2] * groundplane[2];
	shadowMat[3][2] = 0.f - lightpos[2] * groundplane[3];
	shadowMat[0][3] = 0.f - lightpos[3] * groundplane[0];
	shadowMat[1][3] = 0.f - lightpos[3] * groundplane[1];
	shadowMat[2][3] = 0.f - lightpos[3] * groundplane[2];
	shadowMat[3][3] = dot - lightpos[3] * groundplane[3];
}



// Room (Updated for Texture)
void drawPlane(int size, int increment)
{
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);

	for (int i = -100; i < 100; i += 1)
		for (int j = -100; j < 100; j += 1)
		{
			float s = (i + 100) / 200.0f;
			float t = 1 - ((j + 100) / 200.0f);
			float ds = 1 / 200.0f;
			float dt = 1 / 200.0f;
			glTexCoord2f(s + ds, t);
			glVertex3f(i + 1, 0, j);
			glTexCoord2f(s, t);
			glVertex3f(i, 0, j);
			glTexCoord2f(s, t - dt);
			glVertex3f(i, 0, j + 1);
			glTexCoord2f(s + ds, t - dt);
			glVertex3f(i + 1, 0, j + 1);
		}
	glEnd();
}

void drawReflect(int size, int increment)
{
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	float textureQuadSize = (float)increment / ((float)size * 2.0f);
	for (int i = -size; i<size; i += increment)
	{

		for (int j = -size; j<size; j += increment)
		{
			float startSTexture = (float)(i) / ((float)size*2.f) + 0.5f;
			float startTTexture = 1 - ((float)(j + increment) / ((float)size*2.f) + 0.5f); //need to flip the T and offset by one increment
			glTexCoord2f(startSTexture + textureQuadSize, startTTexture + textureQuadSize);
			glVertex3f(i + increment, 0, j);
			glTexCoord2f(startSTexture + 0.0f, startTTexture + textureQuadSize);
			glVertex3f(i, 0, j);
			glTexCoord2f(startSTexture + 0.0f, startTTexture + 0.0f);
			glVertex3f(i, 0, j + increment);
			glTexCoord2f(startSTexture + textureQuadSize, startTTexture + 0.0f);
			glVertex3f(i + increment, 0, j + increment);
		}
	}
	glEnd();
}


// Star (Modified for glEnable(GL_CULL_FACE))
void drawStar(GLfloat size) // 
{
	glBegin(GL_TRIANGLES);

	glNormal3f(0, 1, 0);

	glVertex3f(-size, 0, size / 2);
	glVertex3f(size, 0, size / 2);
	glVertex3f(0, 0, -size);

	glVertex3f(0, 0, size);
	glVertex3f(size, 0, -size / 2);
	glVertex3f(-size, 0, -size / 2);

	glNormal3f(0, -1, 0);

	glVertex3f(0, 0, -size);
	glVertex3f(size, 0, size / 2);
	glVertex3f(-size, 0, size / 2);

	glVertex3f(-size, 0, -size / 2);
	glVertex3f(size, 0, -size / 2);
	glVertex3f(0, 0, size);

	glEnd();

}

// Center Cube (Appropriate Normal vectors Added)
void drawCube(GLfloat size, bool inShadow)
{
	glBegin(GL_QUADS);

	glNormal3f(0, 0, 1);
	if (!inShadow) glColor3f(0.8, 0.8, 0.2);
	glTexCoord2f(1, 1);
	glVertex3f(-size, size, size);
	glTexCoord2f(0, 1);
	glVertex3f(-size, -size, size);
	glTexCoord2f(0, 0);
	glVertex3f(size, -size, size);
	glTexCoord2f(1, 0);
	glVertex3f(size, size, size);

	glNormal3f(0, 0, -1);
	if (!inShadow) glColor3f(0.9, 0.3, 1);
	glTexCoord2f(1, 1);
	glVertex3f(-size, size, -size);
	glTexCoord2f(0, 1);
	glVertex3f(size, size, -size);
	glTexCoord2f(0, 0);
	glVertex3f(size, -size, -size);
	glTexCoord2f(1, 0);
	glVertex3f(-size, -size, -size);

	glNormal3f(0, 1, 0);
	if (!inShadow) glColor3f(1, 0.3, 0.1);
	glTexCoord2f(1, 1);
	glVertex3f(-size, size, -size);
	glTexCoord2f(0, 1);
	glVertex3f(-size, size, size);
	glTexCoord2f(0, 0);
	glVertex3f(size, size, size);
	glTexCoord2f(1, 0);
	glVertex3f(size, size, -size);


	glNormal3f(0, -1, 0);
	if (!inShadow) glColor3f(0.2, 0.9, 0);
	glTexCoord2f(1, 1);
	glVertex3f(-size, -size, -size);
	glTexCoord2f(0, 1);
	glVertex3f(size, -size, -size);
	glTexCoord2f(0, 0);
	glVertex3f(size, -size, size);
	glTexCoord2f(1, 0);
	glVertex3f(-size, -size, size);


	glNormal3f(-1, 0, 0);
	if (!inShadow) glColor3f(0.4, 0.9, 1);
	glTexCoord2f(1, 1);
	glVertex3f(-size, size, -size);
	glTexCoord2f(0, 1);
	glVertex3f(-size, -size, -size);
	glTexCoord2f(0, 0);
	glVertex3f(-size, -size, size);
	glTexCoord2f(1, 0);
	glVertex3f(-size, size, size);


	glNormal3f(1, 0, 1);//
	if (!inShadow) glColor3f(0.1, 0.3, 1);
	glTexCoord2f(1, 1);
	glVertex3f(size, size, size);
	glTexCoord2f(0, 1);
	glVertex3f(size, -size, size);
	glTexCoord2f(0, 0);
	glVertex3f(size, -size, -size);
	glTexCoord2f(1, 0);
	glVertex3f(size, size, -size);

	glEnd();
}



// Center Pyramid
void drawPyramid()
{

	glBegin(GL_TRIANGLES);

	glNormal3f(0, 0, -1);
	glTexCoord2f(0.2f, 1.0f);
	glVertex3f(0, 35, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(8, 0, -8);
	glTexCoord2f(0.4f, 0.0f);
	glVertex3f(-8, 0, -8);

	glNormal3f(0.0, 0.0, 1);
	glTexCoord2f(0.2f, 1.0f);
	glVertex3f(0, 35, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-8, 0, 8);
	glTexCoord2f(0.4f, 0.0f);
	glVertex3f(8, 0, 8);

	glNormal3f(-1, 0.0, 0);
	glTexCoord2f(0.2f, 1.0f);
	glVertex3f(0, 35, 0);
	glTexCoord2f(0.4f, 0.0f);
	glVertex3f(-8, 0, -8);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-8, 0, 8);

	glNormal3f(1, 0.0, 0);
	glTexCoord2f(0.2f, 1.0f);
	glVertex3f(0, 35, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(8, 0, 8);
	glTexCoord2f(0.4f, 0.0f);
	glVertex3f(8, 0, -8);

	glEnd();

	// Bottom
	glBegin(GL_QUADS);

	glNormal3f(0, -1.0, 0);
	glTexCoord2f(0.4f, 0.4f);
	glVertex3f(-8, 0, -8);
	glTexCoord2f(0.0f, 0.4f);
	glVertex3f(8, 0, -8);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(8, 0, 8);
	glTexCoord2f(0.4f, 0.0f);
	glVertex3f(-8, 0, 8);

	glEnd();
}

void DrawSkyBox(void)
{
	GLfloat fExtent = 500.0f;
	glEnable(GL_TEXTURE_CUBE_MAP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBegin(GL_QUADS);
	//////////////////////////////////////////////
	// Negative X
	glTexCoord3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-fExtent, -fExtent, fExtent);

	glTexCoord3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-fExtent, -fExtent, -fExtent);

	glTexCoord3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-fExtent, fExtent, -fExtent);

	glTexCoord3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-fExtent, fExtent, fExtent);


	///////////////////////////////////////////////
	//  Postive X
	glTexCoord3f(1.0f, -1.0f, -1.0f);
	glVertex3f(fExtent, -fExtent, -fExtent);

	glTexCoord3f(1.0f, -1.0f, 1.0f);
	glVertex3f(fExtent, -fExtent, fExtent);

	glTexCoord3f(1.0f, 1.0f, 1.0f);
	glVertex3f(fExtent, fExtent, fExtent);

	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(fExtent, fExtent, -fExtent);


	////////////////////////////////////////////////
	// Negative Z 
	glTexCoord3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-fExtent, -fExtent, -fExtent);

	glTexCoord3f(1.0f, -1.0f, -1.0f);
	glVertex3f(fExtent, -fExtent, -fExtent);

	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(fExtent, fExtent, -fExtent);

	glTexCoord3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-fExtent, fExtent, -fExtent);


	////////////////////////////////////////////////
	// Positive Z 
	glTexCoord3f(1.0f, -1.0f, 1.0f);
	glVertex3f(fExtent, -fExtent, fExtent);

	glTexCoord3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-fExtent, -fExtent, fExtent);

	glTexCoord3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-fExtent, fExtent, fExtent);

	glTexCoord3f(1.0f, 1.0f, 1.0f);
	glVertex3f(fExtent, fExtent, fExtent);


	//////////////////////////////////////////////////
	// Positive Y
	glTexCoord3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-fExtent, fExtent, fExtent);

	glTexCoord3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-fExtent, fExtent, -fExtent);

	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(fExtent, fExtent, -fExtent);

	glTexCoord3f(1.0f, 1.0f, 1.0f);
	glVertex3f(fExtent, fExtent, fExtent);


	///////////////////////////////////////////////////
	// Negative Y
	glTexCoord3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-fExtent, -fExtent, -fExtent);

	glTexCoord3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-fExtent, -fExtent, fExtent);

	glTexCoord3f(1.0f, -1.0f, 1.0f);
	glVertex3f(fExtent, -fExtent, fExtent);

	glTexCoord3f(1.0f, -1.0f, -1.0f);
	glVertex3f(fExtent, -fExtent, -fExtent);
	glEnd();
	glDisable(GL_TEXTURE_CUBE_MAP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

}


// Scene for the Shadow
// bool inShadow: true for when glColor3f should not be called
//				 false for when glColor3f may be called
void shadowScene(bool inShadow)
{

	glPushMatrix(); // push World VIEW SPACE TRANSFORMATION
	glRotatef(-90, 1, 0, 0); // 90


							 // Set Begin
	glDisable(GL_TEXTURE_2D);
	// Star set 1
	glPushMatrix();

	glTranslatef(20, 0, 0);
	if (!inShadow) glColor3f(0.098, 0.901, 0.556);
	glutSolidCone(5, 10, 40, 40);
	glTranslatef(0, 0, 17);
	if (!inShadow) glColor3f(0.0f, 0.0f, 1.0f);
	glPushMatrix();
	glRotatef(-3 * rotateThings, 0, 0, 1);
	glRotatef(90, 0, 1, 0);
	glutSolidTorus(0.4, 5.5, 40, 40);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-4 * rotateThings, 0, 0, 1);
	if (!inShadow) glColor3f(0.9f, 0.9f, 0.1f);
	glMaterialfv(GL_FRONT, GL_SPECULAR, full);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
	drawStar(4.5f);
	glPopMatrix();

	glPopMatrix();



	// Star set 2
	glPushMatrix();

	glTranslatef(0, 20, 0);
	if (!inShadow) glColor3f(0.098, 0.901, 0.556);
	glutSolidCone(5, 10, 40, 40);
	glTranslatef(0, 0, 17);
	if (!inShadow) glColor3f(0.0f, 1.0f, 0.0f);
	glPushMatrix();
	glRotatef(2 * rotateThings, 0, 0, 1);
	glRotatef(90, 0, 1, 0);
	glutSolidTorus(0.4, 5.5, 40, 40);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-2 * rotateThings, 0, 0, 1);
	if (!inShadow) glColor3f(0.8f, 1.0f, 0.0f);
	glMaterialfv(GL_FRONT, GL_SPECULAR, full);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
	drawStar(4.5f);
	glPopMatrix();

	glPopMatrix();


	// Star set 3
	glPushMatrix();

	glTranslatef(-20, 0, 0);
	if (!inShadow) glColor3f(0.098, 0.901, 0.556);
	glutSolidCone(5, 10, 40, 40);
	glTranslatef(0, 0, 17);
	if (!inShadow) glColor3f(1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glRotatef(-2 * rotateThings, 0, 0, 1);
	glRotatef(90, 0, 1, 0);
	glutSolidTorus(0.4, 5.5, 40, 40);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-3 * rotateThings, 0, 0, 1);
	if (!inShadow) glColor3f(1.0f, 0.8f, 0.0f);
	glMaterialfv(GL_FRONT, GL_SPECULAR, full);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
	drawStar(4.5f);
	glPopMatrix();

	glPopMatrix();


	// Star set 4
	glPushMatrix();

	glTranslatef(0, -20, 0);
	if (!inShadow) glColor3f(0.098, 0.901, 0.556);
	glutSolidCone(5, 10, 40, 40);
	glTranslatef(0, 0, 17);
	if (!inShadow) glColor3f(0.9f, 0.0f, 0.9f);
	glPushMatrix();
	glRotatef(3 * rotateThings, 0, 0, 1);
	glRotatef(90, 0, 1, 0);
	glutSolidTorus(0.4, 5.5, 40, 40);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-3 * rotateThings, 0, 0, 1);
	if (!inShadow) glColor3f(1.0f, 0.9f, 0.1f);
	glMaterialfv(GL_FRONT, GL_SPECULAR, full);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
	drawStar(4.5f);
	glPopMatrix();

	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	// Set End


	// Center Pyramid
	if (!inShadow) glBindTexture(GL_TEXTURE_2D, textureObjects[PYRAMID_T]);
	glPushMatrix();
	glTranslatef(0, 0, -50);
	if (!inShadow) glColor3f(1, 1, 1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, none);
	glMateriali(GL_FRONT, GL_SHININESS, 0);
	//glutSolidCone(8, 35, 40, 40);
	//gltDrawSphere(15, 40, 40); //GLT
	glRotatef(90, 1, 0, 0); drawPyramid();
	// Pyramid End


	// Teapot
	if (!inShadow) glBindTexture(GL_TEXTURE_2D, textureObjects[TEAPOT_T]);
	glFrontFace(GL_CW);
	glTranslatef(0, 50, 0);
	glRotatef(-rotateThings, 0, 1, 0);
	glRotatef(-30, 0, 0, 1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, full);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
	glutSolidTeapot(7);
	glFrontFace(GL_CCW);
	// Teapot End

	glPopMatrix();


	// Center Cube
	if (!inShadow) glBindTexture(GL_TEXTURE_2D, textureObjects[2]);
	glPushMatrix();
	glTranslatef(0, 0, 30);
	glRotatef(rotateThings, 0, 1, 1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, none);
	glMateriali(GL_FRONT, GL_SHININESS, 10);
	drawCube(5, inShadow);
	glPopMatrix();
	// Center Cube End

	glPopMatrix(); // pop World VIEW SPACE TRANSFORMATION


}




// Object set for reflection
void renderObjects(bool inShadow)
{
	// Normal Scene Begins

	if (!inShadow)
	{
		glDisable(GL_CULL_FACE);

		glDisable(GL_TEXTURE_2D);
		// Light Source Start
		glPushMatrix();
		glColor3f(1, 1, 1);
		glDisable(GL_LIGHTING);
		glRotatef(rotateThings / 2, 0, 0, 1);
		glTranslatef(55, 0, 8);
		glRotatef(3 * rotateThings, 0, 1, 0);

		// Two Lights are positioned at each end of the rotating Torus.
		glLightfv(GL_LIGHT1, GL_POSITION, mylightPos1);
		glLightfv(GL_LIGHT2, GL_POSITION, mylightPos2);
		glutSolidTorus(1, 10, 40, 40);
		glEnable(GL_LIGHTING);
		glPopMatrix();
		// Light Source End


		// The Ruby
		glPushMatrix();
		glRotatef(rotateThings / 2, 0, 0, 1);
		glTranslatef(55, 0, 8);
		glMaterialfv(GL_FRONT, GL_SPECULAR, full);
		glMateriali(GL_FRONT, GL_SHININESS, 128);
		glColor3f(1, 0, 0);
		glScalef(5, 5, 5); // Re-Size
		glutSolidIcosahedron();
		glPopMatrix();
		// Ruby End
		glEnable(GL_TEXTURE_2D);


		// Room
		glBindTexture(GL_TEXTURE_2D, textureObjects[WALL_T]);
		glPushMatrix();

		glMaterialfv(GL_FRONT, GL_SPECULAR, none);
		glMateriali(GL_FRONT, GL_SHININESS, 20);

		//wall
		glPushMatrix();
		glColor3f(0.3, 0.3, 0.3);
		glTranslatef(0, -100, 0);
		drawPlane(100, 1);
		glPopMatrix();

		//wall
		glPushMatrix();
		glTranslatef(0, 100, 0);
		glRotatef(180, 0, 0, 1);
		glColor3f(0.3, 0.3, 0.3);
		drawPlane(100, 1);
		glPopMatrix();

		//wall	
		glPushMatrix();
		glTranslatef(100, 0, 0);
		glRotatef(90, 0, 0, 1);
		glColor3f(0.3, 0.3, 0.3);
		drawPlane(100, 1);
		glPopMatrix();

		//wall	
		glPushMatrix();
		glTranslatef(-100, 0, 0);
		glRotatef(-90, 0, 0, 1);
		glColor3f(0.3, 0.3, 0.3);
		drawPlane(100, 1);
		glPopMatrix();

		//floor
		glBindTexture(GL_TEXTURE_2D, textureObjects[FLOOR_T]);
		glPushMatrix();
		glTranslatef(0, 0, -100);
		glRotatef(90, 1, 0, 0);
		glColor3f(0.3, 0.3, 0.3);
		drawPlane(100, 1);
		glPopMatrix();


		// original ceiling disabled for reflection
		/*
		glBindTexture(GL_TEXTURE_2D, textureObjects[WALL_T]);
		glPushMatrix();
		glTranslatef(0, 0, 100);
		glRotatef(-90, 1, 0, 0);
		glColor3f(0.3, 0.3, 0.3);
		drawPlane(100, 1);
		glPopMatrix();
		*/

		glPopMatrix();

		glEnable(GL_CULL_FACE);

		// Room end
	}

	// glEnable(GL_CULL_FACE);

	// Center Set Begin
	glDisable(GL_TEXTURE_2D);

	// Star set 1
	glPushMatrix();

	glTranslatef(20, 0, 0);
	if (!inShadow) glColor3f(0.098, 0.901, 0.556);
	glutSolidCone(5, 10, 40, 40);
	glTranslatef(0, 0, 17);
	if (!inShadow) glColor3f(0.0f, 0.0f, 1.0f);
	glPushMatrix();
	glRotatef(-3 * rotateThings, 0, 0, 1);
	glRotatef(90, 0, 1, 0);
	glutSolidTorus(0.4, 5.5, 40, 40);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-4 * rotateThings, 0, 0, 1);
	if (!inShadow) glColor3f(0.9f, 0.9f, 0.1f);
	glMaterialfv(GL_FRONT, GL_SPECULAR, full);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
	drawStar(4.5f);
	glPopMatrix();

	glPopMatrix();



	// Star set 2
	glPushMatrix();

	glTranslatef(0, 20, 0);
	if (!inShadow) glColor3f(0.098, 0.901, 0.556);
	glutSolidCone(5, 10, 40, 40);
	glTranslatef(0, 0, 17);
	if (!inShadow) glColor3f(0.0f, 1.0f, 0.0f);
	glPushMatrix();
	glRotatef(2 * rotateThings, 0, 0, 1);
	glRotatef(90, 0, 1, 0);
	glutSolidTorus(0.4, 5.5, 40, 40);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-2 * rotateThings, 0, 0, 1);
	if (!inShadow) glColor3f(0.8f, 1.0f, 0.0f);
	glMaterialfv(GL_FRONT, GL_SPECULAR, full);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
	drawStar(4.5f);
	glPopMatrix();

	glPopMatrix();


	// Star set 3
	glPushMatrix();

	glTranslatef(-20, 0, 0);
	if (!inShadow) glColor3f(0.098, 0.901, 0.556);
	glutSolidCone(5, 10, 40, 40);
	glTranslatef(0, 0, 17);
	if (!inShadow) glColor3f(1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glRotatef(-2 * rotateThings, 0, 0, 1);
	glRotatef(90, 0, 1, 0);
	glutSolidTorus(0.4, 5.5, 40, 40);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-3 * rotateThings, 0, 0, 1);
	if (!inShadow) glColor3f(1.0f, 0.8f, 0.0f);
	glMaterialfv(GL_FRONT, GL_SPECULAR, full);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
	drawStar(4.5f);
	glPopMatrix();

	glPopMatrix();


	// Star set 4
	glPushMatrix();

	glTranslatef(0, -20, 0);
	if (!inShadow) glColor3f(0.098, 0.901, 0.556);
	glutSolidCone(5, 10, 40, 40);
	glTranslatef(0, 0, 17);
	if (!inShadow) glColor3f(0.9f, 0.0f, 0.9f);
	glPushMatrix();
	glRotatef(3 * rotateThings, 0, 0, 1);
	glRotatef(90, 0, 1, 0);
	glutSolidTorus(0.4, 5.5, 40, 40);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-3 * rotateThings, 0, 0, 1);
	if (!inShadow) glColor3f(1.0f, 0.9f, 0.1f);
	glMaterialfv(GL_FRONT, GL_SPECULAR, full);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
	drawStar(4.5f);
	glPopMatrix();

	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	// Set End


	// Pyramid
	if (!inShadow) glBindTexture(GL_TEXTURE_2D, textureObjects[PYRAMID_T]);
	glPushMatrix();
	glTranslatef(0, 0, -50);
	if (!inShadow) glColor3f(1, 1, 1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, none);
	glMateriali(GL_FRONT, GL_SHININESS, 0);
	//glutSolidCone(8, 35, 40, 40);
	//gltDrawSphere(15, 40, 40); //GLT
	glRotatef(90, 1, 0, 0); drawPyramid();
	// Pyramid

	// Teapot
	if (!inShadow) glBindTexture(GL_TEXTURE_2D, textureObjects[TEAPOT_T]);
	glFrontFace(GL_CW);
	glTranslatef(0, 50, 0);
	glRotatef(-rotateThings, 0, 1, 0);
	glRotatef(-30, 0, 0, 1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, full);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
	glutSolidTeapot(7);
	glFrontFace(GL_CCW);
	// Teapot

	glPopMatrix();


	// Center Cube
	if (!inShadow) glBindTexture(GL_TEXTURE_2D, textureObjects[CUBE_T]);
	glPushMatrix();
	glTranslatef(0, 0, 30);
	glRotatef(rotateThings, 0, 1, 1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, none);
	glMateriali(GL_FRONT, GL_SHININESS, 10);
	drawCube(5, inShadow);
	glPopMatrix();


}
// Reflection Scene End



///////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void)
{
	adjustLight();
	warped = false;
	objectYRotation = fmodf(objectYRotation + 1, 360.0);

	GLUquadricObj *pObj;	// Quadric Object
	pObj = gluNewQuadric();
	gluQuadricNormals(pObj, GLU_SMOOTH);

	GLfloat horizontalMovement = 1;
	GLfloat verticalMovement = 0;

	horizontalMovement = MOVE_DELTA;
	if (lookDown)
	{
		xRotationAngle -= ANGLE_DELTA;
		if (xRotationAngle <= -90)
			xRotationAngle = -90;
	}
	if (lookUp)
	{
		xRotationAngle += ANGLE_DELTA;
		if (xRotationAngle >= 90)
			xRotationAngle = 90;
	}
	if (lookLeft)
	{
		yRotationAngle -= ANGLE_DELTA;
		if (yRotationAngle <= 0)
			yRotationAngle = 360;
	}
	if (lookRight)
	{
		yRotationAngle += ANGLE_DELTA;
		if (yRotationAngle >= 360)
			yRotationAngle = 0;
	}
	horizontalMovement = cos(xRotationAngle*PI / 180)*MOVE_DELTA;
	verticalMovement = sin(xRotationAngle*PI / 180)*MOVE_DELTA;
	if (walkForward)
	{
		zCameraLocation -= cos(yRotationAngle*PI / 180)*horizontalMovement;
		xCameraLocation += sin(yRotationAngle*PI / 180)*horizontalMovement;
		yCameraLocation += verticalMovement;
	}
	if (walkBackward)
	{
		zCameraLocation += cos(yRotationAngle*PI / 180)*horizontalMovement;
		xCameraLocation -= sin(yRotationAngle*PI / 180)*horizontalMovement;
		yCameraLocation -= verticalMovement;
	}
	if (strafeRight)
	{
		zCameraLocation -= cos((yRotationAngle + 90)*PI / 180)*MOVE_DELTA;
		xCameraLocation += sin((yRotationAngle + 90)*PI / 180)*MOVE_DELTA;
	}
	if (strafeLeft)
	{
		zCameraLocation -= cos((yRotationAngle - 90)*PI / 180)*MOVE_DELTA;
		xCameraLocation += sin((yRotationAngle - 90)*PI / 180)*MOVE_DELTA;
	}

	rotateThings += 1.7; // Rotate speed


						 // Calculate projection matrix to draw shadow on the ground
	GLfloat shadowMat[4][4];


	GLfloat	 lightNewPos[] = { 55.0*cos((rotateThings / 2)*PI / 180.0),
		0,
		-55.0*sin((rotateThings / 2)*PI / 180.0), 1.0f };



	// This roughly captures each four positions of the light.
	// Then changes the normal vectors appropriately for each side of the walls.	
	if (-0.8 < sin((rotateThings / 2)*PI / 180.0) && sin((rotateThings / 2)*PI / 180.0) < 0.8 && cos((rotateThings / 2)*PI / 180.0) >= 0)
	{
		GLfloat groundPlane[] = { 1.0f, 0.0f, 0.0f, 100.0f };
		shadowMatrix(shadowMat, groundPlane, lightNewPos);
	}
	else if (-0.8 < sin((rotateThings / 2)*PI / 180.0) && sin((rotateThings / 2)*PI / 180.0) < 0.8 && cos((rotateThings / 2)*PI / 180.0) <= 0)
	{
		GLfloat groundPlane[] = { -1.0f, 0.0f, 0.0f, 100.0f };
		shadowMatrix(shadowMat, groundPlane, lightNewPos);
	}
	else if (-0.8 < cos((rotateThings / 2)*PI / 180.0) && cos((rotateThings / 2)*PI / 180.0) < 0.8 && sin((rotateThings / 2)*PI / 180.0) >= 0)
	{
		GLfloat groundPlane[] = { 0.0f, 0.0f, -1.0f, 100.0f };
		shadowMatrix(shadowMat, groundPlane, lightNewPos);
	}
	else if (-0.8 < cos((rotateThings / 2)*PI / 180.0) && cos((rotateThings / 2)*PI / 180.0) < 0.8 && sin((rotateThings / 2)*PI / 180.0) <= 0)
	{
		GLfloat groundPlane[] = { 0.0f, 0.0f, 1.0f, 100.0f };
		shadowMatrix(shadowMat, groundPlane, lightNewPos);
	}
	else
	{
		GLfloat groundPlane[] = { 0.0f, 0.0f, 0.0f, 100.0f };
		shadowMatrix(shadowMat, groundPlane, lightNewPos);
	}


	// Shadow Objects
	// Reset Model view matrix stack
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(-xRotationAngle, 1, 0, 0);
	glRotatef(yRotationAngle, 0, 1, 0);
	glTranslatef(-xCameraLocation, -yCameraLocation, -zCameraLocation);


	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	DrawSkyBox(); // Skybox



	// Reflection Begin
	// Slight reflection will be on the ceiling.

	glPushMatrix(); // push World VIEW SPACE TRANSFORMATION
	glRotatef(-90, 1, 0, 0); // 90


	glEnable(GL_CULL_FACE);

	// REFLECTION - objects and light source
	glPushMatrix();
	glScalef(1, 1, -1);
	glFrontFace(GL_CW);
	glTranslatef(0, 0, -200);
	renderObjects(false);
	glFrontFace(GL_CCW);
	glPopMatrix();
	// REFLECTION End



	// Regular Objects Begin

	glDisable(GL_TEXTURE_2D);
	// Light Source Start
	glPushMatrix();
	glColor3f(1, 1, 1);
	glDisable(GL_LIGHTING);
	glRotatef(rotateThings / 2, 0, 0, 1);
	glTranslatef(55, 0, 8);
	glRotatef(3 * rotateThings, 0, 1, 0);

	// Two Lights are positioned at each end of the rotating Torus.
	glLightfv(GL_LIGHT1, GL_POSITION, mylightPos1);
	glLightfv(GL_LIGHT2, GL_POSITION, mylightPos2);
	glutSolidTorus(1, 10, 40, 40);
	glEnable(GL_LIGHTING);
	glPopMatrix();
	// Light Source End


	// The Ruby
	glPushMatrix();
	glRotatef(rotateThings / 2, 0, 0, 1);
	glTranslatef(55, 0, 8);
	glMaterialfv(GL_FRONT, GL_SPECULAR, full);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
	glColor3f(1, 0, 0);
	glScalef(5, 5, 5);
	glutSolidIcosahedron();
	glPopMatrix();
	// Ruby End
	glEnable(GL_TEXTURE_2D);

	

	// Room
	glBindTexture(GL_TEXTURE_2D, textureObjects[WALL_T]);
	glPushMatrix();

	glMaterialfv(GL_FRONT, GL_SPECULAR, none);
	glMateriali(GL_FRONT, GL_SHININESS, 20);

	//wall
	glPushMatrix();
	glColor3f(0.3, 0.3, 0.3);
	glTranslatef(0, -100, 0);
	drawPlane(100, 1);
	glPopMatrix();

	//wall
	glPushMatrix();
	glTranslatef(0, 100, 0);
	glRotatef(180, 0, 0, 1);
	glColor3f(0.3, 0.3, 0.3);
	drawPlane(100, 1);
	glPopMatrix();

	//wall	
	glPushMatrix();
	glTranslatef(100, 0, 0);
	glRotatef(90, 0, 0, 1);
	glColor3f(0.3, 0.3, 0.3);
	drawPlane(100, 1);
	glPopMatrix();

	//wall	
	glPushMatrix();
	glTranslatef(-100, 0, 0);
	glRotatef(-90, 0, 0, 1);
	glColor3f(0.3, 0.3, 0.3);
	drawPlane(100, 1);
	glPopMatrix();

	//floor

	glBindTexture(GL_TEXTURE_2D, textureObjects[FLOOR_T]);
	glPushMatrix();
	glTranslatef(0, 0, -100);
	glRotatef(90, 1, 0, 0);
	glColor3f(0.3, 0.3, 0.3);
	drawPlane(100, 1);
	glPopMatrix();


	//ceiling
	/*
	glBindTexture(GL_TEXTURE_2D, textureObjects[WALL_T]);
	glPushMatrix();
	glTranslatef(0, 0, 100);
	glRotatef(-90, 1, 0, 0);
	glColor3f(0.3, 0.3, 0.3);
	drawPlane(100, 1);
	glPopMatrix();
	*/

	glPopMatrix();
	// Room end

	// Regular Objects End





	//floor
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, textureObjects[CEILING_T]);
	glDisable(GL_CULL_FACE);

	glPushMatrix();
	glColor4f(1, 1, 1, 0.8);
	glTranslatef(0, 0, 100);
	glRotatef(90, 1, 0, 0);
	drawReflect(100, 1);
	glPopMatrix();

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	glPopMatrix(); // pop World VIEW SPACE TRANSFORMATION


	// draw the SHADOW
	glPushMatrix();
	glMultMatrixf((GLfloat*)shadowMat);
	glDisable(GL_DEPTH_TEST); //turn of depth bc of Z fighting
	glDisable(GL_LIGHTING); //turn off lighting bc we don't want lit shadows
	glColor3f(0.04, 0.04, 0.04); //black shadows
	shadowScene(true);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
	// SHADOW End
	
	
	shadowScene(false); // Regular Center Object

	glPushMatrix(); // push World VIEW SPACE TRANSFORMATION
	glRotatef(-90, 1, 0, 0); // 90


	// TheParticle
	glPushMatrix();
		glTranslatef(0, 0, -3);
		drawParticles();
	glPopMatrix();
	// TheParticle End

	glPopMatrix(); // pop World VIEW SPACE TRANSFORMATION

	glutSwapBuffers();
}


void TimerFunction(int value)
{
	// Redraw the scene with new coordinates
	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}

///////////////////////////////////////////////////////////
// Setup the rendering context
void SetupRC(void)
{
	GLbyte *pBytes;
	GLint iWidth, iHeight, iComponents;
	GLenum eFormat;
	//
	lookUp = false;
	lookDown = false;
	lookLeft = false;
	lookRight = false;
	walkForward = false;
	walkBackward = false;
	strafeLeft = false;
	strafeRight = false;
	yRotationAngle = 0;
	xRotationAngle = 0;
	zRotationAngle = 0;
	xCameraLocation = 0;
	yCameraLocation = 0;
	zCameraLocation = 0;

	// Background Color
	glClearColor(0.5f, 0.95f, 1.0f, 1.0f);

	// Set color shading model to flat
	glShadeModel(GL_SMOOTH);


	// Clock wise wound polygons are front facing, this is reversed
	// because we are using triangle fans
	//glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);

	//depth test turned on (on be default)
	glEnable(GL_DEPTH_TEST);

	// Enable lighting
	glEnable(GL_LIGHTING);

	// Setup and enable light 0
	// Supply a slight ambient light so the objects can be seen
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	// The light is composed of just a diffuse and specular components

	//Additional Lights
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT2, GL_POSITION, lightPos);

	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);


	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// All materials hereafter have full specular reflectivity
	// with a high shine
	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glMateriali(GL_FRONT, GL_SHININESS, 128);

	glEnable(GL_NORMALIZE);

	glEnable(GL_TEXTURE_2D);

	glutSetCursor(GLUT_CURSOR_NONE);


	// Initial Camera Position
	xCameraLocation = 0;
	yCameraLocation = 0.5;
	zCameraLocation = 100.0;


	// Load General object textures
	glGenTextures(NUM_TEXTURES, textureObjects);

	glBindTexture(GL_TEXTURE_2D, textureObjects[WALL_T]);
	pBytes = gltLoadTGA("txKWall.tga", &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glBindTexture(GL_TEXTURE_2D, textureObjects[FLOOR_T]);
	pBytes = gltLoadTGA("txKFloor.tga", &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glBindTexture(GL_TEXTURE_2D, textureObjects[CUBE_T]);
	pBytes = gltLoadTGA("txKCube.tga", &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glBindTexture(GL_TEXTURE_2D, textureObjects[PYRAMID_T]);
	pBytes = gltLoadTGA("txKPyramid.tga", &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glBindTexture(GL_TEXTURE_2D, textureObjects[CEILING_T]);
	pBytes = gltLoadTGA("RT02_50.tga", &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glBindTexture(GL_TEXTURE_2D, textureObjects[TEAPOT_T]);
	pBytes = gltLoadTGA("txKTeapot.tga", &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// Textures bind end


	// SkyBox Texture
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
	// Load Cube Map images
	for (int i = 0; i < 6; i++)
	{
		// Load this texture map

		pBytes = gltLoadTGA(szCubeFaces[i], &iWidth, &iHeight, &iComponents, &eFormat);
		//gluBuild2DMipmaps(cube[i], iComponents, iWidth, iHeight, eFormat, GL_UNSIGNED_BYTE, pBytes);
		glTexImage2D(cube[i], 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
		free(pBytes);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// Skybox Textures binding end



	//Particle Texture
	glBindTexture(GL_TEXTURE_2D, textureObjects[PARTICLE_T]);
	pBytes = gltLoadTGA("particle.tga", &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	initParticles();



}

void ChangeSize(int w, int h)
{

	// Prevent a divide by zero
	if (h == 0)
		h = 1;

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	// Reset projection matrix stack
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	GLfloat fAspect;
	fAspect = (GLfloat)w / (GLfloat)h;

	//frustum that is 45 degrees fovy, aspect ratio of w/h, 1 unit away from the camera, and will clip anything beyond 1000 units
	gluPerspective(45, fAspect, 0.1, 1500);

	// Reset Model view matrix stack
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


}

// Respond to arrow keys by moving the camera frame of reference
void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
		lookDown = true;

	if (key == GLUT_KEY_DOWN)
		lookUp = true;

	if (key == GLUT_KEY_LEFT)
		lookLeft = true;

	if (key == GLUT_KEY_RIGHT)
		lookRight = true;

	// Refresh the Window
	glutPostRedisplay();
}
void SpecialKeysUp(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
		lookDown = false;

	if (key == GLUT_KEY_DOWN)
		lookUp = false;

	if (key == GLUT_KEY_LEFT)
		lookLeft = false;

	if (key == GLUT_KEY_RIGHT)
		lookRight = false;


	glutPostRedisplay();
}

void keyboardFunc(unsigned char key, int x, int y)
{

	switch (key)
	{
	case 'w':
		walkForward = true;
		break;
	case 's':
		walkBackward = true;
		break;
	case 'a':
		strafeLeft = true;
		break;
	case 'd':
		strafeRight = true;
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}

}

void keyboardUpFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		walkForward = false;
		break;
	case 's':
		walkBackward = false;
		break;
	case 'a':
		strafeLeft = false;
		break;
	case 'd':
		strafeRight = false;
		break;
	default:
		break;
	}


}

///////////////////////////////////////////////////////////
// Main program entry point
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Final Project");
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutSpecialFunc(SpecialKeys);
	glutSpecialUpFunc(SpecialKeysUp);
	glutKeyboardUpFunc(keyboardUpFunc);
	glutKeyboardFunc(keyboardFunc);
	glutPassiveMotionFunc(mouseMovement);
	glutTimerFunc(16, TimerFunction, 1);

	SetupRC();

	glutMainLoop();

	return 0;
}

