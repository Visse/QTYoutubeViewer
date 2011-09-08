#ifndef SETTINGMANAGER_H
#define SETTINGMANAGER_H

#include <QObject>


#include <QColor>
#include <QVector2D>
#include <QMap>

#include <stdexcept>

#include "Info.h"

/* A simple class that creates a static 2D Array with elements of Type
   this class is allmost as declearing Type[R][C] - just a bit more safer since it will check if a index is valid before returning it*/
template< class Type, uint R, uint C >
class Array2D {
public:
    Array2D( const Type& Init = Type() ) {
        for( uint row=0; row < R; row++ )
            for( uint coloumn=0; coloumn < C; coloumn++ )
                mElements[row][coloumn] = Init;
    }

    inline Type& at( uint row, uint coloumn ) {
        /*if( row < R && C < coloumn )
            return mElements[row][coloumn];
        else
            throw std::out_of_range("Array2d::at");*/
        return mElements[row][coloumn];
    }

protected:
    Type mElements[R][C];
};

class SettingManager : public QObject
{
    Q_OBJECT
public:
    explicit SettingManager(QObject *parent = 0);

    enum ElementType {
        ET_Video,
        ET_Playlist,
        ET_User,

        ET_VideoReleated,
        ET_VideoResponses,
        ET_VideoComments,

        ET_UserUploads,
        ET_UserFavorites,
        ET_UserPlaylists,

        ET_VideoSearch,
        ET_PlaylistSearch,
        ET_ChannelSearch


    };

    enum ColorType {
        CT_Element,
        CT_NormalText,
        CT_Heighlighted
    };


    inline QColor getColor( ElementType Element, ColorType Type ) {
        return mColors.at( Element, Type );
    }

    QColor getColor( BasePtr Info, ColorType Type );

    void loadSettings( QString File = "Settings.cfg" );

signals:

public slots:

protected:
    typedef Array2D<QColor,12,3> ColorMap;


    ColorMap mColors;

};



extern SettingManager Settings;

#endif // SETTINGMANAGER_H
