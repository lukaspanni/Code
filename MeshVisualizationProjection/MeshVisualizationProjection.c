#include <glut.h>
#include <math.h>
#include <stdio.h>

#pragma warning(disable:4996)

#define ORTHO 1
#define PERSPECTIVE 2

void mouse(int button, int state, int x, int y);
void key(unsigned char key, int x, int y);
void init(void);
void reshape(int, int);
void display(void);
int main(int, char **);
void define_menu();
void idle();
void timer(int value);
void readcloud(char* filename);
void mouseactive(int x, int y);
void mouse(int button, int state, int x, int y);
void setProjection(int projType);
void setAntiAliasing(int state);

float cpoints[3 * 60000];
float ccolors[3 * 60000];
int ccoord[10 * 3 * 60000];
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

int projType = PERSPECTIVE; // default: perspective projection

int main(int argc, char** argv)
{
	readcloud("G:\\bones.txt");		// change this in case the point cloud is saved somewhere else.
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
	printf("'4' Points, Farbwerte aus Datei\n'5' Wireframe, Farbwerte aus Datei\n'6' Filled, Farbwerte aus Datei\n\n\n");
	printf("Transformationen:\n linke Maustaste und x-y-Bewegung -> Rotation\n mittlere Maustaste und y-Richtung -> Zoom (entspricht einer Skalierung)\n");
	printf(" rechte Maustaste und x-y-Bewegung -> Translation\n\n");
	printf("Projektionsart aendern:\n");
	printf("'o' orthographische Projektion, 'p' perspektivische Projektion \n\n");
	glutMainLoop();
	return 0;
}

void displaycloud(int modus)
{
	int i = 0;
	float range[3];
	for (i = 0; i < 3; i++)
		range[i] = cpointsmax[i] - cpointsmin[i];
	if (modus > 0)
	{
		if (modus == 1 || modus == 4) { // Display only the vertices
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
		if (modus == 2 || modus == 5) { // Display the outlines of the polygons
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (modus == 3 || modus == 6) { // Display filled polygons
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		glBegin(GL_TRIANGLES); // using the polygone mode "GL_TRIANGLES"
		for (i = 0; i < maxcoords + 1; i++)
		{
			if (modus > 3) { // Displaying colors saved in the mesh file (node wise definition!)
				glColor3f(ccolors[ccoord[i] * 3], ccolors[ccoord[i] * 3 + 1], ccolors[ccoord[i] * 3 + 2]);
			}
			else {  // Displaying interpolated colors according to the x-/y-/z-value of the point coordinates (node wise definition!)
				glColor3f((cpoints[ccoord[i] * 3] - cpointsmin[0]) / range[0], (cpoints[ccoord[i] * 3 + 1] - cpointsmin[1]) / range[1], (cpoints[ccoord[i] * 3 + 2] - cpointsmin[2]) / range[2]);
			}
			glVertex3f(cpoints[ccoord[i] * 3], cpoints[ccoord[i] * 3 + 1], cpoints[ccoord[i] * 3 + 2]);
		}
		glEnd();

	}

}
void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (projType) {
	case ORTHO:
		///////////////////////////////////////////////////////////////////////// 
		// note: - vertices are initially all in the range between [-1 1].
		//		 - vertices are rotated around the origin as given by the user input.
		//       - use zoff to define the zoom with glOrtho
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-2 - zoff, 2 + zoff, -2 - zoff, 2 + zoff, 3, 7);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

		/////////////////////////////////////////////////////////////////////////
		break;


	case PERSPECTIVE:
		/////////////////////////////////////////////////////////////////////////
		// note: - use gluPerspective here.
		//		 - use zoff to define the zoom with gluLookAt
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0, 1.0, 3.0, 7.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0.0, 0.0, 5.0 + zoff, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);		

		/////////////////////////////////////////////////////////////////////////
		break;

	}

	
	glColor3f(0.0, 0.0, 0.0);
	
	glTranslatef(xoff, yoff, 0);
	glRotatef(angle2, 1.0, 0.0, 0.0);
	glRotatef(angle1, 0.0, 1.0, 0.0);

	//display the point cloud/mesh
	displaycloud(displaymodus);
	
	// draw box
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
	
	glutSwapBuffers(); // Buffer for animation needs to be swapped
}

void init(void)
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.99, 0.99, 0.99, 0.0);
	glLoadIdentity();
	xoff = 0.0;
	yoff = 0.0;
	zoff = 0.0;
	zoom = 1;
	angle1 = 45;
	angle2 = 45;

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
	FILE * f;
	int abbruch = 0;
	char str[200] = "";
	float temp;
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
	default:
		if (k > '0' - 1 && k < '7')
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