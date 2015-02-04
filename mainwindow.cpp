#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QtMath>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->figLabel->setPixmap(QPixmap(":/fig/cylinderHemisphere.png"));
    ui->saveDirPathLineEdit->setText(QDir::homePath());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_generateButton_clicked()
{
    float deltaTheta, deltaPhi;
    float theta, phi;
    float r, h, layerPitch;
    float x,y,z;
    float interval;
    int revolution;

    x = y = z = theta = phi = 0;
    deltaTheta = ui->thetaSegmentsLineEdit->text().toFloat();
    deltaPhi = ui->phiSegmentsLineEdit->text().toFloat();
    phi = ui->phiLineEdit->text().toFloat();
    r = ui->radiusLineEdit->text().toFloat();
    h = ui->heightLineEdit->text().toFloat();
    layerPitch = ui->layerPitchLineEdit->text().toFloat();
    interval = ui->intervalLineEdit->text().toFloat();
    revolution = ui->revolutionLineEdit->text().toInt();

    float cylinder_r = r - interval * revolution;

    QString scanPattern = QString();
    scanPattern += "X0 Y0 Z0 I0 J0 k1 S0\n";

    // cylinder
    for (float h_i = 0;h_i*layerPitch<=h;h_i++)
    {
        z = h_i * layerPitch;

        for (int i=0;i<revolution;i++)
        {
            for( int j=0;j*deltaTheta<=360;j++)
            {
                float x = (cylinder_r + interval*i) * sinf(j*deltaTheta/180.0*M_PI);
                float y = (cylinder_r + interval*i) * cosf(j*deltaTheta/180.0*M_PI);
                if (abs(x) < 0.0000001)x=0;
                if (abs(y) < 0.0000001)y=0;

                scanPattern += QString("X%1 Y%2 Z%3 I0 J0 K1 S1\n")
                        .arg(x).arg(y).arg(z);
            }
        }
    }


    // hemisphere

    float sphere_z = z - (r / tanf(phi/180.0*M_PI)); // origin of sphere

    for (int i=1;i*deltaPhi<phi;i++)
    {
        float hemi_r = r * sinf((phi-i*deltaPhi)/180.0*M_PI) / sinf(phi/180.0*M_PI);
        for( int j=0;j*deltaTheta<=360;j++)
        {
            float x = hemi_r * sinf(j*deltaTheta/180.0*M_PI);
            float y = hemi_r * cosf(j*deltaTheta/180.0*M_PI);

            // normal Vector

            scanPattern += QString("X%1 Y%2 Z%3 I%4 J%5 K%6 S1\n")
                    .arg(x).arg(y).arg(z).arg(x).arg(y).arg(z-sphere_z);
        }
        z += (r / sinf(phi/180.0*M_PI)) * (cosf((phi - i*deltaPhi)/180.0*M_PI) - cosf((phi - (i-1) * deltaPhi)/180.0*M_PI));
    }

    QString fileName = QFileDialog::getSaveFileName(
                this,
                tr("Save Files"),
                QString(),
                QString(tr("Gcode Files(*.gcode)")));

    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {

        }else{
            QTextStream stream(&file);
            stream << scanPattern;
            stream.flush();
            file.close();
        }
    }
}
