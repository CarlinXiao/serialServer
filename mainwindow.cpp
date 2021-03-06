#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QListView>
#include "windows.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //查找可用的串口
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->comboBox_port->addItem(serial.portName());
            serial.close();
        }
    }

    //设置波特率下拉菜单默认显示第0项
    ui->comboBox_baudreat->addItem("1200");
    ui->comboBox_baudreat->addItem("2400");
    ui->comboBox_baudreat->addItem("4800");
    ui->comboBox_baudreat->addItem("9600");
    ui->comboBox_baudreat->addItem("19200");
    ui->comboBox_baudreat->addItem("38400");
    ui->comboBox_baudreat->addItem("57600");
    ui->comboBox_baudreat->addItem("115200");


    ui->comboBox_check->addItem("n");
    ui->comboBox_check->addItem("e");
    ui->comboBox_check->addItem("o");
    ui->comboBox_check->addItem("s");
    ui->comboBox_check->addItem("m");

    ui->comboBox_data->addItem("5");
    ui->comboBox_data->addItem("6");
    ui->comboBox_data->addItem("7");
    ui->comboBox_data->addItem("8");

    ui->comboBox_stop->addItem("1");
    ui->comboBox_stop->addItem("1.5");
    ui->comboBox_stop->addItem("2");

    ui->comboBox_flow->addItem("None");
    ui->comboBox_flow->addItem("RTS/CTS");
    ui->comboBox_flow->addItem("XON/XOFF");

    ui->comboBox_port->setCurrentIndex(0);
    ui->comboBox_baudreat->setCurrentIndex(4);
    ui->comboBox_check->setCurrentIndex(0);
    ui->comboBox_data->setCurrentIndex(3);
    ui->comboBox_stop->setCurrentIndex(0);
    ui->comboBox_flow->setCurrentIndex(0);
    ui->lineEdit_time->setText("20");

    ui->comboBox_port->setView(new QListView());
    ui->comboBox_baudreat->setView(new QListView());
    ui->comboBox_check->setView(new QListView());
    ui->comboBox_data->setView(new QListView());
    ui->comboBox_stop->setView(new QListView());
    ui->comboBox_flow->setView(new QListView());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_OpenComm_clicked()
{
    if(ui->OpenComm->text() == "打开串口")
    {
        serial = new QSerialPort;
        //设置串口名
        serial->setPortName(ui->comboBox_port->currentText());
        //打开串口
        serial->open(QIODevice::ReadWrite);
        //设置波特率
        serial->setBaudRate(ui->comboBox_baudreat->currentText().toInt());
        //设置数据位数
        switch (ui->comboBox_data->currentText().toInt())
        {
        case 5:
            serial->setDataBits(QSerialPort::Data5);
            break;
        case 6:
            serial->setDataBits(QSerialPort::Data6);
            break;
        case 7:
            serial->setDataBits(QSerialPort::Data7);
        default:
            serial->setDataBits(QSerialPort::Data8);
            break;
        }

        //设置校验位
        switch (ui->comboBox_check->currentIndex())
        {
        case 0:
            serial->setParity(QSerialPort::NoParity);
            break;
        case 1:
            serial->setParity(QSerialPort::EvenParity);
            break;
        case 2:
            serial->setParity(QSerialPort::OddParity);
            break;
        case 3:
            serial->setParity(QSerialPort::SpaceParity);
            break;
        case 4:
            serial->setParity(QSerialPort::MarkParity);
            break;
        default:
            serial->setParity(QSerialPort::NoParity);
            break;
        }
        //设置停止位
        switch (ui->comboBox_stop->currentIndex())
        {
        case 0:
            serial->setStopBits(QSerialPort::OneStop);//停止位设置为1
            break;
        case 1:
            serial->setStopBits(QSerialPort::OneAndHalfStop);//停止位设置为1.5
            break;
        case 2:
            serial->setStopBits(QSerialPort::TwoStop);//停止位设置为2
        default:
            serial->setStopBits(QSerialPort::OneStop);//停止位设置为1
            break;
        }
        //设置流控制
        switch (ui->comboBox_flow->currentIndex())
        {
        case 0:
            serial->setFlowControl(QSerialPort::NoFlowControl);
            break;
        case 1:
            serial->setFlowControl(QSerialPort::HardwareControl);
            break;
        case 2:
            serial->setFlowControl(QSerialPort::SoftwareControl);
        default:
            serial->setFlowControl(QSerialPort::NoFlowControl);
            break;
        }


        //关闭设置菜单使能
        ui->comboBox_port->setEnabled(false);
        ui->comboBox_baudreat->setEnabled(false);
        ui->comboBox_check->setEnabled(false);
        ui->comboBox_data->setEnabled(false);
        ui->comboBox_stop->setEnabled(false);
        ui->comboBox_flow->setEnabled(false);
        ui->ImportFile->setEnabled(false);

        ui->OpenComm->setText("关闭串口");

        //连接信号槽
        QObject::connect(serial,SIGNAL(readyRead()),this,SLOT(readyReadData()));

    }
    else
    {
        //关闭串口
        serial->clear();
        serial->close();
        serial->deleteLater();

        //恢复设置使能
        ui->comboBox_port->setEnabled(true);
        ui->comboBox_baudreat->setEnabled(true);
        ui->comboBox_check->setEnabled(true);
        ui->comboBox_data->setEnabled(true);
        ui->comboBox_stop->setEnabled(true);
        ui->comboBox_flow->setEnabled(true);
        ui->ImportFile->setEnabled(true);
        ui->OpenComm->setText("打开串口");
    }
}

void MainWindow::on_ImportFile_clicked()
{
    map_importinfo.clear();
    selectFile();
    readFileInfo();
}

void MainWindow::readyReadData()
{
    QTimer::singleShot(50,this,SLOT(ReadData()));
}

void MainWindow::ReadData()
{
    QByteArray buf;
    buf = serial->readAll();
    if(!buf.isEmpty())
    {
        QString str;
        QString str1;
        QString str2 = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz[recv] ");
        str.clear();
        for(int i = 0; i < buf.length();i++)
        {
            str1.sprintf("%02X ",(quint8)buf.at(i));
            str.append(str1);
        }
        str2 += str;
        ui->textBrowser->append(str2);
        if(findValueOfReturn(str,strreturn,baSendbuf) == 1)
        {
            qint32 itimewite = ui->lineEdit_time->text().toInt();
            QTimer::singleShot(itimewite,this,SLOT(WriteData()));
        }

    }
    buf.clear();
}

void MainWindow::WriteData()
{
    serial->write(baSendbuf);
    QString str = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz");
    str += "[send] " + strreturn;
    ui->textBrowser->append(str);
    serial->waitForBytesWritten();
}

void MainWindow::on_clear_clicked()
{
    ui->textBrowser->clear();
}

/****************************************
* Qt中使用文件选择对话框步骤如下：
* 1. 定义一个QFileDialog对象
* 2. 设置路径、过滤器等属性
*****************************************/
void MainWindow::selectFile()
{
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(tr("打开文件"));
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
    fileDialog->setNameFilter(tr("Text files (*.txt)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFile
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //打印所有选择的文件的路径
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
    }
}

void MainWindow::readFileInfo()
{
    if(fileNames.isEmpty())
    {
        return;
    }
    QFile file(fileNames.at(0));
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        file.close();
        return ;
    }

    QString key,value;
    key.clear();
    value.clear();

    QTextStream in(&file);
    QString line = in.readLine();
    while (!line.isNull())
    {
        if(line.at(0) == '#' )
        {       //跳过注释
            line = in.readLine();
            continue;
        }

        if(key.isEmpty())
        {
            key = line;
            line = in.readLine();
            continue;
        }else{
            value = line;
        }
        QString str;
        str.clear();
        key = key.toUpper();
        value = value.toUpper();
        QStringList slkey = key.split(" ",QString::SkipEmptyParts);
        for(int i = 0; i < slkey.length();i++)
        {
            str += QString("%1 ").arg(slkey.at(i));
        }

        map_importinfo.insert(key,value);

        key.clear();
        value.clear();

        line = in.readLine();
    }
    file.close();
    return ;


}

quint8 MainWindow::findValueOfReturn(QString key, QString &value,QByteArray &send)
{
    if(map_importinfo.contains(key))
    {
        value = map_importinfo.value(key);

        bool ok;
        value = value.trimmed();    //去掉头尾空格
        value = value.simplified();  //将多个连续空格改为一个空格
        QStringList sl = value.split(" ",QString::SkipEmptyParts);

        send.clear();
        foreach (QString s, sl) {
            if(!s.isEmpty())
            {
                char c = s.toInt(&ok,16)&0xFF;
                if(ok){
                    send.append(c);
                }
            }
        }
        return 1;
    }
    return 0;
}

