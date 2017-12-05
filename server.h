#ifndef MY_SERVER_H
#define MY_SERVER_H
#include <QString>
#include <QWaitCondition>
#include <QMutex>
#include <QMap>
#include <QLinkedList>
#include "login.h"
struct server
{
    int id=-1;
    QString ip;
    short port=0;
    QString username;
    QString password;
    int status=login::FAILED;
};

extern QMutex mute;
extern QWaitCondition waitco;
extern QMap<int,server> servers;
extern QLinkedList<int> notExecTask;
extern int exitth;
#endif
