#include "mainwindow.h"
#include <QApplication>
//#include <QtDBus/QDBusMetaType>
//#include <QtDBus/QtDBus>
#include  <QTextCodec>
#include "btmanager/bt_test.h"
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QFontDatabase>
typedef QMap<QString, QMap<QString, QVariant> > ConnectionDetails;
Q_DECLARE_METATYPE(ConnectionDetails)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOverrideCursor(Qt::BlankCursor);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    system("ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime");
    system("hwclock --hctosys");

#if 1
    //set system Font
          system("echo 0 > /proc/rp_gpio/output_sd");
    int id = QFontDatabase::addApplicationFont("/usr/lib/fonts/DroidSansFallback.ttf");
    QString msyh = QFontDatabase::applicationFontFamilies (id).at(0);
    QFont font(msyh,10);
    int size=11;
    font.setPointSize(size);
    a.setFont(font);

       char *argvv[] = {"bt_test",NULL};
       mainloop(1,argvv);
#endif
      MainWindow w_ui;


      QGraphicsScene *scene = new QGraphicsScene;
      QGraphicsProxyWidget *w = scene->addWidget(&w_ui);
      w->setRotation(90);
      w->setWindowFlags (Qt::FramelessWindowHint);
      QGraphicsView *view = new QGraphicsView(scene);
      view->setWindowFlags (Qt::FramelessWindowHint);
      view->setGeometry(-2,-2,485,805);
      //   view->resize(250,330);
      view->show();
    return a.exec();
}
