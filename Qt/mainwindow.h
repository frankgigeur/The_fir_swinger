#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QCloseEvent>
#include <QDebug>
#include <QtWidgets>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSerialPortInfo>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts>

// Propres librairies
#include "csvwriter.h"
#include "serialprotocol.h"

// Classe definissant l'application
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    const qint32 BAUD_RATE = 115200;

    explicit MainWindow(int updateRate, QWidget *parent = nullptr);
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;
    void closeEvent(QCloseEvent *event) override;

    //void sendMessage(QString msg);
    void setUpdateRate(int rateMs);

    void onPeriodicUpdate();
    void onMessageReceived(QString);

    void portCensus(QComboBox *cbUsed = nullptr);
    void connectComboBox(QComboBox *cbUsed = nullptr);
    QTimer updateTimer_;

public slots:
    void sendMessage(QString msg);

private slots:
    void receiveFromSerial(QString);
    void sendPulseSetting();
    void sendPulseStart();
    void manageRecording(int);
    void changeJsonKeyValue();
    void startSerialCom(QString);
    void sendPID();

private:
    void connectTimers(int updateRate);
    void connectButtons();
    void connectSerialPortRead();
    void connectSpinBoxes();
    void startRecording();
    void stopRecording();
    void connectTextInputs();

    bool record = false;
    CsvWriter* writer_;
    //QTimer updateTimer_;
    QString msgReceived_{""};
    QString msgBuffer_{""};
    SerialProtocol* serialCom_=nullptr;

    QString JsonKey_;
    QLineSeries series_;
    QChart chart_;


protected:
    Ui::MainWindow *ui;

signals:
    void jsonEmit(QJsonObject json);

};

#endif // MAINWINDOW_H
