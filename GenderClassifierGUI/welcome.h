#ifndef WELCOME_H
#define WELCOME_H

#include <QMainWindow>

namespace Ui {
class Welcome;
}

class Welcome : public QMainWindow
{
    Q_OBJECT

public:
    explicit Welcome(QWidget *parent = 0);
    ~Welcome();

private slots:
    void on_selectFile_clicked();

    void on_processFile_clicked();

private:
    Ui::Welcome *ui;
};

#endif // WELCOME_H
