#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QProgressDialog>
#include <QCloseEvent>
#include "server.h"
#include "taskthread.h"
class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = 0);
    ~Widget();
private slots:
   void importFile();
   void executTask();
   void setTableDate();
private:
    QTableWidget* table;
    QLineEdit *commond,*port;
    QLabel *la,*lb,*lc;
    QGroupBox* box;
    QPushButton* bta,*btb;
    TaskThread *threads[MAXTHREAD];
    void initPanel(); 
    void initThread();
    void initTable();
    int noextask=0;
    int texi=MAXTHREAD;
};
#endif // WIDGET_H
