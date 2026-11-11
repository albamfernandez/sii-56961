#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "DatosMemCompartida.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc,char* argv[])
{
	int i;
	DatosMemCompartida* dat;
	char *direccion;
	int fd;
	
	fd=open("/tmp/datosBot", O_RDWR);
	if (fd == -1)
	{
		perror("Error al abrir el archivo"); 
		return 1;
	}
	direccion=(char*)mmap(NULL, sizeof(DatosMemCompartida), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (direccion == MAP_FAILED)
	{
		perror("Error en la proyeccion mmap"); 
		return 1;
	}
	close(fd);
	dat=(DatosMemCompartida*)direccion;

	while(1)
	{
		usleep(25000);
		float centro_raqueta = (dat->raqueta1.y1 + dat->raqueta1.y2) / 2;
		if (dat->esfera.centro.y > centro_raqueta + 0.25f)
			dat->accion = 1;
		else if (dat->esfera.centro.y < centro_raqueta - 0.25f)
			dat->accion = -1;
		else
			dat->accion = 0;
	}
	return 0;
}
