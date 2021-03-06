#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMap>
#include <QDateTime>
#include <QTimer>
#include <QFileDialog>
#include <QStringList>
#include <QTextStream>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_OpenComm_clicked();

    void on_ImportFile_clicked();

    void readyReadData();
    void ReadData();
    void WriteData();

    void on_clear_clicked();

private:
    Ui::MainWindow *ui;

    QSerialPort *serial;

    QMap<QString,QString> map_importinfo;
    QStringList fileNames;
    void selectFile();
    void readFileInfo();

    quint8 findValueOfReturn(QString key,QString &value,QByteArray &send);
    QByteArray baSendbuf;
    QString strreturn;


};

#endif // MAINWINDOW_H

