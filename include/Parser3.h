#ifndef PARSER3_H
#define PARSER3_H


#include <QtNetwork>
#include <QDomNode>
#include <QProgressBar>

#include "Info.h"

/*A simple class that uses slots & signals to make my life easier :P */

class Parser3_Sender : public QObject {
    Q_OBJECT
public:
    Parser3_Sender( QObject *parent, BasePtr Info );


    void setInfo( BasePtr Info );
    BasePtr getInfo();

    void SendSignal( QDomDocument Page );

signals:
    void Signal( BasePtr, QDomDocument );

private:
    BasePtr bInfo;
};


class Parser3 : public QObject {
    Q_OBJECT
public:
    explicit Parser3(QObject *parent=0);

    QNetworkAccessManager *NetworkMgr;
    QProgressBar *ProgressBar;


    enum UrlType {
        UT_Unown = 0,
        UT_Video,
        UT_Video_Feed,
        UT_Video_Responses,
        UT_Video_Releated,
        UT_Playlist,
        UT_Playlist_Feed,
        UT_User,
        UT_User_Feed,
        UT_User_Favorites,
        UT_User_Uploads,
        UT_User_Playlists
    };

    static UrlType getUrlType( QUrl Url );

    static QString extractID( QUrl Url );

signals:
    void VideoParsed( VideoPtr Info );
    void PlaylistParsed( VideoFeedPtr Playlist );
    void UserParsed( UserPtr User );

    void VideoSearchParsed( VideoFeedPtr Search );
    void PlaylistSearchParsed( PlaylistFeedPtr Search );
    void ChannelSearchParsed( ChannelFeedPtr Search );


    void VideosAdded( VideoFeedPtr Feed );
public slots:
    void ParseUrl( QUrl Url );
    void Search( QString String );
    void ParseNextPage( VideoFeedPtr Feed );

    /*If string is a url parseUrl will be parsed, otherwise search*/
    void ParseString( QString String );
protected slots:
    void ReadyRead();

    void VideoRecived( BasePtr Info, QDomDocument Page );
    void PlaylistRecived( BasePtr Feed, QDomDocument Page );
    void UserRecived( BasePtr User, QDomDocument Page );

    void UserPlaylistsRecived( BasePtr Feed, QDomDocument Page );
    void UserFavoritesRecived( BasePtr Feed, QDomDocument Page );
    void UserUploadsRecived( BasePtr Feed, QDomDocument Page );

    void VideoResponcesRecived( BasePtr Feed, QDomDocument Page );
    void VideoReleatedRecived( BasePtr Feed, QDomDocument Page );

    void VideoSeachRecived( BasePtr Feed, QDomDocument Page );

    void Update();

    void NetworkError();
protected:
    void ParseVideoEntry( QDomNode Node, VideoPtr Info );
    void ParseVideoFeed( QDomNode Node, VideoFeedPtr Info );
    void ParseUserEntry( QDomNode Node, UserPtr Info );

    QTimer mTimer;

    typedef QList< QPair< QNetworkRequest, QPointer<Parser3_Sender> > > QueList;
    typedef QHash< QNetworkReply*,  QPointer<Parser3_Sender> > ResiveHash;


    QueList    mQue;
    ResiveHash mResive;
};

#endif
