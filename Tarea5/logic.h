#ifndef LOGIC_H
#define LOGIC_H

#define CANTIDAD 200

// Buffers de datos generados
extern char bufA[CANTIDAD];
extern char bufB[CANTIDAD];

// Funciones de generación de números
void generarA(void);
void generarB(void);

// Modos de operación (antes metodo1 y metodo2)
void modoA(void);
void modoB(void);

#endif