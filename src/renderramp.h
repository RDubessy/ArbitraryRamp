/* Copyright (C) 2011 Romain Dubessy */
#ifndef RENDERRAMP_H
#define RENDERRAMP_H

#include <QWidget>
#include <QPen>
#include <QBrush>
#include <QPoint>
#include <QList>

class RenderRamp : public QWidget
{
Q_OBJECT
public:
    RenderRamp(QWidget *parent = 0);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
    void updatePoints(const QList<QPoint> &p);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QPen pen;
    QBrush brush;
    QPoint *points;
    int nPoints;
};

#endif // RENDERRAMP_H
