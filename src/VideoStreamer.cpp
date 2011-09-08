#include "VideoStreamer.h"

#include <QEventLoop>


#include "Log.h"
/*

VideoStreamer::VideoStreamer( QUrl VideoUrl, QNetworkAccessManager *NetworkMgr, QObject *Parent ) :
    Phonon::AbstractMediaStream(Parent),
    mBytesResived(-1),
    mBytesTotal(-1),
    mBlockSize(4096),
    mCurOffSet(0),
    mCurUrl(0)
{
    mUrls.push_back(VideoUrl);
    Reply = NetworkMgr->get(QNetworkRequest(mUrls[mCurUrl]));
    connect( Reply.data(), SIGNAL(error(QNetworkReply::NetworkError)), SLOT(NetworkError(QNetworkReply::NetworkError)) );
    connect( Reply.data(), SIGNAL(finished()), SLOT(Finished()) );
    connect( Reply.data(), SIGNAL(downloadProgress(qint64,qint64)), SLOT(ProgressChanged(qint64,qint64)) );
    connect( Reply.data(), SIGNAL(readyRead()), SLOT(ReadyRead()) );
}


VideoStreamer::VideoStreamer( QUrlList VideoUrls, QNetworkAccessManager *NetworkMgr, QObject *Parent ) :
    Phonon::AbstractMediaStream(Parent),
    mBytesTotal(-1),
    mBlockSize(4096),
    mCurOffSet(0),
    mCurUrl(0),
    mUrls(VideoUrls)
{
    Reply = NetworkMgr->get(QNetworkRequest(mUrls[mCurUrl]));
    connect( Reply.data(), SIGNAL(error(QNetworkReply::NetworkError)), SLOT(NetworkError(QNetworkReply::NetworkError)) );
    connect( Reply.data(), SIGNAL(finished()), SLOT(Finished()) );
    connect( Reply.data(), SIGNAL(downloadProgress(qint64,qint64)), SLOT(ProgressChanged(qint64,qint64)) );
    connect( Reply.data(), SIGNAL(readyRead()), SLOT(ReadyRead()) );
}

VideoStreamer::~VideoStreamer() {
    if( !Reply.isNull() ) {
        /// I really don't now if it's necisery to call abort and close, but it's better to be safe..
        Reply->abort();
        Reply->close();
        Reply->deleteLater();
    }
    mLog.LogMessage("VideoStream Shutting down...");
}

void VideoStreamer::needData() {
    /// This should newer be able to happen, but if it does its bad.
    Q_ASSERT( mCurOffSet <= mBuffer.size() );

    QByteArray Data;
    if( mCurOffSet < mBuffer.size() ) {
        Data = mBuffer.mid(mCurOffSet, mBlockSize );
    }/*
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

        mBuffer.append(Reply->readAll());

        Data = mBuffer.mid( mCurOffSet, mBlockSize );

        if(Reply->size() > mBuffer.capacity() )
            mBuffer.reserve(Reply->size());
    }*//*
    if( mCurOffSet == mBuffer.size() && Reply.isNull() && Data.isEmpty())
    {
        endOfData();
        return;
    }
    if( !Data.isEmpty() ) {
        writeData(Data);
        mCurOffSet += Data.size();
    }

    return;
}


void VideoStreamer::ProgressChanged( qint64 recived, qint64 total ) {
    mBytesResived   = recived;
    mBytesTotal     = total;

    if( streamSize() < mBytesTotal )
        setStreamSize(mBytesTotal);
}

void VideoStreamer::Finished() {
    mBuffer.append(Reply->readAll());
    Reply->close();
    Reply->deleteLater();

    if( mBuffer.size() < mBlockSize && (mCurUrl+1) < mUrls.size() ) {
        mCurUrl++;
        Reply = Reply->manager()->get(QNetworkRequest(mUrls.at(mCurUrl)));
        connect( Reply.data(), SIGNAL(error(QNetworkReply::NetworkError)), SLOT(NetworkError(QNetworkReply::NetworkError)) );
        connect( Reply.data(), SIGNAL(finished()), SLOT(Finished()) );
        connect( Reply.data(), SIGNAL(downloadProgress(qint64,qint64)), SLOT(ProgressChanged(qint64,qint64)) );
        connect( Reply.data(), SIGNAL(readyRead()), SLOT(ReadyRead()) );

        error( Phonon::NormalError, "Treing another url" );

        mBuffer.clear();

        mLog.LogMessage("Testing next url["+mUrls.at(mCurUrl).toString()+"]");


    }

}

void VideoStreamer::NetworkError(QNetworkReply::NetworkError) {
    Error( static_cast<QNetworkReply*>(sender())->errorString());
}

void VideoStreamer::ReadyRead() {
    mBuffer.append( Reply->readAll() );
}
*/
