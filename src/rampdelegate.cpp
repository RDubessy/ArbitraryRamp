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
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include <QTableWidget>
#include "rampdelegate.h"
QWidget *RampDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */,
                                    const QModelIndex & index) const {
    int j=index.column();
    int i=index.row();
    if(j==0) {
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        double min=0;
        double max=100;
        if(i==0) {
            max=min=0;
        } else {
            min=static_cast<QTableWidget*>(table)->item(i-1,j)->text().toDouble()+1e-4;
            int n=static_cast<QTableWidget*>(table)->rowCount();
            if(i<n-1)
                max=static_cast<QTableWidget*>(table)->item(i+1,j)->text().toDouble()-1e-4;
        }
        editor->setMinimum(min);
        editor->setMaximum(max);
        editor->setDecimals(4);
        return editor;
    } else if(j==1) {
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setMinimum(0.000001);
        editor->setMaximum(100);
        editor->setDecimals(6);
        return editor;
    } else if(j==2) {
        QComboBox *editor = new QComboBox(parent);
        editor->addItem(tr("Linear"),0);
        return editor;
    }
    return new QTextEdit();
}
void RampDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const {
    QVariant value=index.model()->data(index,Qt::EditRole);
    int j=index.column();
    if(j==0 || j==1) {
        static_cast<QDoubleSpinBox*>(editor)->setValue(value.toDouble());
    } else if(j==2) {
        int index=0;
        if(value==tr("Linear"))
            index=0;
        static_cast<QComboBox*>(editor)->setCurrentIndex(index);
    } else {
        static_cast<QTextEdit*>(editor)->setText(value.toString());
    }
}
void RampDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const {
    int j=index.column();
    if(j==0 || j==1) {
        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->interpretText();
        double value = spinBox->value();
        model->setData(index, value, Qt::EditRole);
    } else if(j==2) {
        QComboBox *comboBox=static_cast<QComboBox*>(editor);
        model->setData(index,comboBox->currentText(),Qt::EditRole);
    }
}

void RampDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const {
    editor->setGeometry(option.rect);
}
/* rampdelegate.cpp */