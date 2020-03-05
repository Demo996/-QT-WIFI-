#include "dialog.h"
#include "ui_dialog.h"
#include <string>
#include <QMessageBox>
#include <QTableView>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    timer_speed = new QTimer;
    launch_wlan = true;
    result = false;
    if_scan = true;
    wifidev = new DevGpio(this);
    image = new QImage(":/new/prefix1/back.jpg");
    QImage ima_t = image->scaled(SCREEN_W,SCREEN_H);
    palette.setBrush(this->backgroundRole(),QBrush(ima_t));   //设置背景图片
    setPalette(palette);          //窗口部件的背景色

    system("insmod /home/driver/8188eu.ko");
    system("ifconfig wlan0 up");
    pItem = new QListWidgetItem;
    pItem1 = new QListWidgetItem;
    pItem2= new QListWidgetItem;
    pItem3 = new QListWidgetItem;
    pItem4 = new QListWidgetItem;
    pItem->setSizeHint(QSize(60, 35));
    pItem1->setSizeHint(QSize(60, 35));
    pItem2->setSizeHint(QSize(60, 35));
    pItem3->setSizeHint(QSize(60, 35));
    pItem4->setSizeHint(QSize(60, 35));
    pItem->setText("A、扫描wifi");
    pItem1->setText("B、选择wifi");
    pItem2->setText("C、连接wifi");
    pItem3->setText("D、输入密码");
    pItem4->setText("E、断开连接");
    ui->listWidget->insertItem(0,pItem);
    ui->listWidget->insertItem(1,pItem1);
    ui->listWidget->insertItem(2,pItem2);
    ui->listWidget->insertItem(3,pItem3);
    ui->listWidget->insertItem(4,pItem4);
    ui->listWidget->setFocus();
    ui->listWidget->setCurrentRow(0);
    ui->listWidget->setFrameShadow(QFrame::Plain);
    ui->listWidget->setFrameShape(QFrame::NoFrame);
    ui->listWidget->installEventFilter(this);
    ui->comboBox_wifi->installEventFilter(this);
    ui->lineEdit_pwd->installEventFilter(this);
    setWindowFlags(Qt::FramelessWindowHint);//隐藏标题栏

    ui->lineEdit_pwd->setStyleSheet("color:red");
    list_item = ui->listWidget->currentRow();
    qDebug()<<"list_item"<<list_item;
    QObject::connect(timer_speed,SIGNAL(timeout()),this,SLOT(show_net_speed()));
    QObject::connect(ui->listWidget,SIGNAL(clicked(QModelIndex)),this,SLOT(on_listWidget_clicked(QModelIndex)));
    QObject::connect(ui->comboBox_wifi,SIGNAL(currentIndexChanged(int)),ui->lineEdit_pwd,SLOT(clear()));
}

Dialog::~Dialog()
{
    delete ui;
}

bool Dialog::do_down(QObject *obj)
{
    if(obj == ui->comboBox_wifi)
    {
        ui->listWidget->setFocusPolicy(Qt::NoFocus);
        if(itemFlag != ui->comboBox_wifi->count() - 1)
        {
            ++ itemFlag;
        }
        qDebug()<<"itemFlag==>>"<<itemFlag;
        ui->comboBox_wifi->setCurrentIndex(itemFlag);
        return true;
    }
    if(obj == ui->listWidget)
    {
        ui->comboBox_wifi->setFocusPolicy(Qt::NoFocus);
        if(list_item < 4)
        {
            ++ list_item ;
        }
        else if(list_item == 4)
        {
            list_item = 0;
        }
        ui->listWidget->setCurrentRow(list_item);
        return true;
    }
}

bool Dialog::do_up(QObject *obj)
{
    if(obj == ui->comboBox_wifi)
    {
        ui->listWidget->setFocusPolicy(Qt::NoFocus);
        if(itemFlag != 0)
        {
            -- itemFlag ;
        }
        qDebug()<<"itemFlag==>>"<<itemFlag;
        ui->comboBox_wifi->setCurrentIndex(itemFlag);
        return true;
    }
    if(obj == ui->listWidget)
    {
        ui->comboBox_wifi->setFocusPolicy(Qt::NoFocus);
        if(list_item > 0)
        {
            -- list_item ;
        }
        else if(list_item == 0)
        {
            list_item = 4;
        }
        ui->listWidget->setCurrentRow(list_item);
        return true;
    }
}

bool Dialog::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->listWidget||ui->comboBox_wifi)
    {
        if(event->type() == QEvent::KeyPress)
        {
#ifdef OK
            wifidev->buzzerfun();
#endif
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if(keyEvent->key() == Qt::Key_Down)
            {
                return do_down(obj);
            }
            if(keyEvent->key() == Qt::Key_Up)
            {
                return do_up(obj);
            }
            if(keyEvent->key() == Qt::Key_A)
            {
                ui->listWidget->setFocus();
                ui->listWidget->setCurrentRow(0);
                system("wpa_supplicant -Dwext -iwlan0 -c /etc/b1_wpa_supplicant.conf &");
                if(!launch_wlan) //如果之前断开网卡，那么扫描前先启动网卡
                {
                    system("ifconfig wlan0 up");
                    launch_wlan = true;
                }
                return true;
            }
            if(keyEvent->key() == Qt::Key_B)
            {
                ui->listWidget->setFocus();
                ui->listWidget->setCurrentRow(1);
                return true;
            }
            if(keyEvent->key() == Qt::Key_C)
            {
                ui->listWidget->setFocus();
                ui->listWidget->setCurrentRow(2);
                return true;
            }
            if(keyEvent->key() == Qt::Key_D)
            {
                ui->listWidget->setFocus();
                ui->listWidget->setCurrentRow(3);
                return true;
            }
            if(keyEvent->key() == Qt::Key_E)
            {
                ui->listWidget->setFocus();
                ui->listWidget->setCurrentRow(4);
                return true;
            }
            if(keyEvent->key() == Qt::Key_Return)
            {
                    return dofun();
            }
            if(keyEvent->key() == Qt::Key_F1)
            {
                ui->listWidget->setFocus();
                ui->listWidget->setCurrentRow(0);
                return true;
            }
        }
        return QDialog::eventFilter(obj,event);
    }
    return QDialog::eventFilter(obj,event);
}

void Dialog::on_listWidget_clicked(const QModelIndex &index)
{
    dofun();
}

bool Dialog::dofun()
{
    if(this->focusWidget()->inherits("QListWidget"))
    {
        int ret = ui->listWidget->currentRow();
        qDebug()<<"ret==>>"<<ret;
        if(ret == 0)
        {
            pushbutton_scan();
        }
        if(ret == 1)
        {
            ui->comboBox_wifi->setFocus();
            ui->comboBox_wifi->setCurrentIndex(0);
            if(ui->comboBox_wifi->count() == 0)
                QMessageBox::warning(this,tr("提示"),("未能扫描到周围WIFI !"),QMessageBox::Yes);
        }
        if(ret == 2)
        {
            pushbutton_connect();
        }
        if(ret == 3)
        {
            ui->lineEdit_pwd->setFocus();
        }
        if(ret == 4)  //断开连接
        {
            system("ifconfig wlan0 down"); //卸掉网卡
            system("killall -9 wpa_supplicant");
            launch_wlan = false;
            if_scan = false;
            QMessageBox::warning(this,tr("提示"),("Wifi已经断开连接！"),QMessageBox::Yes);
            timer_speed->stop();
            ui->label_net_speed->hide();
            ui->groupBox_info->setTitle("Wifi已断开");
            ui->label_show_ip->setText("0.0.0.0");
            ui->label_show_gate->setText("0.0.0.0");
            ui->label_show_mask->setText("0.0.0.0");
            ui->comboBox_wifi->clear();
        }
        return true;
    }
    return false;
}


void Dialog::pushbutton_scan()
{
    ui->comboBox_wifi->clear();
    wifi_name.clear();
    wifi_level.clear();
    itemFlag = ui->comboBox_wifi->currentIndex();
    qDebug()<<"itemFlag===>>>"<<itemFlag;

    QString filename = "scan-wifi-list";
    QFile file(filename);
    file.open(QIODevice::ReadWrite);
    system("wpa_cli -i wlan0 scan");
    system(" wpa_cli -i wlan0 scan_result | awk -F 'ESS' '{print $2}'| cut -d ']' -f 2 | grep -v ^[[:space:]]$ >scan-wifi-list");
    system("sed -i '1d' scan-wifi-list");
    QTextStream stream(&file);
    QString name;
    QString level;

    while (!stream.atEnd())
    {
        name = stream.readLine().trimmed();
        wifi_name.append(name);
    }
    removeListSame(&wifi_name);

    ui->comboBox_wifi->currentText().clear();
    ui->comboBox_wifi->clear();

    int i;
    for (i = 0; i < wifi_name.count(); i++)
    {
        if(wifi_name[i] != "")  //去除空的wifi名
        {
            ui->comboBox_wifi->addItem(wifi_name[i],i);
        }
    }
    file.close();
}

void Dialog::pushbutton_connect()
{
    qDebug()<<"connect===>>";
    system("killall -9 wpa_supplicant");
    if(!wifi_name.isEmpty())
    {
        passwd = ui->lineEdit_pwd->text();
        if(passwd.isEmpty())
        {
            ui->groupBox_info->setTitle("error");
            return ;
        }
        connectWifi();
        show_link_msg(); //连接wifi后展示wifi信息
    }
}

void Dialog::removeListSame(QStringList *list) //去除重复wifi名
{
    for(int i = 0; i < list->count(); i++)
    {
        for(int k = i + 1; k < list->count(); k++)
        {
            if(list->at(i) == list->at(k))
            {
                list->removeAt(k);
                k--;
            }
        }
    }
}

void Dialog::cut_net_speed()//实时网速更新并写到文件中
{
    QString str_net_speed;
    system("ifconfig -a | sed '$!N;$!D'  > netSpeed");
    QFile file("netSpeed");
    file.open(QIODevice::ReadWrite);
    QTextStream stream(&file);
    while (!stream.atEnd())
    {
        str_net_speed = stream.readLine();
        if(str_net_speed != "")
            fun_cut(str_net_speed);
    }
}

void Dialog::fun_cut(QString str)//文件中截取网速值
{
    int num1,num2,num3,num4;
    QString str1 = str.remove(QRegExp("\\s"));
    num1 = str1.indexOf('(');
    num2 = str1.indexOf('(',num1+1);
    num3 = str1.indexOf(')');
    num4 = str1.indexOf(')',num3+1);

    deliver_speed = str1.mid(num1+1,num3-num1-1);
    download_speed = str1.mid(num2+1,num4-num2-1);
}

void Dialog::show_link_msg() //连接wifi后展示wifi信息
{
    if(result)
    {
        QMessageBox::warning(this,tr("提示"),("网络连接成功"),QMessageBox::Yes);
        system("ifconfig wlan0 192.168.1.147 broadcast 192.168.1.255 netmask 255.255.255.0 up");
        getInfomation();
    }
    else
    {
        ui->label_show_ip->setText("0.0.0.0");
        ui->label_show_gate->setText("0.0.0.0");
        ui->label_show_mask->setText("0.0.0.0");
        ui->groupBox_info->setTitle("connect error");
    }
}

void Dialog::show_net_speed()
{
    cut_net_speed();
    QString speed = "网速: " + deliver_speed + "/" + download_speed;
    ui->label_net_speed->setText(speed);
    if(ui->label_net_speed->isHidden())
        ui->label_net_speed->show();
    else
        ui->label_net_speed->hide();
}

void Dialog::connectWifi()
{
    QString nessaryMsg1 = "ap_scan=1";
    QString nessaryMsg2 = "update_config=1";
    QString nessaryMsg3= "network={";
    QString wName = "ssid=\""+ ui->comboBox_wifi->currentText() +'\"';  //wifi_name[wifi_which]

    QString str = ui->comboBox_wifi->currentText();
    qDebug()<<str;
    QString wPwd = "psk=\""+passwd+'\"';
    system("rm -rf wpa_supplicant.conf");
    system("cp /etc/b1_wpa_supplicant.conf wpa_supplicant.conf");
    QString filename = "wpa_supplicant.conf";
    QFile file(filename);
    file.open(QIODevice::WriteOnly|QIODevice::Append);
    QTextStream inputInfo(&file);
    inputInfo<<"\n"+nessaryMsg1;
    inputInfo<<"\n"+nessaryMsg2;
    inputInfo<<"\n"+nessaryMsg3;
    inputInfo<<"\n\t"+wName;
    inputInfo<<"\n\t"+wPwd;
    inputInfo<<"\n}";
    file.close();

    system("wpa_supplicant -Dwext -iwlan0 -c wpa_supplicant.conf -B");
    system("udhcpc -b -i wlan0 -R");
    QTest::qSleep(200);
    checkSuccess();
}

void Dialog::getInfomation()
{
    system("ifconfig > tempfile");
    system("grep -A 2 wlan0 tempfile > wifi-info");//获取ip
    QString filename = "wifi-info";
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    stream.readLine();
    QStringList section = stream.readLine().simplified().split(' ');

    QString ip = section[1].mid(5);
    QString gate = section[2].mid(6);
    QString mask = section[3].mid(5);
    ui->label_show_ip->setText(ip);
    ui->label_show_gate->setText(gate);
    ui->label_show_mask->setText(mask);

    ui->groupBox_info->setTitle("已连接");
}

void Dialog::checkSuccess()  //通过(wpa_cli -iwlan0 status)指令判断wifi是否连接成功
{

    system("wpa_cli -i wlan0 status | grep wpa_state > wifi_state");
    QString state_success = "wpa_state=COMPLETED";
    QString state_unknow;
    QFile file("wifi_state");
    file.open(QIODevice::ReadWrite);
    QTextStream stream(&file);
    while (!stream.atEnd())
    {
        state_unknow = stream.readLine().trimmed();
        qDebug()<<"state_unknow"<<state_unknow;
        if( state_success.compare(state_unknow)== 0)
        {
            result = true;
            ui->label_net_speed->show();
            show_net_speed();
            timer_speed->start(5000);
        }
        else
            result = false;
    }
}
