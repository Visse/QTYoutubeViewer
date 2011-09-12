#ifndef LOG_H
#define LOG_H

#include <QFile>
#include <QString>



class Log {
public:
    Log( const QString &fileName );
    virtual ~Log();

    void LogMessage( const QString& mess );

    void LogError( const char* File, const char* Func, int line, const QString& Messege );
    void LogWarning( const char* File, const char* Func, int line, const QString& Messege );

protected:

    QFile mFile;

    unsigned char Flush;
};

extern Log mLog;


#define Error( Mes ) mLog.LogError( __FILE__, __FUNCTION__, __LINE__, Mes )
#define Warning( Mes ) mLog.LogWarning( __FILE__, __FUNCTION__, __LINE__, Mes )

// Makes it a litle more cleaner to write usefull error message,
// Well, I'm still a NOOB on macros( it took me alot of googling before this code worked :P
// Usage: Message = an error string with '%X' there X is the argument nr, for example:
//  Error2( "Something is realy wrong now! Url[%1]", "http://google.se" );
// this will print the string 'Something is realy wrong now! Url[http://google.se]' to the log
// the arguments MUST!!! be strings if you want to pass something else you MUST convert it,
// for example:
//  Error2( "This is a string: [%1], but it is also a number", QString::number(123) );
// This will print the string 'This is a string: [123], but it is also a number' to the log
#define Error2( Message, args...) Error(QString(Message).arg(args))

#endif // LOG_H
