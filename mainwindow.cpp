#include "mainwindow.h"
#include "ui_mainwindow.h"
//Aquí hacemos las funciones de la ALU
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    potencias2.push_back(1);

    for(int i = 1; i < 255; i++) potencias2.push_back(2*potencias2.at(i-1));

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

    ui->opB1->setText(QString::fromStdString(std::to_string(ConversorIEEE754::floattoIEENumex(op1))));
    ui->opB2->setText(QString::fromStdString(std::to_string(ConversorIEEE754::floattoIEENumex(op2))));

    //Pasos previos
    unsigned int salida = 0;

    unsigned int signoA = ConversorIEEE754::floattoIEESign(op1);
    unsigned int signoB = ConversorIEEE754::floattoIEESign(op2);

    unsigned int expA = ConversorIEEE754::floattoIEEExp(op1);
    unsigned int expB = ConversorIEEE754::floattoIEEExp(op2);

    unsigned int manA = ConversorIEEE754::floattoIEEMantisa(op1);
    unsigned int manB = ConversorIEEE754::floattoIEEMantisa(op2);

    //Paso 1
    unsigned int g = 0; unsigned int r = 0; unsigned int st = 0;
    unsigned int n = 24;
    bool opChanged = false;
    bool compP = false;

    //Paso 2
    if(expA<expB){

    }
    //Paso 3
    unsigned int expR = expA;
    unsigned int d = expA - expB;

    //Paso 4
    if(signoA!=signoB){
        manB = !manB+1;
    }
    //Paso 5
    unsigned int P = manB;

    //Paso 6
    g = potencias2.at(d-1)&P;
    r = potencias2.at(d-2)&P;
    st = potencias2.at(d-3)&P;
    for(int i = d-3; i > 0; i--) st = st|(potencias2.at(d-i)&P);

    //Paso 7
    if(signoA!=signoB)
    P = !((!P)<<d);
    else P = P<<d;

    //Paso 8
    P = manA + P;
    unsigned int C = 0;
    if(P > potencias2.at(24)) C = 1;

    //Paso 9
    if(signoA!=signoB && P>= potencias2.at(23) && C == 0){
        P = !P;
        compP = true;
    }

    //Paso 10
    if(signoA == signoB && C==1){

        st = g | r | st;

        r = P%r;

        P = !(!P>>1);

        expR ++;

    }

    ui->rD->setText(QString::fromStdString(std::to_string(ConversorIEEE754::IEEtofloat(0, expR, P))));

    //salida = ConversorIEEE754::floattoIEENumex();

}

void MainWindow::on_pushButton_2_clicked()
{

}

void MainWindow::on_pushButton_3_clicked()
{

}
