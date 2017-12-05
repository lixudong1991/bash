#ifndef TASKTHREAD_H
#define TASKTHREAD_H
#include <QThread>
#include <QTcpSocket>
#include "server.h"
class TaskThread : public QThread
{
    Q_OBJECT
public:
    TaskThread();
    ~TaskThread();
    void setCommond(QString);
    void setTid(int id);
signals:
    void tasend();
protected:
    void run();
private:
    void runCommond(int,QTcpSocket &);
    QString cmd;
    int tid;
};
#endif // TASKTHREAD_H
