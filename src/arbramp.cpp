/* This file is a part of ArbitraryRamp. {{{
 * Copyright (C) 2011 Romain Dubessy
 *
 * ArbitraryRamp is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License, or 
 * (at your option) any later version.
 *
 * ArbitraryRamp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ArbitraryRamp.  If not, see <http://www.gnu.org/licenses/>.
 *
 * }}} */
#include <QtGui>
#include "rampdelegate.h"
#include "renderramp.h"
#include "arbramp.h"
#include "ui_arbramp.h"

ArbRamp::ArbRamp(QWidget *parent) : QMainWindow(parent), ui(new Ui::ArbRamp) {
    ui->setupUi(this);
    curLine=-1;
    nLines=0;
    render=new RenderRamp();
    ui->scrollArea->setWidget(render);
    delegate=new RampDelegate(ui->tableWidget);
    ui->tableWidget->setItemDelegate(delegate);
    readSettings();
    setCurrentFile("");
    addAction=new QAction(tr("Insert"),this);
    delAction=new QAction(tr("Delete"),this);
    updateAction=new QAction(tr("Update view"),this);
    ui->mainToolBar->addAction(addAction);
    ui->mainToolBar->addAction(delAction);
    ui->mainToolBar->addAction(updateAction);
    //Connect signal and slots.
    connect(ui->action_About,SIGNAL(triggered()),this,SLOT(about()));
    connect(ui->action_Quit,SIGNAL(triggered()),this,SLOT(close()));
    connect(ui->action_Save,SIGNAL(triggered()),this,SLOT(save()));
    connect(ui->actionSave_As,SIGNAL(triggered()),this,SLOT(saveAs()));
    connect(ui->action_Open,SIGNAL(triggered()),this,SLOT(open()));
    connect(ui->actionNew,SIGNAL(triggered()),this,SLOT(newRamp()));
    connect(ui->action_Export,SIGNAL(triggered()),this,SLOT(exportRamp()));
    connect(ui->mainToolBar,SIGNAL(actionTriggered(QAction*)),this,SLOT(chooseAction(QAction*)));
    connect(this,SIGNAL(renderUpdate(QList<QPoint>)),render,SLOT(updatePoints(QList<QPoint>)));
    connect(ui->tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(currentLine(int,int,int,int)));
    show();         //So that wasModified has access to the real size of the RenderRamp object.
    wasModified();
}

ArbRamp::~ArbRamp() {
    delete ui;
}

void ArbRamp::closeEvent(QCloseEvent *event) {
    if(maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void ArbRamp::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ArbRamp::resizeEvent(QResizeEvent *) {
    wasModified();
}

void ArbRamp::chooseAction(QAction *action) {
    if(action==addAction)
        addPoint();
    else if(action==delAction)
        delPoint();
    else
        curLine=-1;
    wasModified();
}

void ArbRamp::addPoint() {
    int row=(curLine==-1 ? nLines : curLine+1);
    ui->tableWidget->insertRow(row);
    double t=0;
    double freq=1;
    if(nLines>0) {
        if(row==nLines) {
           t=ui->tableWidget->item(nLines-1,0)->text().toDouble()+1;
        } else {
            t=ui->tableWidget->item(row-1,0)->text().toDouble();
            t+=ui->tableWidget->item(row+1,0)->text().toDouble();
            t/=2;
        }
        freq=ui->tableWidget->item(row-1,1)->text().toDouble();
    }
    QTableWidgetItem *time=new QTableWidgetItem(tr("%1").arg(t));
    ui->tableWidget->setItem(row,0,time);
    QTableWidgetItem *frequency=new QTableWidgetItem(tr("%1").arg(freq));
    ui->tableWidget->setItem(row,1,frequency);
    QTableWidgetItem *type=new QTableWidgetItem(tr("Linear"));
    ui->tableWidget->setItem(row,2,type);
    nLines++;
}

void ArbRamp::delPoint() {
    if(nLines>0) {
        int row=(nLines==1 ? 0 : (curLine==-1 ? nLines-1 : curLine));
        ui->tableWidget->removeRow(row);
        nLines--;
    }
    if(nLines==0)
        curLine=-1;
}

void ArbRamp::about() {
   QMessageBox::about(this, tr("About Arbitrary Ramp"),
            tr("The <b>Arbitrary Ramp</b> application."));
}

void ArbRamp::newRamp() {
    for(int i=0;i<nLines;i++)
        ui->tableWidget->removeRow(0);
    nLines=0;
    curFile="";
    wasModified();
}

void ArbRamp::wasModified() {
    isModified=true;
    QList<QPoint> points;
    double tMax=0;
    if(nLines>0)
        tMax=ui->tableWidget->item(nLines-1,0)->text().toDouble();
    for(int i=0;i<nLines;i++) {
        int x=(int)(render->width()*ui->tableWidget->item(i,0)->text().toDouble()/tMax);
        int y=(int)(render->height()*(1.0-ui->tableWidget->item(i,1)->text().toDouble()/1e2));
        points << QPoint(x,y);
    }
    emit renderUpdate(points);
}

bool ArbRamp::maybeSave() {
    if(isModified && nLines>0) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Arbitrary Ramp"),
                     tr("The ramp has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

bool ArbRamp::save() {
    if(curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveRamp(curFile);
    }
}

bool ArbRamp::saveAs() {
    QString fileName = QFileDialog::getSaveFileName(this);
    if(fileName.isEmpty())
        return false;
    return saveRamp(fileName);
}

bool ArbRamp::open() {
    QString fileName = QFileDialog::getOpenFileName(this);
    if(fileName.isEmpty()||!readRamp(fileName))
        return false;
    wasModified();
    isModified=false;
    return true;
}

void ArbRamp::currentLine(int row, int, int, int) {
    curLine=row;
}

bool ArbRamp::saveRamp(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Arbitrary Ramp"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out << nLines << "\n";
    for(int i=0;i<nLines;i++) {
        out << ui->tableWidget->item(i,0)->text() << " ";
        out << ui->tableWidget->item(i,1)->text() << " ";
        out << ui->tableWidget->item(i,2)->text() << "\n";
    }
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

bool ArbRamp::readRamp(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Arbitrary Ramp"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    //Delete previously stored ramp
    for(int i=0;i<nLines;i++) {
        ui->tableWidget->removeRow(0);
    }
    nLines=0;
    QTextStream in(&file);
    nLines=in.readLine().toInt();
    for(int i=0;i<nLines;i++) {
        ui->tableWidget->insertRow(i);
        QString tmp=in.readLine();
        QStringList data=tmp.split(" ");
        QTableWidgetItem *time=new QTableWidgetItem(data.at(0));
        ui->tableWidget->setItem(i,0,time);
        QTableWidgetItem *frequency=new QTableWidgetItem(data.at(1));
        ui->tableWidget->setItem(i,1,frequency);
        QTableWidgetItem *type=new QTableWidgetItem(data.at(2));
        ui->tableWidget->setItem(i,2,type);
    }
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("Ramp loaded"), 2000);
    return true;
}

void ArbRamp::setCurrentFile(const QString &fileName)
 {
     curFile = fileName;
     isModified=false;
     QString shownName = curFile;
     if (curFile.isEmpty())
         shownName = "untitled.txt";
     setWindowFilePath(shownName);
 }

void ArbRamp::readSettings()
{
    QSettings settings("R. Dubessy", "Arbitrary Ramp");
    nLines=settings.value("nLines", 0).toInt();
    for(int i=0;i<nLines;i++) {
        ui->tableWidget->insertRow(i);
        QTableWidgetItem *time=new QTableWidgetItem(settings.value(tr("time%1").arg(i),"0").toString());
        ui->tableWidget->setItem(i,0,time);
        QTableWidgetItem *frequency=new QTableWidgetItem(settings.value(tr("frequency%1").arg(i),"1000").toString());
        ui->tableWidget->setItem(i,1,frequency);
        QTableWidgetItem *type=new QTableWidgetItem(settings.value(tr("type%1").arg(i),"Linear").toString());
        ui->tableWidget->setItem(i,2,type);
    }
}

void ArbRamp::writeSettings()
{
    QSettings settings("R. Dubessy", "Arbitrary Ramp");
    settings.setValue("nLines",nLines);
    for(int i=0;i<nLines;i++) {
        settings.setValue(tr("time%1").arg(i),ui->tableWidget->item(i,0)->text());
        settings.setValue(tr("frequency%1").arg(i),ui->tableWidget->item(i,1)->text());
        settings.setValue(tr("type%1").arg(i),ui->tableWidget->item(i,2)->text());
    }
}

bool ArbRamp::exportRamp() {
    if(nLines<2) {
        QMessageBox::warning(this,tr("Export Ramp"),
                             tr("Need at least two points to export a ramp !"),
                             QMessageBox::Ok,QMessageBox::Ok);
        return false;
    }
    bool ok;
    QStringList items;
    items << "Front Panel" << "Rear Panel";
    QString output=QInputDialog::getItem(this,tr("Export Ramp"),tr("Choose output:"),items,1,false,&ok);
    if(ok) {
        QString fileName = QFileDialog::getSaveFileName(this);
        if(fileName.isEmpty()||!rearPanelRamp(fileName))
            return false;

        return true;
    }
    return false;
}
bool ArbRamp::rearPanelRamp(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Arbitrary Ramp"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    //Build data
    double data[20000];
    double Tmax=ui->tableWidget->item(nLines-1,0)->text().toDouble();
    double step=Tmax/20000;
    double tprev,tnext,fprev,fnext;
    tprev=tnext=0;
    fprev=fnext=ui->tableWidget->item(0,1)->text().toDouble();
    data[0]=fprev*1e6*10;
    double c=0;
    for(int i=1;i<19999;i++) {
        double t=i*step;
        if(t>tnext) {
            tprev=tnext;
            fprev=fnext;
            c++;
            tnext=ui->tableWidget->item(c,0)->text().toDouble();
            fnext=ui->tableWidget->item(c,1)->text().toDouble();
        }
        data[i]=((t-tprev)/(tnext-tprev)*(fnext-fprev)+fprev)*1e6*10;
    }
    data[19999]=ui->tableWidget->item(nLines-1,1)->text().toDouble()*1e6*10;
    //Export to file
    QTextStream out(&file);
    for(int i=0;i<20000;i++)
        out << data[i] << " ";
    statusBar()->showMessage(tr("Rear panel ramp exported"), 2000);
    return true;
}
/* arbramp.cpp */
