#include "mainwindow.h"
#include <QApplication>
// #include <QtDBus/QDBusMetaType>
// #include <QtDBus/QtDBus>
#include <QTextCodec>
#include "btmanager/bt_test.h"
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QFontDatabase>
typedef QMap<QString, QMap<QString, QVariant>> ConnectionDetails;
Q_DECLARE_METATYPE(ConnectionDetails)

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

void rtc_init() {
    // Check if /dev/rtc1 exists
    struct dirent *entry;
    DIR *dp = opendir("/dev/");
    if (dp == NULL) {
        perror("Failed to open /dev/");
        return;
    }

    int rtc_found = 0;
    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, "rtc1") == 0) {
            rtc_found = 1;
            break;
        }
    }
    closedir(dp);

    if (rtc_found) {
        // If rtc1 exists, execute hwclock command
        printf("rtc1 driver already exists, executing hwclock command...\n");
        system("hwclock --hctosys -f /dev/rtc1");
        printf("hym8563 initialization successful\n");
    } else {
        // If rtc1 doesn't exist, unload and load rtc-hym8563 module
        printf("rtc1 driver does not exist, unloading and loading rtc-hym8563 module...\n");
        system("rmmod /lib/modules/5.4.61/rtc-hym8563.ko");
        system("insmod /lib/modules/5.4.61/rtc-hym8563.ko");

        // Poll for the presence of /dev/rtc1 for up to 5 seconds
        int retries = 5;
        while (retries > 0) {
            dp = opendir("/dev/");
            if (dp == NULL) {
                perror("Failed to open /dev/");
                return;
            }

            rtc_found = 0;
            while ((entry = readdir(dp)) != NULL) {
                if (strcmp(entry->d_name, "rtc1") == 0) {
                    rtc_found = 1;
                    break;
                }
            }
            closedir(dp);

            if (rtc_found) {
                // If rtc1 is found, execute hwclock command
                printf("hym8563 initialization successful\n");
                system("hwclock --hctosys -f /dev/rtc1");
                return;
            }

            printf("Waiting for rtc1 to be loaded... %d seconds remaining.\n", retries - 1);
            sleep(1); // Wait for 1 second before retrying
            retries--;
        }

        // If rtc1 is not found within 5 seconds, print error
        printf("Failed to initialize hym8563: rtc1 not found after 5 seconds.\n");
    }
}



int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QApplication::setOverrideCursor(Qt::BlankCursor);
  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
  system("ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime");
  // system("hwclock --hctosys -f /dev/rtc1");
  rtc_init();
#if 0
  // set system Font
  //       system("echo 0 > /proc/rp_gpio/output_sd");
  int id = QFontDatabase::addApplicationFont("/usr/lib/fonts/DroidSansFallback.ttf");
  QString msyh = QFontDatabase::applicationFontFamilies(id).at(0);
  QFont font(msyh, 10);
  int size = 11;
  font.setPointSize(size);
  a.setFont(font);

  char *argvv[] = {"bt_test", NULL};
  mainloop(1, argvv);
#endif
  MainWindow w_ui;

  QGraphicsScene *scene = new QGraphicsScene;
  QGraphicsProxyWidget *w = scene->addWidget(&w_ui);
  w->setRotation(90);
  w->setWindowFlags(Qt::FramelessWindowHint);
  QGraphicsView *view = new QGraphicsView(scene);
  view->setWindowFlags(Qt::FramelessWindowHint);
  view->setGeometry(-2, -2, 485, 805);
  //   view->resize(250,330);
  view->show();
  return a.exec();
}
