#include "welcome.h"
#include "ui_welcome.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QProcess>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

Welcome::Welcome(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Welcome)
{
    ui->setupUi(this);
    connect(ui->selectFile, SIGNAL(clicked()), ui->filenameLabel, SLOT(on_selectFile_clicked()));
    connect(ui->processFile, SIGNAL(clicked()), ui->filenameLabel, SLOT(on_processFile_clicked()));
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

    //QMessageBox::information(this, tr("Current Directory"), QDir::currentPath());
    if (!filename.isEmpty())
        filenameLabel->setText(dir.relativeFilePath(filename));
        QPushButton * processFile = this->findChild<QPushButton*>("processFile");
        processFile->setEnabled(true);
}

void Welcome::on_processFile_clicked()
{
    QLabel * filenameLabel = this->findChild<QLabel*>("filenameLabel");
    QStringList arguments;
    QString program = "../gender";
    QString path = "../Images/";
    QString csv = "../att_faces/att.csv";
    QString image = path.append(filenameLabel->text());

    QMessageBox::information(this, tr("test"), image);

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QPixmap qp = QPixmap(image);
    if(qp.isNull())
    {
        printf("Yes its null\n");
    }
    else
    {
        printf("HAHA");
        QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap(image));
        scene.addItem(item);
    }
    view.show();

    arguments << csv << image;
    QProcess * process = new QProcess(this);
    process->start(program, arguments);
    process->waitForFinished();
    QString error(process->readAllStandardError());
    QString output(process->readAllStandardOutput());
    QMessageBox::information(this, tr("output"), output);
    QMessageBox::information(this, tr("error"), error);
}
