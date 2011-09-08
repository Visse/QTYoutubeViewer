#ifndef IMAGE_H
#define IMAGE_H

#include <QtGui/QLabel>
#include <QtNetwork>
#include <QImage>

class Image : public QLabel
{
    Q_OBJECT

public:
    Image(QWidget *parent = 0);

    void setImage( const QString& File );

    /*seting image from the intenet*/
    void setImage( QNetworkReply *reply );


    virtual int heightForWidth(int width) const;//{return (float)width*0.75;}
    virtual QSize sizeHint() const;//{return QSize(width(), heightForWidth(width()));}
    virtual QSize minimumSizeHint() const{return sizeHint();}

public slots:
    /**/
    void ImageLoaded();

    void setImage( const QPixmap *Image );

    void resize(const QSize &);


protected:
    QNetworkReply *mCurReply;
    QPixmap mImage;
};

#endif
