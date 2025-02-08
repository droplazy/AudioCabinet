#-------------------------------------------------
#
# Project created by QtCreator 2024-12-20T02:25:51
#
#-------------------------------------------------

QT       += core gui dbus  network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = application
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS OS_NET_LINUX_OS
INCLUDEPATH +=/home/zhangjh/1AAA_T113/TINA/T113-Series_Tina5.0_V1.1/T113-Tina/platform/allwinner/wireless/btmanager/src/include

INCLUDEPATH +=/home/zhangjh/1AAA_T113/TINA/T113-Series_Tina5.0_V1.1/T113-Tina/out/t113/evb1_auto/buildroot/buildroot/host/arm-buildroot-linux-gnueabi/sysroot/usr/include/dbus-1.0
INCLUDEPATH +=/home/zhangjh/1AAA_T113/TINA/T113-Series_Tina5.0_V1.1/T113-Tina/out/t113/evb1_auto/buildroot/buildroot/host/arm-buildroot-linux-gnueabi/sysroot/usr/include/glib-2.0
INCLUDEPATH +=/home/zhangjh/1AAA_T113/TINA/T113-Series_Tina5.0_V1.1/T113-Tina/out/t113/evb1_auto/buildroot/buildroot/host/arm-buildroot-linux-gnueabi/sysroot/usr/lib/glib-2.0/include/
INCLUDEPATH +=/home/zhangjh/1AAA_T113/TINA/T113-Series_Tina5.0_V1.1/T113-Tina/out/t113/evb1_auto/buildroot/buildroot/host/arm-buildroot-linux-gnueabi/sysroot/usr/include/gio-unix-2.0
INCLUDEPATH +=/home/zhangjh/1AAA_T113/TINA/T113-Series_Tina5.0_V1.1/T113-Tina/out/t113/evb1_auto/buildroot/buildroot/host/arm-buildroot-linux-gnueabi/sysroot/usr/include/dbus-1.0
INCLUDEPATH +=/home/zhangjh/1AAA_T113/TINA/T113-Series_Tina5.0_V1.1/T113-Tina/out/t113/evb1_auto/buildroot/buildroot/host/arm-buildroot-linux-gnueabi/sysroot/usr/lib/dbus-1.0/include/
INCLUDEPATH +=/home/zhangjh/1AAA_T113/TINA/T113-Series_Tina5.0_V1.1/T113-Tina/out/t113/evb1_auto/buildroot/buildroot/host/arm-buildroot-linux-gnueabi/sysroot/usr/include

LIBS += -L/home/zhangjh/1AAA_T113/TINA/T113-Series_Tina5.0_V1.1/T113-Tina/out/t113/evb1_auto/buildroot/buildroot/target/lib


#LIBS += -L/home/zhangjh/1AAA_T113/T113/linux/source/out/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabi/arm-linux-gnueabi/lib

LIBS    += -lbluetooth -lglib-2.0 -lsbc -ljson-c -lgio-2.0 -lgobject-2.0 -lasound -lbtmg -ldl -lm -lresolv -ldbus-1 -lwirelesscom -lpthread
LIBS+=-lshared-mainloop -lbluetooth-internal -lreadline -lncurses
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    httpclient.cpp \
    rotatingroundedlabel.cpp \
    btmanager/a2dp/bt_a2dp_sink.c \
    btmanager/a2dp/bt_a2dp_source.c \
    btmanager/a2dp/bt_alsa.c \
    btmanager/avrcp/bt_avrcp.c \
    btmanager/avrcp/bt_avrcp_tg.c \
    btmanager/common/bt_action.c \
    btmanager/common/bt_agent.c \
    btmanager/common/bt_alarm.c \
    btmanager/common/bt_bluez.c \
    btmanager/common/bt_bluez_signals.c \
    btmanager/common/bt_dev_list.c \
    btmanager/common/bt_list.c \
    btmanager/common/bt_pcm_supervise.c \
    btmanager/common/bt_queue.c \
    btmanager/common/bt_reactor.c \
    btmanager/common/bt_semaphore.c \
    btmanager/common/bt_thread.c \
    btmanager/common/common.c \
    btmanager/common/transmit.c \
    btmanager/device/bt_adapter.c \
    btmanager/device/bt_device.c \
    btmanager/gatt/bt_gatt_client.c \
    btmanager/gatt/bt_gatt_db.c \
    btmanager/gatt/bt_gatt_server.c \
    btmanager/gatt/bt_gattc_conn.c \
    btmanager/gatt/bt_gatts_conn.c \
    btmanager/gatt/bt_le5_hci.c \
    btmanager/gatt/bt_le_hci.c \
    btmanager/gatt/bt_mainloop.c \
    btmanager/hfp/at.c \
    btmanager/hfp/hfp_rfcomm.c \
    btmanager/hfp/hfp_transport.c \
    btmanager/log/bt_log.c \
    btmanager/platform/bt_config_json.c \
    btmanager/platform/platform.c \
    btmanager/shared/dbus-client.c \
    btmanager/shared/ffb.c \
    btmanager/spp/bt_rfcomm.c \
    btmanager/spp/bt_spp_client.c \
    btmanager/spp/bt_spp_service.c \
    btmanager/spp/channel.c \
    btmanager/bt_manager.c \
    btmanager/bt_cmd.c \
    btmanager/bt_gatt_client_demo.c \
    btmanager/bt_gatt_server_demo.c \
    btmanager/bt_gatt_test_demo.c \
    btmanager/bt_test.c \
    main_thread.cpp

HEADERS  += mainwindow.h \
    my_define.h \
    httpclient.h \
    rotatingroundedlabel.h \
    btmanager/include/at.h \
    btmanager/include/bt_a2dp_sink.h \
    btmanager/include/bt_a2dp_source.h \
    btmanager/include/bt_action.h \
    btmanager/include/bt_adapter.h \
    btmanager/include/bt_agent.h \
    btmanager/include/bt_alarm.h \
    btmanager/include/bt_alsa.h \
    btmanager/include/bt_avrcp.h \
    btmanager/include/bt_avrcp_tg.h \
    btmanager/include/bt_bluez.h \
    btmanager/include/bt_bluez_signals.h \
    btmanager/include/bt_config_json.h \
    btmanager/include/bt_dev_list.h \
    btmanager/include/bt_device.h \
    btmanager/include/bt_gatt_client.h \
    btmanager/include/bt_gatt_inner.h \
    btmanager/include/bt_gatt_server.h \
    btmanager/include/bt_le5_hci.h \
    btmanager/include/bt_le_hci.h \
    btmanager/include/bt_list.h \
    btmanager/include/bt_log.h \
    btmanager/include/bt_mainloop.h \
    btmanager/include/bt_manager.h \
    btmanager/include/bt_pcm_supervise.h \
    btmanager/include/bt_queue.h \
    btmanager/include/bt_reactor.h \
    btmanager/include/bt_rfcomm.h \
    btmanager/include/bt_semaphore.h \
    btmanager/include/bt_spp_client.h \
    btmanager/include/bt_spp_service.h \
    btmanager/include/bt_thread.h \
    btmanager/include/channel.h \
    btmanager/include/common.h \
    btmanager/include/dbus-client.h \
    btmanager/include/defs.h \
    btmanager/include/ffb.h \
    btmanager/include/hfp_rfcomm.h \
    btmanager/include/hfp_transport.h \
    btmanager/include/platform.h \
    btmanager/include/transmit.h \
    btmanager/bt_test.h \
    main_thread.h

FORMS    += mainwindow.ui
