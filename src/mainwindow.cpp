#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    trayIcon=new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/icon/logo.ico"));
    trayIcon->setToolTip("PStream");
    trayIcon->show();
    createActions();
    createTrayIcon();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));


    //get devices
    pcap_if_t *devs,*p;
    int cnt=0;
    char errBuf[PCAP_ERRBUF_SIZE];
    if(pcap_findalldevs(&devs,errBuf) == -1){
        return;
    }
    for(p=devs;p!=NULL;p=p->next,cnt++){
        ui->deviceCombo->addItem(p->name);
    }
    if(!cnt)
        ui->deviceCombo->addItem(QString("No Device Found."));
    pcap_freealldevs(devs);
    //initialize statis frame
    statis = new Frame();
    //initialize thread
    ps = new pStream();
    ps->setMessage(ui->resTable,ui->statLabel,ui->tcpBox->isChecked(),ui->udpBox->isChecked(),ui->arpBox->isChecked(),ui->icmpBox->isChecked(),ui->ipBox->isChecked(),ui->etherBox->isChecked());
    connect(ui->startButton,SIGNAL(clicked()),this,SLOT(stCap()));
    connect(ui->stopButton,SIGNAL(clicked()),this,SLOT(stopCap()));
    connect(ui->clearButton,SIGNAL(clicked()),this,SLOT(clearTable()));
    connect(ui->saveButton,SIGNAL(clicked()),this,SLOT(save()));
    connect(ui->resTable,SIGNAL(cellClicked(int,int)),this,SLOT(setH(int,int)));
    connect(ui->statisButton,SIGNAL(clicked()),this,SLOT(staitsShow()));
    ui->tcpBox->setChecked(true);
    ui->udpBox->setChecked(true);
    ui->arpBox->setChecked(true);
    ui->icmpBox->setChecked(true);
    ui->ipBox->setChecked(true);
    ui->etherBox->setChecked(true);
    ui->rawBox->hide();
    ui->resTable->setColumnWidth(0,500);
    ui->resTable->setColumnWidth(1,100);
    ui->resTable->setAlternatingRowColors(true);
    ui->resTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->resTable->setAutoScroll(true);
    ui->statLabel->setText(QString::fromLocal8Bit("准备就绪."));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::stCap(){
    if(!ps->isRunning()){                
        if(ui->deviceCombo->currentText()=="No Device Found.")
            return;
        ps->setIf(ui->deviceCombo->currentText());
        ps->setMessage(ui->resTable,ui->statLabel,ui->tcpBox->isChecked(),ui->udpBox->isChecked(),ui->arpBox->isChecked(),ui->icmpBox->isChecked(),ui->ipBox->isChecked(),ui->etherBox->isChecked());
        ui->statLabel->setText(QString::fromLocal8Bit("正在捕获..."));
        ps->start();
    }
}

void MainWindow::setH(int x, int y){
    ui->resTable->setRowHeight(x,100);
    int total=ui->resTable->rowCount();
    for(int i=0;i<total;i++)
        if(i!=x)
            ui->resTable->setRowHeight(i,32);
}

void MainWindow::staitsShow(){
    statis->show();
    statis->setUpdate(ps->tcpFlow,ps->udpFlow,ps->arpFlow,ps->icmpFlow,ps->ipFlow,ps->etherFlow,ps->totalFlow);
}

void MainWindow::clearTable(){
    ui->resTable->setRowCount(0);
    ps->clearFlow();
}

void MainWindow::save(){
    ps->saveData();
}

void MainWindow::stopCap(){    
    ps->setStop();
    while(ps->isRunning());
    ps->addItem(QString::fromLocal8Bit("已停止捕获."),2);
    ps->setStat(QString::fromLocal8Bit("准备就绪."));
}
void MainWindow::showMessage()
{
    QString titlec="PStream";
    QString textc=QString::fromLocal8Bit("Gui by Paroid.");
    trayIcon->showMessage(titlec, textc, QSystemTrayIcon::Information, 5000);
}

void MainWindow::showMessage(QString msg)
{
    QString titlec=msg;
    QString textc=QString::fromLocal8Bit("Gui by Paroid.");
    trayIcon->showMessage(titlec, textc, QSystemTrayIcon::Information, 5000);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason){
    switch (reason){
        case QSystemTrayIcon::Trigger:        
            if(this->isHidden()){
                this->showNormal();
            }
            else{
                this->showMinimized();
            }
        break;
        case QSystemTrayIcon::DoubleClick:
        showMessage(QString::fromLocal8Bit("鼠标双击！"));
        break;
        case QSystemTrayIcon::MiddleClick:
        showMessage(QString::fromLocal8Bit("鼠标中键！"));
        break;
        default:
        break;
    }
}

void MainWindow::createActions(){
    minimizeAction = new QAction(QString::fromLocal8Bit("最小化 (&I)"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(QString::fromLocal8Bit("最大化 (&X)"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(QString::fromLocal8Bit("还原 (&R)"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(QString::fromLocal8Bit("退出 (&Q)"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::createTrayIcon(){    
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayIconMenu);
}

