#include <QtGui/QApplication>
#include "MainWindow.h"

#include "Log.h"
#include "ThumbnailsHandler2.h"
#include "SettingManager.h"

Log mLog( "YoutubeViewer.log" );

ThumbnailsHandler2 Thumnails;
SettingManager Settings;

int main(int argc, char *argv[])
{

    Settings.loadSettings();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    return a.exec();
}
