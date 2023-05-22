#include "hidcoldlg.h"
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QTableView>
#include <QCheckBox>

HidColDlg::HidColDlg(QWidget *parent) :   QDialog(parent)
{
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    setWindowTitle(tr("Видимость колонок"));
}

void HidColDlg::setTableView(QTableView* t)
{
    tv = t;
    vlayout = new QVBoxLayout;
    for(int i=0; i<t->model()->columnCount(); i++)
    {
        QCheckBox* chb = new QCheckBox(t->model()->headerData(i,Qt::Horizontal).toString());
        chb->setChecked(!t->isColumnHidden(i));
        connect(chb, SIGNAL(clicked()),this, SLOT(setColumnHidden()));
        vlayout->addWidget(chb);
    }
    vlayout->addWidget(buttonBox);
    setLayout(vlayout);
}

/*virtual*/
void HidColDlg::accept()
{
    setColumnHidden();
    QDialog::accept();
}

void HidColDlg::setColumnHidden()
{
    for(int i=0; i<tv->model()->columnCount(); i++)
    {
        QCheckBox* chb = dynamic_cast<QCheckBox*> (vlayout->itemAt(i)->widget());
        if(chb!=0)
        {
            tv->setColumnHidden(i,!chb->isChecked());
        }
    }
}
