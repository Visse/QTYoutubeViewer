#ifndef INFO_H
#define INFO_H

#include <QObject>
#include <QPointer>
#include <QUrl>
#include <QVector>
#include <QColor>
#include <QDate>


class BaseInfo;
class VideoInfo;
class VideoFeed;
class UserInfo;
class PlaylistFeed;
class ChannelFeed;

enum InfoType {
    IT_VideoInfo = 1,
    IT_VideoFeed = 2,
    IT_UserInfo = 3,

    IT_PlaylistFeed = 4,
    IT_ChannelFeed = 5
};

typedef QPointer< BaseInfo > BasePtr;
typedef QPointer< VideoInfo > VideoPtr;
typedef QPointer< VideoFeed > VideoFeedPtr;
typedef QPointer< UserInfo > UserPtr;
typedef QPointer< PlaylistFeed > PlaylistFeedPtr;
typedef QPointer< ChannelFeed > ChannelFeedPtr;


QString GenerateRichText_ListElement( BasePtr Info, QColor TextColor, QColor HeighLightColor );
QString GenerateRichText_FullInfo( BasePtr Info, QColor TextColor, QColor HeighLightColor );

class BaseInfo : public QObject {
    Q_OBJECT
public:
    BaseInfo();
    BaseInfo( const BaseInfo& Copy );

    BaseInfo& operator = ( const BaseInfo& Copy );

    /*The id can't change*/
    const QString ID;

    QString Author,
            Title,
            Description;

    QUrl UrlAuthor,
         UrlThumbnail;

    virtual InfoType getType() const = 0;//{return IT_BaseInfo;}


    /*Some cast to make life easier*/
    virtual BaseInfo*       cast_BaseInfo()     {return this;}
    virtual VideoInfo*      cast_VideoInfo()    {return 0;}
    virtual VideoFeed*      cast_VideoFeed()    {return 0;}
    virtual UserInfo*       cast_UserInfo()     {return 0;}
    virtual PlaylistFeed*   cast_PlaylistFeed() {return 0;}
    virtual ChannelFeed*    cast_ChannelFeed()  {return 0;}

    //A Virtual function to make my life a little easies
    //it fixes Numbers -> Strings for easier display :P
    virtual void FixInfo() {};

signals:
    void InfoUpdated();

public slots:
    void SendUpdate(){InfoUpdated();}
};

class VideoInfo : public BaseInfo {
public:
    VideoInfo();
    VideoInfo( const VideoInfo& Copy );

    VideoInfo& operator = ( const VideoInfo& Copy );

    virtual InfoType getType() const {
        return IT_VideoInfo;
    }

    QString KeyWords,
            sViews,
            sFavorites,
            sComments,
            sRating,
            sLikes,
            sDisLikes,
            sLength,
            sUploaded;

    QUrl UrlComments;

    VideoFeedPtr Releated,
                 Responces;

    size_t Views,
           Favorites,
           Comments,
           Likes,
           DisLikes,
           Length;


    QDate Uploaded;

    float Rating;

    virtual VideoInfo*      cast_VideoInfo()    {return this;}

    virtual void FixInfo();
};

class UserInfo : public BaseInfo {
public:
    UserInfo();
    UserInfo( const UserInfo& Copy );

    UserInfo& operator = ( const UserInfo& Copy );

    QString FirstName,
            LastName,
            FullName,
            sJoined,
            sAge,
            sSubscribers,
            sChannelsViews,
            sTotalViews,
            sTotalUploads;

    size_t Age,
           Subscribers,
           ChannelsViews,
           TotalViews,
           TotalUploads;

    QDate Joined;

    virtual InfoType getType() const {
        return IT_UserInfo;
    }

    virtual UserInfo*       cast_UserInfo()     {return this;}

    virtual void FixInfo();

    PlaylistFeedPtr Playlists;

    VideoFeedPtr Favorites, Uploads;

};

template< class Type >
class Feed {
public:
    Feed();
    Feed( const Feed<Type>& Copy );

    Feed<Type>& operator=( const Feed<Type>& Copy );

    QVector< Type > Content;

    uint ParsedPages;
    bool HasFullFeed;

    QUrl NextPageInFeed;

    size_t Size;

    QString sSize;

    virtual void FixInfo();
};

class VideoFeed : public BaseInfo, public Feed<VideoPtr> {
public:
    enum  FeedType {
        FT_Unown,
        FT_Playlist,
        FT_VideoSearch,
        FT_UserFavorites,
        FT_UserUploads,
        FT_VideoReleated,
        FT_VideoResponses
    };

    VideoFeed();

    VideoFeed( const VideoFeed& Copy );

    VideoFeed& operator = ( const VideoFeed& Copy );

    virtual VideoFeed*      cast_VideoFeed()    {return this;}


    virtual InfoType getType() const {
        return IT_VideoFeed;
    }

    FeedType Type;

    virtual void FixInfo();

};

class PlaylistFeed : public BaseInfo, public Feed<VideoFeedPtr> {
public:
    enum FeedType {
        FT_Unown,
        FT_Search,
        FT_UserPlaylists
    };

    PlaylistFeed();
    PlaylistFeed( const PlaylistFeed& Copy );

    PlaylistFeed& operator = ( const PlaylistFeed& Copy );
    virtual PlaylistFeed*   cast_PlaylistFeed() {return this;}

    virtual void FixInfo();

    FeedType Type;

    virtual InfoType getType() const {
        return IT_PlaylistFeed;
    }
};

/*Channel, or User, feed */
class ChannelFeed : public BaseInfo, public Feed<UserPtr> {
public:

    ChannelFeed();
    ChannelFeed( const ChannelFeed& Copy );

    ChannelFeed& operator = ( const ChannelFeed& Copy );
    virtual ChannelFeed*    cast_ChannelFeed()  {return this;}

    virtual void FixInfo();

    virtual InfoType getType() const {
        return IT_ChannelFeed;
    }
};


#endif // INFO_H
