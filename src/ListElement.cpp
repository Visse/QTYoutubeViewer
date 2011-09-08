#include "ListElement.h"

#include "Parser3.h"

#include <QApplication>
#include <QPainter>
#include <QResizeEvent>

#include "SettingManager.h"


/**
  * Blend 2 colors with the weight B
  * B is a value between 0 and 1
  * 0 = returns C1
  * 1 = returns C2
  * 0.5 = returns a mix of the two, exm C1(255,255,255) C2(0,0,0) return: (127,127,127)
*/

inline QColor Blend( QColor C1, QColor C2, float B=0.5f)
{
    return QColor::fromRgb(
                (C1.red()*B)+(C2.red()*(1-B)),
                (C1.green()*B)+(C2.green()*(1-B)),
                (C1.blue()*B)+(C2.blue()*(1-B)),
                (C1.alpha()*B)+(C2.alpha()*(1-B))
                );
}


ListElement::ListElement( QWidget* parent )
    : QWidget(parent),
      Thumview( new Image(this)),
      TextLabel( new QLabel(this) ),
      mCurAlpha(0),
      mCurColor(),
      mSelColor(),
      mSelected(true),
      mInfo(0),
      mTimer(this)
{
    setGeometry( QRect( QPoint(0,0), QPoint(300,64) ) );
    Thumview->setGeometry( QRect( QPoint(0,0), QPoint(80,64) ) );
    TextLabel->setGeometry( QRect( QPoint(85,0), QPoint(300,64) ) );

    Thumview->setImage((QPixmap*)(void*) 0);

    setAutoFillBackground(true);

    mCurAlpha = 255;

    mTimer.setInterval(33);
    mTimer.start();
    connect( &mTimer, SIGNAL(timeout()), SLOT(update()) );

}

ListElement::~ListElement()
{
    Thumview->deleteLater();
    TextLabel->deleteLater();
}

void ListElement::setColor( QColor Color )
{

    //QPalette p = palette();
    //p.setColor( backgroundRole(), Color );
   // setPalette(p);

    mCurColor = Color;
    update();
}

void ListElement::setSelected(bool sel)
{
    if( sel == mSelected )
        return;

    mSelected = sel;

    if( sel )
    {
        setColor(mSelColor);
    }
    else
    {
        mSelColor = mCurColor;
        mCurColor.setAlpha( mCurColor.alpha()/2 );
        setColor(mCurColor);
    }
}

void ListElement::paintEvent(QPaintEvent *event)
{
    QPainter Painter(this);
    QWidget::paintEvent(event);

    if( mCurAlpha < 0 )
    {
        Painter.fillRect(rect(), mCurColor );
        mTimer.stop();
        return;
    }

    QColor From( palette().color(QPalette::Background) );
    From.setAlpha(mCurColor.alpha());
    QColor color =  Blend(From, mCurColor, float(mCurAlpha)/255.f );
    //color.setAlpha(curAlpha);

    Painter.fillRect(rect(), color );

    mCurAlpha -= 255.f * mTimer.interval() / 500.f;
    if( mCurAlpha <= 0 ) {
        mTimer.stop();
    }
}

void ListElement::setInfo(BasePtr Info) {
    if( !mInfo.isNull() ) {
        mInfo->disconnect( this, SLOT(UpdateInfo()) );
    }
    mInfo = Info;
    if( !mInfo.isNull() ) {
       connect( mInfo.data(), SIGNAL(InfoUpdated()), SLOT(UpdateInfo()) );
    }
    UpdateInfo();
}

void ListElement::UpdateInfo() {
    if( !mInfo.isNull() )
    {
        setColor( Settings.getColor(mInfo, SettingManager::CT_Element) );
        TextLabel->setText( GenerateRichText_ListElement(mInfo,
                                                         Settings.getColor(mInfo, SettingManager::CT_NormalText),
                                                         Settings.getColor(mInfo, SettingManager::CT_Heighlighted)) );
        setToolTip( TextLabel->text() );
        Thumnails.Download( mInfo->UrlThumbnail, Thumview );
    }
}

void ListElement::resizeEvent(QResizeEvent *event)
{
    TextLabel->setGeometry( QRect( QPoint(85,0), QPoint(event->size().width(),64)) );

    QWidget::resizeEvent(event);
}
