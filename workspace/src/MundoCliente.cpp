// Mundo.cpp: implementation of the CMundo class.
//
//////////////////////////////////////////////////////////////////////
#include <fstream>
#include "MundoCliente.h"
#include "glut.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMundo::CMundo()
{
	Init();
}

CMundo::~CMundo()
{
	close(fd_coord);
	close(fd_teclas);
	unlink("/tmp/FIFO_TECLAS");
	munmap(pdatos, sizeof(DatosMemCompartida));
	unlink("/tmp/datosBot");
}

void CMundo::InitGL()
{
	//Habilitamos las luces, la renderizacion y el color de los materiales
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	
	glMatrixMode(GL_PROJECTION);
	gluPerspective( 40.0, 800/600.0f, 0.1, 150);
}

void print(char *mensaje, int x, int y, float r, float g, float b)
{
	glDisable (GL_LIGHTING);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT) );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glColor3f(r,g,b);
	glRasterPos3f(x, glutGet(GLUT_WINDOW_HEIGHT)-18-y, 0);
	int len = strlen (mensaje );
	for (int i = 0; i < len; i++) 
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, mensaje[i] );
		
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable( GL_DEPTH_TEST );
}
void CMundo::OnDraw()
{
	//Borrado de la pantalla	
   	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Para definir el punto de vista
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	
	gluLookAt(0.0, 0, 17,  // posicion del ojo
		0.0, 0.0, 0.0,      // hacia que punto mira  (0,0,0) 
		0.0, 1.0, 0.0);      // definimos hacia arriba (eje Y)    

	/////////////////
	///////////
	//		AQUI EMPIEZA MI DIBUJO
	char cad[100];
	sprintf(cad,"Jugador1: %d",puntos1);
	print(cad,10,0,1,1,1);
	sprintf(cad,"Jugador2: %d",puntos2);
	print(cad,650,0,1,1,1);
	int i;
	for(i=0;i<paredes.size();i++)
		paredes[i].Dibuja();

	fondo_izq.Dibuja();
	fondo_dcho.Dibuja();
	jugador1.Dibuja();
	jugador2.Dibuja();
	esfera.Dibuja();

	/////////////////
	///////////
	//		AQUI TERMINA MI DIBUJO
	////////////////////////////

	//Al final, cambiar el buffer
	glutSwapBuffers();
}

void CMundo::OnTimer(int value)
{
	// Recibir el estado del juego del servidor
	char cad[200];
	int n = read(fd_coord, cad, sizeof(cad));
	if (n <= 0)   // el servidor ha terminado
	{
		pdatos->fin = 1;
		exit(0);
	}
	sscanf(cad, "%f %f %f %f %f %f %f %f %f %f %d %d",
		&esfera.centro.x, &esfera.centro.y,
		&jugador1.x1, &jugador1.y1, &jugador1.x2, &jugador1.y2,
		&jugador2.x1, &jugador2.y1, &jugador2.x2, &jugador2.y2,
		&puntos1, &puntos2);

	// Datos para el bot
	pdatos->esfera = esfera;
	pdatos->raqueta1 = jugador1;
	pdatos->puntos1 = puntos1;
	pdatos->puntos2 = puntos2;

	// Accion del bot: se envia al servidor solo cuando cambia
	if (pdatos->accion != accion_anterior)
	{
		if (pdatos->accion == 1)
			OnKeyboardDown('w', 0, 0);
		else if (pdatos->accion == -1)
			OnKeyboardDown('s', 0, 0);
		else
			OnKeyboardDown('x', 0, 0);
		accion_anterior = pdatos->accion;
	}
}

void CMundo::OnKeyboardDown(unsigned char key, int x, int y)
{
	char cad[10];
	sprintf(cad, "%c", key);
	write(fd_teclas, cad, 1);
}

void CMundo::Init()
{
	Plano p;
//pared inferior
	p.x1=-7;p.y1=-5;
	p.x2=7;p.y2=-5;
	paredes.push_back(p);

//superior
	p.x1=-7;p.y1=5;
	p.x2=7;p.y2=5;
	paredes.push_back(p);

	fondo_izq.r=0;
	fondo_izq.x1=-7;fondo_izq.y1=-5;
	fondo_izq.x2=-7;fondo_izq.y2=5;

	fondo_dcho.r=0;
	fondo_dcho.x1=7;fondo_dcho.y1=-5;
	fondo_dcho.x2=7;fondo_dcho.y2=5;

	//a la izq
	jugador1.g=0;
	jugador1.x1=-6;jugador1.y1=-1;
	jugador1.x2=-6;jugador1.y2=1;

	//a la dcha
	jugador2.g=0;
	jugador2.x1=6;jugador2.y1=-1;
	jugador2.x2=6;jugador2.y2=1;

	// Tuberia de coordenadas servidor -> cliente (la crea el servidor)
	fd_coord = open("/tmp/FIFO_COORD", O_RDONLY);
	if (fd_coord < 0)
		perror("No puede abrirse la FIFO de coordenadas");

	// Tuberia de teclas cliente -> servidor
	mkfifo("/tmp/FIFO_TECLAS", 0666);
	fd_teclas = open("/tmp/FIFO_TECLAS", O_WRONLY);
	if (fd_teclas < 0)
		perror("No puede abrirse la FIFO de teclas");

	// Memoria compartida con el bot
	int fd_mem = open("/tmp/datosBot", O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (fd_mem < 0)
		perror("Error al crear el fichero");
	ftruncate(fd_mem, sizeof(DatosMemCompartida));
	void* dir = mmap(NULL, sizeof(DatosMemCompartida), PROT_READ | PROT_WRITE, MAP_SHARED, fd_mem, 0);
	if (dir == MAP_FAILED)
		perror("Error en la proyeccion mmap");
	close(fd_mem);
	pdatos = (DatosMemCompartida*)dir;
	pdatos->accion = 0;
	pdatos->puntos1 = 0;
	pdatos->puntos2 = 0;
	pdatos->fin = 0;
	accion_anterior = 0;
}
