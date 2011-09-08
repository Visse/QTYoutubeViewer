#include "image.h"

#include "iostream"
#include "fstream"

Image::Image(QWidget *parent) :
    QLabel(parent), mImage()
{
    //setScaledContents(true);
}

void Image::setImage( const QString& File )
{

}

void Image::setImage( QNetworkReply *reply )
{
    if( reply )
        connect( reply, SIGNAL(finished()), SLOT(ImageLoaded()) );

    mCurReply = reply;
}

void Image::setImage( const QPixmap *Image)
{
    if( Image && !Image->isNull() )
    {
        //setMinimumSize(width(),heightForWidth(width()));
        //resize(minimumSize());
        //updateGeometry();
        //update();

        QPixmap Pixmap = Image->scaled(size(), Qt::KeepAspectRatio );
        setPixmap( Pixmap );
    }
    else
        setPixmap( QPixmap(QString::fromUtf8(":/Resources/image-missing.jpg")).scaled(this->size()) );
}

void Image::ImageLoaded()
{
    if( mCurReply && mCurReply->isFinished() ) {

        mImage.loadFromData( mCurReply->readAll() );

        mImage = mImage.scaled( this->size(), Qt::KeepAspectRatio );
    }

    setPixmap( mImage);
}


int Image::heightForWidth(int width) const
{
    if( !mImage.isNull() )
    {
        return (int)(float)mImage.height()/(float)mImage.width()*(float)width;
    }
    return 0;
}

QSize Image::sizeHint() const
{
    return QSize(width(), heightForWidth(width()));
}


void Image::resize(const QSize &Size)
{
    if( !mImage.isNull() )
        setPixmap( mImage.scaled(Size, Qt::KeepAspectRatio) );
}
