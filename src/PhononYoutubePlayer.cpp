#include "PhononYoutubePlayer.h"

#include <QGridLayout>
#include <QResizeEvent>

#include <QtNetwork>

#include <QNetworkCookieJar>

#include <AbstractMediaStream>
#include <iostream>

#include "Log.h"


inline QString FromUrlEscape( const QString& String ) {
    return QUrl::fromEncoded(String.toLocal8Bit()).toString();
}


struct VideoFormat {
    VideoFormat( QString Container_, int width_, int height_, float videoBitrate_, float audioBitrate_, uint fmt_value_ )
        :Container(Container_), Width(width_), Height(height_), VideoBitrate(videoBitrate_), AudioBitrate(audioBitrate_), Fmt_value(fmt_value_) {}

    QString Container;

    int Width, Height;

    float VideoBitrate, AudioBitrate;

    const uint Fmt_value;
};

/// Youtube videoformats, i got the information from:
/// http://en.wikipedia.org/wiki/YouTube#Quality_and_codecs

static const VideoFormat Formats[] = {
            VideoFormat("FLV", 400, 240, 0.25, 64, 5),
            VideoFormat("FLV", 640, 360, 0.5, 128, 34),
            VideoFormat("FLV", 854, 480, 1.0, 128, 35),
            VideoFormat("MP4", 640, 360, 0.5, 1024, 18),
            VideoFormat("MP4", 1280, 720, 2.9, 192, 22),
            VideoFormat("MP4", 1920, 1080, 5.0, 192, 37),
            VideoFormat("MP4", 4096, 2304, -1, 192, 38), // I don't think there are any videos in this format
            VideoFormat("WebM", 640, 360, 0.5, 128, 43),
            VideoFormat("WebM", 854, 480, 1.0, 128, 44),
            VideoFormat("WebM", 1280,720, 2.0, 192, 45),
            VideoFormat("3GP", 176, 144, -1, -1, 17)// Mobile format

};


const VideoFormat& getFormat( int Type ) {
    for( int i=0; i < sizeof(Formats)/sizeof(VideoFormat); i++ ) {
        if( Formats[i].Fmt_value == Type )
            return Formats[i];
    }
}

PhononYoutubePlayer::PhononYoutubePlayer(QWidget *parent) :
    QWidget(parent),
    mMedia(0),
    mAudio(0),
    mVideo(0),
    //mPlayPause(0),
    mStream(0),
    mControlls(0)
{
    mMedia = new Phonon::MediaObject(this);


    mAudio = new Phonon::AudioOutput(this);
    mVideo = new Phonon::VideoWidget(this);


    mControlls = new VideoController(this);
        mControlls->setMediaObject(mMedia, mAudio);

    setMinimumSize(200,200);


    Phonon::createPath( mMedia, mVideo );
    Phonon::createPath( mMedia, mAudio );


   /* mPlayPause = new QToolButton(this);

    mPlayPause->setText("Play/Pause");
    connect( mPlayPause, SIGNAL(clicked()), SLOT(TooglePlay()) );*/



}

PhononYoutubePlayer::~PhononYoutubePlayer() {
    Stop();

    mMedia->clear();
    mMedia->deleteLater();
}

void PhononYoutubePlayer::Play() {
    mMedia->play();
}

void PhononYoutubePlayer::Pause() {
    mMedia->pause();
}

void PhononYoutubePlayer::TooglePlay() {
    if( mMedia->state() != Phonon::PlayingState )
        Pause();
    else
        Play();
}

void PhononYoutubePlayer::Stop() {
    mMedia->stop();
    mMedia->clear();
    // well, i belive this solved a freeze problem :)
    // I'm not sure exacly why, but I belive Phonon did not like it then
    // you stoped a video and started playing a new one right after,
    // some kind of ennoying deadlock occured
    // Okay, so this does not fully solves it, but... its ALOT rarer now,
    // And i belive I have a memory leak somethere, not sure were, it does not seem
    // to want to crash then running in a debuger :(
    usleep(5000);
}

void PhononYoutubePlayer::readyReadError() {
    Error(static_cast<QNetworkReply*>(sender())->errorString());
}

void PhononYoutubePlayer::PlayVideo(QString ID) {
    Stop();
/*
    QNetworkReply *reply = NetworkMgr->get(QNetworkRequest(QUrl(
                QString("http://www.youtube.com/get_video_info?&video_id=%1&el=detailpage&ps=default&eurl=&gl=US&hl=en").arg(ID))));
                //QString("http://www.youtube.com/get_video_info?&video_id=%1&ps=default&eurl=&gl=US&hl=en").arg(ID))));

    //connect( reply, SIGNAL(finished()), SLOT(ReadyReadVideoInfo()) );
    connect( reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(readyReadError()) );*/

    /*This is a pretty mush straight copy from youtube-dl, look at lines at around 1000 to see it,

*/

    mLog.LogMessage( "Playing videoID: "+ID );
    static char* Modes[] = {"&el=embedded", "&el=detailpage", "&el=vevo", ""};

    QMap< int, QUrl > FeedUrls;

    for( int i=0; i < sizeof(Modes)/4; i++ ) {
        QUrl VideoInfoUrl(QString("http://www.youtube.com/get_video_info?&video_id=%1%2&ps=default&eurl=&gl=US&hl=en").arg(ID,Modes[i]));

        QNetworkReply *Reply = NetworkMgr->get(QNetworkRequest(VideoInfoUrl));
        connect(Reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(readyReadError()));

        QEventLoop Loop(this);
        connect( Reply, SIGNAL(finished()), &Loop, SLOT(quit()) );
        Loop.exec();

        QStringList InfoList = QString(Reply->readAll()).split('&');
        if( !InfoList.filter("token=").isEmpty()) {
            QString Encoded_Fmt_Stream_Map = InfoList.filter("url_encoded_fmt_stream_map=").first();
            Encoded_Fmt_Stream_Map.remove(0,27);


            QStringList FmtStreamMap = Encoded_Fmt_Stream_Map.split("url%3D", QString::SkipEmptyParts);
            for( int i=0; i < FmtStreamMap.size(); i++ ) {
                QString Url = FmtStreamMap.at(i);

                /// Youtube have url escaped a url escaped url :P
                Url = FromUrlEscape(Url);
                Url = FromUrlEscape(Url);

                /// The format of the stream is last in the url, so we have to remove it
                QStringList UrlMap = Url.split('/');
                QString Format = FromUrlEscape( UrlMap.takeLast() );/// The format also has to be removed from the url                Url = UrlMap.join("/");/// put every thing back together
                Url = UrlMap.join("/");/// put every thing back together

                /// A Format looks like this  [webm;+codecs="vp8.0,+vorbis"&itag=45]
                int FmtFormat = Format.split('&').last().split('=').last().remove(',').toInt();

                QUrl EncodedUrl = QUrl::fromEncoded(Url.toAscii());
                if( EncodedUrl.isValid() )
                    FeedUrls.insert(FmtFormat, EncodedUrl );
            }

            break;
        }
    }

    if( FeedUrls.isEmpty() )
    {
        Error( "Could not find any videoUrls!" );
        return;
    }
    QUrlList Urls;

    foreach( QUrl VideeoUrl, FeedUrls.values() ) {
        Urls.push_front( VideeoUrl );
    }



    mStream = new VideoStreamer(Urls,this);
    /*QFile *File = new QFile("Test", this);
    File->open(QIODevice::ReadWrite);
    mStream = new YoutubeStream(this);
    mStream->setStream( getYoutubeStream(ID), File );
    mStream->startStream();*/

    Phonon::MediaSource Source(mStream);

    Source.setAutoDelete(true);
    //mMedia->clear();
    Stop();
    mMedia->setCurrentSource(Source);

    Play();
    return;
}
void PhononYoutubePlayer::resizeEvent(QResizeEvent *event)
{
    QSize Size = event->size();

    Size.setHeight(Size.height()-30);
    mVideo->resize(Size);

    //mPlayPause->setGeometry(0,Size.height()-30,100,30);
    mControlls->setGeometry(0, Size.height(), Size.width(), 30 );

    QWidget::resizeEvent(event);
}
/// url Escape Codes - can be found here: http://www.december.com/html/spec/esccodes.html

static const char *URL_Escape_Codes[][2] = { {" ", "%20" }, {"<", "%3C" }, {">", "%3E" },
    {"#", "%23" }, {"%", "%25" }, {"{", "%7B" }, {"}", "%7D" }, {"|", "%7C" },
    {"\\", "%5C" }, {"^", "%5E" }, {"~", "%7E" }, {"[", "%5B" }, {"]", "%5D" },
    {"`", "%60" }, {";", "%3B" }, {"/", "%2F" }, {"?", "%3F" }, {":", "%3A" },
    {"@", "%40" }, {"=", "%3D" }, {"&", "%26" }, {"$", "%24" } };



void PhononYoutubePlayer::EnqueVideo(QString ID) {
    /// @todo
}

void PhononYoutubePlayer::Next() {
    /// @todo
}
