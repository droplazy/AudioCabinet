#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdus.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    p_dbus = new qDbus();
    p_dbus->p_main = this;
   // connect(p_dbus, SIGNAL(updateMeta(S_Aduio_Meta *)), this, SLOT(displayAudioMeta(S_Aduio_Meta *)), Qt::AutoConnection);						 // updatefile

    p_dbus->start();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayAudioMeta(S_Aduio_Meta *p_meta)
{
    ui->label_aumble->setText(p_meta->Album);
    ui->label_artist->setText(p_meta->Artist);
    ui->label_duration ->setText(convertDurationToTimeFormat(p_meta->Duration));
    ui->label_position->setText(convertDurationToTimeFormat(p_meta->Position));
    ui->label_title->setText(p_meta->Title);
    qDebug() << "fresh DISplay ~~~~~";
}
QString MainWindow::convertDurationToTimeFormat(const QString &durationStr) {
    // 将输入的 QString 转换为整数（毫秒数）
    bool ok;
    int durationMs = durationStr.toInt(&ok);

    // 检查转换是否成功
    if (!ok) {
        return "00:00";  // 如果转换失败，返回默认值
    }

    // 将毫秒转换为秒
    int durationSec = durationMs / 1000;

    // 计算分钟和秒
    int minutes = durationSec / 60;
    int seconds = durationSec % 60;

    // 格式化为 "mm:ss" 格式
    return QString::asprintf("%02d:%02d", minutes, seconds);
}
