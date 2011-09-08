#include "ThumbnailsHandler2.h"
#include <iostream>

#include <QMessageBox>



ThumbnailsHandler2::ThumbnailsHandler2(QObject *parent) :
    QObject(parent), NetworkMgr(0)
{
}

void ThumbnailsHandler2::Update()
{
    if( NetworkMgr )
    {
        while( mRecive.size() < 5 && !mQue.empty() ) {
            ImageQueElement Elem = mQue.dequeue();
            QNetworkReply *reply = NetworkMgr->get( QNetworkRequest(QUrl(Elem.first)) );

            connect( reply, SIGNAL(finished()), SLOT(ImageRecived()) );
            connect( reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(NetworkError()) );

            mRecive.insert( Elem.first, Elem.second );
        }
    }
}

void ThumbnailsHandler2::ImageRecived()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

    ImageRecive::iterator iter = mRecive.find( reply->url().toString() );
    if( iter == mRecive.end() )
    {
        Error( "Url not in recive!" );
        reply->deleteLater();
        return;
    }
    QPixmap *Image = new QPixmap;
    Image->loadFromData( reply->readAll() );

    if( !Image->isNull() )
    {
        QFile *File = new QFile( GenCacheName(reply->url().toString()) );
        File->open( QIODevice::WriteOnly | QIODevice::Truncate );

        Image->save(File,"jpg");
        File->close();

        mCache.insert(reply->url().toString(), Image );

        iter.value()->ImageRecived( Image );
        delete iter.value();

    }
    else
    {
        Error( "Image is null!" );
        delete Image;
    }
    mRecive.erase(iter);

    reply->deleteLater();

    Update();

}

void ThumbnailsHandler2::NetworkError()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

    Error( "Network error: "+reply->errorString() );
}

void ThumbnailsHandler2::Download( const QUrl &U, Image *I )
{
    QString Url = U.toString();

    if( mCache.contains(Url) )
    //if( mCache.find(Url.toStdString()) != mCache.end() )
    {
        QPixmap *Pixmap = mCache.object(Url);


        Pixmap->size();
        I->setImage(Pixmap);

        return;
    }
    ImageReciver *resive = mRecive.value(Url,0);
    if( resive )
    {
        connect( resive, SIGNAL(ImageLoaded(const QPixmap*)), I, SLOT(setImage(const QPixmap*)) );
        return;
    }
    QString CacheName = GenCacheName(Url);
    if( QFile::exists(CacheName) )
    {
        QFile ImageFile( CacheName );
        ImageFile.open( QIODevice::ReadOnly );

        QPixmap *Pixmap = new QPixmap();
        Pixmap->loadFromData(ImageFile.readAll());
        ImageFile.close();

        I->setImage(Pixmap);

        mCache.insert( Url, Pixmap );
        //mCache.insert(std::make_pair(Url.toStdString(),Pixmap));
        return;
    }

    ImageQue::iterator iter=mQue.begin();
    for( ; iter != mQue.end(); iter++ )
    {
        if( iter->first == Url )
        {
            connect( iter->second, SIGNAL(ImageLoaded(const QPixmap*)), I, SLOT(setImage(const QPixmap*)) );
            return;
        }
    }

    resive = new ImageReciver;

    connect( resive, SIGNAL(ImageLoaded(const QPixmap*)), I, SLOT(setImage(const QPixmap*)) );

    ImageQueElement Elem( Url, resive );
    mQue.enqueue(Elem);
    Update();
}

QString ThumbnailsHandler2::GenCacheName(QString Url)
{
    QString Name = "Cache/"+QString::number( qHash(Url) )+".jpg.cache";
    return Name;
}
