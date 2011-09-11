#include "VideoStreamer.h"

#include <QEventLoop>


#include "Log.h"
// if you lower this you get a more even network usage
#define BUFFERSIZE 1024*1024 /// 1 MB

VideoStreamer::VideoStreamer( QUrl VideoUrl, QNetworkAccessManager *NetworkMgr, QObject *Parent ) :
    Phonon::AbstractMediaStream(Parent),
    mBuffer(),
    mBufferOffset(0),
    mCurOffset(0),
    mUrls(),
    mCurUrl(0),
    mCurrentUrl(),
    mFoundStream(false),
    mReadData(false),
    mSocket(0),
    mCookies(0)
{
    mUrls.push_back(VideoUrl);
    mSocket = new QTcpSocket(this);
    mSocket->setReadBufferSize(BUFFERSIZE);

    mCurrentUrl = mUrls.first();
    if( !ConnectToHost(mCurrentUrl) ) {
        Error(QString("Could not connect to host! host(%1)").arg(mUrls.first().host()));
    }

    mCookies = NetworkMgr->cookieJar();
    mSocket->write( genHEAD(mCurrentUrl) );
    connect( mSocket, SIGNAL(readyRead()), SLOT(HeaderResived()) );

    setStreamSeekable(true);
}

/// All the urls in VideoUrls should point at the same video, AKA it's NOT a playlist
/// If we can't get a stream out of the first on, we tries the second one
/// and if it still does not work we tries the third one and so on...
VideoStreamer::VideoStreamer( QUrlList VideoUrls, QNetworkAccessManager *NetworkMgr, QObject* Parent ) :
    Phonon::AbstractMediaStream(Parent),
    mBuffer(),
    mBufferOffset(0),
    mCurOffset(0),
    mUrls(VideoUrls),
    mCurUrl(0),
    mCurrentUrl(),
    mFoundStream(false),
    mReadData(false),
    mSocket(0),
    mCookies(0)
{
    mSocket = new QTcpSocket(this);
    mSocket->setReadBufferSize(BUFFERSIZE);

    mCurrentUrl = mUrls.first();
    if( !ConnectToHost(mCurrentUrl) ) {
        Error(QString("Could not connect to host! host(%1)").arg(mUrls.first().host()));
    }

    mCookies = NetworkMgr->cookieJar();
    mSocket->write( genHEAD(mCurrentUrl) );
    connect( mSocket, SIGNAL(readyRead()), SLOT(HeaderResived()) );

    /// For some reason this does not work, the MediaObject does not reconize that the stream is seekable :(
    /// I belelive i found the reason, there is a bug in phonon then using the default gstreamer backend,
    /// But... I cant find any guid to change the backend...
    setStreamSeekable(true);

}


VideoStreamer::~VideoStreamer() {
    mSocket->close();
}

QByteArray VideoStreamer::genHeader(QUrl VideoUrl ) const {
    QByteArray Header;

    QString Host = VideoUrl.host();
    QString Path = VideoUrl.path() + '?' +VideoUrl.encodedQuery();

    QString Request = "GET %1 HTTP/1.0\r\n";
    Request = Request.arg(Path);

    Header = Request.toAscii();

    Header += "Host: "+Host+"\r\n";

    if( mCookies ) {
        QList<QNetworkCookie> CookieList = mCookies->cookiesForUrl(VideoUrl);

        for( int i=0; i < CookieList.size(); i++ ) {
            QByteArray Name = CookieList.at(i).name(),
                       Value = CookieList.at(i).value();
            Header += "Cookie: "+Name+"="+Value+"\r\n";
        }
    }
    Header += "\r\n";
    return Header;
}

QByteArray VideoStreamer::genHEAD(QUrl VideoUrl) const {
    QByteArray Header;

    QString Host = VideoUrl.host();
    QString Path = VideoUrl.path() + '?' +VideoUrl.encodedQuery();

    QString Request = "HEAD %1 HTTP/1.0\r\n";
    Request = Request.arg(Path);

    Header = Request.toAscii();

    Header += "Host: "+Host+"\r\n";

    if( mCookies ) {
        QList<QNetworkCookie> CookieList = mCookies->cookiesForUrl(VideoUrl);

        for( int i=0; i < CookieList.size(); i++ ) {
            QByteArray Name = CookieList.at(i).name(),
                       Value = CookieList.at(i).value();
            Header += "Cookie: "+Name+"="+Value+"\r\n";
        }
    }
    Header += "\r\n";
    return Header;
}

void VideoStreamer::reset() {
    seekStream(0);
}

void VideoStreamer::needData(){

    mReadData = true;
    if( !mFoundStream )
        return;

    ReadyRead();

/*
    if( (!mCurOffset < (mBuffer.size()+mBufferOffset) ))
        mBuffer.append( mSocket->readAll() );

    QByteArray Data = mBuffer.mid(mCurOffset-mBufferOffset);
    if( Data.isEmpty() )
        return;
    writeData(Data);
    mCurOffset += Data.size();*/
}

void VideoStreamer::seekStream( qint64 offset ) {
    /// Check to see if we allready have the data
    if( offset <= (mBuffer.size()+mBufferOffset) && offset >= mBufferOffset ) {
        mCurOffset = offset;
    }/// Else we have to try something else :P
    else {
        /// @todo Add a better buffering system
        mBuffer.clear();
        mSocket->close();
        mSocket->readAll();// Clear the sockets buffer

        QUrl Url = mCurrentUrl;
        Url.addQueryItem("start", QString::number(offset));
        if( !ConnectToHost(Url) ) {
            Error( QString("Could not connect to host(%1)").arg(Url.host()) );
        }
    }
}

bool VideoStreamer::ConnectToHost( QUrl Url, int timeout ) {
    if( !mSocket )
        return false;
    mSocket->disconnectFromHost();
    mSocket->close();
    mSocket->abort();
    mSocket->connectToHost( Url.host(), 80 );
    return mSocket->waitForConnected(timeout);
}

VideoStreamer::Header VideoStreamer::ParseHeader( QString Responce,  bool *Ok ) {
    if( Ok )
        (*Ok) = false;

    KeyList<QString,QString> Info = SplitToList( Responce, "\r\n", ":", true );
    QString ResponceCode = Info.begin()->first;

    /// Well the first line is the responce, for example 'HTTP/1.1 200 OK'
    /// the first bit is the protocol, in this case HTTP version 1.1, there is also a verson 1.0
    /// the second part is the responce code, in this case 200 this means the request was succesfull
    /// and the last part is the responce code in a human-friendly way
    if( !ResponceCode.startsWith("HTTP/1.") )
        return Header();

    bool IntOk=false;

    int Code = ResponceCode.split(' ').at(1).toInt(&IntOk);

    if( !IntOk )
        return Header();

    Header Head;
    Head.ResponceCode = Code;
    Head.Info = Info;

    if( Ok )
        (*Ok) = true;

    return Head;
}

void VideoStreamer::HeaderResived() {
    QString Responce = mSocket->readAll();

    disconnect( mSocket, 0, 0, 0 );
    bool Ok=false;
    Header Head = ParseHeader(Responce, &Ok);

    if( !Ok ) {
        Error( "Could not parse header! Responce["+Responce+"]" );
        return;
    }

    switch( Head.ResponceCode ) {
    case( 200 ): /// Responce OK
        if( Head.Info.value("Connection", "") == "close" ) {
            if( !ConnectToHost( mCurrentUrl ) ) {
                Error( "Failed to connect! Url["+mCurrentUrl.toString()+"]" );
                return;
            }
        }

        if( Head.Info.countains("Content-Length") ) {
            qint64 StreamLength = Head.Info.value("Content-Length").toInt(&Ok);
            if( Ok ) {
                mBuffer.reserve(StreamLength+1024);// Just some pading so it won't reserve more at the end..
                setStreamSize(StreamLength);
            }
            else {
                Error( "Failed to parse Content-Length!" );
                setStreamSize(-1);
            }
        }
        else
            setStreamSize(-1);

        mSocket->write(genHeader(mCurrentUrl));
        if( mSocket->waitForBytesWritten())
        {
            mSocket->waitForReadyRead(3000);
            QByteArray Data = mSocket->readAll();
            mBuffer.append(Data.mid(QString(Data).split("\r\n\r\n").first().size()+5));
            mFoundStream = true;


            connect( mSocket, SIGNAL(readyRead()), SLOT(ReadyRead()) );
        }



        break;
    default:
        break;
    }
}

void VideoStreamer::ReadyRead() {
    // If we don't have open a stream, or we don't need any data
    // We do not read from the socket, and saves some internet traffic
    if( !(mReadData && mFoundStream) )
        return;

    mBuffer.append(mSocket->readAll());
    QByteArray Data = mBuffer.mid( mCurOffset-mBufferOffset );
    if( Data.isEmpty() )
        return;
    writeData(Data);
    mCurOffset += Data.size();


    if( streamSize() > 0 && (mCurOffset >= streamSize() ) )
        endOfData();
}

void VideoStreamer::enoughData() {
    mReadData = false;
}


