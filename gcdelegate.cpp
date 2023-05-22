#include "gcdelegate.h"

#include "geocoordedit.h"

GCDelegate::GCDelegate(int ccolumn, int cotype, QObject *parent) : QItemDelegate(parent)
{
    coordColumn = ccolumn;
    coordtype = cotype;
}

GCDelegate::~GCDelegate()
{

}

void GCDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column()==coordColumn)
    {
        QString scoord = index.model()->data(index, Qt::DisplayRole).toString();
        QStyleOptionViewItem coOption = option;
        coOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
//        coOption.features = QStyleOptionViewItem::HasDecoration;
        drawDisplay(painter, coOption, coOption.rect, scoord);
        drawFocus(painter, coOption, coOption.rect);
    }
    else
        QItemDelegate::paint(painter, option, index);

}

QWidget* GCDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column()==coordColumn)
    {
        GeoCoordEdit* gceditor = new  GeoCoordEdit(static_cast<GeoPoint::CoordType>(coordtype), parent);
        connect(gceditor, SIGNAL(editingFinished()), this, SLOT(commitAddCloseEditor()));
        return gceditor;
    }
    else
        return QItemDelegate::createEditor(parent, option, index);
}

void GCDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if(index.column() == coordColumn)
    {
        double val = index.model()->data(index, Qt::EditRole).toDouble();
        GeoCoordEdit *gceditor = qobject_cast<GeoCoordEdit*>(editor);
        gceditor->setDisplayFormat(static_cast<GeoPoint::DispFormat>(dispformat));
        gceditor->setValue(val);
    }
    else
    {
        QItemDelegate::setEditorData(editor, index);
    }
}

void GCDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if(index.column() == coordColumn)
    {
        GeoCoordEdit *gceditor = qobject_cast<GeoCoordEdit*>(editor);
        double val = gceditor->value();
        model->setData(index, val);

    }
    else
    {
        QItemDelegate::setModelData(editor, model, index);
    }

}
void GCDelegate::commitAddCloseEditor()
{
    GeoCoordEdit *gceditor = qobject_cast<GeoCoordEdit*>(sender());
    emit commitData(gceditor);
    emit closeEditor(gceditor);
}
