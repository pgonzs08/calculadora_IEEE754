#ifndef CONVERSORIEEE754_H
#define CONVERSORIEEE754_H


class ConversorIEEE754
{
public:
    ConversorIEEE754();
    void floattoIEE ();
    void floattoIEE2 ();
    void IEEtofloat ();
    void IEEtofloat2 ();

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
