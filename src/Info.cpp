#include "Info.h"

BaseInfo::BaseInfo() :
    QObject(),
    ID(""),
    Author(""), Title(""), Description(""),
    UrlAuthor(""), UrlThumbnail("")
{
}

BaseInfo::BaseInfo( const BaseInfo& Copy ) :
    QObject()
{
    (*this) = Copy;
}

BaseInfo& BaseInfo::operator = ( const BaseInfo& Copy )
{
    /// Since ID is const, I have to const cast it...
    const_cast<QString&>(ID) = Copy.ID;

    Author = Copy.Author;
    Title = Copy.Title;
    Description = Copy.Description;
    UrlAuthor = Copy.UrlAuthor;
    UrlThumbnail = Copy.UrlThumbnail;

    return *this;
}

// *************************************************************

VideoInfo::VideoInfo() :
    BaseInfo(),
    Uploaded(), KeyWords(""),
    sViews(""), sFavorites(""), sComments(""),
    sRating(""), sLikes(""), sDisLikes(""), sLength(""),
    UrlComments(""),
    Releated(0), Responces(0),
    Views(0), Favorites(0), Comments(0), Likes(0),
    DisLikes(0), Length(0), Rating(0)
{
}

VideoInfo::VideoInfo( const VideoInfo& Copy ) :
    BaseInfo()
{
    (*this) = Copy;
}

VideoInfo& VideoInfo::operator = ( const VideoInfo& Copy ) {
    Uploaded    = Copy.Uploaded;
    KeyWords    = Copy.KeyWords;

    sViews      = Copy.sViews;
    sFavorites  = Copy.sFavorites;
    sComments   = Copy.sComments;
    sRating     = Copy.sRating;
    sLikes      = Copy.sLikes;
    sDisLikes   = Copy.sDisLikes;
    sLength     = Copy.sLength;

    UrlComments     = Copy.UrlComments;

    Responces   = Copy.Responces;
    Releated    = Copy.Releated;

    Views       = Copy.Views;
    Favorites   = Copy.Favorites;
    Comments    = Copy.Comments;
    Likes       = Copy.Likes;
    DisLikes    = Copy.DisLikes;
    Length      = Copy.Length;

    Rating  = Copy.Rating;

    BaseInfo::operator =( Copy );

    return *this;
}

void VideoInfo::FixInfo()
{
    sViews      = QString::number(Views);
    sFavorites  = QString::number(Favorites);
    sComments   = QString::number(Comments);
    sLikes      = QString::number(Likes);
    sDisLikes   = QString::number(DisLikes);

    sUploaded   = Uploaded.toString("ddd dd MMM-yyyy");

    /// We want it in the format mm:ss
    sLength     = QString::number(Length/60)+":"+QString::number(Length%60);

    //Should format to something like 2.4, 4.9, 1.7 ...
    sRating     = QString::number( Rating, 'f', 1 );

    if( !Responces.isNull() ) {
        Responces->Author = Author;
        Responces->Title = Title;
        Responces->UrlThumbnail = UrlThumbnail;

        Responces->FixInfo();
    }
    if( !Releated.isNull() ) {
        Releated->Author = Author;
        Releated->Title = Title;
        Releated->UrlThumbnail = UrlThumbnail;

        Releated->FixInfo();
    }
    return;
}

// *************************************************************

VideoFeed::VideoFeed() :
    BaseInfo(),
    Type(FT_Unown)
{
}

VideoFeed::VideoFeed( const VideoFeed& Copy )
    :BaseInfo()
{
    (*this) = Copy;
}

VideoFeed& VideoFeed::operator = ( const VideoFeed& Copy )
{
    Type    = Copy.Type;
    BaseInfo::operator =(Copy);
    Feed<VideoPtr>::operator =(Copy);
    return *this;
}


void VideoFeed::FixInfo()
{
    sSize = QString::number(Size);
    BaseInfo::FixInfo();
    Feed<VideoPtr>::FixInfo();
    return;
}

// *************************************************************

UserInfo::UserInfo() :
    BaseInfo(),
    FirstName(""), LastName(""), FullName(""),
    sJoined(""), sAge(""), sSubscribers(""),
    sChannelsViews(""), sTotalViews(""), sTotalUploads(""),
    Age(0), Subscribers(0), ChannelsViews(0), TotalViews(0), TotalUploads(0),
    Joined(), Playlists(), Favorites(0), Uploads(0)
{
}

UserInfo::UserInfo( const UserInfo &Copy ) :
    BaseInfo()
{
    (*this) = Copy;
}

UserInfo& UserInfo::operator = ( const UserInfo& Copy )
{
    FirstName       = Copy.FirstName;
    LastName        = Copy.LastName;
    FullName        = Copy.FullName;

    sJoined         = Copy.sJoined;
    sAge            = Copy.sAge;
    sSubscribers    = Copy.sSubscribers;
    sChannelsViews  = Copy.sChannelsViews;
    sTotalViews     = Copy.sTotalViews;
    sTotalUploads        = Copy.sTotalUploads;

    Age             = Copy.Age;
    Subscribers     = Copy.Subscribers;
    ChannelsViews   = Copy.ChannelsViews;
    TotalViews      = Copy.TotalViews;
    TotalUploads    = Copy.TotalUploads;

    Joined          = Copy.Joined;

    Playlists       = Copy.Playlists;
    Favorites       = Copy.Favorites;
    Uploads         = Copy.Uploads;

    BaseInfo::operator =( Copy );

    return *this;
}

void UserInfo::FixInfo()
{
    sJoined = Joined.toString("dd/MM-yyyy");

    sAge            = QString::number(Age);
    sSubscribers    = QString::number(Subscribers);
    sChannelsViews  = QString::number(ChannelsViews);
    sTotalViews     = QString::number(TotalViews);
    sTotalUploads   = QString::number(TotalUploads);


    FullName = "";// we don't wan't an extra space...
    if( !FirstName.isEmpty() )
        FullName = FirstName + " ";
    FullName += LastName;

    if( !Favorites.isNull() )
        Favorites->FixInfo();
    if( !Uploads.isNull() )
        Uploads->FixInfo();
    if( !Playlists.isNull() )
        Playlists->FixInfo();
}

// *************************************************************
template< class Type >
Feed<Type>::Feed() :
    Content(),
    ParsedPages(0), HasFullFeed(false),
    NextPageInFeed(""), Size(0), sSize("")
{
}

template< class Type >
Feed<Type>::Feed( const Feed<Type> &Copy )
{
    (*this) = Copy;
}

template< class Type >
Feed<Type>& Feed<Type>::operator = ( const Feed<Type>& Copy )
{
    Content     = Copy.Content;

    ParsedPages = Copy.ParsedPages;
    HasFullFeed = Copy.HasFullFeed;

    NextPageInFeed = Copy.NextPageInFeed;

    Size = Copy.Size;

    sSize = Copy.sSize;

    return *this;
}

template< class Type >
void Feed<Type>::FixInfo()
{
    sSize = QString::number(Size);

    HasFullFeed = !NextPageInFeed.isValid();
}

// *************************************************************


PlaylistFeed::PlaylistFeed()
    :Feed<VideoFeedPtr>(), BaseInfo(), Type(FT_Unown)
{
}

PlaylistFeed::PlaylistFeed( const PlaylistFeed& Copy )
    :Feed<VideoFeedPtr>(), BaseInfo()
{
    (*this) = Copy;
}

PlaylistFeed& PlaylistFeed::operator = ( const PlaylistFeed& Copy )
{
    Type = Copy.Type;

    Feed<VideoFeedPtr>::operator =(Copy);
    BaseInfo::operator =(Copy);
}

void PlaylistFeed::FixInfo()
{
    Feed<VideoFeedPtr>::FixInfo();
    BaseInfo::FixInfo();
}

// *************************************************************
QString GenerateRichText_ListElement( BasePtr Info, QColor TextColor, QColor HeighLightColor )
{
    if( Info.isNull() )
        return "";

    QString RichText = "";

    QString Mall = "<font color=%1>%3</font><font color=%2>%4</font><br><font color=%1>%5</font><font color=%2>%6</font><br><font color=%1>%7</font><font color=%2>%8</font>";
/*
    if( Info->getType() == IT_PlaylistFeed || Info->getType() == IT_ChannelFeed || (Info->getType() == IT_VideoFeed && Info->cast_VideoFeed()->Type == VideoFeed::FT_VideoSearch ) )
    {
        Mall = QString("<font color=%1>%3</font><font color=%2>%4</font><br>").arg(HeighLightColor.name(), TextColor.name());
        RichText = Mall.arg("Search: ", Info->Title);
        switch( Info->getType() ) {
        case( IT_PlaylistFeed ):
            if()
            RichText += Mall.arg("Hits: ",Info->cast_PlaylistFeed()->sSize );
            break;
        case( IT_ChannelFeed ):
            RichText += Mall.arg("Hits: ",Info->cast_ChannelFeed()->sSize );
            break;
        case( IT_VideoFeed ):
            RichText += Mall.arg("Hits: ",Info->cast_VideoFeed()->sSize );
            break;
        default:
            //// should newer happen
            break;
        }
        return RichText;
    }*/
    RichText = Mall.arg(HeighLightColor.name(), TextColor.name() );
    RichText = RichText.arg("Title: ", Info->Title);
    RichText = RichText.arg("Author: ", Info->Author );

    switch( Info->getType() ) {
    case( IT_VideoInfo ):
        RichText = RichText.arg("Lenght: ", Info->cast_VideoInfo()->sLength );
        break;
    case( IT_VideoFeed ):
        switch( Info->cast_VideoFeed()->Type ) {
        case( VideoFeed::FT_Playlist ):
            RichText = RichText.arg( "Size: ", Info->cast_VideoFeed()->sSize );
            break;
        case( VideoFeed::FT_VideoSearch ):
            Mall = QString( "<font color=%1>%3</font><font color=%2>%4</font><br>" ).arg(HeighLightColor.name(), TextColor.name());
            RichText = Mall.arg( "Search: ", Info->Title );
            RichText += Mall.arg("Hits: ", Info->cast_VideoFeed()->sSize);
            break;
        case( VideoFeed::FT_UserFavorites ):
        case( VideoFeed::FT_UserUploads ):
            RichText = RichText.arg("Size: ", Info->cast_VideoFeed()->sSize );
            break;
        case( VideoFeed::FT_VideoReleated ):
            Mall = QString( "<font color=%1>%3</font><font color=%2>%4</font><br>" ).arg(HeighLightColor.name(), TextColor.name());
            RichText = Mall.arg("Releated To: ", Info->Title);
            RichText += "<br>";
            break;
        case( VideoFeed::FT_VideoResponses ):
            Mall = QString( "<font color=%1>%3</font><font color=%2>%4</font><br>" ).arg(HeighLightColor.name(), TextColor.name());
            RichText = Mall.arg("Responses To: ", Info->Title);
            RichText += "<br>";
            break;
        default:
            break;
        }
        break;
    case( IT_UserInfo ):
        RichText = RichText.arg( "Uploads: ", Info->cast_UserInfo()->sTotalUploads );
        break;
    case( IT_PlaylistFeed ):
        switch( Info->cast_PlaylistFeed()->Type ) {
        case( PlaylistFeed::FT_Search ):
            Mall = QString( "<font color=%1>%3</font><font color=%2>%4</font><br>" ).arg(HeighLightColor.name(), TextColor.name());
            RichText = Mall.arg( "Search: ", Info->Title );
            RichText += Mall.arg("Hits: ", Info->cast_PlaylistFeed()->sSize);
            break;
        case( PlaylistFeed::FT_UserPlaylists ):
            Mall = QString( "<font color=%1>%3</font><font color=%2>%4</font><br>" ).arg(HeighLightColor.name(), TextColor.name());
            RichText = Mall.arg( "Search: ", Info->Title );
            RichText += Mall.arg("Hits: ", Info->cast_PlaylistFeed()->sSize);
            break;
        default:
            break;
        }

        break;
    case( IT_ChannelFeed ):
        /// @todo
        break;
    }
    return RichText;
}

QString GenerateRichText_FullInfo( BasePtr Info, QColor TextColor, QColor HeighLightColor )
{
    if( Info.isNull() )
        return "";

    QString RichText = "";

    QString Mall = QString( "<font color=%1>%3</font><font color=%2>%4</font><br>" ).arg( HeighLightColor.name(), TextColor.name() );

    switch( Info->getType() ) {
    case( IT_VideoInfo ): {
        VideoPtr vInfo = Info->cast_VideoInfo();
        RichText += Mall.arg("Title: ", vInfo->Title);
        RichText += Mall.arg("Author: ", vInfo->Author);
        RichText += Mall.arg("Lenght: ", vInfo->sLength);
        RichText += Mall.arg("Views: ", vInfo->sViews);
        RichText += Mall.arg("Upladed: ", vInfo->sUploaded);
        RichText += Mall.arg("Rating: ", vInfo->sRating);
        RichText += Mall.arg("Favorites: ", vInfo->sFavorites);
        RichText += Mall.arg("Likes: ", vInfo->sLikes);
        RichText += Mall.arg("DisLikes: ", vInfo->sDisLikes);
        RichText += Mall.arg("Comments: ", vInfo->sComments);
        RichText += Mall.arg("ID: ", vInfo->ID );
        /// First of we want the description a seperate line, secondly we have to replace all the linebreaks
        RichText += Mall.arg("Description: ", "<br>"+QString(vInfo->Description).replace('\n',"<br>"));

        break;
    }
    case( IT_VideoFeed ): {
        VideoFeedPtr fInfo = Info->cast_VideoFeed();
        RichText += Mall.arg("Title: ", fInfo->Title);
        RichText += Mall.arg("Author: ", fInfo->Author);
        RichText += Mall.arg("Size: ", fInfo->sSize);
        if( fInfo->Type == VideoFeed::FT_Playlist )
            RichText += Mall.arg("ID: ",fInfo->ID );
        /// First of we want the description a seperate line, secondly we have to replace all the linebreaks
        RichText += Mall.arg("Description: ", "<br>"+QString(fInfo->Description).replace('\n', "<br>"));
        break;
    }
    case( IT_UserInfo ): {
        UserPtr uInfo = Info->cast_UserInfo();
        RichText += Mall.arg("Title: ", uInfo->Title );
        RichText += Mall.arg("Author: ", uInfo->Author );
        RichText += Mall.arg("Uploads: ", uInfo->sTotalUploads );
        RichText += Mall.arg("Joined: ", uInfo->sJoined );
        RichText += Mall.arg("Views: ", uInfo->sTotalViews );
        RichText += Mall.arg("Channel Views: ", uInfo->sChannelsViews );
        RichText += Mall.arg("Subscribers: ", uInfo->sSubscribers );
        /// We don't wan't any emphty lines
        if( !uInfo->FullName.isEmpty() )
            RichText += Mall.arg("Name: ", uInfo->FullName );
        if( !uInfo->sAge.isEmpty() )
            RichText += Mall.arg("Age: ", uInfo->sAge );
        /// First of we want the description a seperate line, secondly we have to replace all the linebreaks
        if( !uInfo->Description.isEmpty() )
            RichText += Mall.arg("Description: ", "<br>"+QString(uInfo->Description).replace('\n', "<br>"));
        break;
    }
    }
    return RichText;
}
