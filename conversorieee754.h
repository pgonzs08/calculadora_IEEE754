#ifndef CONVERSORIEEE754_H
#define CONVERSORIEEE754_H


class ConversorIEEE754
{
public:
    ConversorIEEE754();
    static unsigned int floattoIEENumex (float num);
    static unsigned int floattoIEESign (float num);
    static unsigned int floattoIEEExp (float num);
    static unsigned int floattoIEEMantisa (float num);
    static unsigned int floattoIEE2 ();
    static float IEEtofloat (int signo, int exponente, int mantisa);
    static float IEEtofloat2 ();

private:
    union Code {

        struct{
            unsigned int partFrac : 23;
            unsigned int expo : 8;
            unsigned int sign : 1;
        }bitfield;

        float numero;
        unsigned int numerox;

    };
};

#endif // CONVERSORIEEE754_H
