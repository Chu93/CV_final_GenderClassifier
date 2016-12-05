#include "welcome.h"
#include "ui_welcome.h"
#include <QFileDialog>
#include <qmessagebox>
#include <QLabel>

Welcome::Welcome(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Welcome)
{
    ui->setupUi(this);
    connect(ui->selectFile, SIGNAL(clicked()), ui->filenameLabel, SLOT(on_selectFile_clicked()));
}

Welcome::~Welcome()
{
    delete ui;
}

void Welcome::on_selectFile_clicked()
{
    QDir dir("../Images");
    QLabel * filenameLabel = this->findChild<QLabel*>("filenameLabel");
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Select an Image File"),
                "../Images",
                "All files (*.*);; JPEG file (*.jpg);; Bitmap file (*.bmp);; Portable Graymap File (*.pgm)"
                );

    //QMessageBox::information(this, tr("File Name"), filename);
    if (!filename.isEmpty())
        filenameLabel->setText(dir.relativeFilePath(filename));
        QPushButton * processFile = this->findChild<QPushButton*>("processFile");
        processFile->setEnabled(true);
}
/*
void Welcome::on_processFile_clicked()
{
    QStringList commandAndParameters<<"gender.cpp"<<
}
*/
