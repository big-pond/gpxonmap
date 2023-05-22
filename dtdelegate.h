#ifndef DTDELEGATE_H
#define DTDELEGATE_H

#include <QItemDelegate>

class DTDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    DTDelegate(int column, QObject *parent = 0);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget* editr, const QModelIndex& index) const;
    void setModelData(QWidget* editr, QAbstractItemModel* model,const QModelIndex& index) const;
private slots:
    void commitAddCloseEditor();
private:
    int dtcolumn;
};

#endif // DTDELEGATE_H
