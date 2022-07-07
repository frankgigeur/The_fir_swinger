/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCharts>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QLabel *label_pathCSV;
    QLineEdit *lineEdit_Kp;
    QLabel *label_10;
    QLabel *label_9;
    QFrame *line_3;
    QLineEdit *JsonKey;
    QLabel *label_2;
    QLabel *label;
    QDoubleSpinBox *PWMBox;
    QLabel *label_8;
    QLabel *label_3;
    QLabel *label_4;
    QFrame *line;
    QLineEdit *lineEdit_Kd;
    QLineEdit *lineEdit_Ki;
    QLabel *label_6;
    QTextBrowser *textBrowser;
    QLabel *label_7;
    QCheckBox *checkBox;
    QLabel *label_11;
    QLineEdit *lineEdit_DesVal;
    QSpinBox *DurationBox;
    QLabel *label_5;
    QChartView *graph;
    QPushButton *pulseButton;
    QLineEdit *lineEdit_Thresh;
    QComboBox *comboBoxPort;
    QPushButton *pushButton_Params;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(900, 700);
        MainWindow->setAcceptDrops(false);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setHorizontalSpacing(6);
        gridLayout->setVerticalSpacing(9);
        gridLayout->setContentsMargins(9, -1, -1, 9);
        label_pathCSV = new QLabel(centralWidget);
        label_pathCSV->setObjectName(QString::fromUtf8("label_pathCSV"));
        label_pathCSV->setFrameShape(QFrame::StyledPanel);
        label_pathCSV->setTextFormat(Qt::AutoText);

        gridLayout->addWidget(label_pathCSV, 6, 0, 1, 2);

        lineEdit_Kp = new QLineEdit(centralWidget);
        lineEdit_Kp->setObjectName(QString::fromUtf8("lineEdit_Kp"));

        gridLayout->addWidget(lineEdit_Kp, 7, 6, 1, 1);

        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout->addWidget(label_10, 9, 5, 1, 1);

        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout->addWidget(label_9, 8, 5, 1, 1);

        line_3 = new QFrame(centralWidget);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setFrameShape(QFrame::VLine);
        line_3->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line_3, 4, 4, 13, 1);

        JsonKey = new QLineEdit(centralWidget);
        JsonKey->setObjectName(QString::fromUtf8("JsonKey"));
        JsonKey->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(JsonKey, 15, 1, 1, 1);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 8, 0, 1, 1);

        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 7, 0, 1, 1);

        PWMBox = new QDoubleSpinBox(centralWidget);
        PWMBox->setObjectName(QString::fromUtf8("PWMBox"));
        PWMBox->setMinimum(-1.000000000000000);
        PWMBox->setMaximum(1.000000000000000);
        PWMBox->setSingleStep(0.100000000000000);

        gridLayout->addWidget(PWMBox, 7, 1, 1, 1);

        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout->addWidget(label_8, 7, 5, 1, 1);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 15, 0, 1, 1);

        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(label_4, 4, 3, 1, 1);

        line = new QFrame(centralWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 14, 2, 1, 1);

        lineEdit_Kd = new QLineEdit(centralWidget);
        lineEdit_Kd->setObjectName(QString::fromUtf8("lineEdit_Kd"));

        gridLayout->addWidget(lineEdit_Kd, 9, 6, 1, 1);

        lineEdit_Ki = new QLineEdit(centralWidget);
        lineEdit_Ki->setObjectName(QString::fromUtf8("lineEdit_Ki"));

        gridLayout->addWidget(lineEdit_Ki, 8, 6, 1, 1);

        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_6, 4, 5, 1, 2);

        textBrowser = new QTextBrowser(centralWidget);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        QFont font;
        font.setPointSize(9);
        textBrowser->setFont(font);

        gridLayout->addWidget(textBrowser, 5, 3, 11, 1);

        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 6, 5, 1, 1);

        checkBox = new QCheckBox(centralWidget);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        gridLayout->addWidget(checkBox, 5, 0, 1, 2);

        label_11 = new QLabel(centralWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout->addWidget(label_11, 10, 5, 1, 1);

        lineEdit_DesVal = new QLineEdit(centralWidget);
        lineEdit_DesVal->setObjectName(QString::fromUtf8("lineEdit_DesVal"));

        gridLayout->addWidget(lineEdit_DesVal, 6, 6, 1, 1);

        DurationBox = new QSpinBox(centralWidget);
        DurationBox->setObjectName(QString::fromUtf8("DurationBox"));
        DurationBox->setKeyboardTracking(false);
        DurationBox->setMinimum(0);
        DurationBox->setMaximum(5000);
        DurationBox->setSingleStep(25);

        gridLayout->addWidget(DurationBox, 8, 1, 1, 1);

        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 4, 0, 1, 1);

        graph = new QChartView(centralWidget);
        graph->setObjectName(QString::fromUtf8("graph"));

        gridLayout->addWidget(graph, 16, 0, 1, 4);

        pulseButton = new QPushButton(centralWidget);
        pulseButton->setObjectName(QString::fromUtf8("pulseButton"));

        gridLayout->addWidget(pulseButton, 9, 0, 1, 2);

        lineEdit_Thresh = new QLineEdit(centralWidget);
        lineEdit_Thresh->setObjectName(QString::fromUtf8("lineEdit_Thresh"));

        gridLayout->addWidget(lineEdit_Thresh, 10, 6, 1, 1);

        comboBoxPort = new QComboBox(centralWidget);
        comboBoxPort->setObjectName(QString::fromUtf8("comboBoxPort"));

        gridLayout->addWidget(comboBoxPort, 4, 1, 1, 1);

        pushButton_Params = new QPushButton(centralWidget);
        pushButton_Params->setObjectName(QString::fromUtf8("pushButton_Params"));

        gridLayout->addWidget(pushButton_Params, 14, 5, 1, 2);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Interface Identification", nullptr));
        label_pathCSV->setText(QString());
        label_10->setText(QCoreApplication::translate("MainWindow", "Valeur Kd", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Valeur Ki", nullptr));
        JsonKey->setText(QCoreApplication::translate("MainWindow", "potVex", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Duree (ms)", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Tension [-1,1]", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Valeur Kp", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Donnees brutes:", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Messages Json de l'Arduino:", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Execice PID", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Val. desiree", nullptr));
        checkBox->setText(QCoreApplication::translate("MainWindow", "Enregistrement des donnees sous:", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "Seuil", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Port:", nullptr));
        pulseButton->setText(QCoreApplication::translate("MainWindow", "Commande de pulse", nullptr));
        pushButton_Params->setText(QCoreApplication::translate("MainWindow", "Envoie Parametres", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
