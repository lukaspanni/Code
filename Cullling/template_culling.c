#include <glut.h>


int width = 1920;
int height = 1080;

void init(int argc, char** argv) {
	glutInit(&argc, argv);					
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);		
	glutInitWindowSize(width, height);		
	glutInitWindowPosition(100, 100);		
	glViewport(0,0,width,height);
	glutCreateWindow("Culling");		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glOrtho(-2.0, 2.0,-2.0, 2.0, -2.0, 2.0);
	gluLookAt(0.25,0.5,0.1,0,0,0,0,1,0);


	////////////////////////////////////////////////////////////////////


	

	////////////////////////////////////////////////////////////////////
}


void display(void)
{

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glBegin(GL_QUADS);

		glColor3f(1.0f, 0.0f, 0.0f);
		// FRONT
		glVertex3f(-0.5f, -0.5f,  0.5f);
		glVertex3f( 0.5f, -0.5f,  0.5f);
		glVertex3f( 0.5f,  0.5f,  0.5f);
		glVertex3f(-0.5f,  0.5f,  0.5f);
		// BACK
		glVertex3f(-0.5f, -0.5f, -0.5f);
		glVertex3f(-0.5f,  0.5f, -0.5f);
		glVertex3f( 0.5f,  0.5f, -0.5f);
		glVertex3f( 0.5f, -0.5f, -0.5f);

		glColor3f(0.0f, 1.0f, 0.0f);
		// LEFT
		glVertex3f(-0.5f, -0.5f,  0.5f);
		glVertex3f(-0.5f,  0.5f,  0.5f);
		glVertex3f(-0.5f,  0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);
		// RIGHT
		glVertex3f( 0.5f, -0.5f, -0.5f);
		glVertex3f( 0.5f,  0.5f, -0.5f);
		glVertex3f( 0.5f,  0.5f,  0.5f);
		glVertex3f( 0.5f, -0.5f,  0.5f);

		glColor3f(0.0f, 0.0f, 1.0f);
		// BOTTOM
		glVertex3f(-0.5f, -0.5f,  0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);
		glVertex3f( 0.5f, -0.5f, -0.5f);
		glVertex3f( 0.5f, -0.5f,  0.5f);
	glEnd();

	glFlush();
}


int main(int argc, char** argv)
{
	init(argc, argv);
	glutDisplayFunc(display);
	glutMainLoop();	
	return 0;
}