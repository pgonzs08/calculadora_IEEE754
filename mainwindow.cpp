#include "mainwindow.h"
#include "ui_mainwindow.h"
//Aquí hacemos las funciones de la ALU
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

