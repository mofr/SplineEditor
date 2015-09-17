#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "SplineView.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->controlsHint->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->paramPanel->hide();

    connect(ui->actionUndo, &QAction::triggered, this, [this](){
        ui->splineView->undo();
    });

    connect(ui->actionRedo, &QAction::triggered, this, [this](){
        ui->splineView->redo();
    });

    connect(ui->actionNew, &QAction::triggered, this, [this](){
        ui->splineView->setSpline(Spline());
    });

    connect(ui->actionQuit, &QAction::triggered, this, [](){
        qApp->exit();
    });

    connect(ui->actionSave, &QAction::triggered, this, [this](){
        save();
    });

    connect(ui->actionOpen, &QAction::triggered, this, [this](){
        load();
    });

    connect(ui->splineView, &SplineView::knotSelected, this, [this](Spline::Knot knot){
        ui->paramPanel->show();
        ui->xSpinBox->setValue(knot.x());
        ui->ySpinBox->setValue(knot.y());
        ui->tensionSpinBox->setValue(knot.t);
        ui->biasSpinBox->setValue(knot.b);
        ui->continuitySpinBox->setValue(knot.c);
    });

    connect(ui->splineView, &SplineView::knotDeselected, this, [this](){
        ui->paramPanel->hide();
    });

    connect(ui->xSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateKnot()));
    connect(ui->ySpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateKnot()));
    connect(ui->tensionSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateKnot()));
    connect(ui->biasSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateKnot()));
    connect(ui->continuitySpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateKnot()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::save()
{
    QString filename = QFileDialog::getSaveFileName(this, QString(), QString(), "Splines (*.spline)");
    if(filename.isEmpty())
    {
        return;
    }

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, tr("File save error"), tr("Can't open/create selected file"));
        return;
    }

    QTextStream stream(&file);
    for(const Spline::Knot & knot : ui->splineView->getSpline().getKnots())
    {
        QString str;
        stream << str.sprintf("%6.6f", knot.x()) << " ";
        stream << str.sprintf("%6.6f", knot.y()) << " ";
        stream << str.sprintf("%6.6f", knot.t) << " ";
        stream << str.sprintf("%6.6f", knot.b) << " ";
        stream << str.sprintf("%6.6f", knot.c) << "\n";
    }
}

void MainWindow::load()
{
    QString filename = QFileDialog::getOpenFileName(this, QString(), QString(), "Splines (*.spline)");
    if(filename.isEmpty())
    {
        return;
    }

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("File load error"), tr("Can't open selected file"));
        return;
    }

    Spline spline;
    bool ok = true;

    QTextStream stream(&file);
    QString line;
    while( !(line = stream.readLine()).isEmpty() )
    {
        QStringList knotParams = line.split(" ");
        if(knotParams.size() != 5)
        {
            QMessageBox::warning(this, tr("File format error"), tr("Wrong number of knot parameters"));
            return;
        }
        qreal x = knotParams[0].toDouble(&ok);
        if(!ok)
        {
            QMessageBox::warning(this, tr("File format error"), tr("Invalid X parameter"));
            return;
        }
        qreal y = knotParams[1].toDouble(&ok);
        if(!ok)
        {
            QMessageBox::warning(this, tr("File format error"), tr("Invalid Y parameter"));
            return;
        }
        float t = knotParams[2].toDouble(&ok);
        if(!ok)
        {
            QMessageBox::warning(this, tr("File format error"), tr("Invalid tension parameter"));
            return;
        }
        float b = knotParams[3].toDouble(&ok);
        if(!ok)
        {
            QMessageBox::warning(this, tr("File format error"), tr("Invalid bias parameter"));
            return;
        }
        float c = knotParams[4].toDouble(&ok);
        if(!ok)
        {
            QMessageBox::warning(this, tr("File format error"), tr("Invalid continuity parameter"));
            return;
        }

        spline.add(Spline::Knot(x, y, t, b, c));
    }

    if(stream.status() != QTextStream::Ok)
    {
        QMessageBox::warning(this, tr("File read error"), tr("File read error"));
        return;
    }

    ui->splineView->setSpline(spline);
}

void MainWindow::updateKnot()
{
    Spline::Knot knot(ui->xSpinBox->value(),
                      ui->ySpinBox->value(),
                      ui->tensionSpinBox->value(),
                      ui->biasSpinBox->value(),
                      ui->continuitySpinBox->value());
    ui->splineView->updateSelectedKnot(knot);
}
