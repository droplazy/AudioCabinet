#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "my_define.h"


class qDbus;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString convertDurationToTimeFormat(const QString &durationStr);
public slots:
    void displayAudioMeta(S_Aduio_Meta *p_meta);
private:
    Ui::MainWindow *ui;
    qDbus *p_dbus;

};

#endif // MAINWINDOW_H
