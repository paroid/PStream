#ifndef PSTREAM_H
#define PSTREAM_H

#include <QThread>
#include <QTableWidget>
#include <QLabel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlDriver>
#include <QTime>

class pStream:public QThread{
public:
    pStream();
    void setMessage(QTableWidget *tab,QLabel *la,bool t=1,bool u=1,bool a=1,bool ic=1,bool ip=1,bool eth=1);
    void run();
    void print_ethernet(struct ether_header *eth);
    void print_arp(struct ether_arp *arp);
    void print_ip(struct ip *ip);
    void print_icmp(struct icmp *icmp);
    void print_tcp(struct tcphdr *tcp);
    void print_udp(struct udphdr *udp);
    void dump_print(unsigned char *buff, int len);
    char *mac_ntoa(u_char *d);
    char *tcp_ftoa(int flag);
    char *ip_ttoa(int flag);       //ip_tos to char
    char *ip_ftoa(int flag);
    void help(char *cmd);
    void addItem(QString str,int color=0);
    void setStat(QString str);
    void setStop();
    void saveData();
    void clearFlow();
    void setTime();
    void setIf(QString );
    QSqlDatabase db;

    QTableWidget *table;
    QLabel *label;
    QString time,devname;
    bool enable;
    bool ftcp,fudp,farp,ficmp,fip,fether;
    int totalFlow,tcpFlow,udpFlow,arpFlow,icmpFlow,ipFlow,etherFlow,otherFlow;
};

#endif // PSTREAM_H
