#include "mainwindow.h"
#include <QApplication>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QtDBus>
#include "btmanager/bt_test.h"
#include <QGraphicsView>
#include <QGraphicsProxyWidget>

typedef QMap<QString, QMap<QString, QVariant> > ConnectionDetails;
Q_DECLARE_METATYPE(ConnectionDetails)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

          QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
//      MainWindow w;
//      w.show();
//       char *argvv[] = {"bt_test",NULL};
//       mainloop(1,argvv);
      MainWindow w_ui;


      QGraphicsScene *scene = new QGraphicsScene;
      QGraphicsProxyWidget *w = scene->addWidget(&w_ui);
      w->setRotation(270);
      w->setWindowFlags (Qt::FramelessWindowHint);
      QGraphicsView *view = new QGraphicsView(scene);
      view->setWindowFlags (Qt::FramelessWindowHint);
      view->setGeometry(-5,-5,330,970);
      //   view->resize(250,330);
      view->show();
    return a.exec();
}
