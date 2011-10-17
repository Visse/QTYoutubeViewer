#ifndef YOUTUBESTREAM_H
#define YOUTUBESTREAM_H

#include <QIODevice>

#include <QNetworkCookie>
#include <QByteArray>

#include "KeyList.h"

#include <QFile>

class QTcpSocket;

typedef QList<QNetworkCookie> CookieList;

struct Stream {
    Stream() :
        StreamUrl(), Cookies() {}
    QUrl StreamUrl;
    CookieList Cookies;
};


struct HttpHeader {
    HttpHeader() :
        Values(), ResponceCode(-1) {}
    KeyList< QString, QString > Values;
    int ResponceCode;
};


/// RawData = the raw data from the socket
/// Excess = will contain any data that was after the header
HttpHeader ParseHeader( QByteArray RawData, QByteArray &Excess, bool *Ok=0 );

Stream  getYoutubeStream( QString VideoID, int maxQuality=-1 );

/** *** YoutubeStream ***
  * This is my fourth try to make a youtube stream
  * Yea I now fourth... it was nothing wrong with the previus tries,
  * but in this class I will use everything I learnt from creating the others
  * and hopyfully it will work :)
  * The first try to read data will open up the stream that you have setThrue setStream
  * if you don't want this, call startStream after you have cald setStream :)
  */
class YoutubeStream : public QObject
{
    Q_OBJECT
public:
    enum BufferMode {
        /// Tries to get the video as fast as posible
        BM_Download,
        /// Tries to act a little more as a stream,
        /// it buffers a bit a head and then slows down
        BM_Stream
    };

    explicit YoutubeStream(QObject *parent = 0);
    ~YoutubeStream();

    /** Sets the stream to use
      * this function may take a while
      * depending on internet connection, about 100 to 200 millisecond,
      * at most a couple of seconds :(
      */
    void setStream( Stream YoutubeStream, QFile *File );

    void startStream();

   /* bool isSequential() const {
        return true;
    }

    virtual qint64 pos() const {
        return mReadPos;
    }
    virtual qint64 size() const {
        return 0;//mStreamSize;
    }

    virtual bool seek(qint64 pos);
    virtual bool atEnd() const {
        /// I could use mReadPos == mStreamSize, but this feels safer :)
        return mReadPos >= mStreamSize;
    }
    virtual bool reset() {
        return seek(0);
    }

    virtual qint64 bytesAvailable() const {
        // well size-(CurrentPos-BufferOffsett) == size-CurrentPos+BufferOffsett
        return (mBuffer.size()-mReadPos+mBufferOffsett)+QIODevice::bytesAvailable();
    }

    virtual qint64 bytesToWrite() const {
        return 0;
    }
    */
signals:

public slots:

protected slots:
    void ReadyReadHeader();
    void ReadyReadStream();

protected:
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *, qint64) {
        return -1; /// We cant write anything to the stream
    }

    void CloseConnection();
    bool ConnectToHost();


    // Generate a GET request
    static QByteArray genGETrequest( QUrl Url, CookieList Cookies, qint64 start=0 );
    // Generate a HEAD request
    static QByteArray genHEADrequest( QUrl Url, CookieList Cookies, qint64 start=0 );


    qint64 mStreamSize,
           mReadPos,
           mBufferOffsett;

    /** The different between StreamOpen and StreamFound is
      * that StreamOpen is if we have OPEN the stream or not,
      * StreamFound is if we have FOUND the stream, used by startStream()
      * StreamFound is true if we has sent a HEAD request to the server and got
      * a the responce code 200 ( OK ) from the server
      * if StreamOpen is true then we has sent a GET request and got responce code 200
      */
    bool mStreamOpen,
         mStreamFound;

    QTcpSocket *mSocket;

    //QByteArray mBuffer;
    QFile *mFile;

    Stream mStream;
};

#endif // YOUTUBESTREAM_H
