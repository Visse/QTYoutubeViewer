#include "include/DownloadDialog.h"
#include "ui_DownloadDialog.h"

DownloadDialog::DownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

DownloadDialog::~DownloadDialog()
{
    delete ui;
}

void DownloadDialog::setVideoID(QString ID) {

}
