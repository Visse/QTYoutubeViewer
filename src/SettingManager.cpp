#include "SettingManager.h"

#include <QFile>
#include "Log.h"

SettingManager::SettingManager(QObject *parent) :
    QObject(parent)//,
    //VideoElement        (0x00, 0xFF, 0x00), VideoText      (0x00, 0x00, 0x00), VideoHeigh       (0x00, 0x12, 0x9F),
    //PlaylistElement     (0x22, 0x22, 0xFF), PlaylistText   (0x00, 0x00, 0x00), PlaylistHeigh    (0xFF, 0xFF, 0x00),
    //UserElement         (0xFF, 0x00, 0x00), UserText       (0x00, 0x00, 0x00), UserHeigh        (0x00, 0xFF, 0xFF),
    //VideoSearchElement  (0x55, 0xFF, 0x55), VideoSearchText(0x00, 0x00, 0x00), VideoSearchHeigh (0x00, 0x12, 0x9f)
{
}


QColor SettingManager::getColor( BasePtr Info, SettingManager::ColorType Type ) {
    if( Info.isNull() )
        return QColor();

    switch( Info->getType() ) {
    case( IT_VideoInfo ):
        return getColor( ET_Video, Type );
    case( IT_VideoFeed ):
        switch( Info->cast_VideoFeed()->Type ) {
        case( VideoFeed::FT_Playlist ):
            return getColor( ET_Playlist, Type );
        case( VideoFeed::FT_UserFavorites ):
            return getColor( ET_UserFavorites, Type );
        case( VideoFeed::FT_UserUploads ):
            return getColor( ET_UserUploads, Type );
        case( VideoFeed::FT_VideoReleated ):
            return getColor( ET_VideoReleated, Type );
        case( VideoFeed::FT_VideoResponses ):
            return getColor( ET_VideoResponses, Type );
        case( VideoFeed::FT_VideoSearch ):
            return getColor( ET_VideoSearch, Type );
        default:
            /// Should never happen( VideoFeed is of unown type
            break;
        }
        break;
    case( IT_UserInfo ):
        return getColor( ET_User, Type );
    case( IT_PlaylistFeed ):
        return getColor( ET_PlaylistSearch, Type );
    case( IT_ChannelFeed ):
        return getColor( ET_ChannelSearch, Type );
    default:
        /// Should never happen
        break;
    }
    return QColor();
}

void SettingManager::loadSettings( QString FilePath ) {
    QFile File(FilePath);
    File.open(QFile::ReadOnly);
    if( !File.isOpen() ) {
        Error( "File could not be open! FilePath["+FilePath+"]" );

        for( int i=0; i < sizeof(ElementType); i++ ) {
            mColors.at(i, CT_Element ).setNamedColor( "#000" );
            mColors.at(i, CT_Heighlighted ).setNamedColor( "#0FF" );
            mColors.at(i, CT_NormalText ).setNamedColor( "#FFF" );
        }

        return;
    }

    Array2D<bool,12,3> Assigment(false);

    QString Content = File.readAll();
    QStringList Settings = Content.split('\n', QString::SkipEmptyParts );

    QStringList::iterator iter;
    for( iter = Settings.begin(); iter != Settings.end(); iter++ ) {
        QString Name, Value;
        QStringList SplitSett = iter->split(':');
        if( SplitSett.isEmpty() )
            continue;
        if( SplitSett.size() < 2 ) {
            Error( "Setting is to small!" );
            continue;
        }
        Name = SplitSett.at(0).simplified();
        Value = SplitSett.at(1).simplified();


 /* This define make my life a lot easier, so DON'T TOUCH!!
    this is that it does on a more read-friendly way:

    if(Name.compare( String, Qt::CaseInsensitive ) == 0 ) {
        bool & A = Assigment.at( ET, CT );
        if( A )
            Warning( QString(String) + " has allready been assigned! - ignoring value[" + Value + "]" );
        else {
            A=true;
            mColors.at( ET, CT ).setNamedColor( Value );
        }
        continue;
    }
*/
#define Compare( String, ET, CT )if(Name.compare(String,Qt::CaseInsensitive)==0){bool&A=Assigment.at(ET,CT);if(A)Warning(QString(String)+" has allready been assigned! - ignoring value["+Value+"]");else{A=true;mColors.at(ET,CT).setNamedColor(Value);}continue;}

        Compare( "Video-Element", ET_Video, CT_Element );
        Compare( "Video-NormalText", ET_Video, CT_NormalText );
        Compare( "Video-HeighLight", ET_Video, CT_Heighlighted );

        Compare( "Playlist-Element", ET_Playlist, CT_Element );
        Compare( "Playlist-NormalText", ET_Playlist, CT_NormalText );
        Compare( "Playlist-HeighLight", ET_Playlist, CT_Heighlighted );

        Compare( "User-Element", ET_User, CT_Element );
        Compare( "User-NormalText", ET_User, CT_NormalText );
        Compare( "User-HeighLight", ET_User, CT_Heighlighted );

        Compare( "VideoReleated-Element", ET_VideoReleated, CT_Element );
        Compare( "VideoReleated-NormalText", ET_VideoReleated, CT_NormalText );
        Compare( "VideoReleated-HeighLight", ET_VideoReleated, CT_Heighlighted );

        Compare( "VideoResponses-Element", ET_VideoResponses, CT_Element );
        Compare( "VideoResponses-NormalText", ET_VideoResponses, CT_NormalText );
        Compare( "VideoResponses-HeighLight", ET_VideoResponses, CT_Heighlighted );

        Compare( "VideoComments-Element", ET_VideoComments, CT_Element );
        Compare( "VideoComments-NormalText", ET_VideoComments, CT_NormalText );
        Compare( "VideoComments-HeighLight", ET_VideoComments, CT_Heighlighted );

        Compare( "UserUploads-Element", ET_UserUploads, CT_Element );
        Compare( "UserUploads-NormalText", ET_UserUploads, CT_NormalText );
        Compare( "UserUploads-HeighLight", ET_UserUploads, CT_Heighlighted );

        Compare( "UserFavorites-Element", ET_UserFavorites, CT_Element );
        Compare( "UserFavorites-NormalText", ET_UserFavorites, CT_NormalText );
        Compare( "UserFavorites-HeighLight", ET_UserFavorites, CT_Heighlighted );

        Compare( "UserPlaylists-Element", ET_UserPlaylists, CT_Element );
        Compare( "UserPlaylists-NormalText", ET_UserPlaylists, CT_NormalText );
        Compare( "UserPlaylists-HeighLight", ET_UserPlaylists, CT_Heighlighted );

        Compare( "VideoSearch-Element", ET_VideoSearch, CT_Element );
        Compare( "VideoSearch-NormalText", ET_VideoSearch, CT_NormalText );
        Compare( "VideoSearch-HeighLight", ET_VideoSearch, CT_Heighlighted );

        Compare( "PlaylistSearch-Element", ET_PlaylistSearch, CT_Element );
        Compare( "PlaylistSearch-NormalText", ET_PlaylistSearch, CT_NormalText );
        Compare( "PlaylistSearch-HeighLight", ET_PlaylistSearch, CT_Heighlighted );

        Compare( "ChannelSearch-Element", ET_ChannelSearch, CT_Element );
        Compare( "ChannelSearch-NormalText", ET_ChannelSearch, CT_NormalText );
        Compare( "ChannelSearch-HeighLight", ET_ChannelSearch, CT_Heighlighted );

#undef Compare
    }

    /// Check to see if all settings have been assigned a value
#define Check( ET, CT ) if( !Assigment.at(ET,CT) ) Warning( QString(#ET)+':'+#CT+" - Have not been assign!" );
    Check( ET_Video, CT_Element );
    Check( ET_Video, CT_NormalText );
    Check( ET_Video, CT_Heighlighted );

    Check( ET_Playlist, CT_Element );
    Check( ET_Playlist, CT_NormalText );
    Check( ET_Playlist, CT_Heighlighted );

    Check( ET_User, CT_Element );
    Check( ET_User, CT_NormalText );
    Check( ET_User, CT_Heighlighted );

    Check( ET_VideoReleated, CT_Element );
    Check( ET_VideoReleated, CT_NormalText );
    Check( ET_VideoReleated, CT_Heighlighted );

    Check( ET_VideoResponses, CT_Element );
    Check( ET_VideoResponses, CT_NormalText );
    Check( ET_VideoResponses, CT_Heighlighted );

    Check( ET_VideoComments, CT_Element );
    Check( ET_VideoComments, CT_NormalText );
    Check( ET_VideoComments, CT_Heighlighted );

    Check( ET_UserUploads, CT_Element );
    Check( ET_UserUploads, CT_NormalText );
    Check( ET_UserUploads, CT_Heighlighted );

    Check( ET_UserFavorites, CT_Element );
    Check( ET_UserFavorites, CT_NormalText );
    Check( ET_UserFavorites, CT_Heighlighted );

    Check( ET_UserPlaylists, CT_Element );
    Check( ET_UserPlaylists, CT_NormalText );
    Check( ET_UserPlaylists, CT_Heighlighted );

    Check( ET_VideoSearch, CT_Element );
    Check( ET_VideoSearch, CT_NormalText );
    Check( ET_VideoSearch, CT_Heighlighted );

    Check( ET_PlaylistSearch, CT_Element );
    Check( ET_PlaylistSearch, CT_NormalText );
    Check( ET_PlaylistSearch, CT_Heighlighted );

    Check( ET_ChannelSearch, CT_Element );
    Check( ET_ChannelSearch, CT_NormalText );
    Check( ET_ChannelSearch, CT_Heighlighted );
#undef Check
}
