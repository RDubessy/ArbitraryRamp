/* Copyright (C) 2011 Romain Dubessy */
#ifndef ARBRAMP_H
#define ARBRAMP_H

#include <QMainWindow>
#include <ww107x.h>
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
    double buildDataPoints(ViInt32 *data);
    void checkTaborError(ViStatus status, ViSession vi);

    QString curFile;            //!<\brief Current output file name.
    QString curDir;             //!<\brief Cuurent output directory.
    RenderRamp *render;
    RampDelegate *delegate;
    Ui::ArbRamp *ui;
    QAction *addAction;
    QAction *delAction;
    QAction *updateAction;
    int nLines;                 //!<\brief Number of frequency points.
    int curLine;                //!<\brief Current line index.
    bool isModified;            //!<\brief Modification tracking flag.
};

#endif // ARBRAMP_H
/* arbramp.h */