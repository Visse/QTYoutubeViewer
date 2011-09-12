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
    QPalette Palette;

    Settings.loadSettings();
    QApplication a(argc, argv);

    a.setApplicationName("Youtube Viewer");

    int PageNr = 12445;

    Palette = QApplication::palette();
    {
        Palette.setColor(QPalette::Base, QColor(80, 80, 80));
        Palette.setColor(QPalette::Window, QColor(120, 120, 120));
    }

    a.setPalette(Palette);


    MainWindow w;
    w.show();


    return a.exec();
}
