#ifndef VIDEOCONTROLLER_H
#define VIDEOCONTROLLER_H

#include <QFrame>

#include <QGridLayout>
#include <SeekSlider>
#include <VolumeSlider>
#include <QToolButton>

/**
  * A simple widget that basicly just wraps some controlls
  */
class VideoController : public QFrame
{
    Q_OBJECT
public:
    explicit VideoController(QWidget *parent = 0);

    virtual ~VideoController();

    void setMediaObject( Phonon::MediaObject *MediaObject, Phonon::AudioOutput *Audio );
signals:

protected slots:
    void stateChanged( Phonon::State NewState );


    void PlayPauseClicked();
    void PreviusClicked();
    void NextClicked();
private:    
    void setUp();

    QGridLayout *mLayout;
    Phonon::SeekSlider *mSeekSlider;
    Phonon::VolumeSlider *mVolumeSlider;
    QToolButton *mPlayPauseButton;
    QToolButton *mPreviusButton;
    QToolButton *mNextButton;

    QIcon mPlayIcon, mPauseIcon, mPreviusIcon, mNextIcon;


    Phonon::MediaObject *mMedia;
    Phonon::AudioOutput *mAudio;
};

#endif // VIDEOCONTROLLER_H
