#include "VideoController.h"

#include <Phonon/MediaObject>

#include <Phonon/Path>

VideoController::VideoController(QWidget *parent) :
    QFrame(parent),
    mLayout(0),
    mSeekSlider(0),
    mVolumeSlider(0),
    mPlayPauseButton(0),
    mPreviusButton(0),
    mNextButton(0),
    mPlayIcon(),
    mPauseIcon(),
    mPreviusIcon(),
    mNextIcon(),
    mMedia(0),
    mAudio(0)
{
    mPlayIcon.addFile(":/VideoController/Play.png");
    mPauseIcon.addFile(":/VideoController/Pause.png");
    mPreviusIcon.addFile(":/VideoController/Previus.png");
    mNextIcon.addFile(":/VideoController/Next.png");

    setAutoFillBackground(true);

    QPalette P = palette();
    P.setColor( QPalette::Background, Qt::black );
    setPalette(P);

    setUp();

    {
        connect( mPlayPauseButton, SIGNAL(clicked()), SLOT(PlayPauseClicked()) );
        connect( mPreviusButton, SIGNAL(clicked()), SLOT(PreviusClicked()) );
        connect( mNextButton, SIGNAL(clicked()), SLOT(NextClicked()) );
    }

}

VideoController::~VideoController() {
    delete mLayout;
        mLayout = 0;
    delete mSeekSlider;
        mSeekSlider = 0;
    delete mVolumeSlider;
        mVolumeSlider = 0;
    delete mPlayPauseButton;
        mPlayPauseButton = 0;
    delete mPreviusButton;
        mPreviusButton = 0;
    delete mNextButton;
        mNextButton = 0;
}

void VideoController::setMediaObject( Phonon::MediaObject *MediaObject, Phonon::AudioOutput *Audio ) {
    mMedia = MediaObject;
    mAudio = Audio;

    mSeekSlider->setMediaObject(mMedia);
    mVolumeSlider->setAudioOutput(mAudio);


    //mVolumeSlider->setAudioOutput( mMedia);

    connect( mMedia, SIGNAL(stateChanged(Phonon::State,Phonon::State)), SLOT(stateChanged(Phonon::State)) );
}

void VideoController::stateChanged(Phonon::State NewState) {
    switch( NewState ) {
    case( Phonon::PlayingState ):
        mPlayPauseButton->setIcon(mPauseIcon);
        break;
    case( Phonon::PausedState ):
    case( Phonon::StoppedState ):
        mPlayPauseButton->setIcon(mPlayIcon);
        break;
    default:
        break;
    }
}

void VideoController::PlayPauseClicked() {
    if( !mMedia )
        return;
    switch( mMedia->state() ) {
    case( Phonon::PlayingState ):
        mMedia->pause();
        break;
    case( Phonon::PausedState ):
    case( Phonon::StoppedState ):
        mMedia->play();
        break;
    }
}

#include "VideoStreamer.h"
void VideoController::PreviusClicked() {
    if( !mMedia )
        return;

    if( mMedia->isSeekable() )
        mMedia->seek(0);
}

void VideoController::NextClicked() {
    if( !mMedia )
        return;
    /// @todo
}


void VideoController::setUp() {

    if( objectName().isEmpty() )
        setObjectName( "VideoController");
    resize(500,40);

    {
        mLayout = new QGridLayout(this);
        mLayout->setSpacing(1);
        mLayout->setContentsMargins( 1,1,1,1 );
        mLayout->setObjectName("GridLayout");
    }
    {
        mSeekSlider = new Phonon::SeekSlider(this);

        QSizePolicy SizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
        SizePolicy.setHorizontalStretch(4);
        SizePolicy.setVerticalStretch(0);
        SizePolicy.setHeightForWidth(mSeekSlider->sizePolicy().hasHeightForWidth());

        mSeekSlider->setObjectName("SeekSlider");
        mSeekSlider->setSizePolicy(SizePolicy);
        mSeekSlider->setIconVisible(true);
        mSeekSlider->setOrientation(Qt::Horizontal);

        mLayout->addWidget(mSeekSlider, 0, 2, 1, 1);
    }
    {
        mVolumeSlider = new Phonon::VolumeSlider(this);

        QSizePolicy SizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
        SizePolicy.setHorizontalStretch(1);
        SizePolicy.setVerticalStretch(0);
        SizePolicy.setHeightForWidth(mVolumeSlider->sizePolicy().hasHeightForWidth());

        mVolumeSlider->setObjectName("VolumeSlider");
        mVolumeSlider->setSizePolicy(SizePolicy);
        mVolumeSlider->setMuteVisible(true);

        mLayout->addWidget(mVolumeSlider, 0, 3, 1, 1);
    }
    {
         mPlayPauseButton = new QToolButton(this);
         mPlayPauseButton->setObjectName("PlayPauseButton");
         mPlayPauseButton->setFixedSize(QSize(20,20));
         mPlayPauseButton->setIcon(mPlayIcon);
         mPlayPauseButton->setIconSize(QSize(16,16));
         mPlayPauseButton->setAutoRaise(true);

         mLayout->addWidget(mPlayPauseButton, 0, 0, 1, 1);
    }
    {
        mPreviusButton = new QToolButton(this);
        mPreviusButton->setObjectName("PreviusButton");
        mPreviusButton->setFixedSize(QSize(35,20));
        mPreviusButton->setIcon(mPreviusIcon);
        mPreviusButton->setIconSize(QSize(30,16));
        mPreviusButton->setAutoRaise(true);

        mLayout->addWidget(mPreviusButton, 0, 1, 1,1);
    }
    {
        mNextButton = new QToolButton(this);
        mNextButton->setObjectName("NextButton");
        mNextButton->setFixedSize(QSize(35,20));
        mNextButton->setIcon(mNextIcon);
        mNextButton->setIconSize(QSize(30, 16));
        mNextButton->setAutoRaise(true);

        mLayout->addWidget(mNextButton, 0, 4, 1, 1);
    }
}
