#ifndef PHONONYOUTUBEPLAYER_H
#define PHONONYOUTUBEPLAYER_H

#include <QWidget>

#include <MediaObject>
#include <VideoWidget>
#include <AudioOutput>
#include <QToolButton>

#include <QNetworkAccessManager>



#include "VideoStreamer.h"
#include "VideoController.h"

class PhononYoutubePlayer : public QWidget
{
    Q_OBJECT
public:
    explicit PhononYoutubePlayer(QWidget *parent = 0);
    ~PhononYoutubePlayer();

    QNetworkAccessManager *NetworkMgr;
signals:

public slots:

    void PlayVideo( QString ID );

    void EnqueVideo( QString ID );

    void Play();

    void Pause();

    void Next();

    /*Dont call this function unless you does not wan't to play this video anymore*/
    void Stop();

    void TooglePlay();


protected slots:
    void readyReadError();

    void ReadyReadVideoInfo();

    void ProgressUpdate( qint64 loaded, qint64 total );

protected:
    void resizeEvent(QResizeEvent *event);


    //VideoBuffer *mBuffer;
    //YoutubeStream *mStream;
    VideoStreamer *mStream;

    VideoController *mControlls;

    Phonon::MediaObject *mMedia;

    Phonon::AudioOutput *mAudio;
    Phonon::VideoWidget *mVideo;




    //QToolButton *mPlayPause;

    bool JustStarted;

};

#endif // PHONONYOUTUBEPLAYER_H
