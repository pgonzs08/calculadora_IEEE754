#include <stdio.h>
#include <unistd.h>
#include "conversorieee754.h"

/*
 *
 *  Este código es una modificación del código dado en el ágora
 *
 *
 */

unsigned int ConversorIEEE754::floattoIEENumex(float num)
{
    union Code a;
    a.numero = num;
    return a.numerox;
}

unsigned int ConversorIEEE754::floattoIEESign(float num)
{
    union Code a;
    a.numero = num;
    return a.bitfield.sign;
}

unsigned int ConversorIEEE754::floattoIEEExp(float num)
{
    union Code a;
    a.numero = num;
    return a.bitfield.expo;
}

unsigned int ConversorIEEE754::floattoIEEMantisa(float num)
{
    union Code a;
    a.numero = num;
    return a.bitfield.partFrac;
}

unsigned int ConversorIEEE754::floattoIEE2 (){

    union Code a;
    a.numero=2.1;
    return a.bitfield.partFrac;

}

float ConversorIEEE754::IEEtofloat (int signo, int exponente, int mantisa){

    union Code a;

    a.bitfield.sign = signo;
    a.bitfield.expo = exponente;
    a.bitfield.partFrac = mantisa;

    return a.numero;
}

float ConversorIEEE754::IEEtofloat2 (){

    union Code a;

    a.bitfield.partFrac = 4718592;
    a.bitfield.expo = 131;
    a.bitfield.sign = 0;
    return a.numero;

}
