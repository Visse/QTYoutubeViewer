#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QDialog>


namespace Ui {
    class Dialog;
}

struct DownloadInfo {

};

class DownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadDialog(QWidget *parent = 0);
    ~DownloadDialog();


    void setVideoID( QString ID );
private:
    Ui::Dialog *ui;
};

#endif // DOWNLOADDIALOG_H
