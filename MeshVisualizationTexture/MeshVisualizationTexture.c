#include <glut.h>
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ORTHO 1
#define PERSPECTIVE 2

// Width & height of texture
#define HEIGHT 512
#define WIDTH 512

#pragma warning(disable:4996)

void mouse(int button, int state, int x, int y);
void key(unsigned char key, int x, int y);
void init(void);
void reshape(int, int);
void display(void);
int main(int, char**);
void define_menu();
void idle();
void timer(int value);
void readcloud(char* filename);
void mouseactive(int x, int y);
void mouse(int button, int state, int x, int y);
void setProjection(int projType);
void setAntiAliasing(int state);
void readBitmap(void);


float cpoints[3 * 60000];
float cvnormals[3 * 60000];
float ccolors[3 * 60000];
int ccoord[10 * 3 * 60000];
float cnormals[10 * 3 * 60000];
int maxcoords = 0;
float cpointsmax[3];
float cpointsmin[3];
int cpoints_n = 0;

float xoff;
float yoff;
float zoff;
float zoom;
int angle1;
int angle2;

const float stepsize = 0.05;
const float anglestepsize = 0.01;
int displaymodus = 1;
int pressedbutton = 0;
int startx, starty, startz;
int startangle1;
int startangle2;
float startxoff;
float startyoff;
float startzoff;

GLubyte bitmapImage[HEIGHT][WIDTH][4];

// default values
int projType = PERSPECTIVE; // default: perspective projection
int lights = 0;
int shading = 0;
float shininess = 2;
int textureMode = 1;

float ambientLightColor[3] = { 0.1, 0.1, 0.1 };
float diffuseLightColor[3] = { 0.5, 0.5, 0.5 };
float specularLightColor[3] = { 1.0, 1.0, 1.0 };

float lightPosition[4] = { 0,0,1,1 };

// short cut color white
float white[3] = { 0.5, 0.5, 0.5 };

char meshFile[] = "G:\\bones.txt";					// change this in case the point cloud is saved somewhere else.
char textureFile[] = "G:\\boneTexture.bmp";		// change this in case the texture is saved somewhere else.

int main(int argc, char** argv)
{
	readcloud(meshFile);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Doublebuffer for animation
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("Mesh Visualization");
	init();
	glutMouseFunc(mouse);
	glutMotionFunc(mouseactive);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	printf("\n\nSTEUERUNG\nAnzeigemodi:\n");
	printf("'0' nur Box\n'1' Points, Farbwerte nach Koordinate\n'2' Wireframe, Farbwerte nach Koordinate\n'3' Filled, Farbwerte nach Koordinate\n");
	printf("'4' Points, Farbwerte aus Datei\n'5' Wireframe, Farbwerte aus Datei\n'6' Filled, Farbwerte aus Datei\n");
	printf("'7' Texturmodus\n\n\n");
	printf("Transformationen:\n linke Maustaste und x-y-Bewegung -> Rotation\n mittlere Maustaste und y-Richtung -> Zoom (entspricht einer Skalierung)\n");
	printf(" rechte Maustaste und x-y-Bewegung -> Translation\n\n");
	printf("Projektionsart aendern:\n");
	printf("'o' orthographische Projektion, 'p' perspektivische Projektion \n\n");
	printf("Licht Optionen\n");
	printf(" 's'     : Shading Modus aendern (Flat / Gouraud)\n");
	printf(" 'l'     : Licht ein-/ausschalten\n");
	printf(" '+'/'-' : Spekular-Exponent aendern\n\n");
	printf("Textur Optionen\n");
	printf(" 't'     : Automatische Texturkoordinaten aesndern (Object Linear / Eye Linear)\n\n");
	glutMainLoop();
	return 0;
}


void readBitmap(void) {
	int i, j, k;
	GLubyte c;

	FILE* img;
	img = fopen(textureFile, "rb");
	fseek(img, sizeof(unsigned char) * 54, 0); // offset to pixel data

	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			for (k = 0; k < 3; k++) {
				fread(&c, sizeof(GLubyte), 1, img);
				bitmapImage[i][j][k] = (GLubyte)c;
			}
			bitmapImage[i][j][3] = (GLubyte)255;
		}
	}
	fclose(img);
}



void displaycloud(int modus)
{
	int i = 0;
	float range[3];
	float directionVector[3][2];
	float n[3];
	float currentColor[3];
	int counter = 0;

	glEnable(GL_NORMALIZE);
	glFrontFace(GL_CW);

	for (i = 0; i < 3; i++)
		range[i] = cpointsmax[i] - cpointsmin[i];


	if (modus == 7) {
		glEnable(GL_TEXTURE_2D);
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}



	if (modus > 0)
	{
		if (modus == 1 || modus == 4) { // Darstellung von Punkten
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
		if (modus == 2 || modus == 5) { // Darstellung des Drahtgittermodells
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (modus == 3 || modus == 6 || modus == 7) { // Darstellung gefüllter Polygone
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		glBegin(GL_TRIANGLES);
		for (i = 0; i < maxcoords + 1; i++)
		{
			if (modus > 3) { // Darstellung der Farben aus dem Mesh-File
				currentColor[0] = ccolors[ccoord[i] * 3];
				currentColor[1] = ccolors[ccoord[i] * 3 + 1];
				currentColor[2] = ccolors[ccoord[i] * 3 + 2];
			}
			else {  // Darstellung der interpolierten Farben entsprechend der Koordinaten
				currentColor[0] = (cpoints[ccoord[i] * 3] - cpointsmin[0]) / range[0];
				currentColor[1] = (cpoints[ccoord[i] * 3 + 1] - cpointsmin[1]) / range[1];
				currentColor[2] = (cpoints[ccoord[i] * 3 + 2] - cpointsmin[2]) / range[2];
			}

			if (lights == 1) {
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, currentColor);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, currentColor);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, currentColor);
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
			}
			else {
				glColor3f(currentColor[0], currentColor[1], currentColor[2]);
			}

			// for flat shading: one normal per triangle (before defintion of vertices) is sufficient
			// cnormals contains the surface normal
			if (counter == 0) {
				if (shading == 0) {
					glNormal3f(cnormals[i], cnormals[i + 1], cnormals[i + 2]);
				}
			}
			counter++;
			if (counter == 3) {
				counter = 0;
			}

			// for gouraud shading we need the normal of each vertex
			// cvnormals contains the vertex normals
			if (shading == 1) {
				glNormal3f(cvnormals[ccoord[i] * 3], cvnormals[ccoord[i] * 3 + 1], cvnormals[ccoord[i] * 3 + 2]);
			}

			glVertex3f(cpoints[ccoord[i] * 3], cpoints[ccoord[i] * 3 + 1], cpoints[ccoord[i] * 3 + 2]);
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

}
void display(void) {

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// automatic generation of texture coordinates (object/eye linear)

	// Define s and t planes
	GLfloat s_plane[] = { 0,0,1,0 };
	GLfloat t_plane[] = { 0,1,0,0 };

	if (textureMode == 1)
	{
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	}
	else if (textureMode == 2)
	{
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	}
	glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	if (lights == 1) {

		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLightColor);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLightColor);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specularLightColor);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

		glEnable(GL_LIGHTING);

		if (shading == 0) { // Flat Shading
			glShadeModel(GL_FLAT);
		}
		else if (shading == 1) { // Gouraud Shading
			glShadeModel(GL_SMOOTH);
		}
	}
	else {
		glDisable(GL_LIGHTING);
	}

	// projection switch
	switch (projType) {
	case ORTHO:
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-2 - zoff, 2 + zoff, -2 - zoff, 2 + zoff, -2, 10);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0.0, 0.0, 0.01, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		break;

	case PERSPECTIVE:
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0, 1.0, 3.0, 7.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0, 0, 5 + zoff, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		break;
	}



	glPushMatrix();

	// enable depth buffer and clear color/depth buffer
	glClearDepth(1);			// Default: 1 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);	// Default: GL_LESS


	glColor3f(0.0, 0.0, 0.0);
	// center and rotate
	glTranslatef(xoff, yoff, 0);
	glRotatef(angle2, 1.0, 0.0, 0.0);
	glRotatef(angle1, 0.0, 1.0, 0.0);
	//display
	displaycloud(displaymodus);

	// draw box
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex3f(cpointsmax[0], cpointsmax[1], cpointsmax[2]);
	glVertex3f(cpointsmin[0], cpointsmax[1], cpointsmax[2]);
	glVertex3f(cpointsmin[0], cpointsmin[1], cpointsmax[2]);
	glVertex3f(cpointsmax[0], cpointsmin[1], cpointsmax[2]);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3f(cpointsmax[0], cpointsmax[1], cpointsmin[2]);
	glVertex3f(cpointsmin[0], cpointsmax[1], cpointsmin[2]);
	glVertex3f(cpointsmin[0], cpointsmin[1], cpointsmin[2]);
	glVertex3f(cpointsmax[0], cpointsmin[1], cpointsmin[2]);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(cpointsmax[0], cpointsmax[1], cpointsmax[2]);
	glVertex3f(cpointsmax[0], cpointsmax[1], cpointsmin[2]);
	glVertex3f(cpointsmin[0], cpointsmax[1], cpointsmax[2]);
	glVertex3f(cpointsmin[0], cpointsmax[1], cpointsmin[2]);
	glVertex3f(cpointsmin[0], cpointsmin[1], cpointsmax[2]);
	glVertex3f(cpointsmin[0], cpointsmin[1], cpointsmin[2]);
	glVertex3f(cpointsmax[0], cpointsmin[1], cpointsmax[2]);
	glVertex3f(cpointsmax[0], cpointsmin[1], cpointsmin[2]);
	glEnd();
	glPopMatrix();
	glPopMatrix();
	glutSwapBuffers(); // Buffer for animation needs to be swapped
}


void init(void)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glClearColor(0.99, 0.99, 0.99, 0.0);
	glLoadIdentity();
	xoff = 0.0;
	yoff = 0.0;
	zoff = 0.0;
	zoom = 1;
	angle1 = 45;
	angle2 = 45;

	//////////////////////////////////////////////////////////////////////////////////////////////////////

	// Read bitmap file
	readBitmap();

	// Texture wrap settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Filter settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Connecting lighting and texture
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Initialize texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmapImage);

	// enable automatic texture generation
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);


	//////////////////////////////////////////////////////////////////////////////////////////////////////

}



void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT);
}

void idle()
{
}

void timer(int value)
{
}

void readcloud(char* filename)
{

	int i = 0;
	int j = 0;
	int k = 0;
	int numVertices = 0;
	int counter = 0;
	float directionVector[3][2];
	float n[3];
	float x, y, z;
	float temp;
	int index;
	int indexBegin;
	int numNeighbouringFaces = 0;
	FILE* f;
	int abbruch = 0;
	char str[200] = "";
	printf("Lese '%s' ein\n", filename);

	f = fopen(filename, "r");


	printf("Ueberspringe Kopf...\n");
	// Kopf Überspringen
	while (!feof(f) && str[0] != '[')
		fscanf(f, "%s", str);
	printf("Lese Punkte ein...\n");
	//Punkte einlesen
	while (!feof(f) && abbruch == 0)
	{
		//einlesen
		if (((i + 1) % 3) == 0)
			fscanf(f, "%f %c", &cpoints[i], str);
		else
			fscanf(f, "%f", &cpoints[i]);
		// Extremalwerte initialisieren
		if (i < 3)
		{
			cpointsmax[i % 3] = cpoints[i];
			cpointsmin[i % 3] = cpoints[i];
		}
		//Abbruch, wenn alle Punkte 0 sind, (nicht ganz sauber, aber funktioniert, wenn nicht zufällig der Urspung ein gültiger Punkt ist)
		if (i > 3 && cpoints[i - 2] == 0 && cpoints[i - 1] == 0 && cpoints[i] == 0)
			abbruch = 1;
		//Extremalwerte gegebenenfalls erneuern
		if (cpoints[i] > cpointsmax[i % 3] && cpoints[i] != 0)
			cpointsmax[i % 3] = cpoints[i];
		if (cpoints[i] < cpointsmin[i % 3] && cpoints[i] != 0)
			cpointsmin[i % 3] = cpoints[i];
		i++;
	}
	cpoints_n = i - 1;
	printf("Es wurden %i Vertices gelesen\n", cpoints_n / 3);
	printf("Koordinaten sind in den Intervallen  [%f,%f]  [%f,%f] [%f,%f]\n\n", cpointsmin[0], cpointsmax[0], cpointsmin[1], cpointsmax[1], cpointsmin[2], cpointsmax[2]);
	abbruch = 0; i = 0;
	//warten, bis es zu den colors geht
	while (!feof(f) && str[0] != '[')
		fscanf(f, "%s", str);
	printf("Lese Farben ein...\n");
	// Farben einlesen
	while (!feof(f) && abbruch == 0)
	{
		//einlesen
		if (((i + 1) % 3) == 0)
			fscanf(f, "%f %c", &ccolors[i], str);
		else
			fscanf(f, "%f", &ccolors[i]);
		//Abbruch, wenn alle farben 0 sind, (nicht ganz sauber, aber funktioniert, wenn nicht zufällig schwarz eine gültige Farbe ist)
		if (i > 3 && ccolors[i - 2] == 0 && ccolors[i - 1] == 0 && ccolors[i] == 0)
			abbruch = 1;
		i++;
	}
	printf("Es wurden %i Farben eingelesen\n\n", (i - 1) / 3);
	abbruch = 0; i = 0;
	//warten, bis es zu den koordinaten geht
	while (!feof(f) && str[0] != '[')
		fscanf(f, "%s", str);
	printf("Lese Koordinaten fuer die Dreiecke ein...\n");
	// Koordinaten einlesen
	while (!feof(f) && abbruch < 2)
	{
		//einlesen
		fscanf(f, "%i %c", &ccoord[i], str);
		//printf("%i\n",ccoord[i]);
		//Abbruch, wenn alle Dreiecke 0 sind, (nicht ganz sauber, aber funktioniert, wenn nicht zufällig der Urspung ein gültiger Punkt ist)
		if (ccoord[i] == -1)
		{
			i--;
			abbruch++;
		}
		else
			abbruch = 0;
		i++;
	}
	maxcoords = i - 1;
	printf("Es wurden %i Dreiecke eingelesen\n", (maxcoords + 1) / 3);// drei Punkte bilden ein Dreieck
	fclose(f);
	printf("Einlesen beendet\n\n");

	for (j = 0; j < cpoints_n; j++) {
		// normalize
		cpoints[j] = cpoints[j] - cpointsmin[j % 3];
		cpoints[j] = 2 * cpoints[j] / (cpointsmax[j % 3] - cpointsmin[j % 3]);
		cpoints[j] = cpoints[j] - 1;
	}
	cpointsmin[0] = -1;
	cpointsmin[1] = -1;
	cpointsmin[2] = -1;

	cpointsmax[0] = 1;
	cpointsmax[1] = 1;
	cpointsmax[2] = 1;

	for (j = 0; j < cpoints_n; j++) {
		if (j % 3 == 1) { // y-coordinate change with z-coordinate
			temp = cpoints[j];
			cpoints[j] = cpoints[j + 1];
			cpoints[j + 1] = temp;
		}
	}

	printf("Berechne Flächen- und Vertexnormalen...\n");
	counter = 0;
	for (i = 0; i < maxcoords + 1; i++) {
		if (counter == 0) {
			// Richtungsvektoren der Ebene aus jeweils zwei Seiten des Dreiecks
			directionVector[0][0] = cpoints[ccoord[i + 1] * 3] - cpoints[ccoord[i] * 3];
			directionVector[1][0] = cpoints[ccoord[i + 1] * 3 + 1] - cpoints[ccoord[i] * 3 + 1];
			directionVector[2][0] = cpoints[ccoord[i + 1] * 3 + 2] - cpoints[ccoord[i] * 3 + 2];

			directionVector[0][1] = cpoints[ccoord[i + 2] * 3] - cpoints[ccoord[i] * 3];
			directionVector[1][1] = cpoints[ccoord[i + 2] * 3 + 1] - cpoints[ccoord[i] * 3 + 1];
			directionVector[2][1] = cpoints[ccoord[i + 2] * 3 + 2] - cpoints[ccoord[i] * 3 + 2];

			// Normalenvektor als Kreuzprodukt der beiden Seiten
			n[0] = (directionVector[1][0] * directionVector[2][1]) - (directionVector[2][0] * directionVector[1][1]);
			n[1] = (directionVector[2][0] * directionVector[0][1]) - (directionVector[0][0] * directionVector[2][1]);
			n[2] = (directionVector[0][0] * directionVector[1][1]) - (directionVector[1][0] * directionVector[0][1]);

			// Normalenvektor in Array speichern
			cnormals[i] = n[0];
			cnormals[i + 1] = n[1];
			cnormals[i + 2] = n[2];

			// Aufaddieren der Normalen an den Betroffenen Vertices, die das Dreieck bilden
			cvnormals[ccoord[i] * 3] = cvnormals[ccoord[i] * 3] + n[0];
			cvnormals[ccoord[i] * 3 + 1] = cvnormals[ccoord[i] * 3 + 1] + n[1];
			cvnormals[ccoord[i] * 3 + 2] = cvnormals[ccoord[i] * 3 + 2] + n[2];

			cvnormals[ccoord[i + 1] * 3] = cvnormals[ccoord[i + 1] * 3] + n[0];
			cvnormals[ccoord[i + 1] * 3 + 1] = cvnormals[ccoord[i + 1] * 3 + 1] + n[1];
			cvnormals[ccoord[i + 1] * 3 + 2] = cvnormals[ccoord[i + 1] * 3 + 2] + n[2];

			cvnormals[ccoord[i + 2] * 3] = cvnormals[ccoord[i + 2] * 3] + n[0];
			cvnormals[ccoord[i + 2] * 3 + 1] = cvnormals[ccoord[i + 2] * 3 + 1] + n[1];
			cvnormals[ccoord[i + 2] * 3 + 2] = cvnormals[ccoord[i + 2] * 3 + 2] + n[2];

		}
		counter++;
		if (counter == 3) {
			counter = 0;
		}
	}
	printf("... beendet.\n");
}





void key(unsigned char k, int x, int y);
void mouseactive(int x, int y)
{
	if (pressedbutton == GLUT_LEFT_BUTTON)
	{
		angle1 = startangle1 + (x - startx) / 10;
		angle2 = startangle2 + (y - starty) / 10;
	}
	if (pressedbutton == GLUT_RIGHT_BUTTON)
	{
		xoff = startxoff + (float)(x - startx) / 100;
		yoff = startyoff + (float)(y - starty) / 100;
	}
	if (pressedbutton == GLUT_MIDDLE_BUTTON)
	{
		zoff = startzoff + ((float)(y - startz) / 100);
	}
	glutPostRedisplay();
}
void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		pressedbutton = button;
		startx = x;
		starty = y;
		startz = y;
		startangle1 = angle1;
		startangle2 = angle2;
		startxoff = xoff;
		startyoff = yoff;
		startzoff = zoff;
	}
	else
		pressedbutton = 0;

}


void MainMenu(int value)
{
	switch (value) {

	case 2:
		key('q', 0, 0);
		break;

	}
}

void
submenu1(int value)
{
}


void define_menu()
{
}

void key(unsigned char k, int x, int y)
{
	switch (k) {
	case 8:  //BACKSPACE
		init();
		break;
	case 27:
	case 'q':
	case 'Q':
		exit(0);
	case 'o':
		projType = ORTHO;
		printf("Projektion: ORTHOGRAPHIC\n");
		glutPostRedisplay();
		break;
	case 'p':
		projType = PERSPECTIVE;
		printf("Projektion: PERSPECTIVE\n");
		glutPostRedisplay();
		break;
	case 'l':
		if (lights == 0)
			lights = 1;
		else
			lights = 0;
		break;
	case '+':
		shininess = shininess + 0.1;
		printf("  Shininess: %f\n", shininess);
		break;
	case '-':
		shininess = shininess - 0.1;
		printf("  Shininess: %f\n", shininess);
		break;
	case 's':
		if (shading == 1) {
			shading = 0;
			printf("  Shading = FLAT\n");
		}
		else if (shading == 0) {
			shading = 1;
			printf("  Shading = GOURAUD\n");
		}
		break;
	case 't':
		if (textureMode == 1) {
			textureMode = 2;
			printf("  Texture Mode = EYE LINEAR\n");
		}
		else if (textureMode == 2) {
			textureMode = 1;
			printf("  Texture Mode = OBJECT LINEAR\n");
		}
		break;
	default:
		if (k > '0' - 1 && k < '8')
		{
			displaymodus = k - '0';
			printf("Display-Modus: %i\n", displaymodus);
		}
		else
		{
			printf("Taste %c mit Steuerzeichen %i nicht belegt\n", k, k);
		}
		break;
	}
	glutPostRedisplay();
}