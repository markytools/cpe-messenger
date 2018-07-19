#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QByteArray>

class Settings
{
public:
    const static QString PROFNAMES_OBJ_ID, APPLICATION_ID, REST_API_KEY;
    const static QByteArray APPLICATION_TYPE;

    Settings();
};

#endif // SETTINGS_H
