#include "include/YoutubeStream.h"

#include <QTcpSocket>

#include "Log.h"

inline QString fromUrlEncoded( const QString& Encoded ) {
    return QUrl::fromEncoded( Encoded.toLocal8Bit() ).toString();
}

Stream getYoutubeStream( QString VideoID, int maxQuality ) {
    QTcpSocket Socket;

    bool FoundStream=false;

    QStringList RawCookies;
    KeyList< QString, QString > StreamInfo;
    {
        QByteArray Data;
        static const char* Modes[] = {"&el=embedded", "&el=detailpage", "&el=vevo", ""};

        // go thrue all the modes until we found one that suits us :)
        for( int i=0; i < sizeof(Modes)/4; i++, Socket.disconnectFromHost() ) {
            QUrl CurrentUrl = QString("http://www.youtube.com/get_video_info?video_id=%1%2&ps=default&eurl=&gl=US&hl=en").arg(VideoID, Modes[i]);
            Socket.connectToHost(CurrentUrl.host(), 80);
            if( !Socket.waitForConnected(3000) )
                continue; /// could not connect to host

            QString RawHeader = "GET %1 HTTP/1.1\r\nHost: %2\r\n\r\n";
            RawHeader = RawHeader.arg( QString(CurrentUrl.encodedPath()+'?'+CurrentUrl.encodedQuery()), CurrentUrl.host() );
            Socket.write( RawHeader.toLocal8Bit() );

            if( !Socket.waitForBytesWritten(3000) )
                continue; /// could not write

            if( !Socket.waitForReadyRead(3000) )
                continue; /// Did not get a responce

            // this may seem meningless but we want the full page before we move on
            while( Socket.waitForReadyRead(1000) );

            Data = Socket.readAll();
            bool Ok;
            HttpHeader Header = ParseHeader( Data, Data, &Ok );

            if( !Ok )
                continue; /// Something went wrong then parsed the header

            if( Header.ResponceCode != 200 )
                continue; /// something was not right with the responce...

            RawCookies = Header.Values.values("Set-Cookie");

            StreamInfo = SplitToKeyList( Data, "&", "=" );
            if( StreamInfo.countains("token") && StreamInfo.countains("url_encoded_fmt_stream_map") ) {
                FoundStream = true;
                break; // We found or stream!
            }
        }
    }
    if( !FoundStream ) /// We didn't found any thing
        return Stream();

    Stream S;
    /// Parse Cookies
    {
        foreach( QString C, RawCookies ) {
            KeyList<QString,QString> CookieInfo = SplitToKeyList(C, ";", "=", false);
            if( CookieInfo.isEmpty() )
                continue;
            QNetworkCookie Cookie;
            Cookie.setName( CookieInfo.begin()->first.toLocal8Bit() );
            Cookie.setValue( CookieInfo.begin()->second.toLocal8Bit() );

            Cookie.setPath( CookieInfo.value("path", "") );
            Cookie.setDomain( CookieInfo.value("domain","") );

            S.Cookies.push_back(Cookie);
        }
    }

    /// 'url%3D' == 'url='
    QStringList UrlFmtMap = StreamInfo.value("url_encoded_fmt_stream_map","").split("url%3D");

    QMap< int, QString > StreamUrls;

    foreach( QString UrlInfo, UrlFmtMap ) {
        /// Youtube has encoded the url not once but twice!
        UrlInfo = fromUrlEncoded(UrlInfo);
        UrlInfo = fromUrlEncoded(UrlInfo);

        QStringList UrlMap = UrlInfo.split('/');
        int format = fromUrlEncoded(UrlMap.takeLast())
                        .split('&').last()
                        .split('=').last()
                        .remove(',')
                        .toInt();
        UrlInfo = UrlMap.join("/");

        if( QUrl::fromEncoded(UrlInfo.toLocal8Bit()).isValid() )
            StreamUrls.insert( format, UrlInfo );
    }

    if( maxQuality == -1 ) {
        S.StreamUrl = QUrl::fromEncoded((StreamUrls.end()-1).value().toLocal8Bit());
        return S;
    }

    return S;
}

YoutubeStream::YoutubeStream(QObject *parent) :
    QObject(parent),//QIODevice(parent),
    mStreamSize(0), mReadPos(0), mBufferOffsett(0),
    mStreamOpen(false), mStreamFound(false),
    mSocket(0), //mBuffer(),
    mStream()
{
    mSocket = new QTcpSocket(this);

}

YoutubeStream::~YoutubeStream() {
    CloseConnection();
}

void YoutubeStream::ReadyReadHeader() {
    disconnect( mSocket, 0,0,0 );// disconnect everything from the socket

    QByteArray Data = mSocket->readAll();

    bool Ok;
    /// After this data will no longer countain a header
    HttpHeader H = ParseHeader( Data, Data, &Ok );

    if( !Ok ) {
        Error( "An error occurd then parsing header!" );
        return;
    }

    switch( H.ResponceCode ) {
    case( 200 ): // OK
        if( H.Values.countains("Content-Length") ) {
            qint64 Length =  H.Values.value("Content-Length","").toLongLong(&Ok); // qint64 is same 'long long int'
            if( !Ok ) {
                Warning2("Could not parse Content-Length[%1]", H.Values.value("Content-Length",""));
                mStreamSize = -1;/// Size is unown
            }
            else
                mStreamSize = Length;
        }
        else
            mStreamSize = -1;/// Size is unown

        /// The stream is found! but not open :P
        mStreamFound = true;
        mStreamOpen = false;

        //if( mStreamSize != -1 )
            //mBuffer.reserve( mStreamSize );
        startStream();
        break;
    case( 302 ): // Found
        /// @todo REMEMBER to set mStream.streamUrl to the new url !!!

        break;
    case( 403 ): // Forbidden
        Error2( "The server replied 403 Forbidden! reason[%1]", H.Values.value("reason","") );
        break;
    default:
        Error2( "Unown responce code[%1]!", QString::number(H.ResponceCode) );
    }

}


void YoutubeStream::ReadyReadStream() {
    /// The stream is not open get!
    /// it's problily something else, like a header
    if( !mStreamOpen )
        return;

    QByteArray Data = mSocket->readAll();
    //mBuffer.append( Data );

    mFile->write(Data);

    //bytesWritten(Data.size());
    //readyRead();


    return;
}

void YoutubeStream::startStream() {
    if( mStreamOpen || !mStreamFound ) {
        Error( "Stream allready open or not found!" );
        return;
    }

    if( !ConnectToHost() ) {
        Error( "Could not connect to host!" );
        return;
    }

    mSocket->write( genGETrequest(mStream.StreamUrl, mStream.Cookies) );
    if( !mSocket->waitForBytesWritten(3000) ) {
        Error( "Failed to write GET request to the server!" );
        return;
    }

    if( !mSocket->waitForReadyRead(3000) ) {
        Error( "Failed to get a responce from the server!" );
        return;
    }

    QByteArray Data;
    ParseHeader( mSocket->readAll(), Data, &mStreamOpen );
    if( !mStreamOpen )
        return;
    mFile->write(Data);

    connect( mSocket, SIGNAL(readyRead()), SLOT(ReadyReadStream()) );
}
/*
bool YoutubeStream::seek(qint64 pos) {
    // the position has to be between the start of the stream and the end
    if( pos >= mBufferOffsett && pos <= (mBuffer.size()+mBufferOffsett) ) {
        mReadPos = pos;
    }
    else {
        return false;
    }
    return true;
}
*/
qint64 YoutubeStream::readData(char *dest, qint64 maxlen) {
    /*/// If we have found the stream, but not started it, start it now;
    if( !mStreamOpen && mStreamFound ) {
        startStream();
        return 0; /// We havent been able to read anything yet.
    }
    // Get the data needed
    QByteArray Data = mBuffer.mid(mReadPos, maxlen);
    // Copy to data to destiny
    memcpy( dest, Data.data(), Data.length() );
    // dont forget to advance :)
    mReadPos += Data.size();
    return Data.size();*/
}

void YoutubeStream::setStream(Stream YoutubeStream, QFile *File) {
    if( mStreamOpen ) {
        Error( "Stream allready open!" );
        return;
    }

    mStreamFound = false;
    mFile = File;
    mStream = YoutubeStream;
    if( !ConnectToHost() ) {
        Error( "Could not connect to host!" );
        return;
    }
    connect( mSocket, SIGNAL(readyRead()), SLOT(ReadyReadHeader()) );

    mSocket->write( genHEADrequest(mStream.StreamUrl, mStream.Cookies) );
}

bool YoutubeStream::ConnectToHost() {
    CloseConnection();

    // Clear the buffer
    mSocket->readAll();
    // No need to try to connect then it's not a valid url :P
    if( !mStream.StreamUrl.isValid() )
        return false;

    mSocket->connectToHost( mStream.StreamUrl.host(), 80 ); /// Port 80 is the HTTP port

    return mSocket->waitForConnected(3000);
}

void YoutubeStream::CloseConnection() {
    if( mSocket->state() == QAbstractSocket::UnconnectedState )
        return;
    mSocket->disconnectFromHost();
    mSocket->waitForDisconnected(3000);
    return;
}

QByteArray YoutubeStream::genHEADrequest(QUrl Url, CookieList Cookies, qint64 start) {
    QByteArray Header;

    Url.addQueryItem( "start",QString::number(start) );
    Header =  "HEAD "+Url.encodedPath()+'?'+Url.encodedQuery()+" HTTP/1.1\r\n";
    Header += "Host: "+Url.host()+"\r\n";

    foreach( QNetworkCookie Cookie, Cookies ) {
        Header += "Cookie: "+Cookie.name()+"="+Cookie.value()+"\r\n";
    }

    Header += "\r\n";

    return Header;
}

QByteArray YoutubeStream::genGETrequest(QUrl Url, CookieList Cookies, qint64 start) {
    QByteArray Header;

    Url.addQueryItem( "start",QString::number(start) );
    Header =  "GET "+Url.encodedPath()+'?'+Url.encodedQuery()+" HTTP/1.1\r\n";
    Header += "Host: "+Url.host()+"\r\n";

    foreach( QNetworkCookie Cookie, Cookies ) {
        Header += "Cookie: "+Cookie.name()+"="+Cookie.value()+"\r\n";
    }

    Header += "\r\n";

    return Header;
}

HttpHeader ParseHeader(QByteArray RawData, QByteArray &Excess, bool *Ok) {
    if( Ok )
        (*Ok) = false;
    int index = RawData.indexOf("\r\n\r\n");
    if( index == -1 ) // Header was not found!
        return HttpHeader();

    QString Responce = RawData.mid(0, index);
    Excess = RawData.mid(index+4); /// Sometimes the responce countains more than just the header

    HttpHeader H;
    H.Values = SplitToKeyList( Responce, "\r\n", ":", true );
    QString ResponceCode = H.Values.begin()->first;

    /// Well the first line is the responce, for example 'HTTP/1.1 200 OK'
    /// the first bit is the protocol, in this case HTTP version 1.1, there is also a verson 1.0
    /// the second part is the responce code, in this case 200 this means the request was succesfull
    /// and the last part is the responce code in a human-friendly way
    if( !ResponceCode.startsWith("HTTP/1.") )
        return H;

    bool IntOk;

    H.ResponceCode = ResponceCode.split(' ').at(1).toInt(&IntOk);
    if( !IntOk )
        return H;

    if( Ok )
        (*Ok) = true;

    return H;

}
