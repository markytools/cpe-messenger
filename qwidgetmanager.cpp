#include "qwidgetmanager.h"

QWidgetManager::QWidgetManager()
{
    widgets = new vector<QWidget*>();
}

void QWidgetManager::showWidget(Form form)
{
    for (int i = 0; i < widgets->size(); i++){
        QWidget *widget = widgets->at(i);
        if (widgetMap[widget] == form) widget->show();
        else widget->hide();
    }
}

void QWidgetManager::addWidget(QWidget *widget, Form form)
{
    widgetMap[widget] = form;
    widgets->push_back(widget);
}
void QWidgetManager::setUserInfo(UserInfo *value)
{
    userInfo = value;
}
UserInfo *QWidgetManager::getUserInfo() const
{
    return userInfo;
}



