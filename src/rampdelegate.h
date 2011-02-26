/* Copyright (C) 2011 Romain Dubessy */
#ifndef RAMPDELEGATE_H
#define RAMPDELEGATE_H

#include <QItemDelegate>

class RampDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    RampDelegate(QObject *parent = 0) : QItemDelegate(parent) {
        table=parent;
    };

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                               const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                           const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
             const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QObject *table;
};

#endif // RAMPDELEGATE_H
