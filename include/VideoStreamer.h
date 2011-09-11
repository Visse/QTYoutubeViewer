#ifndef VIDEOSTREAMER_H
#define VIDEOSTREAMER_H

#include <AbstractMediaStream>
#include <QTcpSocket>
#include <QUrl>
#include <QStringList>
#include <QNetworkCookieJar>

#include <QNetworkAccessManager>

#include "KeyList.h"


typedef QList<QUrl> QUrlList;

/// My compiler start to give me linking errors with out the 'static' key word...
/// This function splits up a string using QString::Split and those strings are split up to 2 new strings, 1 key and 1 value string
/// And put those in a KeyList
static KeyList< QString, QString > SplitToList( QString String, QString ItemSplit, QString KeySplit, bool Simplifed=false ) {
    KeyList< QString, QString > Map;

    QStringList Items = String.split(ItemSplit);
    for( int i=0; i < Items.size(); i++ ) {
        if( Items.isEmpty() )
            continue;

        QStringList Item = Items.at(i).split(KeySplit);

        QString Key = Item.takeFirst();
        // if we had more than 1 keySplit in it we have to put every thing back together
        if( Simplifed )
            Map.insert(Key.simplified(), Item.join(KeySplit).simplified());
        else
            Map.insert(Key, Item.join(KeySplit));
    }
    return Map;
}

/**
  * I now there is easier way to stream a video than doing it with sockets,
  * but i want more controll than I got with QTs NetworkManager & Networkreply
  * and... I realy like to learn new ways of doing thing :P
  * @todo Add function for speed limits, and some buffering...
  * For example if you check the network traffic then you watching a video on youtube,
  * you see that first it goes up towards max, then it buffering, then it goes down
  * and keep that speed until the video is fully buffered OR the user seeks the video.
  */
class VideoStreamer : public Phonon::AbstractMediaStream {
    Q_OBJECT
public:
    VideoStreamer( QUrl VideoUrl, QNetworkAccessManager *NetworkMgr, QObject *Parent = 0 );
    /// All the urls in VideoUrls should point at the same video, AKA it's NOT a playlist
    /// If we can't get a stream out of the first on, we tries the second one
    /// and if it still does not work we tries the third one and so on...
    VideoStreamer( QUrlList VideoUrls, QNetworkAccessManager *NetworkMgr, QObject* Parent = 0 );



    virtual ~VideoStreamer();

protected:
    virtual void reset();

    virtual void needData();
    virtual void enoughData();

    virtual void seekStream(qint64 offset);

    bool ConnectToHost(QUrl Url, int timeout=5000);


    QByteArray mBuffer;
    quint64 mBufferOffset, mCurOffset;

/// if the first url is not soutable it tries the next one...
    QUrlList mUrls;
    quint8 mCurUrl;

    QUrl mCurrentUrl;

    bool mFoundStream;

    bool mReadData;

    QTcpSocket *mSocket;

    /**
      * Just a simple struct, designed to make my life easier :)
      */
    struct Header {
        Header()
            :ResponceCode(-1), Info() {}
        int ResponceCode;
        KeyList< QString, QString > Info;
    };

    Header ParseHeader( QString Responce,  bool *Ok=0 );

    QNetworkCookieJar *mCookies;


    QByteArray genHeader( QUrl VideoUrl ) const;
    QByteArray genHEAD(QUrl VideoUrl) const;
protected slots:
    /// The header of the video stream is resived
    void HeaderResived();

    void ReadyRead();
};
#endif // VIDEOSTREAMER_H
