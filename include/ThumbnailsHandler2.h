#ifndef THUMBNAILSHANDLER2_H
#define THUMBNAILSHANDLER2_H

#include <QObject>
#include <QtNetwork>

#include "image.h"
#include "Log.h"

/*A dummy class that just send a signal then the image is recived*/
class ImageReciver : public QObject {
Q_OBJECT
public:
    ImageReciver( QObject *parent = 0 )
        :QObject(parent) {}

public slots:
    void ImageRecived( const QPixmap *Image )
    {
        ImageLoaded(Image);
    }

signals:
    void ImageLoaded( const QPixmap *Image );
};


class ThumbnailsHandler2 : public QObject
{
    Q_OBJECT
public:
    explicit ThumbnailsHandler2(QObject *parent = 0);

    QNetworkAccessManager *NetworkMgr;

    void Download( const QUrl& Url, Image*I );


protected:
    void UpdateFileCache();
    void DownloadNext();

    inline QString GenCacheName( QString Url );

    typedef QCache< QString, QPixmap > CacheMap;
    //typedef std::map< std::string, QPixmap*> CacheMap;
    typedef QPair< QString, ImageReciver * > ImageQueElement;
    typedef QQueue< ImageQueElement > ImageQue;
    typedef QHash< QString, ImageReciver * > ImageRecive;


    CacheMap mCache;


    ImageQue mQue;
    ImageRecive mRecive;

signals:

public slots:

    void Update();

    void ImageRecived();
    void NetworkError();

};

extern ThumbnailsHandler2 Thumnails;

#endif // THUMBNAILSHANDLER2_H
