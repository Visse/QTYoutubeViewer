#include "Log.h"

#include <QDate>
#include <QTime>

Log::Log( const QString& File )
    :mFile(File), Flush(0)
{
    mFile.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text );

    QString Text = "";
    Text += "********************************\nDate: ";
    Text += QDate::currentDate().toString("dd/MM-yyyy");
    Text += " - ";
    Text += QTime::currentTime().toString();
    Text += "\n********************************\n";

    mFile.write( qPrintable(Text) );

    mFile.flush();
}

Log::~Log()
{
    mFile.close();
}

void Log::LogMessage(const QString &mess)
{
    QString Text = "";
    Text += QTime::currentTime().toString("HH-mm:ss" );
    Text += '\t';
    Text += mess;
    Text += '\n';
    mFile.write( qPrintable(Text) );

   // if( ++Flush >= 10 )
    {
        mFile.flush();
        Flush = 0;
    }

}

void Log::LogError( const char *File, const char *Func, int line, const QString &Messege )
{
    QString Text = "";
    Text += QTime::currentTime().toString("HH-mm:ss" );
    Text += "\tERROR[";
    Text += Func;
    Text += ':';
    Text += File;
    Text += ':';
    Text += QString::number(line);
    Text += "] - ";
    Text += Messege;
    Text += '\n';
    mFile.write( qPrintable(Text) );

    mFile.flush();
    Flush = 0;
}

void Log::LogWarning( const char *File, const char *Func, int line, const QString &Messege )
{
    QString Text = "";
    Text += "WARNING[";
    Text += Func;
    Text += ':';
    Text += File;
    Text += ':';
    Text += QString::number(line);
    Text += "] - ";
    Text += Messege;

    LogMessage( Text );
}

