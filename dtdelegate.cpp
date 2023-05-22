#include "dtdelegate.h"

#include <QDateTimeEdit>

DTDelegate::DTDelegate(int column, QObject *parent) : QItemDelegate(parent)
{
    dtcolumn = column;
}

//DTDelegate::~DTDelegate()
//{

//}

QWidget* DTDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column()==dtcolumn)
    {
        QDateTimeEdit* editor = new  QDateTimeEdit(parent);
        editor->setDisplayFormat("dd.MM.yyyy hh:mm:ss");
        editor->setTimeSpec(Qt::UTC);
        connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAddCloseEditor()));
        return editor;
    }
    else
        return QItemDelegate::createEditor(parent, option, index);
}

void DTDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if(index.column() == dtcolumn)
    {
        QDateTime dt = index.model()->data(index, Qt::EditRole).toDateTime();
        QDateTimeEdit *dteditor = qobject_cast<QDateTimeEdit*>(editor);
//        dteditor->setDisplayFormat("dd.MM.yyyy hh:mm:ss");
        dteditor->setDateTime(dt);
    }
    else
        QItemDelegate::setEditorData(editor, index);
}

void DTDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if(index.column() == dtcolumn)
    {
        QDateTimeEdit *dteditor = qobject_cast<QDateTimeEdit*>(editor);
        model->setData(index, dteditor->dateTime());

    }
    else
        QItemDelegate::setModelData(editor, model, index);

}

void DTDelegate::commitAddCloseEditor()
{
    QDateTimeEdit *editor = qobject_cast<QDateTimeEdit*>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
