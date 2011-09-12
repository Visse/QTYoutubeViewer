#include "Parser3.h"

#include "Log.h"

#include <iostream>

#include <QMessageBox>

Parser3_Sender::Parser3_Sender( QObject *parent, BasePtr Info )
    : QObject(parent), bInfo(Info.data())
{
}


void Parser3_Sender::setInfo( BasePtr Info ) {
    bInfo = Info;
}

BasePtr Parser3_Sender::getInfo() {
    return bInfo;
}

void Parser3_Sender::SendSignal( QDomDocument Page ) {
    Signal( bInfo, Page );
    return;
}

//// *******************************************

Parser3::Parser3(QObject *parent) :
    QObject(parent), mTimer(this), mQue(), mResive(), NetworkMgr(0), ProgressBar(0)
{
    mTimer.setInterval(100);
    mTimer.start();

    connect( &mTimer, SIGNAL(timeout()), SLOT(Update()) );
}

/*
/// url Escape Codes - can be found here: http://www.december.com/html/spec/esccodes.html
static const char *URL_Escape_Codes[][2] = { {" ", "%20" }, {"<", "%3C" }, {">", "%3E" },
    {"#", "%23" }, {"%", "%25" }, {"{", "%7B" }, {"}", "%7D" }, {"|", "%7C" },
    {"\\", "%5C" }, {"^", "%5E" }, {"~", "%7E" }, {"[", "%5B" }, {"]", "%5D" },
    {"`", "%60" }, {";", "%3B" }, {"/", "%2F" }, {"?", "%3F" }, {":", "%3A" },
    {"@", "%40" }, {"=", "%3D" }, {"&", "%26" }, {"$", "%24" } };


QString ToUrlEscape( const QString& String )
{
    QString Temp(String);
    /// Each element is 8 bytes in size ( 4*2, pointers are 4 bits and its 2 in each )
    for( int i=0; i < (sizeof(URL_Escape_Codes)/8); i++ ) {
        Temp.replace( URL_Escape_Codes[i][0], URL_Escape_Codes[i][1] );
    }
    return Temp;
}*/

void Parser3::Update() {
    /// While we have elements in the que and we still has space to make a request DO
    while( !mQue.isEmpty() && mResive.size() < 5 && NetworkMgr ) {
        QPair< QNetworkRequest, QPointer<Parser3_Sender> > Pair = mQue.takeFirst();
        // get the reply
        QNetworkReply *Reply = NetworkMgr->get( Pair.first );

        mResive.insert( Reply, Pair.second );
        // connect the signals to the slots :)
        connect( Reply, SIGNAL(finished()), SLOT(ReadyRead()) );
        connect( Reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(NetworkError()) );
    }

    if( ProgressBar )
    {/// @todo make a better version that is a bit more acurate about progress...
        int Max = mQue.size()+mResive.size();
        ProgressBar->setMaximum( Max );
        ProgressBar->setValue( mResive.size() );
        if( Max == 0 )
            ProgressBar->hide();
        else
            ProgressBar->show();
    }
}

void Parser3::NetworkError()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    Error( "Network error: " + reply->errorString() );
}

Parser3::UrlType Parser3::getUrlType( QUrl U ) {
    QString Url = U.toString();

    QStringList Elements = Url.split('/',QString::SkipEmptyParts);

    if( !U.isValid() )
        return UT_Unown;

    if( Elements.size() > 2 ) {
        if( Elements.at(2).startsWith("watch?v=", Qt::CaseInsensitive) ) {
            return UT_Video;
        }
        if( Elements.at(2).startsWith("playlist?list=", Qt::CaseInsensitive) ) {
            return UT_Playlist;
        }
        if( Elements.at(2).startsWith("user", Qt::CaseInsensitive) ) {
            // A video that is being viewed on a user chanel
            /*if( Elements.at(2).toLower().endsWith("#p") ) {
                return UT_Video;
            }*/
            return UT_User;
        }
        if( Elements.at(2).toLower() == "feeds" ) {
            if( Elements.size() > 6 ) {
                if( Elements.at(4) == "playlists" ) {
                    return UT_Playlist_Feed;
                }
                if( Elements.at(4) == "videos" ) {
                    if( Elements.at(6).startsWith("related", Qt::CaseInsensitive) ) {
                        return UT_Video_Releated;
                    }
                    /// @todo chech if spelled correcly
                    if( Elements.at(6).startsWith("responses", Qt::CaseInsensitive) ) {
                        return UT_Video_Responses;
                    }
                    return UT_Video_Feed;
                }
                if( Elements.at(4) == "users" ) {
                    if( Elements.at(6).startsWith("favorites", Qt::CaseInsensitive) ) {
                        return UT_User_Favorites;
                    }
                    if( Elements.at(6).startsWith("uploads", Qt::CaseInsensitive) ) {
                        return UT_User_Uploads;
                    }
                    if( Elements.at(6).startsWith("playlists", Qt::CaseInsensitive) ) {
                        return UT_User_Playlists;
                    }
                    return UT_User_Feed;
                }
            }
            else {
                Error( "Url looks like a feed url, but is too short! Url("+Url+")" );
            }
        }
    }
    return  UT_Unown;
}

QString Parser3::extractID( QUrl U ) {
    ///@ todo add suport for feeds

    QString Url = U.toString();

    UrlType Type = getUrlType( U );

    QStringList Elements = Url.split('/', QString::SkipEmptyParts);

    QString ID = "";
    switch( Type ) {
    case( UT_Video ):
        ID = Elements.at(2).mid(8,11);
        break;
    case( UT_Playlist ):
        ID = Elements.at(2).mid(16,17);
        break;
    case( UT_User ):
        ID = Elements.at(3);
        break;
    default:
        Error( QString("Unown url! cant extract ID! Url[1%]").arg(Url) );
        break;
    }

    /// Characters to remove from id,
    /// example USER#p will be USER
    static const char RemoveCharacter[] = { '#', '?', '=', '&' };
    int temp = 0;
    for( int i=0; i < sizeof(RemoveCharacter); i++, temp=0 ){
        temp = ID.indexOf(RemoveCharacter[i]);
        if( temp != -1 )// found?
            ID.remove(temp,ID.size()-temp);
    }
    return ID;
}

void Parser3::ParseUrl( QUrl U )
{
    if( !U.isValid() )
        return;
    BasePtr Info;

    Parser3_Sender *S=0;


    QString Url;

    QString ID = extractID( U );
    UrlType Type = getUrlType( U );

    switch( Type ) {
    case( UT_Video ):
    case( UT_Video_Feed ):
        Url = "http://gdata.youtube.com/feeds/api/videos/"+ID+"?v=2";
        Info = new VideoInfo;
        S = new Parser3_Sender(this, Info );
        connect( S, SIGNAL(Signal(BasePtr,QDomDocument)), SLOT(VideoRecived(BasePtr,QDomDocument)) );
        break;
    case( UT_Playlist ):
    case( UT_Playlist_Feed ):
        Url = "http://gdata.youtube.com/feeds/api/playlists/"+ID+"?v=2&start-index=1&max-results=50";
        Info = new VideoFeed;
        Info->cast_VideoFeed()->Type = VideoFeed::FT_Playlist;
        S = new Parser3_Sender( this, Info );
        connect( S, SIGNAL(Signal(BasePtr,QDomDocument)), SLOT(PlaylistRecived(BasePtr,QDomDocument)) );
        break;
    case( UT_User ):
    case( UT_User_Feed ):
        Url = "http://gdata.youtube.com/feeds/api/users/"+ID+"?v=2";
        Info = new UserInfo;
        S = new Parser3_Sender( this, Info );
        connect( S, SIGNAL(Signal(BasePtr,QDomDocument)), SLOT(UserRecived(BasePtr,QDomDocument)) );
        break;
    default:
        Error( QString("Unown url type! Url[%1]").arg(Url) );
    }

    mQue.push_back( qMakePair( QNetworkRequest(QUrl(Url)),QPointer<Parser3_Sender>(S)) );

}

void Parser3::Search( QString String )
{
    QString Url="";
    Parser3_Sender *S=0;

    if( String.startsWith("videoID:") )
    {
        String.remove("videoID:");
        String = String.simplified();
        if( String.size() != 11 ) /// A VideoID is 11 case senitive characters
        {
            Error( "VideoID is not the right size ID["+String+"]" );
            QMessageBox::warning( 0, "WARNING", "The string[<font color=red>"+String+"</font>] is not the right size to be a videoID!, it has to be 11 characters to be a ID");
            return;
        }
        Url = "http://gdata.youtube.com/feeds/api/videos/"+String+"?v=2";
        BasePtr Info = new VideoInfo;
        S = new Parser3_Sender(this, Info );
        connect( S, SIGNAL(Signal(BasePtr,QDomDocument)), SLOT(VideoRecived(BasePtr,QDomDocument)) );
    }
    else if( String.startsWith("playlistID:") )
    {
        String.remove("playlistID:");
        String = String.simplified();
        if( String.size() != 16 )
        {
            if( String.size() == 18 && String.startsWith("PL") )
                String.remove(0,2);
            else
            {
                Error( "VideoID is not the right size ID["+String+"]" );
                QMessageBox::warning( 0, "WARNING", "The string[<font color=red>"+String+"</font>] is not the right size to be a playlistID!, it has to be 16 characters to be a ID");
                return;
            }
        }
        Url = "http://gdata.youtube.com/feeds/api/playlists/"+String+"?v=2&start-index=1&max-results=50";
        BasePtr Info = new VideoFeed;
        Info->cast_VideoFeed()->Type = VideoFeed::FT_Playlist;
        S = new Parser3_Sender(this, Info );
        connect( S, SIGNAL(Signal(BasePtr,QDomDocument)), SLOT(PlaylistRecived(BasePtr,QDomDocument)) );
    }
    else if( String.startsWith("user:") )
    {
        String.remove("user:");
        String = String.simplified();
        Url = "http://gdata.youtube.com/feeds/api/users/"+String+"?v=2";
        BasePtr Info = new UserInfo;
        S = new Parser3_Sender( this, Info );
        connect( S, SIGNAL(Signal(BasePtr,QDomDocument)), SLOT(UserRecived(BasePtr,QDomDocument)) );
    }
    else if( String.startsWith("video:") )
    {
        String.remove("video:");
        String = String.simplified();
        String = String.replace(' ','+');
        Url = "http://gdata.youtube.com/feeds/api/videos?q=" + String + "&v=2&start-index=1&max-results=50";
        BasePtr Info = new VideoFeed;
        Info->cast_VideoFeed()->Type = VideoFeed::FT_VideoSearch;
        S = new Parser3_Sender( this, Info );
        connect( S, SIGNAL(Signal(BasePtr,QDomDocument)), SLOT(VideoSeachRecived(BasePtr,QDomDocument)) );

    }
    else if( String.startsWith("playlist:") )
    {
        /// @todo
    }
    else if( String.startsWith("channel:") )
    {
        /// @todo
    }
    else
    {/// Simple treat it like a video search
        Search("video:"+String.simplified());
    }

    if( S )
        mQue.push_back( qMakePair( QNetworkRequest(QUrl(Url)),QPointer<Parser3_Sender>(S)) );

    return;
}

void Parser3::ParseString( QString String ) {
    /// @todo
}

void Parser3::ParseNextPage( VideoFeedPtr Feed )
{
    Parser3_Sender *S = new Parser3_Sender( this, Feed->cast_BaseInfo() );

    if( Feed.isNull() || !Feed->NextPageInFeed.isValid() )
        return;
    switch( Feed->Type ) {
    case( VideoFeed::FT_Playlist ):
        connect( S, SIGNAL(Signal(BasePtr,QDomDocument)), SLOT(PlaylistRecived(BasePtr,QDomDocument)) );
        break;
    case( VideoFeed::FT_UserFavorites ):
        connect( S, SIGNAL(Signal(BasePtr,QDomDocument)), SLOT(UserFavoritesRecived(BasePtr,QDomDocument)) );
        break;
    case( VideoFeed::FT_UserUploads ):
        connect( S, SIGNAL(Signal(BasePtr,QDomDocument)), SLOT(UserUploadsRecived(BasePtr,QDomDocument)) );
        break;
    case( VideoFeed::FT_VideoReleated ):
        connect( S, SIGNAL(Signal(BasePtr,QDomDocument)), SLOT(VideoReleatedRecived(BasePtr,QDomDocument)) );
        break;
    case( VideoFeed::FT_VideoResponses ):
        connect( S, SIGNAL(Signal(BasePtr,QDomDocument)), SLOT(VideoResponcesRecived(BasePtr,QDomDocument)) );
        break;
    case( VideoFeed::FT_VideoSearch ):
        connect( S, SIGNAL(Signal(BasePtr,QDomDocument)), SLOT(VideoSeachRecived(BasePtr,QDomDocument)) );
        break;
    default:
        /// should newer happen
        Error( "Something is realy messed up :/" );
        break;
    }

    mQue.push_back( qMakePair(QNetworkRequest(Feed->NextPageInFeed), QPointer<Parser3_Sender>(S)) );

    Feed->NextPageInFeed.clear();
}

void Parser3::ReadyRead() {
    QNetworkReply *reply = static_cast<QNetworkReply*>( sender() );

    QPointer<Parser3_Sender> S = mResive.take(reply);
    if( S.isNull() ) {
        Error( "Reply not in resive! or is null" );
        return;
    }
    QDomDocument Page;
    {
        QString Error;
        int Line, Column;
        if( !Page.setContent(reply->readAll(), &Error, &Line, &Column ) )
        {
            Error( "Error in parsing page { "+Error+":"+QString::number(Line)+":"+QString::number(Column)+" }" );
            return;
        }
    }

    S->SendSignal(Page);
    S->deleteLater();
    reply->deleteLater();

    return;
}

void Parser3::VideoRecived( BasePtr bInfo, QDomDocument Page ) {
    if( !bInfo )
    {
        Error( "Null pointer to Info!");
        return;
    }
    if( bInfo->getType() != IT_VideoInfo ) {
        Error( "Info is not videoInfo! type("+QString::number(bInfo->getType())+")" );
        return;
    }
    VideoPtr Info = bInfo->cast_VideoInfo();

    QDomNode Entry = Page.namedItem("entry");

    if( Entry.isNull() ) {
        Error( "Entry is null!, maybe page is not a videoEntry?" );
        return;
    }

    QStringList IDString = Entry.namedItem("id").namedItem("#text").nodeValue().split(':', QString::SkipEmptyParts );
    if( IDString.size() != 4 || IDString.at(2) != "video" ) {
        Error( "Page is not a video Entry!!" );
        return;
    }
    ParseVideoEntry(Entry, Info);
    //(*Info) = vInfo;

    VideoParsed( Info );

    return;
}

void Parser3::PlaylistRecived( BasePtr bInfo, QDomDocument Page )
{
    if( !bInfo )
    {
        Error( "Null Pointer to Info!" );
        return;
    }
    if( bInfo->getType() != IT_VideoFeed )
    {
        Error( "Info is not playlist(videofeed)!" );
        return;
    }

    VideoFeedPtr Info = bInfo->cast_VideoFeed();

    QDomNode Feed = Page.namedItem("feed");

    if( Feed.isNull() )
    {
        Error( "Feed is null!, Maybe page is not a playlist(videofeed)" );
        return;
    }

    QStringList IDString = Feed.namedItem("id").namedItem("#text").nodeValue().split(':', QString::SkipEmptyParts );
    if( IDString.size() != 4 || IDString.at(2) != "playlist" ) {
        Error( "Page is not a playlist!!" );
        return;
    }

    ParseVideoFeed( Feed, Info );

    // the id is const :/
    const_cast<QString&>(Info->ID) = IDString.at(3).toLower();

    /*If we don't have a thumbnail, we take the first video's thumbview*/
    if( !Info->UrlThumbnail.isValid() ) {
        Info->UrlThumbnail = Info->Content.first()->UrlThumbnail;
    }

    if( Info->ParsedPages == 1 )
        PlaylistParsed(Info);
    else
        VideosAdded(Info);

    return;

}

void Parser3::UserRecived( BasePtr bInfo, QDomDocument Page ) {
    if( !bInfo )
    {
        Error( "Null Pointer to Info!" );
        return;
    }
    if( bInfo->getType() != IT_UserInfo )
    {
        Error( "Info is not UserInfo!" );
        return;
    }
    UserPtr Info = bInfo->cast_UserInfo();

    QDomNode Entry = Page.namedItem("entry");

    if( Entry.isNull() ) {
        Error( "Entry is null!, maybe page is not a userEntry?" );
        return;
    }

    QStringList IDString = Entry.namedItem("id").namedItem("#text").nodeValue().split(':', QString::SkipEmptyParts );
    if( IDString.size() != 4 || IDString.at(2) != "user" ) {
        Error( "Page is not a user Entry!!" );
        return;
    }

    ParseUserEntry(Entry, Info );

    const_cast<QString&>(Info->ID) = Info->Author.toLower();

    UserParsed( Info );
    return;
}

void Parser3::UserPlaylistsRecived( BasePtr Info, QDomDocument Page )
{
}

void Parser3::UserFavoritesRecived( BasePtr bInfo, QDomDocument Page )
{
    if( !bInfo )
    {
        Error( "Null Pointer to Info!" );
        return;
    }
    if( bInfo->getType() != IT_VideoFeed )
    {
        Error( "Info is not UserFavorites(videofeed)!" );
        return;
    }

    VideoFeedPtr Info = bInfo->cast_VideoFeed();

    QDomNode Feed = Page.namedItem("feed");

    if( Feed.isNull() )
    {
        Error( "Feed is null!, Maybe page is not a UserFavorites(videofeed)" );
        return;
    }

    QStringList IDString = Feed.namedItem("id").namedItem("#text").nodeValue().split(':', QString::SkipEmptyParts );
    if( IDString.size() != 5 || IDString.at(4) != "favorites" ) {
        Error( "Page is not a UserFavorites!!" );
        return;
    }

    ParseVideoFeed( Feed, Info );

    /*If we don't have a thumbnail, we take the first video's thumbview*/
    if( !Info->UrlThumbnail.isValid() ) {
        Info->UrlThumbnail = Info->Content.first()->UrlThumbnail;
    }

    VideosAdded(Info);

    return;
}

void Parser3::UserUploadsRecived( BasePtr bInfo, QDomDocument Page )
{
    if( !bInfo )
    {
        Error( "Null Pointer to Info!" );
        return;
    }
    if( bInfo->getType() != IT_VideoFeed )
    {
        Error( "Info is not UserFavorites(videofeed)!" );
        return;
    }

    VideoFeedPtr Info = bInfo->cast_VideoFeed();

    QDomNode Feed = Page.namedItem("feed");

    if( Feed.isNull() )
    {
        Error( "Feed is null!, Maybe page is not a UserUploads(videofeed)" );
        return;
    }

    QStringList IDString = Feed.namedItem("id").namedItem("#text").nodeValue().split(':', QString::SkipEmptyParts );
    if( IDString.size() != 5 || IDString.at(4) != "uploads" ) {
        Error( "Page is not a UserUpload!!" );
        return;
    }

    ParseVideoFeed( Feed, Info );

    /*If we don't have a thumbnail, we take the first video's thumbview*/
    if( !Info->UrlThumbnail.isValid() ) {
        Info->UrlThumbnail = Info->Content.first()->UrlThumbnail;
    }

    VideosAdded(Info);
    return;
}

void Parser3::VideoResponcesRecived( BasePtr bInfo, QDomDocument Page )
{
    if( !bInfo )
    {
        Error( "Null Pointer to Info!" );
        return;
    }
    if( bInfo->getType() != IT_VideoFeed )
    {
        Error( "Info is not a video Responce(videofeed)!" );
        return;
    }

    VideoFeedPtr Info = bInfo->cast_VideoFeed();

    QDomNode Feed = Page.namedItem("feed");

    if( Feed.isNull() )
    {
        Error( "Feed is null!, Maybe page is not a video Responce(videofeed)" );
        return;
    }

    QStringList IDString = Feed.namedItem("id").namedItem("#text").nodeValue().split(':', QString::SkipEmptyParts );
    if( IDString.size() != 5 || (IDString.at(2) != "video" && IDString.at(4) != "responses" )) {
        Error( "Page is not a video responce!!" );
        return;
    }

    QString Title = Info->Title;
    ParseVideoFeed( Feed, Info );
    /// Youtube will say that the title is some thing like "Videos responses to 'TITLE'"
    /// But we really does not want it :)
    if( !Title.isNull() )
        Info->Title = Title;

    // the id is const :/
    const_cast<QString&>(Info->ID) = IDString.at(3).toLower();

    // If we don't have a thumbnail, we take the first video's thumbview
    if( !Info->UrlThumbnail.isValid() ) {
        Info->UrlThumbnail = Info->Content.first()->UrlThumbnail;
    }

    VideosAdded(Info);
}

void Parser3::VideoReleatedRecived( BasePtr bInfo, QDomDocument Page )
{
    if( !bInfo )
    {
        Error( "Null Pointer to Info!" );
        return;
    }
    if( bInfo->getType() != IT_VideoFeed )
    {
        Error( "Info is not a video Responce(videofeed)!" );
        return;
    }

    VideoFeedPtr Info = bInfo->cast_VideoFeed();

    QDomNode Feed = Page.namedItem("feed");

    if( Feed.isNull() )
    {
        Error( "Feed is null!, Maybe page is not a video Releated(videofeed)" );
        return;
    }

    QStringList IDString = Feed.namedItem("id").namedItem("#text").nodeValue().split(':', QString::SkipEmptyParts );
    if( IDString.size() != 5 || (IDString.at(2) != "video" && IDString.at(4) != "related" )) {
        Error( "Page is not a video Releated!!" );
        return;
    }

    QString Title = Info->Title;
    ParseVideoFeed( Feed, Info );
    /// Youtube will say that the title is some thing like "Videos related to 'TITLE'"
    /// But we really does not want it :)
    if( !Title.isNull() )
        Info->Title = Title;

    // the id is const :/
    const_cast<QString&>(Info->ID) = IDString.at(3).toLower();

    // If we don't have a thumbnail, we take the first video's thumbview
    if( !Info->UrlThumbnail.isValid() ) {
        Info->UrlThumbnail = Info->Content.first()->UrlThumbnail;
    }

    VideoFeed *vInfo = Info;
    VideosAdded(Info);
}

void Parser3::VideoSeachRecived( BasePtr bInfo, QDomDocument Page ) {
    if( !bInfo )
    {
        Error( "Null Pointer to Info!" );
        return;
    }
    if( bInfo->getType() != IT_VideoFeed )
    {
        Error( "Info is not playlist(videofeed)!" );
        return;
    }

    VideoFeedPtr Info = bInfo->cast_VideoFeed();

    QDomNode Feed = Page.namedItem("feed");

    if( Feed.isNull() )
    {
        Error( "Feed is null!, Maybe page is not a playlist(videofeed)" );
        return;
    }

    QStringList IDString = Feed.namedItem("id").namedItem("#text").nodeValue().split(':', QString::SkipEmptyParts );
    if( IDString.size() != 3 || IDString.at(2) != "videos" ) {
        Error( "Page is not a videoSeach!!" );
        return;
    }

    ParseVideoFeed( Feed, Info );

    Info->Title.remove("YouTube Videos matching query: ");

    // the id is const :/
    const_cast<QString&>(Info->ID) = "[VideoSearch]"+Info->Title;

    /*If we don't have a thumbnail, we take the first video's thumbview*/
    if( !Info->UrlThumbnail.isValid() ) {
        Info->UrlThumbnail = Info->Content.first()->UrlThumbnail;
    }

    if( Info->ParsedPages == 1 )
        VideoSearchParsed(Info);
    else
        VideosAdded(Info);

    return;

}

void Parser3::ParseVideoEntry( QDomNode Node, VideoPtr Info ) {

    QDomNode Child = Node.firstChild();

    if( Info->Releated.isNull() ) {
        Info->Releated = new VideoFeed;
        Info->Releated->Type = VideoFeed::FT_VideoReleated;
        Info->Releated->ParsedPages = 0;
        Info->Releated->HasFullFeed = false;
    } if( Info->Responces.isNull() ) {
        Info->Responces = new VideoFeed;
        Info->Responces->Type = VideoFeed::FT_VideoResponses;
        Info->Responces->ParsedPages = 0;
        Info->Responces->HasFullFeed = false;
    }


    while( !Child.isNull() )
    {
        QString NodeName = Child.nodeName();
        if( NodeName == "published" )
        {
            Info->Uploaded = QDate::fromString( Child.namedItem("#text").nodeValue(), Qt::ISODate );
        }
        else if( NodeName == "title" )
        {
            Info->Title = Child.namedItem("#text").nodeValue();
        }
        else if( NodeName == "link" )
        {
            QString RelValue = Child.attributes().namedItem("rel").nodeValue();
            if( RelValue == "http://gdata.youtube.com/schemas/2007#video.responses" )
            {
                Info->Releated->NextPageInFeed = QUrl( Child.attributes().namedItem("href").nodeValue() );
            }
            else if( RelValue == "http://gdata.youtube.com/schemas/2007#video.related" )
            {
                if( Info->Responces.isNull() )
                {
                Info->Responces->NextPageInFeed = QUrl( Child.attributes().namedItem("href").nodeValue() );
            }
            }
        }
        else if( NodeName == "author" )
        {
            Info->Author = Child.namedItem("name").namedItem("#text").nodeValue();
            Info->UrlAuthor = QUrl( Child.namedItem("uri").namedItem("#text").nodeValue() );
        }
        else if( NodeName == "gd:comments" )
        {
            Info->UrlComments = QUrl( Child.namedItem("gd:feedLink").attributes().namedItem("href").nodeValue() );
            Info->Comments = Child.namedItem("gd:feedLink").attributes().namedItem("countHint").nodeValue().toInt();
        }
        else if( NodeName == "media:group" )
        {
            Info->Description = Child.namedItem("media:description").namedItem("#text").nodeValue();
            Info->KeyWords = Child.namedItem("media:keywords").namedItem("#text").nodeValue();
            Info->Length = Child.namedItem("yt:duration").attributes().namedItem("seconds").nodeValue().toInt();
            /// The ID is const, but here we have to change it :/
            const_cast<QString&>( Info->ID ) = Child.namedItem("yt:videoid").namedItem("#text").nodeValue();
        }
        else if( NodeName == "gd:rating" )
        {
            Info->Rating = Child.attributes().namedItem("average").nodeValue().toFloat();
        }
        else if( NodeName == "yt:statistics" )
        {
            Info->Views = Child.attributes().namedItem("viewCount").nodeValue().toInt();
            Info->Favorites = Child.attributes().namedItem("favoriteCount").nodeValue().toInt();
        }
        else if( NodeName == "yt:rating" )
        {
            Info->Likes = Child.attributes().namedItem("numLikes").nodeValue().toInt();
            Info->DisLikes = Child.attributes().namedItem("numDislikes").nodeValue().toInt();
        }

        Child = Child.nextSibling();
    }

    Info->UrlThumbnail = QUrl( "http://i.ytimg.com/vi/"+Info->ID+"/hqdefault.jpg" );

    Info->FixInfo();
    return;
}

void Parser3::ParseVideoFeed( QDomNode Node, VideoFeedPtr Info ) {
    QDomNode Child = Node.firstChild();

    while( !Child.isNull() )
    {
        QString NodeName = Child.nodeName();
        if( NodeName == "title" )
        {
            Info->Title = Child.namedItem("#text").nodeValue();
        }
        else if( NodeName == "link" && ( Child.attributes().namedItem("rel").nodeValue() == "next" ) )
        {
            Info->NextPageInFeed = QUrl( Child.attributes().namedItem("href").nodeValue() );
        }
        else if( NodeName == "author" )
        {
            Info->Author = Child.namedItem("name").namedItem("#text").nodeValue();
            Info->UrlAuthor = QUrl( Child.namedItem("uri").namedItem("#text").nodeValue() );
        }
        else if( NodeName == "openSearch:totalResults" )
        {
            Info->Size = Child.namedItem("#text").nodeValue().toInt();
        }
        else if( NodeName == "media:group" )
        {
            Info->Description = Child.namedItem("media:description").namedItem("#text").nodeValue();
            if( !Child.namedItem("media:thumbnail").isNull() )
            {/*If we allready has a thumbnail we don't want to erase it*/
                Info->UrlThumbnail = QUrl( Child.namedItem("media:thumbnail").namedItem("#text").nodeValue() );
            }
        }
        else if( NodeName == "entry" )
        {
            VideoInfo *Video = new VideoInfo;
            ParseVideoEntry(Child,Video);
            Info->Content.push_back( Video );
        }

        Child = Child.nextSibling();
    }

    Info->ParsedPages++;
    Info->FixInfo();
    return;
}

void Parser3::ParseUserEntry( QDomNode Node, UserPtr Info ) {
    QDomNode Child = Node.firstChild();

    /// Just fill it up with some video feeds, if not already assigned
    if( Info->Favorites.isNull() ) {
        Info->Favorites = new VideoFeed;
        Info->Favorites->Type = VideoFeed::FT_UserFavorites;
        Info->Favorites->ParsedPages = 0;
        Info->Favorites->HasFullFeed = false;
    } if( Info->Uploads.isNull() ) {
        Info->Uploads = new VideoFeed;
        Info->Uploads->Type = VideoFeed::FT_UserUploads;
        Info->Uploads->ParsedPages = 0;
        Info->Uploads->HasFullFeed = false;
    } if( Info->Playlists.isNull() ) {
        Info->Playlists = new PlaylistFeed;
        Info->Playlists->ParsedPages = 0;
        Info->Playlists->HasFullFeed = false;
        Info->Playlists->Type = PlaylistFeed::FT_UserPlaylists;
    }

    while( !Child.isNull() )
    {
        QString NodeName = Child.nodeName();
        if( NodeName == "published" )
        {
            Info->Joined = QDate::fromString( Child.namedItem("#text").nodeValue(), Qt::ISODate );
        }
        else if( NodeName == "title" )
        {
            Info->Title = Child.namedItem("#text").nodeValue();
        }
        else if( NodeName == "author" )
        {
            Info->Author = Child.namedItem("name").namedItem("#text").nodeValue();
        }
        else if( NodeName == "yt:age" )
        {
            Info->Age = Child.namedItem("#text").nodeValue().toInt();
        }
        else if( NodeName == "yt:aboutMe" )
        {
            Info->Description = Child.namedItem("#text").nodeValue();
        }
        else if( NodeName == "gd:feedLink" )
        {
            QString RelValue = Child.attributes().namedItem("rel").nodeValue();
            if( RelValue == "http://gdata.youtube.com/schemas/2007#user.favorites" )
            {
                Info->Favorites->Size = Child.attributes().namedItem("countHint").nodeValue().toInt();
            }
            else if( RelValue == "http://gdata.youtube.com/schemas/2007#user.playlists" )
            {
                Info->Playlists->Size = Child.attributes().namedItem("countHint").nodeValue().toInt();
            }
            else if( RelValue == "http://gdata.youtube.com/schemas/2007#user.uploads" )
            {
                Info->Uploads->Size = Child.attributes().namedItem("countHint").nodeValue().toInt();
                Info->TotalUploads = Info->Uploads->Size;
            }
        }
        else if( NodeName == "yt:firstName" )
        {
            Info->FirstName = Child.namedItem("#text").nodeValue();
        }
        else if( NodeName == "yt:lastName" )
        {
            Info->LastName = Child.namedItem("#text").nodeValue();
        }
        else if( NodeName == "yt:statistics" )
        {
            Info->Subscribers = Child.attributes().namedItem("subscriberCount").nodeValue().toInt();
            Info->ChannelsViews = Child.attributes().namedItem("viewCount").nodeValue().toInt();
            Info->TotalViews = Child.attributes().namedItem("totalUploadViews").nodeValue().toInt();
        }
        else if( NodeName == "media:thumbnail" )
        {
            Info->UrlThumbnail = QUrl( Child.attributes().namedItem("url").nodeValue() );
        }

        Child = Child.nextSibling();
    }

    /// Set some important information about the feeds :)
    Info->Favorites->NextPageInFeed = QUrl(QString("http://gdata.youtube.com/feeds/api/users/%1/favorites?v=2&start-index=1&max-results=50").arg(Info->Author));
    Info->Uploads->NextPageInFeed = QUrl(QString("http://gdata.youtube.com/feeds/api/users/%1/uploads?v=2&start-index=1&max-results=50").arg(Info->Author));
    Info->Playlists->NextPageInFeed = QUrl(QString("http://gdata.youtube.com/feeds/api/users/%1/playlists?v=2&start-index=1&max-results=50").arg(Info->Author));

    Info->Favorites->Author = Info->Author;
    Info->Favorites->Title = QString("%1 Favorites").arg(Info->Author);
    Info->Favorites->UrlThumbnail = Info->UrlThumbnail;
    Info->Favorites->UrlAuthor = QUrl(QString("http://gdata.youtube.com/feeds/api/users/%1?v=2").arg(Info->Author));

    Info->Uploads->Author = Info->Author;
    Info->Uploads->Title = QString("%1 Uploads").arg(Info->Author);
    Info->Uploads->UrlThumbnail = Info->UrlThumbnail;
    Info->Uploads->UrlAuthor = QUrl(QString("http://gdata.youtube.com/feeds/api/users/%1?v=2").arg(Info->Author));

    Info->Playlists->Author = Info->Author;
    Info->Playlists->Title = QString("%1 Playlists").arg(Info->Author);
    Info->Playlists->UrlThumbnail = Info->UrlThumbnail;
    Info->Playlists->UrlAuthor = QUrl(QString("http://gdata.youtube.com/feeds/api/users/%1?v=2").arg(Info->Author));


    /// The ID is const so I have to make a cast :/
    const_cast<QString&>(Info->Favorites->ID) = Info->Author+"/favorites";
    const_cast<QString&>(Info->Uploads->ID) = Info->Author+"/uploads";
    const_cast<QString&>(Info->Playlists->ID) = Info->Author+"/playlists";

    Info->FixInfo();

    return;
}
