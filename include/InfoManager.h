#ifndef INFOMANAGER_H
#define INFOMANAGER_H

#include <QObject>

#include "Info.h"


typedef const uint unqueID;

template< class Type >
unqueID getID( const Type& Element ) {
    return qHash(Element);
}


class InfoManager : public QObject {
    Q_OBJECT
public:
    InfoManager( QObject *parent=0);
    virtual ~InfoManager();


    BasePtr getInfo( unqueID Info );
public slots:
    void addInfo( BasePtr  Info );
    void addInfo( VideoPtr Info ){
        addInfo(Info->cast_BaseInfo());
    }
    void addInfo( VideoFeedPtr  Info ){
        addInfo( Info->cast_BaseInfo() );
    }
    void addInfo( UserPtr  Info ) {
        addInfo( Info->cast_BaseInfo() );
    }
    void addInfo( PlaylistFeedPtr Info ) {
        addInfo(Info->cast_BaseInfo());
    }
    void addInfo( ChannelFeedPtr Info ) {
        addInfo(Info->cast_BaseInfo());
    }

protected:
    typedef QHash< unqueID, BasePtr > InfoHash;

    InfoHash mInfos;

};


extern InfoManager InfoMgr;

#endif // INFOMANAGER_H
