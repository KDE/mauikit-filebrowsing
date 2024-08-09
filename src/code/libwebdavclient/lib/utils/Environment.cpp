#include <QString>

#include "Environment.hpp"

QString Environment::get(QString key)
{
    char *val = getenv(key.toStdString().c_str());
    return val == nullptr ? QString(QStringLiteral("")) : QString::fromUtf8(val);
}
