#include "mapinfodlg.h"
#include <QLabel>
#include <QDialogButtonBox>
#include <QVBoxLayout>

MapInfoDlg::MapInfoDlg(QDialog *parent)
    : QDialog{parent}
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    lbInfo = new QLabel();
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    QVBoxLayout* vl = new QVBoxLayout();
    vl->setSpacing(4);
    vl->setContentsMargins(8, 1, 8, 1);
    vl->addWidget(lbInfo);
    vl->addWidget(buttonBox);
    setLayout(vl);
    connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
    //        buttonBox.accepted.connect(accept);
    //        self.buttonBox.rejected.connect(self.reject);
    setWindowTitle(tr("About map"));
}


void MapInfoDlg::setMapProperty(const QString& prop)
{
    lbInfo->setText(prop);
}
