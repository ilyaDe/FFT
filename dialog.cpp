#include <QString>
#include <QFileDialog>


#include "dialog.h"
#include "ui_dialog.h"

#include "FFT_test.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_toolButton_clicked()
{
    // load input file
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.*)"));
    ui->label->setText(fileName);
    compute(fileName.toStdWString());
}

