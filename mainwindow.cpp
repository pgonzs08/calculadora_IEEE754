#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

//Aquí hacemos las funciones de la ALU
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bitPos.push_back(1);

    for(int i = 1; i < 32; i++) bitPos.push_back(2*bitPos.at(i-1));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

    float op1 = ui->opD1->text().toFloat();

    float op2 = ui->opD2->text().toFloat();

    float salida = aluAdd(op1,op2);

    binaryWriteIn( ui->opB1, ConversorIEEE754::floattoIEESign(op1), ConversorIEEE754::floattoIEEExp(op1), ConversorIEEE754::floattoIEEMantisa(op1));
    hexWriteIn(ui->opH1,  ConversorIEEE754::floattoIEESign(op1), ConversorIEEE754::floattoIEEExp(op1), ConversorIEEE754::floattoIEEMantisa(op1));

    binaryWriteIn( ui->opB2,  ConversorIEEE754::floattoIEESign(op2), ConversorIEEE754::floattoIEEExp(op2), ConversorIEEE754::floattoIEEMantisa(op2));
    hexWriteIn(ui->opH2, ConversorIEEE754::floattoIEESign(op2), ConversorIEEE754::floattoIEEExp(op2), ConversorIEEE754::floattoIEEMantisa(op2));

    ui->rD->setText(QString::fromStdString(std::to_string(salida)));
    binaryWriteIn(ui -> rB, ConversorIEEE754::floattoIEESign(salida), ConversorIEEE754::floattoIEEExp(salida), ConversorIEEE754::floattoIEEMantisa(salida));
    hexWriteIn(ui->rH, ConversorIEEE754::floattoIEESign(salida), ConversorIEEE754::floattoIEEExp(salida), ConversorIEEE754::floattoIEEMantisa(salida));

}

float MainWindow::aluAdd(float op1, float op2){
    //Pasos previos

    unsigned int signoA = ConversorIEEE754::floattoIEESign(op1);
    unsigned int signoB = ConversorIEEE754::floattoIEESign(op2);

    unsigned int expA = ConversorIEEE754::floattoIEEExp(op1);
    unsigned int expB = ConversorIEEE754::floattoIEEExp(op2);

    unsigned int manA = ConversorIEEE754::floattoIEEMantisa(op1) + bitPos.at(23);
    unsigned int manB = ConversorIEEE754::floattoIEEMantisa(op2) + bitPos.at(23);


    const unsigned int excsBits = bitPos.at(31)+bitPos.at(30)+bitPos.at(29)+bitPos.at(28)+bitPos.at(27)+bitPos.at(26)+bitPos.at(25)+bitPos.at(24);

    //Paso 1
    unsigned int signoSuma;

    unsigned int g = 0; unsigned int r = 0; unsigned int st = 0;
    bool opChanged = false;
    bool compP = false;

    //Paso 2
    if(expA < expB){
        expA = ConversorIEEE754::floattoIEEExp(op2);
        expB = ConversorIEEE754::floattoIEEExp(op1);
        manA = ConversorIEEE754::floattoIEEMantisa(op2) + bitPos.at(23);
        manB = ConversorIEEE754::floattoIEEMantisa(op1) + bitPos.at(23);
        signoA = ConversorIEEE754::floattoIEESign(op2);
        signoB = ConversorIEEE754::floattoIEESign(op1);
        opChanged = true;
    }
    //Paso 3
    unsigned int expR = expA;
    unsigned int d = expA - expB;
    //Paso 4

    if(signoA!=signoB) manB = (~manB)%0b1000000000000000000000000+1;

    //Paso 5
    unsigned int P = manB;
    //Paso 6
    if(d>=3 && d < 25){
        g = (bitPos.at(d-1)&P)!=0;
        r = (bitPos.at(d-2)&P)!=0;
        st = (bitPos.at(d-3)&P)!=0;
    }
    for(int i = d-3; i > 0 && !st; i--) st = st|(bitPos.at(d-i)&P);
    //Paso 7
    if(signoA!=signoB){
        for(unsigned int i = 0; i < d; i++){
            P >>= 1;
            P += bitPos.at(23);
        }
    }
    else P = P>>d;
    //Paso 8
    unsigned int C = 0;
    C = calcularAcarreo(manA, P, 0, 0);
    P = manA + P;

    //Paso 9
    if(signoA!=signoB && (P & bitPos.at(23)) != 0 && C == 0){
        P = (~P)%0b1000000000000000000000000+1;
        compP = true;
    }

    //Paso 10
    if(signoA == signoB && C==1){

        st = g | r | st;

        r = P%2;

        P = P>>1;
        P += bitPos.at(23);

        expR++;

    }
    else{

        int k = 0;
        for(unsigned int aux = P; aux != 0 && (aux & bitPos.at(23)) == 0; aux <<=1) k++;
        if (k == 0){
            st = r|st;
            r = g;
        }
        else{
            st = 0; r = 0;
        }

        for(int i = 0; i < k; i++){

            P<<=1;
            P += g;

        }

        expR -= k;

        if(P == 0) expR = 0;

    }

    //Paso 11:
    if((r==1 && st == 1)||(r==1 && st == 0 && P%2 == 1)){
        unsigned int C2 = calcularAcarreo(P, 0b100000000000000000000,0,0);
        P = P+1;

        if(C2) {
            P >>= 1;
            P += bitPos.at(23);
            expR++;
        }
    }

    //Paso 12:

    signoSuma = (!opChanged && compP) ? signoB:signoA;

    if(expR > 255) return (signoSuma)? -Q_INFINITY:Q_INFINITY;

    return ConversorIEEE754::IEEtofloat(signoSuma, expR, P);

}

void MainWindow::on_pushButton_2_clicked()
{

    float op1 = ui->opD1->text().toFloat();

    float op2 = ui->opD2->text().toFloat();

    float salida = aluMultiply(op1,op2);

    binaryWriteIn( ui->opB1, ConversorIEEE754::floattoIEESign(op1), ConversorIEEE754::floattoIEEExp(op1), ConversorIEEE754::floattoIEEMantisa(op1));
    hexWriteIn(ui->opH1,  ConversorIEEE754::floattoIEESign(op1), ConversorIEEE754::floattoIEEExp(op1), ConversorIEEE754::floattoIEEMantisa(op1));

    binaryWriteIn( ui->opB2,  ConversorIEEE754::floattoIEESign(op2), ConversorIEEE754::floattoIEEExp(op2), ConversorIEEE754::floattoIEEMantisa(op2));
    hexWriteIn(ui->opH2, ConversorIEEE754::floattoIEESign(op2), ConversorIEEE754::floattoIEEExp(op2), ConversorIEEE754::floattoIEEMantisa(op2));

    ui->rD->setText(QString::fromStdString(std::to_string(salida)));
    binaryWriteIn(ui -> rB, ConversorIEEE754::floattoIEESign(salida), ConversorIEEE754::floattoIEEExp(salida), ConversorIEEE754::floattoIEEMantisa(salida));
    hexWriteIn(ui->rH, ConversorIEEE754::floattoIEESign(salida), ConversorIEEE754::floattoIEEExp(salida), ConversorIEEE754::floattoIEEMantisa(salida));


}

float MainWindow::aluMultiply(float op1, float op2){

    unsigned int signoA = ConversorIEEE754::floattoIEESign(op1);
    unsigned int signoB = ConversorIEEE754::floattoIEESign(op2);

    unsigned int expA = ConversorIEEE754::floattoIEEExp(op1);
    unsigned int expB = ConversorIEEE754::floattoIEEExp(op2);

    unsigned int manA = ConversorIEEE754::floattoIEEMantisa(op1) + bitPos.at(23);
    unsigned int manB = ConversorIEEE754::floattoIEEMantisa(op2) + bitPos.at(23);

    //Paso 1:
    unsigned int signoR = signoA ^ signoB;
    //Paso 2:
    int expR = expA + expB - 0b1111111;

    //Paso 3:
    //Paso 3i:
    unsigned int c = 0;
    unsigned int P = 0;
    unsigned int A = manA;

    for(int i = 0; i < 24; i++){
            P+=A%2*manB;
            A = (A>>1) + (P%2)*bitPos.at(23);
            P = (P>>1) + c*bitPos.at(23);
            c>>=1;
    }

    //Paso 3ii:

    if((P & bitPos.at(23))==0){
        P <<= 1;
    }
    else{
        expR++;
    }

    //Paso 3iii:
    unsigned int r = (A & bitPos.at(23))!= 0;

    //Paso 3iv:
    unsigned int st = 0;
    for(int i = 0; i < 23; i++) st |= (A & bitPos.at(i))!= 0;

    //Paso 3v:
    if((r&&st) ||(r&&!st&&P%2)) P+=1;

    //DESBORDAMIENTOS
    if(expR>0b11111111){
        return (signoR)? -Q_INFINITY:Q_INFINITY;
    }
    else if(expR<0){

        unsigned int t = 1 - expR;

        P >>=t;

        expR = 0;

    }

    return ConversorIEEE754::IEEtofloat(signoR, expR, P);

}

void MainWindow::on_pushButton_3_clicked()
{
    float op1 = ui->opD1->text().toFloat();
    float op2 = ui->opD2->text().toFloat();

    //Pasos previos

    unsigned int signoA = ConversorIEEE754::floattoIEESign(op1);
    unsigned int signoB = ConversorIEEE754::floattoIEESign(op2);

    unsigned int expA = ConversorIEEE754::floattoIEEExp(op1);
    unsigned int expB = ConversorIEEE754::floattoIEEExp(op2);

    unsigned int manA = ConversorIEEE754::floattoIEEMantisa(op1);// + bitPos.at(23);
    unsigned int manB = ConversorIEEE754::floattoIEEMantisa(op2);// + bitPos.at(23);





    binaryWriteIn( ui->opB1, signoA, expA, manA);
    binaryWriteIn( ui->opB2, signoB, expB, manB);
    hexWriteIn(ui->opH1, signoA, expA, manA);
    hexWriteIn(ui->opH2, signoB, expB, manB);

    //Caso del cero

    if(expB==0&&manB==0){

            ui->rB->setText("NaN");
            ui->rD->setText("NaN");
            ui->rH->setText("NaN");
        return;
    }
//    //Arreglo para los denormales exponente -127 pasa a ser -126

//    if(expA==0){

//        expA=1;

//    }

//    if(expB==0){

//        expB=1;

//    }





    //Casos de "infinito"
    //Aquí la ALU comprobaría si todos los números del exponente son 1s

    if(expA>=255){

        ui->rD->setText((signoA==0)?"inf":"-inf");
        binaryWriteIn(ui->rB,signoA,255,0);
        hexWriteIn(ui->rH,signoA,255,0);


        return;
    }else if(expB>=255){

        ui->rD->setText("0");
        binaryWriteIn(ui->rB,0,0,0);
        hexWriteIn(ui->rH,0,0,0);


        return;
    }

    // 1.-Escalamos a [1,2)


    float escA =ConversorIEEE754::IEEtofloat(0,127,manA);
    float escB =ConversorIEEE754::IEEtofloat(0,127,manB);


    //2.-Aproximamos b'=1/b

    float inversoB=(escB<1.25)? 1.25:0.8;



    //3.- Asignamos x e y sub cero
    float x= MainWindow::aluMultiply(escA,inversoB);
    float y= MainWindow::aluMultiply(escB,inversoB);
    //4.-Iteramos hasta tener la precision correcta
    float lastx;
    float r;
    do{
        r=2-y;
        y=MainWindow::aluMultiply(y,r);
        lastx=x;
        x=MainWindow::aluMultiply(x,r);

    }while(abs(x-lastx)>=0.0001);

    //Ahora mismo x es a*1/b
    //unsigned int signoX = ConversorIEEE754::floattoIEESign(x);

    unsigned int expX = ConversorIEEE754::floattoIEEExp(x);

    unsigned int manX = ConversorIEEE754::floattoIEEMantisa(x);


    //5.-Signo

    unsigned int signoDiv =(signoA==signoB)? 0:1;

    //6.-Exponente

    int exponenteDiv;

    exponenteDiv=expA-expB+expX;
    if(exponenteDiv>=255){
        exponenteDiv=255;
    }
    if(exponenteDiv==255){

            ui->rD->setText((signoDiv==0)?"inf":"-inf");
            binaryWriteIn(ui->rB,signoDiv,255,0);
            hexWriteIn(ui->rH,signoDiv,255,0);

        return;
    }
    //7.- Final


    binaryWriteIn(ui->rB,signoDiv,exponenteDiv,manX);
    hexWriteIn(ui->rH,signoDiv,exponenteDiv,manX);


    if(exponenteDiv!=0){
        ui->rD->setText(QString::number(ConversorIEEE754::IEEtofloat(signoDiv,exponenteDiv,manX)));
    }else{
        ui->rD->setText(QString::number(MainWindow::denormalCalculator(signoDiv,manX)));
    }
}

float MainWindow::denormalCalculator(unsigned int sign, unsigned int mantissa){
    float preMantissa =1-ConversorIEEE754::IEEtofloat(0,127,mantissa);
    if(sign){
        return mantissa * 1.1754944e-38;//2^-126 para la alu
    }else{
        return mantissa * -1.1754944e-38;//-2^-126 para la alu
    }
}

void MainWindow::binaryWriteIn(QLineEdit* child, unsigned int sign, unsigned int exp, unsigned int mantisa)
{
    QString binaryNumber;

    for(int i =0;i< 23;i++){
        binaryNumber.push_front(QString::fromStdString(std::to_string(mantisa%2)));
        mantisa/=2;
    }

    for(int i=0;i<8;i++){
        binaryNumber.push_front(QString::fromStdString(std::to_string(exp%2)));
        exp /= 2;
    }

    binaryNumber.push_front(QString::fromStdString(std::to_string(sign)));


    binaryNumber.push_front("0b");

    child->setText(binaryNumber);
}

unsigned int MainWindow::calcularAcarreo(unsigned int manA, unsigned int manB, unsigned int pos, unsigned int acarreoActual)
{

    if(pos == 24) return acarreoActual;

    if((manB & bitPos.at(pos)) != 0 && (manA & bitPos.at(pos)) != 0) return calcularAcarreo(manA, manB, pos+1, 1);
    else if(((manB & bitPos.at(pos)) != 0 || (manA & bitPos.at(pos)) != 0) && acarreoActual != 0) return calcularAcarreo(manA, manB, pos+1, 1);
    else return calcularAcarreo(manA, manB, pos+1, 0);

}

void MainWindow::on_Reset_clicked()
{
    ui->opD1->setText("");
    ui->opD2->setText("");
    ui->opB1->setText("");
    ui->opB2->setText("");
    ui->opH1->setText("");
    ui->opH2->setText("");
    ui->rD->setText("");
    ui->rB->setText("");
    ui->rH->setText("");

}

void MainWindow::hexWriteIn(QLineEdit* child, unsigned int sign, unsigned int exp, unsigned int mantisa){

    QString stringHex;

    unsigned int aux = (sign << 31) + (exp << 23) + (mantisa);

    for(int i = 0; i < 8; i++) {

        unsigned int mod = aux%16;

        switch (mod) {

        case 10: stringHex.push_front('A');break;
        case 11: stringHex.push_front('B');break;
        case 12: stringHex.push_front('C');break;
        case 13: stringHex.push_front('D');break;
        case 14: stringHex.push_front('E');break;
        case 15: stringHex.push_front('F');break;

        default: stringHex.push_front(QString::fromStdString(std::to_string(mod)));

        }

        aux >>= 4;

    }
    child->setText("0x"+stringHex);


}

