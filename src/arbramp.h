/* Copyright (C) 2011 Romain Dubessy */
#ifndef ARBRAMP_H
#define ARBRAMP_H

#include <QMainWindow>
class RenderRamp;
class RampDelegate;

namespace Ui {
    class ArbRamp;
}


class ArbRamp : public QMainWindow {
    Q_OBJECT
public:
    ArbRamp(QWidget *parent = 0);
    ~ArbRamp();

signals:
    void renderUpdate(const QList<QPoint> &p);

protected:
    void closeEvent(QCloseEvent *);
    void changeEvent(QEvent *);
    void resizeEvent(QResizeEvent *);

private slots:
    void addPoint();
    void delPoint();
    void chooseAction(QAction *);
    void about();
    bool save();
    bool saveAs();
    bool open();
    void newRamp();
    void wasModified();
    void currentLine(int,int,int,int);
    bool exportRamp();

private:
    bool maybeSave();
    bool saveRamp(const QString &);
    bool readRamp(const QString &);
    bool rearPanelRamp(const QString &);
    void setCurrentFile(const QString &);
    void readSettings();
    void writeSettings();

    QString curFile;
    RenderRamp *render;
    RampDelegate *delegate;
    Ui::ArbRamp *ui;
    QAction *addAction;
    QAction *delAction;
    QAction *updateAction;
    int nLines;
    int curLine;
    bool isModified;
};

#endif // ARBRAMP_H
