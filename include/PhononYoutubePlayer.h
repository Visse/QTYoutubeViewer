#ifndef PHONONYOUTUBEPLAYER_H
#define PHONONYOUTUBEPLAYER_H

#include <QWidget>

#include <MediaObject>
#include <VideoWidget>
#include <AudioOutput>

#include <QToolButton>

#include <QProcess>
#include <QBuffer>
#include <QTimer>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <AbstractMediaStream>

#include <QPointer>
#include <QEventLoop>
#include <Log.h>

#include <iostream>


#include "VideoStreamer.h"

class VideoBuffer : public QBuffer {
    Q_OBJECT
public:
    VideoBuffer( QNetworkReply *Stream, bool ownsStream=false ) :
        QBuffer(), mStream(Stream), mOwnsStream(ownsStream)
    {

        connect( mStream, SIGNAL(readyRead()), SLOT(readyRead()) );
    }

    ~VideoBuffer() {
        if( mOwnsStream )
        {
            mStream->abort();
            mStream->deleteLater();
        }
    }

    virtual void reserve( qint64 len ) {
        buffer().reserve(len);
    }

    virtual qint64 capacity() const {
        buffer().capacity();
    }

    /*qint64 size() const {
        return buffer().capacity();
    }*/


    bool isSequential() const {
        return true;
    }

protected slots:
    qint64 writeData(const char *data, qint64 len) {
        buffer().append(data,len);
        bytesWritten(len);

        return len;
    }


    void readyRead() {
        write( mStream->readAll() );
    }

protected:

    QNetworkReply *mStream;
    bool mOwnsStream;


};



class YoutubeStream : public Phonon::AbstractMediaStream {
    Q_OBJECT
public:
    YoutubeStream( QUrl url, QNetworkAccessManager *Manager,QObject *parent ) :
        Phonon::AbstractMediaStream(parent), Buffer(), curOfsett(0)
    {
        Reply = Manager->get(QNetworkRequest(url));
       // Reply->setReadBufferSize(0);
        connect( Reply.data(), SIGNAL(error(QNetworkReply::NetworkError)), SLOT(NetworkError(QNetworkReply::NetworkError)) );
        connect( Reply.data(), SIGNAL(finished()), SLOT(FullVideoResived()) );
    }

    ~YoutubeStream() {
        if( !Reply.isNull() )
        {
            Reply->abort();
            Reply->deleteLater();
        }
    }

protected:
    virtual void reset() {
        curOfsett = 0;
    }

    virtual void needData() {
        Q_ASSERT( curOfsett <= Buffer.size() );
        QByteArray Data;
        if( curOfsett < Buffer.size() ) {
            Data = Buffer.mid(curOfsett, 4096);
        }
        else {
            if( Reply.isNull() )
            {
                endOfData();
                return;
            }
            /// Start a event loop to process any events and exit it then we are ready to read
            QEventLoop Loop;
            connect( Reply, SIGNAL(readyRead()), &Loop, SLOT(quit()) );
            Loop.exec();



            Buffer.append(Reply->readAll());

            Data = Buffer.mid(curOfsett, 4096);

            if(Reply->size() > Buffer.capacity() )
                Buffer.reserve(Reply->size());
        }

        writeData(Data);
        curOfsett += Data.size();
        return;
    }

    void seekStream(qint64 offset) {
        curOfsett = offset;
    }

    QPointer<QNetworkReply> Reply;

    QByteArray Buffer;
    qint64 curOfsett;
protected slots:
    void NetworkError(QNetworkReply::NetworkError code ) {
        if( Reply->errorString() != "Operation canceled" )
            Error( Reply->errorString() );
    }

    void FullVideoResived() {
        Buffer.append(Reply->readAll());
        Reply->close();
        Reply->deleteLater();

        setStreamSeekable(true);
    }
};



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

    void Play();

    void Pause();

    /*Dont call this function unless you does not wan't to play this video anymore*/
    void Stop();

    void TooglePlay();


protected slots:
    void readyReadError();

    void ReadyReadVideoInfo();

    void ProgressUpdate( qint64 loaded, qint64 total );

protected:
    void resizeEvent(QResizeEvent *event);

    bool mPlaying;

    //VideoBuffer *mBuffer;
    //YoutubeStream *mStream;
    VideoStreamer *mStream;


    Phonon::MediaObject *mMedia;

    Phonon::AudioOutput *mAudio;
    Phonon::VideoWidget *mVideo;


    QToolButton *mPlayPause;

    bool JustStarted;

};

#endif // PHONONYOUTUBEPLAYER_H
