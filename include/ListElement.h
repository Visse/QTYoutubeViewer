#ifndef LISTELEMENT_H
#define LISTELEMENT_H

#include <QFrame>
#include <QLabel>

#include "image.h"
#include "ThumbnailsHandler2.h"

#include "Info.h"


class ListElement : public QWidget {
    Q_OBJECT
public:
    explicit ListElement( QWidget *Parent=0 );
    ~ListElement();

    void setSelected( bool sel );
    void setColor( QColor Color );

    void setInfo( BasePtr Info );

    inline BasePtr getInfo() {
        return mInfo;
    }


    Image  *Thumview;
    QLabel *TextLabel;

public slots:
    void UpdateInfo();
    void resizeEvent(QResizeEvent *event);

protected:
    void paintEvent( QPaintEvent *event );

    int mCurAlpha;

    QTimer mTimer;

    QColor mCurColor;
    QColor mSelColor;

    bool mSelected;

    BasePtr mInfo;



};

#endif // LISTELEMENT_H
