#include "argument.h"
#include <QtDebug>

using namespace std;

int main()
{
    QString alphabet = "abc!@#";

    Argument<QString, QChar, QString, QString> a(alphabet, QString("a"), QString("cc"));

    qDebug() << a++;
    qDebug() << a;

    return 0;
}
