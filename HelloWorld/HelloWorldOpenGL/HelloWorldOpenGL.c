#include <glut.h>

int width = 1920;
int height = 1080;

void init(int argc, char** argv) {
	glutInit(&argc, argv);					// Initialisierung der GLUT Bibliothek
	glutInitDisplayMode(GLUT_SINGLE);		// Initialisierung des Single Buffer Modes
	glutInitWindowSize(width, height);		// Fenstergröße in Pixel (Breite, Hoehe)
	glutInitWindowPosition(0, 0);		// Fensterposition in Pixel, ausgehend vom Ursprung des Window Systems
	glutCreateWindow("Hello world");		// Erstellen des Fensters
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
}

void display(void)
{
	char* myText = "Hello World!";
	int j;

	glClearColor(0.0, 0.5, 0.0, 0.9);
	glClear(GL_COLOR_BUFFER_BIT);

	GLfloat color[3] = { 0.8, 0.5, 0.2 };
	glColor3fv(color);

	GLfloat offset = -20;
	glBegin(GL_POLYGON);
	glVertex3f((width / 2) - (width / 4) + offset, (height / 2) - (height / 4) + offset, 0.0);
	glVertex3f((width / 2) + (width / 4) + offset, (height / 2) - (height / 4) + offset, 0.0);
	glVertex3f((width / 2) + (width / 4) + offset, (height / 2) + (height / 4) + offset, 0.0);
	glVertex3f((width / 2) - (width / 4) + offset, (height / 2) + (height / 4) + offset, 0.0);
	glEnd();

	GLfloat color2[3] = { 0.9, 0.1, 0.1 };
	glColor3fv(color2);

	glBegin(GL_POLYGON);
	glVertex3f(10, 500, 0.0);
	glVertex3f(300, 600, 0.0);
	glVertex3f(500, 10, 0.0);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2i((width / 2) - (width / 4) + 10, (height / 2) - (height / 4) + 10);
	for (j = 0; j < strlen(myText); j++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, myText[j]);
	}

	glFlush();
}


int main(int argc, char** argv)
{
	init(argc, argv);
	glutDisplayFunc(display);				// Callback-Funktion für das Fenster
	glutMainLoop();							// Abgabe der Kontrolle an GLUT-Bibliothek
	return 0;
}