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

#endif // LOG_H
