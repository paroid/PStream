#ifndef FRAME_H
#define FRAME_H

#include <QFrame>
#include <QPainter>

namespace Ui {
    class Frame;
}

class Frame : public QFrame
{
    Q_OBJECT

public:
    explicit Frame(QWidget *parent = 0);
    ~Frame();
    void setUpdate(int ,int ,int ,int ,int ,int,int);
    //void draw();
    void paintEvent(QPaintEvent *);
private:
    Ui::Frame *ui;
    int tcp,udp,arp,icmp,ip,ether,total;
};

#endif // FRAME_H
