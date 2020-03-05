#ifndef DIALOG_H
#define DIALOG_H
#include <QTest>
#include <QDialog>
#include <QKeyEvent>
#include <QEvent>
#include <QString>
#include <QTimer>
#include <QListWidgetItem>
#include "devgpio.h"
#include "common.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    bool do_down(QObject *obj);
    bool do_up(QObject *obj);
    bool dofun();
    bool eventFilter(QObject *obj, QEvent *event);
    void pushbutton_scan();
    void pushbutton_connect();
    void removeListSame(QStringList *list);
    void cut_net_speed();
    void fun_cut(QString str);
    void show_link_msg();

private slots:
    void show_net_speed();
    void on_listWidget_clicked(const QModelIndex &index);
private:
    Ui::Dialog *ui;
    DevGpio *wifidev;
    QImage *image;
    QPalette palette;//tiao se ban
    QStringList wifi_name;
    QStringList wifi_level;
    uint wifi_which;
    QString passwd;
    QString deliver_speed;
    QString download_speed;
    bool openflag;
    bool result;
    bool if_scan;
    bool launch_wlan;
    int itemFlag;
    int list_item;

    QTimer *timer_speed;
//    QTimer *timer_if_link;

    QListWidgetItem *pItem;
    QListWidgetItem *pItem1;
    QListWidgetItem *pItem2;
    QListWidgetItem *pItem3;
    QListWidgetItem *pItem4;
private:
    void connectWifi(void);
    void getInfomation(void);
    void checkSuccess(void);
};

#endif // DIALOG_H
