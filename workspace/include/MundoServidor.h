// Mundo.h: interface for the CMundo class.
//
//////////////////////////////////////////////////////////////////////

// MundoServidor.h: interface for the CMundo class (servidor).
#pragma once
#include <vector>
#include <pthread.h>
#include "Plano.h"
#include "Esfera.h"
#include "Raqueta.h"

class CMundo  
{
public:
	void Init();
	CMundo();
	virtual ~CMundo();	
	
	void InitGL();	
	void OnKeyboardDown(unsigned char key, int x, int y);
	void OnTimer(int value);
	void OnDraw();
	void RecibeComandosJugador();

	Esfera esfera;
	std::vector<Plano> paredes;
	Plano fondo_izq;
	Plano fondo_dcho;
	Raqueta jugador1;
	Raqueta jugador2;

	int puntos1;
	int puntos2;

	int fd_logger;
	int fd_coord;
	int fd_teclas;
	pthread_t thid1;
};
