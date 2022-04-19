#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

//Aquí hacemos las funciones de la ALU
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bitPos = {0b000000000000000000000001, 0b000000000000000000000010, 0b000000000000000000000100,
              0b000000000000000000001000, 0b000000000000000000010000, 0b000000000000000000100000,
              0b000000000000000001000000, 0b000000000000000010000000, 0b000000000000000100000000,
              0b000000000000001000000000, 0b000000000000010000000000, 0b000000000000100000000000,
              0b000000000001000000000000, 0b000000000010000000000000, 0b000000000100000000000000,
              0b000000001000000000000000, 0b000000010000000000000000, 0b000000100000000000000000,
              0b000001000000000000000000, 0b000010000000000000000000, 0b000100000000000000000000,
              0b001000000000000000000000, 0b010000000000000000000000, 0b100000000000000000000000};

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

    float op1 = ui->opD1->text().toFloat();
    float op2 = ui->opD2->text().toFloat();

    ui->rD->setText(QString::fromStdString(std::to_string(op1+op2)));

    //Pasos previos

    unsigned int signoA = ConversorIEEE754::floattoIEESign(op1);
    unsigned int signoB = ConversorIEEE754::floattoIEESign(op2);

    unsigned int expA = ConversorIEEE754::floattoIEEExp(op1);
    unsigned int expB = ConversorIEEE754::floattoIEEExp(op2);

    unsigned int manA = ConversorIEEE754::floattoIEEMantisa(op1) +0b100000000000000000000000000;
    unsigned int manB = ConversorIEEE754::floattoIEEMantisa(op2) +0b100000000000000000000000000;


    binaryWriteIn( ui->opB1, signoA, expA, manA);
    binaryWriteIn( ui->opB2, signoB, expB, manB);

    //Paso 1
    std::cout << "Paso1: "<<std::endl;
    unsigned int signoSuma;

    unsigned int g = 0; unsigned int r = 0; unsigned int st = 0;
    unsigned int n = 24;
    bool opChanged = false;
    bool compP = false;

    //Paso 2
    std::cout << "Paso2: ";
    if(expA<expB){
        expA = ConversorIEEE754::floattoIEEExp(op2);
        expB = ConversorIEEE754::floattoIEEExp(op1);
        manA = ConversorIEEE754::floattoIEEMantisa(op2) +0b100000000000000000000000;
        manB = ConversorIEEE754::floattoIEEMantisa(op1) +0b100000000000000000000000;
        signoA = ConversorIEEE754::floattoIEESign(op2);
        signoB = ConversorIEEE754::floattoIEESign(op1);
        opChanged = true;

    }
    std::cout <<" A= "<< signoA << expA << manA -0b100000000000000000000000 <<", B = "<< signoB << expB << manB -0b100000000000000000000000 << std::endl;
    //Paso 3
    std::cout << "Paso3: ";
    unsigned int expR = expA;
    unsigned int d = expA - expB;
    d = (d>=0)? d:-d;
    std::cout << d<< std::endl;
    //Paso 4
    std::cout << "Paso4: ";
    if(signoA!=signoB){
        manB = (~manB)+1;
    }
    std::cout << manB<< std::endl;
    //Paso 5
    std::cout << "Paso5: ";
    unsigned int P = manB;
    std::cout << P<< std::endl;
    //Paso 6
    std::cout << "Paso6: ";
    if(d>=3){
        g = (bitPos.at(d-1)&P)!=0;
        r = (bitPos.at(d-2)&P)!=0;
        st = (bitPos.at(d-3)&P)!=0;
    }
    for(int i = d-3; i > 0 && !st; i--) st = st|(bitPos.at(d-i)&P);
    std::cout <<"g = "<< g <<", r = "<<r<<", st = "<<st<< std::endl;
    //Paso 7
    std::cout << "Paso7: ";
    if(signoA!=signoB){
        P >>= d;
        P+= 0b100000000000000000000000;
    }
    else P = P>>d;
    std::cout << P<< std::endl;
    //Paso 8
    std::cout << "Paso8: " << " P = " << P << ", A = " << manA;
    P = manA + P;
    unsigned int C = 0;
    C = P > 0b111111111111111111111111;
    std::cout << ", R = " << P << ", C = " << C << std::endl;

    //Paso 9
    if(signoA!=signoB && P>= 0b100000000000000000000 && C == 0){
        std::cout << "Paso9"<< std::endl;
        P = ~P+1;
        compP = true;
    }

    //Paso 10
    if(signoA == signoB && C==1){

        st = g | r | st;

        std::cout << "Paso10"<< std::endl;

        r = P%2;

        P = P>>1;
        P += C;

        expR++;

    }
    else{

        int k = 0;

        for(unsigned int aux = P; aux < 0b100000000000000000000000; aux<<=1) k++;

        if (k>0){
            st = r|st;
            r = g;
        }
        else{
            st = 0; r = 0;
        }

        for(int i = 0; i < k; i++) {

            P<<=1;
            P+=g;
        }

        expR-=k;

    }

    //Paso 11:
    if((r==1 && st == 1)||(r==1 && st == 0 && P%2)){
        unsigned int aux = P;
        P = P+1;
        unsigned int C2 = P < aux;

        if(C2) {
            P>>=1;
            P+= 0b100000000000000000000000;
            expR++;
        }
    }

    //Paso 12:

    signoSuma = (!opChanged && compP)? signoB:signoA;

    float salida = ConversorIEEE754::IEEtofloat(signoSuma, expR, P);

    ui->rD->setText(QString::fromStdString(std::to_string(salida)));

    binaryWriteIn(ui->rB,signoSuma, expR, P-0b100000000000000000000000);

}

void MainWindow::on_pushButton_2_clicked()
{

}

void MainWindow::on_pushButton_3_clicked()
{

}

void MainWindow::binaryWriteIn(QLineEdit* child, unsigned int sign, unsigned int exp, unsigned int mantisa)
{
    QString binaryNumber;

    binaryNumber.append(QString::fromStdString(std::to_string(sign)));

    while(exp >= 1){

        binaryNumber.push_back(QString::fromStdString(std::to_string(exp%2)));
        exp /= 2;
    }

    while(mantisa >= 1){

        binaryNumber.push_back(QString::fromStdString(std::to_string(mantisa%2)));
        mantisa /= 2;
    }

    child->setText(binaryNumber);
}
