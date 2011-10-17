#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>

#include <QListWidget>

#include "Parser3.h"

#include "ListElement.h"

#include "PhononYoutubePlayer.h"

#include "DownloadDialog.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



protected:
    QNetworkAccessManager mNetworkMgr;

    Parser3 *mParser;

    QPointer<ListElement> mCurrentSel;

    QListWidgetItem* mMoreItems;

    BasePtr mCurContent;

    PhononYoutubePlayer *mPlayer;

    DownloadDialog *mDownload;
public slots:
    void SearchButton_Clicked();
    void InputChanged( QString Input );

    void VideoParsed( VideoPtr Video );
    void PlaylistParsed( VideoFeedPtr Playlist );
    void UserParsed( UserPtr User );

    void VideoSearchParsed( VideoFeedPtr Search );
    void PlaylistSearchParsed( PlaylistFeedPtr Search );
    void ChannelSearchParsed( ChannelFeedPtr Search );

    void VideosAddded( VideoFeedPtr Info );

    void ItemDoubleClicked( QListWidgetItem *Item );

    void selectionChanged_ItemView( QListWidgetItem *NewItem );
    void selectionChanged_ItemContent( QListWidgetItem *NewItem );


    /// Buttons
    void Play_Clicked();
    void Download_Clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
