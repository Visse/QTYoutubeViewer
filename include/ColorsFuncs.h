#ifndef COLORSFUNCS_H
#define COLORSFUNCS_H

#include <QColor>

QColor AlphaBlend( const QColor& C1, const QColor& C2 ) {
    return QColor(
                   (int) (C1.alphaF() * C1.red()   + C2.alphaF() * (1 - C1.alphaF()) * C2.red() ),
                   (int) (C1.alphaF() * C1.green() + C2.alphaF() * (1 - C1.alphaF()) * C2.green()),
                   (int) (C1.alphaF() * C1.blue()  + C2.alphaF() * (1 - C1.alphaF()) * C2.blue()),
                   (int) (255 * (C1.alphaF() + C2.alphaF() * (1 - C1.alphaF())) )
                 );
}


#endif // COLORSFUNCS_H
