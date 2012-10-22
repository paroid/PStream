#include "frame.h"
#include "ui_frame.h"

Frame::Frame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Frame)
{
    ui->setupUi(this);
}

Frame::~Frame()
{
    delete ui;
}

void Frame::setUpdate(int t,int u,int a,int ic,int i,int e,int to){
    tcp=t;
    udp=u;
    arp=a;
    icmp=ic;
    ip=i;
    ether=e;
    total=to;
}

void Frame::paintEvent(QPaintEvent *event){
    QPainter pain(this);
    pain.setRenderHint(QPainter::Antialiasing);
    //pain.setPen(QPen(Qt::black,15,Qt::SolidLine,Qt::RoundCap,Qt::MiterJoin));
    pain.setPen(QPen("blue"));
    pain.setBrush(QBrush(Qt::green,Qt::DiagCrossPattern));
    //pain.drawPie(80,80,400,240,60*16,270*16);
    //axis x-y
    pain.drawLine(60,60,60,300);
    pain.drawLine(60,300,440,300);
    //y-label
    pain.drawText(60,260,"-");
    pain.drawText(25,260,"40");
    pain.drawText(60,220,"-");
    pain.drawText(25,220,"80");
    pain.drawText(60,180,"-");
    pain.drawText(25,180,"120");
    pain.drawText(60,140,"-");
    pain.drawText(25,140,"160");
    pain.drawText(60,100,"-");
    pain.drawText(25,100,"200");
    //x-label
    pain.drawText(80,320,"TCP");
    pain.drawText(140,320,"UDP");
    pain.drawText(200,320,"ARP");
    pain.drawText(260,320,"ICMP");
    pain.drawText(320,320,"IP");
    pain.drawText(380,320,"Ethernet");
    //statistics
    pain.drawRect(80,300-tcp,40,tcp);
    pain.drawText(80,280-tcp,QString::number(tcp));
    pain.drawRect(140,300-udp,40,udp);
    pain.drawText(140,280-udp,QString::number(udp));
    pain.drawRect(200,300-arp,40,arp);
    pain.drawText(200,280-arp,QString::number(arp));
    pain.drawRect(260,300-icmp,40,icmp);
    pain.drawText(260,280-icmp,QString::number(icmp));
    pain.drawRect(320,300-ip,40,ip);
    pain.drawText(320,280-ip,QString::number(ip));
    pain.drawRect(380,300-ether,40,ether);
    pain.drawText(380,280-ether,QString::number(ether));
}
