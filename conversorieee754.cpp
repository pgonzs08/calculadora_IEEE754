#include <stdio.h>
#include <unistd.h>
#include "conversorieee754.h"

/*
 *
 *  Este código es una modificación del código dado en el ágora
 *
 *
 */

void ConversorIEEE754::floattoIEE (){

    union Code a;
    printf ("Introduce un número: ");
    scanf ("%f", &a.numero);
    printf("Signo: %u \n",a.bitfield.sign);
    printf("Exponente: %u \n",a.bitfield.expo);
    printf("Parte Fraccionaria : %u \n", a.bitfield.partFrac);

}

void ConversorIEEE754::floattoIEE2 (){

    union Code a;
    a.numero=2.1;
    printf("Numero a imprimir: %f \n", a.numero);
    //printf("Numero unsigned: %u \n", a.numerox);
    printf("Signo: %u \n",a.bitfield.sign);
    printf("Exponente: %u \n",a.bitfield.expo);
    printf("Parte Fraccionaria : %u \n", a.bitfield.partFrac);

}

void ConversorIEEE754::IEEtofloat (){

    union Code a;
    int b, c, d;
    printf ("Introduce el signo: ");
    scanf ("%d", &b);
    printf ("Introduce el exponente: ");
    scanf ("%d", &c);
    printf ("Introduce la mantisa: ");
    scanf ("%d", &d);

    a.bitfield.sign = b;
    a.bitfield.expo = c;
    a.bitfield.partFrac = d;

    printf("El número pasado a real es: %f \n", a.numero);
}

void ConversorIEEE754::IEEtofloat2 (){

    union Code a;

    a.bitfield.partFrac = 4718592;
    a.bitfield.expo = 131;
    a.bitfield.sign = 0;
    printf("El número es: %f \n", a.numero);

}
