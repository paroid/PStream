#include "pstream.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>

#define __FAVOR_BSD
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>

#include <linux/sockios.h>
#include <linux/if.h>

#define MAXSIZE 4096
#define OPTNUM	  8
#define ON        1
#define OFF	  0

enum{ETHER,ARP,IP,TCP,UDP,ICMP,DUMP,ALL};


pStream::pStream(){
    enable=1;
    clearFlow();   
}
void pStream::clearFlow(){
    totalFlow=0;
    tcpFlow=0;
    udpFlow=0;
    arpFlow=0;
    icmpFlow=0;
    ipFlow=0;
    etherFlow=0;
    otherFlow=0;
}

void pStream::setTime(){
    time=QTime::currentTime().toString("hh:mm:ss");
}

void pStream::setMessage(QTableWidget *tab,QLabel *la,bool t,bool u,bool a,bool ic,bool i,bool e){
    table=tab;
    label =la;
    ftcp=t;
    fudp=u;
    farp=a;
    ficmp=ic;
    fip=i;
    fether=e;
}

void pStream::addItem(QString str, int color){
    int  total=table->rowCount();
    table->setRowCount(total+1);
    table->setItem(total,0,new QTableWidgetItem(str));
    switch(color){
    case 1:
        table->item(total,0)->setTextColor("blue");
        break;
    case 2:
        table->item(total,0)->setTextColor(QColor::fromRgb(0,206,209));
        break;
    case 3:
        table->item(total,0)->setTextColor("red");
        break;
    }
    table->setItem(total,1,new QTableWidgetItem(time));
    table->scrollToBottom();

}

void pStream::saveData(){
    db=QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("pstream");
    db.setUserName("root");
    db.setPassword("123456");
    db.open();
    if(!db.open()){
        addItem("Save Error!",3);
    }
    QSqlQuery query;
    for(int i=0;i<table->rowCount();i++){
        query.prepare("insert into capdata values(?,?,?)");
        query.addBindValue(i);
        query.addBindValue(table->item(i,0)->text().toLocal8Bit());
        query.addBindValue(table->item(i,1)->text().toLocal8Bit());
        query.exec();
    }    
    addItem("Data Saved.",2);
    db.close();
}
void pStream::setStat(QString str){
    label->setText(str);
}

void pStream::setStop(){
    enable=0;
}
void pStream::setIf(QString ifn){
    devname=ifn;
}

void pStream::run(){
        struct ether_header *eth;                               //Ethernet structure
        struct ether_arp *arp;					//Arp structure
        struct ip *ip;                                          //ip structure
        struct icmp *icmp;                                      //Icmp structure
        struct tcphdr *tcp;                                     //tcp structure
        struct udphdr *udp;                                     //udp sturcture
        int s;                                                  //socket descriptor
        int len;                                                //data length received
        int c;                                                  //character received to use getopt
        int disp;                                               //diplay switch
        char buff[MAXSIZE];					//buffer
        char *p;												//initial pointer
        char *p0;												//packet pointer
        char *ifname;                                           //interface name
        bool opt[OPTNUM];        				//getopt() variable
        opt[ETHER] = fether;
        opt[ARP]   = farp;
        opt[IP]    = fip;
        opt[TCP]   = ftcp;
        opt[UDP]   = fudp;
        opt[ICMP]  = ficmp;
        opt[DUMP]  = OFF;
        opt[ALL]   = OFF;

        ifname=devname.toLatin1().data();
        if((s = socket(AF_INET, SOCK_PACKET, htons(ETH_P_ALL))) == -1){
                addItem("Socket Error!",3);
                return;
        }

        if(strcmp(ifname, "xlo") != 0){
                struct sockaddr sa;
                memset(&sa, 0, sizeof(sa));
                sa.sa_family = AF_INET;
                strcpy(sa.sa_data, ifname);
                if(bind(s, &sa, sizeof(sa)) < 0){
                        addItem("Bind Error!",3);
                        return;
                }
        }
        enable=1;
        while(enable){                                                   // main circle
                if ((len = read(s, buff, MAXSIZE)) < 0){
                        addItem("Read Error!",3);
                        return;
                }
                p = p0 = buff;
                disp = OFF;

                setTime();
                totalFlow++;

                eth = (struct ether_header*)p;
                p = p + sizeof(struct ether_header);

                if(ntohs(eth->ether_type) == ETHERTYPE_ARP){                                 //for arp
                        if(opt[ARP] == ON){
                                if(opt[ETHER] == ON){
                                        print_ethernet(eth);                                        
                                }
                                arp = (struct ether_arp *)p;
                                print_arp(arp);
                                disp = ON;                                
                        }
                }
                else if(ntohs(eth->ether_type) == ETHERTYPE_IP){                             //for ip
                        ip = (struct ip*)p;
                        p = p + ((int)(ip->ip_hl) << 2);

                        if((opt[IP] == ON) && (opt[TCP] == OFF)  && (opt[UDP] == OFF) && (opt[ICMP] == OFF)){
                                if (opt[ETHER] == ON){
                                        print_ethernet(eth);                                        
                                }
                                print_ip(ip);
                                disp = ON;
                        }
                        switch(ip->ip_p){
                        case IPPROTO_TCP:
                                tcp = (struct tcphdr*)p;
                                p = p +((int)(tcp->th_off) << 2);

                                if(opt[TCP] == ON){
                                        if(opt[IP] == ON){
                                                if(opt[ETHER] == ON){
                                                        print_ethernet(eth);
                                                }
                                                print_ip(ip);
                                        }
                                        print_tcp(tcp);
                                        disp = ON;
                                }
                                break;
                        case IPPROTO_UDP:
                                udp = (struct udphdr *)p;
                                p = p + sizeof(struct udphdr);

                                if(opt[UDP] == ON){
                                        if(opt[IP] == ON){
                                                if(opt[ETHER] == ON){
                                                        print_ethernet(eth);
                                                }
                                                print_ip(ip);
                                        }
                                        print_udp(udp);
                                        disp = ON;
                                }
                                break;
                        case IPPROTO_ICMP:
                                icmp = (struct icmp*)p;
                                p = p + sizeof(struct icmp);

                                if(opt[ICMP] == ON){
                                        if(opt[IP] == ON){
                                                if(opt[ETHER] == ON){
                                                        print_ethernet(eth);
                                                }
                                                print_ip(ip);
                                        }
                                        print_icmp(icmp);
                                }
                                break;

                        default:
                                if(opt[ALL] == ON){
                                        if(opt[IP] == ON){
                                                if(opt[ETHER] == ON){
                                                        print_ethernet(eth);
                                                }
                                                print_ip(ip);
                                        }
                                        //printf("Protocol unknown\n");
                                        addItem(QString::fromLocal8Bit("未知协议类型."),3);
                                        disp = ON;
                                }
                                break;
                        }
                }
/*
                else{                                                                        //for unkwown
                        if(opt[ALL] == ON){
                                if(opt[ETHER] == ON){
                                        print_ethernet(eth);
                                }
                                printf("Protocol unknown\n");
                                disp = ON;
                        }
                }
*/
                if(disp == ON){
                        if(opt[DUMP] == ON){
                                //dump_packet(p0, len);
                        }
                        //printf("\n");
                }
        }        
        return;
}


//convert MAC to string
char *pStream::mac_ntoa(u_char *d)
{
        static char str[50];
        sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x", d[0], d[1], d[2], d[3], d[4], d[5]);
        return str;
}

//Ethernet header
void pStream::print_ethernet(struct ether_header* eth){
        QString info;
        char str[1024];
        int type = ntohs(eth->ether_type);
        if(type <= 1500){
                info+="IEEE 802.3 Ethernet Frame: \n";
        }
        else{
                info+="Ethernet Frame: \n";
        }

        sprintf(str,"| MAC: %17s -> ", mac_ntoa(eth->ether_shost));
        info+=str;
        sprintf(str,"%17s \n",mac_ntoa(eth->ether_dhost));
        info+=str;
        if(type < 1500){
                sprintf(str,"| Length: %-5u   ", type);
                info+=str;
        }
        else{
                sprintf(str,"| Ethernet Type: 0x%04x  - ", type);
                info+=str;
                if(type == 0X0200){
                        info+="xerox PUP   ";
                }
                else if(type == ETHERTYPE_IP){                      //0x0800
                        info+="IP          ";
                }
                else if(type == ETHERTYPE_ARP){                      //0x0806
                        info+="ARP         ";
                }
                else if(type == ETHERTYPE_REVARP){                  //0X8035
                        info+="RARP        ";
                }
                else{
                        info+="Unknown     ";
                }
        }
        addItem(info);
        etherFlow++;
}

//ARP
void pStream::print_arp(struct ether_arp *arp){
        char str[1024];
        QString info;
        static char* arp_operation[]={
                "Undefine",
                "(ARP Request)",
                "(ARP Reply)",
                "(RARP Request)",
                "(RARP Reply)"
        };

        int op = ntohs(arp->ea_hdr.ar_op);

        if(op <= 0 || op > 5){
                op = 0;
        }

        info+="Protocol:ARP \n";
        sprintf(str,"| Datalink Type: %2u % -11s | Protocol: 0x%04x %-9s \n| Operation: %4d %16s \n",
                 ntohs(arp->ea_hdr.ar_hrd),
                (ntohs(arp->ea_hdr.ar_hrd) == ARPHRD_ETHER) ? "(ETHERNET)":"( NOT OTHER)",
                 ntohs(arp->ea_hdr.ar_pro),
                 (ntohs(arp->ea_hdr.ar_pro) == ETHERTYPE_IP) ? "(IP)": "(NOT IP)", ntohs(arp->ea_hdr.ar_op), arp_operation[op]);
        info+=str;
        sprintf(str,"| MAC: %17s -> ", mac_ntoa(arp->arp_sha));
        info+=str;
        sprintf(str,"%17s\n", mac_ntoa(arp->arp_tha));
        info+=str;
        sprintf(str,"| IP: %15s -> ", inet_ntoa(*(struct in_addr *)&arp->arp_spa));
        info+=str;
        sprintf(str,"%15s", inet_ntoa(*(struct in_addr *)&arp->arp_tha));
        info+=str;
        addItem(info);
        arpFlow++;
}


//IP header
void pStream::print_ip(struct ip *ip){
        char str[1024];
        QString info;
        info+="Protocol:IP ";
        sprintf(str,"| Version:%1u | HeaderLen: %2u | TOS:%8s | Totallen: %-10u \n", ip->ip_v, ip->ip_hl, ip_ttoa(ip->ip_tos), ntohs(ip->ip_len));
        info+=str;
        sprintf(str,"| Identifier:%-5u | Flag(R D M):%3s | Offset:%-5u \n", ntohs(ip->ip_id), ip_ftoa(ntohs(ip->ip_off)), ntohs(ip->ip_off & IP_OFFMASK));            //IP_OFFMASK = 0x1fff
        info+=str;
        sprintf(str,"| TTL:%3u | Protocol:%3u | Header checksum: %-5u \n", ip->ip_ttl, ip->ip_p, ntohs(ip->ip_sum));
        info+=str;
        sprintf(str,"| IP: %15s -> ", inet_ntoa(*(struct in_addr*)&(ip->ip_src)));
        info+=str;
        sprintf(str,"%15s", inet_ntoa(*(struct in_addr *)&(ip->ip_dst)));
        info+=str;
        addItem(info);
        ipFlow++;
}

//convert ip->ip_off to string
char *pStream::ip_ftoa(int flag){
        static int f[] = {'R', 'D', 'M'};
        static char str[17];
        u_int mask = 0x8000;
        int i;

        for(i = 0; i < 3; i++){
                if(((flag << i) & mask) != 0){
                        str[i] = f[i];
                }
                else{
                        str[i] = '0';
                }
        }
        str[i] = '\0';

        return str;
}


// convert ip->ip_tos to string
char *pStream::ip_ttoa(int flag){
        static int f[] = {'1', '1','1', 'D', 'T', 'R', 'C', 'X'};
        static char str[17];
        u_int mask = 0x80;
        int i;
        for(i = 0; i < 8; i++){
                if(((flag << i) & mask) != 0){
                        str[i] = f[i];
                }
                else{
                        str[i] = '0';
                }
        }
        str[i] = '\0';

        return str;
}


//ICMP
void pStream::print_icmp(struct icmp *icmp){
        static char *type_name[] = {
                "Echo Reply ",
                "Undefine",
                "Undefine",
                "Destination Unreachable",
                "Source Quench",
                "Redirect(change route)",
                "Undefine",
                "Undefine",
                "Echo Reqest",
                "Undefine",
                "Undefine",
                "Timeout",
                "Parameter Problem",
                "Timestamp Request",
                "Timestamp Reply",
                "Inforamation Request",
                "Information Reply",
                "Address Mask Request",
                "Address Mask Reply",
                "Unknown"
        };
        char str[1024];
        QString info;
        int type = icmp->icmp_type;
        if(type < 0 || type >18){
                type = 19;
        }

        sprintf(str,"Protocol:ICMP - %s \n", type_name[type]);
        info+=str;
        sprintf(str,"| Type:%3u | Code:%3u | CheckSum:%-5u \n", icmp->icmp_type, icmp->icmp_code, ntohs(icmp->icmp_cksum));
        info+=str;

        if(icmp->icmp_type == 0 || icmp->icmp_type == 8){
                sprintf(str,"| Identification: %-5u | Seq_num %-5u \n", ntohs(icmp->icmp_id), ntohs(icmp->icmp_seq));
                info+=str;
        }
        else if(icmp->icmp_type == 3){
                if(icmp->icmp_code == 4){
                        sprintf(str,"| void  %-5u | Next_mtu %-5u \n", ntohs(icmp->icmp_pmvoid), ntohs(icmp->icmp_nextmtu));
                        info+=str;
                }
                else{
                        sprintf(str,"| Unused %10u ", (u_long)ntohl(icmp->icmp_void));
                        info+=str;
                }
        }
        else if(icmp->icmp_type == 5){
                sprintf(str,"| Router IP Address: %15s \n", inet_ntoa(*(struct in_addr*)&(icmp->icmp_gwaddr)));
                info+=str;
        }
        else if(icmp->icmp_type == 11){
                sprintf(str,"| Unused: %-10lu ", (u_long)ntohl(icmp->icmp_void));
                info+=str;
        }

        addItem(info);
        icmpFlow++;
        if(icmp->icmp_type == 3 || icmp->icmp_type == 5 || icmp->icmp_type == 11){
                print_ip((struct ip*)(((char *)icmp) + 8));
        }

}

//TCP header
void pStream::print_tcp(struct tcphdr *tcp){
        char str[1024];
        QString info;
        info+="Protocol:TCP ";
        sprintf(str,"| Port: %5u -> %-5u | Seq_num: %-10lu |Ack_num: %-10lu \n", ntohs(tcp->th_sport), ntohs(tcp->th_dport), (u_long)ntohl(tcp->th_seq), (u_long)ntohl(tcp->th_ack));
        info+=str;
        sprintf(str,"| Off_len: %-2u | Reserved | F:%6s | Win_size: %-5u \n", tcp->th_off, tcp_ftoa(tcp->th_flags), ntohs(tcp->th_win));
        info+=str;
        sprintf(str,"| Checksum: %-5u | Urg_pointer: %-5u ", ntohs(tcp->th_sum), ntohs(tcp->th_urp));
        info+=str;
        addItem(info);
        tcpFlow++;
}

//resolve falg
char *pStream::tcp_ftoa(int flag){
        static int f[] = {
                'U',
                'A',
                'P',
                'R',
                'S',
                'F'
        };

        static char str[17];
        u_int mask = 1 << 5; //0x20;
        int i;

        for(i = 0; i < 6; i++){
                if(((flag << i) & mask) != 0){
                        str[i] = f[i];
                }
                else{
                        str[i] = '0';
                }
        }
        str[i] = '\0';

        return str;
}

// UDP header
void pStream::print_udp(struct udphdr *udp){
        char str[1024];
        QString info;
        info+="Protocol: UDP ";
        sprintf(str,"| Port: %5u -> %5u | Length:%5u | Checksum: %5u ",ntohs(udp->uh_sport), (udp->uh_dport), ntohs(udp->uh_ulen), ntohs(udp->uh_sum));
        info+=str;
        addItem(info);
        udpFlow++;
}



