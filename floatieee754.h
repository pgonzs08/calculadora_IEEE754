#ifndef FLOATIEEE754_H
#define FLOATIEEE754_H


class FloatIEEE754
{
public:

    FloatIEEE754(float num);

    unsigned int getPartFrac();
    unsigned int getExpo();
    unsigned int getSign();

    float getNum();
    unsigned int getNumerox();

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

    union Code a;

};

#endif // FLOATIEEE754_H
