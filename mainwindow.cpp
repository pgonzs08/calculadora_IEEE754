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

    std::cout << "bitPos values: ";
    for(int i = 0; i < 32; i++) std::cout << " " << bitPos.at(i);
    std::cout << std::endl; //bit de mantisa normalizada

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

    unsigned int manA = ConversorIEEE754::floattoIEEMantisa(op1) + bitPos.at(23);
    unsigned int manB = ConversorIEEE754::floattoIEEMantisa(op2) + bitPos.at(23);


    binaryWriteIn( ui->opB1, signoA, expA, manA);
    binaryWriteIn( ui->opB2, signoB, expB, manB);
    toHexUpdate();

    const unsigned int excsBits = bitPos.at(31)+bitPos.at(30)+bitPos.at(29)+bitPos.at(28)+bitPos.at(27)+bitPos.at(26)+bitPos.at(25)+bitPos.at(24);
    std::cout << "excsBits = " << excsBits << std::endl;

    //Paso 1
    std::cout << "Paso1: "<<std::endl;
    unsigned int signoSuma;

    unsigned int g = 0; unsigned int r = 0; unsigned int st = 0;
    bool opChanged = false;
    bool compP = false;

    //Paso 2
    std::cout << "Paso2: ";
    if(expA < expB){
        expA = ConversorIEEE754::floattoIEEExp(op2);
        expB = ConversorIEEE754::floattoIEEExp(op1);
        manA = ConversorIEEE754::floattoIEEMantisa(op2) + bitPos.at(23);
        manB = ConversorIEEE754::floattoIEEMantisa(op1) + bitPos.at(23);
        signoA = ConversorIEEE754::floattoIEESign(op2);
        signoB = ConversorIEEE754::floattoIEESign(op1);
        opChanged = true;
    }
    std::cout <<" A = "<< signoA << expA -127 << manA - bitPos.at(23) <<", B = "<< signoB << expB -127 << manB - bitPos.at(23) << std::endl;
    //Paso 3
    std::cout << "Paso3: ";
    unsigned int expR = expA;
    unsigned int d = expA - expB;
    d = (d>=0)? d:-d;
    std::cout << d<< std::endl;
    //Paso 4
    std::cout << "Paso4: ";
    if(signoA!=signoB){
        manB = (~manB)+1-excsBits;
    }
    std::cout <<"manB = "<<~(manB-1) - bitPos.at(23)-excsBits<<" ~manB = "<<manB -excsBits<< std::endl;
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
        for(unsigned int i = 0; i < d; i++){
            P >>= 1;
            P += bitPos.at(23);
        }
    }
    else P = P>>d;
    std::cout << P<< std::endl;
    //Paso 8
    std::cout << "Paso8: " << " P = " << P << ", A = " << manA;
    unsigned int C = 0;
    C = (P & bitPos.at(23)) != 0 && (manA & bitPos.at(23)) != 0;
    P = manA + P;
    std::cout << ", R = " << P << ", C = " << C << std::endl;

    //Paso 9
    if(signoA!=signoB && (P & bitPos.at(23)) != 0 && C == 0){
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

        for(unsigned int aux = P; aux!=0 && (aux & bitPos.at(23)) == 0 ; aux<<=1) k++;

        if (k == 0){
            st = r|st;
            r = g;
        }
        else{
            st = 0; r = 0;
        }

        for(int i = 0; i < k; i++) {

            P <<= 1;
            P += g;
        }

        expR -= k;

    }

    //Paso 11:
    if((r==1 && st == 1)||(r==1 && st == 0 && P%2)){
        unsigned int aux = P;
        P = P+1;
        unsigned int C2 = P < aux;

        if(C2) {
            P >>= 1;
            P += bitPos.at(23);
            expR++;
        }
    }

    //Paso 12:

    signoSuma = (!opChanged && compP) ? signoB:signoA;

    float salida = ConversorIEEE754::IEEtofloat(signoSuma, expR, P);

    ui->rD->setText(QString::fromStdString(std::to_string(salida)));

    binaryWriteIn(ui -> rB, signoSuma, expR, P - bitPos.at(23));

}

void MainWindow::on_pushButton_2_clicked()
{

    float op1 = ui->opD1->text().toFloat();
    float op2 = ui->opD2->text().toFloat();

    ui->rD->setText(QString::fromStdString(std::to_string(op1+op2)));

    //Pasos previos

    unsigned int signoA = ConversorIEEE754::floattoIEESign(op1);
    unsigned int signoB = ConversorIEEE754::floattoIEESign(op2);

    unsigned int expA = ConversorIEEE754::floattoIEEExp(op1);
    unsigned int expB = ConversorIEEE754::floattoIEEExp(op2);

    unsigned int manA = ConversorIEEE754::floattoIEEMantisa(op1) + bitPos.at(23);
    unsigned int manB = ConversorIEEE754::floattoIEEMantisa(op2) + bitPos.at(23);


    binaryWriteIn( ui->opB1, signoA, expA, manA);
    binaryWriteIn( ui->opB2, signoB, expB, manB);
    toHexUpdate();

    const unsigned int excsBits = bitPos.at(31)+bitPos.at(30)+bitPos.at(29)+bitPos.at(28)+bitPos.at(27)+bitPos.at(26)+bitPos.at(25)+bitPos.at(24);
    std::cout << "excsBits = " << excsBits << std::endl;

    //Paso 1:
    std::cout << "Paso1:";
    unsigned int signoR = signoA | signoB;
    std::cout << "Signo = " << signoR << std::endl;
    //Paso 2:
    std::cout << "Paso2:";
    unsigned int expR = expA + expB - 127;
    std::cout << "Exponente = " << expR-127 << std::endl;

    //Paso 3:
    std::cout << "Paso3:"<<std::endl;

        //Paso 3i:
        std::cout << "  Paso3i:";
        unsigned long PA = (long)manA*(long)manB;
        std::cout << "PA = " << PA;
        unsigned int P = PA >> 24;
        unsigned int A = PA;
        std::cout << " P = " << P << " A = "<< A<< std::endl;;

        //Paso 3ii:
        std::cout << "  Paso3ii:";

        if((P & bitPos.at(23))==0){
            P <<= 1;
        }
        else{
            expR++;
        }
        std::cout << " P = " << P << " expR = " << expR << std::endl;

        //Paso 3iii:
        std::cout << "  Paso3iii:";
        unsigned int r = (A & bitPos.at(23))!= 0;
        std::cout << " r = " << r << std::endl;

        //Paso 3iv:
        std::cout << "  Paso3iv:";
        unsigned int st = 0;
        for(int i = 0; i < 23; i++) st |= (A & bitPos.at(i))!= 0;
        std::cout << " st = " << st << std::endl;

        //Paso 3v:
        std::cout << "  Paso3v:";
        if((r&&st) ||(r&&!st&&P%2)){
            P = P+1;
        }
        std::cout << " P = " << P << std::endl;

        //DESBORDAMIENTOS
        if(expR>0b11111111){

            ui->rD->setText((signoR)? "-infinito":"infinito");
            binaryWriteIn( ui->rB, signoA, expA, manA);

        }
        else if(expR<0);
        else{
        float salida = ConversorIEEE754::IEEtofloat(signoR, expR, P);

        ui->rD->setText(QString::fromStdString(std::to_string(salida)));

        binaryWriteIn( ui->rB, signoA, expA, manA);
        }

}

void MainWindow::on_pushButton_3_clicked()
{

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


    binaryNumber.push_front("0x");

    child->setText(binaryNumber);
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

    op1=0;
    op2=0;

}

void MainWindow::toHexUpdate(){
    QString stringHex;
    QString stringBin= ui->opB1->text();

    for (int i=2 ; i<stringBin.length() ; i=i+4 ) {

        QStringRef substringtemp(&stringBin,i,i+3);

        if(substringtemp.at(0)=='0'){
            if(substringtemp.at(1)=='0'){
                if(substringtemp.at(2)=='0'){
                    if(substringtemp.at(3)=='0'){
                        stringHex.append('0');
                    } else{
                        stringHex.append('1');
                    }
                }else{
                    if(substringtemp.at(3)=='0'){
                        stringHex.append('2');
                    } else{
                        stringHex.append('3');
                    }

                }
            }else{
                if(substringtemp.at(2)=='0'){
                    if(substringtemp.at(3)=='0'){
                        stringHex.append('4');
                    } else{
                        stringHex.append('5');
                    }
                }else{
                    if(substringtemp.at(3)=='0'){
                        stringHex.append('6');
                    } else{
                        stringHex.append('7');
                    }
                }
            }
        }else{
            if(substringtemp.at(1)=='0'){
                if(substringtemp.at(2)=='0'){
                    if(substringtemp.at(3)=='0'){
                        stringHex.append('8');
                    } else{
                        stringHex.append('9');
                    }
                }else{
                    if(substringtemp.at(3)=='0'){
                        stringHex.append('A');
                    } else{
                        stringHex.append('B');
                    }

                }
            }else{
                if(substringtemp.at(2)=='0'){
                    if(substringtemp.at(3)=='0'){
                        stringHex.append('C');
                    } else{
                        stringHex.append('D');
                    }
                }else{
                    if(substringtemp.at(3)=='0'){
                        stringHex.append('E');
                    } else{
                        stringHex.append('F');
                    }

                }

            }

        }



    }


    ui->opH1->setText("0x"+stringHex);


}

