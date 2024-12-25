#ifndef ROTATINGROUNDLABEL_H
#define ROTATINGROUNDLABEL_H

#include <QLabel>
#include <QPixmap>
#include <QTimer>

class RotatingRoundLabel : public QLabel
{
    Q_OBJECT

public:
    // 构造函数，允许设置半径
    explicit RotatingRoundLabel(int radius = 100, QWidget *parent = nullptr);
    ~RotatingRoundLabel();

    // 设置图片
    void loadImage(const QPixmap &newPixmap);

protected:
    // 重写paintEvent进行绘制
    void paintEvent(QPaintEvent *event) override;

private slots:
    // 旋转定时器槽函数
    void onTimeout();

private:
    // 半径
    int radius;
    // 当前旋转角度
    int angle;
    // 定时器用于控制旋转
    QTimer *timer;
    // 图片
    QPixmap pixmap;

    // 启动定时器
    void runimage();
};

#endif // ROTATINGROUNDLABEL_H
