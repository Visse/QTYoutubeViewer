#ifndef VIDEOSTREAMER_H
#define VIDEOSTREAMER_H

#include <AbstractMediaStream>
#include <QTcpSocket>
#include <QUrl>
#include <QStringList>
#include <QNetworkCookieJar>

#include "KeyList.h"

#include "Log.h"

typedef QList<QUrl> QUrlList;
/*
class VideoStreamer : public Phonon::AbstractMediaStream {
    Q_OBJECT
public:
    VideoStreamer( QUrl VideoUrl, QNetworkAccessManager *NetworkMgr, QObject *Parent = 0 );

    /// All the urls in VideoUrls should point at the same video, AKA it's NOT a playlist
    /// If we can't get a stream out of the first on, we tries the second one
    /// and if it still does not work we tries the third one and so on...
    VideoStreamer( QUrlList VideoUrls, QNetworkAccessManager *NetworkMgr, QObject* Parent = 0 );


    ~VideoStreamer();

    /// Returns the number of bytes that we have recived from the stream
    /// if the stream has not started or we have not resived any bytes -1 will be return
    qint64 BytesResived() const {
        return mBytesResived;
    }
    /// Returns the total number of bytes in the stream
    /// If the size of the stream is unown, -1 is return
    qint64 BytesTotal() const {
        return mBytesTotal;
    }
protected:
    virtual void reset() {
        mCurOffSet= 0;
    }

    virtual void needData();

    void seekStream(qint64 offset) {
        mCurOffSet = offset;
    }

    QPointer<QNetworkReply> Reply;

    QByteArray mBuffer;
    qint64 mCurOffSet;

    quint16 mBlockSize;

    qint64 mBytesResived,
           mBytesTotal;

/// if the first url is not soutable it tries the next one...
    QUrlList mUrls;
    quint8 mCurUrl;

protected slots:
    void ProgressChanged( qint64 recived, qint64 total );
    void Finished();
    void NetworkError(QNetworkReply::NetworkError);
    void ReadyRead();


};
*/



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

class VideoStreamer : public Phonon::AbstractMediaStream {
    Q_OBJECT
public:
    VideoStreamer( QUrl VideoUrl, QObject *Parent = 0 ) :
        Phonon::AbstractMediaStream(Parent),
        mBuffer(),
        mBufferOffset(0),
        mCurOffset(0),
        mUrls(),
        mCurUrl(0),
        mCurrentUrl(),
        mFoundStream(false),
        mSocket(0)
    {
        mUrls.push_back(VideoUrl);
        mSocket = new QTcpSocket(this);
        mSocket->setReadBufferSize(4194304);/// a 4 MB buffer size ( 1024*1024*4 )

        if( !ConnectToHost(mUrls.first()) ) {
            Error(QString("Could not connect to host! host(%1)").arg(mUrls.first().host()));
        }
    }

    /// All the urls in VideoUrls should point at the same video, AKA it's NOT a playlist
    /// If we can't get a stream out of the first on, we tries the second one
    /// and if it still does not work we tries the third one and so on...
    VideoStreamer( QUrlList VideoUrls, QObject* Parent = 0 );


    ~VideoStreamer();

protected:
    virtual void reset() {
    }

    virtual void needData();

    void seekStream(qint64 offset) {

    }

    bool ConnectToHost(QUrl Url, int timeout=5000) {
        if( !mSocket )
            return false;
        mSocket->connectToHost( Url.host(), 80 );
        return mSocket->waitForConnected(timeout);
    }


    QByteArray mBuffer;


    quint64 mBufferOffset, mCurOffset;


/// if the first url is not soutable it tries the next one...
    QUrlList mUrls;
    quint8 mCurUrl;

    QUrl mCurrentUrl;

    bool mFoundStream;

    QTcpSocket *mSocket;
protected slots:
    void HeadResived() {
        KeyList<QString, QString> Head = SplitToList(mSocket->readAll(), "\r\n", "=" );

        if( !Head.isEmpty() && Head.first().startsWith("HTTP/") ) {
            bool Succes=false;
            int returnCode = Head.first().split(' ').at(1).toInt(&Succes);
            if( Succes ) {
                switch( returnCode ) {
                case( 200 ): /// OK

                    break;
                case( 302 ): /// Found
                    break;
                default:
                    Error( QString("Unown returnCode: %1").arg(QString::number(returnCode)) );
                    break;
                }
            }
        }
    }

protected:
    QByteArray genHeader(QUrl VideoUrl, const QNetworkCookieJar *Cookies) {
        QByteArray Header;

        QString Host = VideoUrl.host();
        QString Path = VideoUrl.path() + '?' +VideoUrl.encodedQuery();

        QString Request = "GET %1 HTTP/1.0\r\n";
        Request = Request.arg(Path);

        Header = Request.toAscii();

        Header += "Host: "+Host+"\r\n";

        if( Cookies ) {
            QList<QNetworkCookie> CookieList = Cookies->cookiesForUrl(VideoUrl);

            for( int i=0; i < CookieList.size(); i++ ) {
                QByteArray Name = CookieList.at(i).name(),
                           Value = CookieList.at(i).value();
                Header += "Cookie: "+Name+"="+Value+"\r\n";
            }
        }
        Header += "\r\n";
        return Header;
    }
    QByteArray genHEAD(QUrl VideoUrl, const QNetworkCookieJar *Cookies) {
        QByteArray Header;

        QString Host = VideoUrl.host();
        QString Path = VideoUrl.path() + '?' +VideoUrl.encodedQuery();

        QString Request = "HEAD %1 HTTP/1.0\r\n";
        Request = Request.arg(Path);

        Header = Request.toAscii();

        Header += "Host: "+Host+"\r\n";

        if( Cookies ) {
            QList<QNetworkCookie> CookieList = Cookies->cookiesForUrl(VideoUrl);

            for( int i=0; i < CookieList.size(); i++ ) {
                QByteArray Name = CookieList.at(i).name(),
                           Value = CookieList.at(i).value();
                Header += "Cookie: "+Name+"="+Value+"\r\n";
            }
        }
        Header += "\r\n";
        return Header;
    }


};
#endif // VIDEOSTREAMER_H
