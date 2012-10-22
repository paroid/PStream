#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QIcon>
#include <QSystemTrayIcon>
#include "pstream.h"
#include "pcap.h"
#include "frame.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void showMessage(QString msg);
    void createActions();
    void createTrayIcon();

public slots:
    void stCap();
    void clearTable();
    void stopCap();
    void showMessage();
    void save();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void setH(int x,int y);
    void staitsShow();
private:
    Ui::MainWindow *ui;
    pStream *ps;
    Frame *statis;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;
};

#endif // MAINWINDOW_H
