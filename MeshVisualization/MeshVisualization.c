#include <glut.h>
#include <math.h>
#include <stdio.h>

#pragma warning(disable:4996)


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

float vertices[3 * 60000];
float ccolors[3 * 60000];
int triangle[10 * 3 * 60000];
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
int startx, starty;
int startangle1;
int startangle2;
float startxoff;
float startyoff;
float startzoff;


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
			//////////////////////
			// TODO: set fill mode to render only the vertices
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

			



			//////////////////////  
		}
		if (modus == 2 || modus == 5) { // Display the outlines of the polygons
			//////////////////////
			// TODO: set fill mode to render only outlines
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			


			//////////////////////
		}
		if (modus == 3 || modus == 6) { // Display filled polygons
			//////////////////////
			// TODO: set fill mode to render filled polygons
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			


			/////////////////////
		}
		
		
		glBegin(GL_TRIANGLES); // using the polygone mode "GL_TRIANGLES"
		
		for (i = 0; i < maxcoords + 1; i++)
		{
			if (modus > 3) { // Displaying colors saved in the mesh file (node wise definition!)
				glColor3f(ccolors[triangle[i] * 3], ccolors[triangle[i] * 3 + 1], ccolors[triangle[i] * 3 + 2]);
			}
			else {  // Displaying interpolated colors according to the x-/y-/z-value of the point coordinates (node wise definition!)
				glColor3f((vertices[triangle[i] * 3] - cpointsmin[0]) / range[0], (vertices[triangle[i] * 3 + 1] - cpointsmin[1]) / range[1], (vertices[triangle[i] * 3 + 2] - cpointsmin[2]) / range[2]);
			}


			// Definition of vertices
			/////////////////////////////////////////////////////////////////////
			// TODO: definition of vertices.

			// note the data structures: vertices holds all coordinate components (x,y,z) of all vertices in a 1D array: 
			// vertex j_x can be accessed via vertices[j*3]
			// vertex j_y can be accessed via vertices[j*3+1]
			// vertex j_z can be accessed via vertices[j*3+2]

			// note the data structure: triangle holds the indices into the vertices array (data structure "Knotenliste") in a 1D array 
			// first triangle consists of the vertices referenced in triangle[0], triangle[1], triangle[2].
			// note the for loop running over all triangle indices, i.e. with i = 0 => references first vertex of first triangle

			glVertex3f(vertices[triangle[i] * 3], vertices[triangle[i] * 3 + 1], vertices[triangle[i] * 3 + 2]);
			



			/////////////////////////////////////////////////////////////////////
		}
		glEnd();

	}

}
void display(void)
{
	glPushMatrix();
	gluLookAt(0, -zoom, 0, 0, zoom*zoom + 5, 0, 0.0, 0.0, 1.0);
	glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glColor3f(0.0, 0.0, 0.0);
	// center and rotate
	glTranslatef(xoff, yoff, zoff);
	glRotatef(angle2, 1.0, 0.0, 0.0);
	glRotatef(angle1, 0.0, 0.0, 1.0);
	glTranslatef(-(cpointsmax[0] - cpointsmin[0]) / 2 - cpointsmin[0], -(cpointsmax[1] - cpointsmin[1]) / 2 - cpointsmin[1], -(cpointsmax[2] - cpointsmin[2]) / 2 - cpointsmin[2]);
	//display
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
	glPopMatrix();
	glPopMatrix();
	glutSwapBuffers(); // Buffer for animation needs to be swapped
}

void init(void)
{
	glClearColor(0.99, 0.99, 0.99, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	xoff = 0.0;
	yoff = (cpointsmax[1] - cpointsmin[1]);
	zoff = 0.0;
	zoom = 1;
	angle1 = -45;
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
			fscanf(f, "%f %c", &vertices[i], str);
		else
			fscanf(f, "%f", &vertices[i]);
		// Extremalwerte initialisieren
		if (i < 3)
		{
			cpointsmax[i % 3] = vertices[i];
			cpointsmin[i % 3] = vertices[i];
		}
		//Abbruch, wenn alle Punkte 0 sind, (nicht ganz sauber, aber funktioniert, wenn nicht zufällig der Urspung ein gültiger Punkt ist)
		if (i > 3 && vertices[i - 2] == 0 && vertices[i - 1] == 0 && vertices[i] == 0)
			abbruch = 1;
		//Extremalwerte gegebenenfalls erneuern
		if (vertices[i] > cpointsmax[i % 3] && vertices[i] != 0)
			cpointsmax[i % 3] = vertices[i];
		if (vertices[i] < cpointsmin[i % 3] && vertices[i] != 0)
			cpointsmin[i % 3] = vertices[i];
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
		fscanf_s(f, "%i %c", &triangle[i], str);
		//printf("%i\n",ccoord[i]);
		//Abbruch, wenn alle Dreiecke 0 sind, (nicht ganz sauber, aber funktioniert, wenn nicht zufällig der Urspung ein gültiger Punkt ist)
		if (triangle[i] == -1)
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
		zoff = startzoff - (float)(y - starty) / 100;
	}
	if (pressedbutton == GLUT_MIDDLE_BUTTON)
	{
		yoff = startyoff + ((float)(y - starty) / 100);
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
