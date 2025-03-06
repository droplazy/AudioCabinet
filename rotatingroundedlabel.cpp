#include "rotatingroundedlabel.h"
#include <QPainter>
#include <QPainterPath>
#include <QTimerEvent>
#include <QDebug>
RotatingRoundLabel::RotatingRoundLabel(int radius, QWidget *parent)
    : QLabel(parent), radius(radius), angle(0), timer(nullptr)
{
    // 设置标签的固定大小为圆形
    setFixedSize(radius * 2, radius * 2);

    // 默认背景为蓝色
    setStyleSheet("background-color: black;");

    // 启动定时器控制旋转
    runimage();
}

RotatingRoundLabel::~RotatingRoundLabel()
{
    if (timer) {
        delete timer;
    }
}

void RotatingRoundLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // 启用抗锯齿  //运行内存消耗过大
   // painter.setRenderHint(QPainter::Antialiasing);

    // 创建圆形路径
    QPainterPath path;
    QRect rect(0, 0, width(), height());
    path.addEllipse(rect);  // 创建一个圆形路径

    // 使用该路径裁剪绘制区域
    painter.setClipPath(path);

    // 如果图片有效，绘制图片
    if (!pixmap.isNull()) {
        // 缩放图片为圆形内适合的尺寸
        QPixmap scaledPixmap = pixmap.scaled(radius * 4, radius * 4, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // 将坐标系的原点移到标签的中心
        painter.translate(width() / 2, height() / 2);

        // 旋转图片
        painter.rotate(angle);

        // 绘制图片时，从图片的中心开始绘制
        painter.drawPixmap(-scaledPixmap.width() / 2, -scaledPixmap.height() / 2, scaledPixmap);
    }

    // 调用父类的paintEvent
    QLabel::paintEvent(event);
}
void RotatingRoundLabel::onTimeout()
{
    // 更新旋转角度
    angle += 1;  // 每次增加1度
    if (angle >= 360) {
        angle = 0;  // 重置角度
    }
    update();  // 触发重绘
}

void RotatingRoundLabel::loadImage(const QPixmap &newPixmap)
{
    pixmap = newPixmap;  // 设置新的 QPixmap
    update();  // 加载完图片后重新绘制
}

void RotatingRoundLabel::runimage()
{
    // 启动定时器来控制旋转
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &RotatingRoundLabel::onTimeout);
    timer->start(20);  // 每20ms更新一次（可根据需要调整旋转速度）
}
