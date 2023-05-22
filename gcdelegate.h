#ifndef GCDELEGATE_H
#define GCDELEGATE_H

#include <QItemDelegate>

class GCDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    GCDelegate(int ccolumn, int cotype, QObject *parent = 0);
    ~GCDelegate();
    void setDispFormat(int df){dispformat = df;}
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget* editr, const QModelIndex& index) const;
    void setModelData(QWidget* editr, QAbstractItemModel* model,const QModelIndex& index) const;
private slots:
    void commitAddCloseEditor();
private:
    int coordColumn;
    int coordtype;
    int dispformat;
};

#endif // GCDELEGATE_H
