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
#include "renderramp.h"

RenderRamp::RenderRamp(QWidget *parent) : QWidget(parent) {
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    points=0;
    nPoints=0;
}

QSize RenderRamp::minimumSizeHint() const {
    return QSize(300,100);
}
QSize RenderRamp::sizeHint() const {
    return QSize(600,200);
}

void RenderRamp::updatePoints(const QList<QPoint> &p) {
    nPoints=p.size();
    if(points!=0)
        delete[] points;
    points=0;
    if(nPoints>0) {
        points=new QPoint[nPoints];
        for(int i=0;i<nPoints;i++)
            points[i]=p.at(i);
    }
    update();
}

void RenderRamp::paintEvent(QPaintEvent */*event*/) {
    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawPolyline(points,nPoints);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0,0,width()-1,height()-1));
}
/* renderramp.cpp */
