#include "widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPalette>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QMapIterator>
#include <QTableWidgetItem>
#include <QProgressDialog>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    initPanel();
    initThread();
    connect(bta,SIGNAL(clicked()),this,SLOT(importFile()));
    connect(btb,SIGNAL(clicked()),this,SLOT(executTask()));
}

void Widget::initPanel()
{
    table=new QTableWidget;
    commond=new QLineEdit;
    port=new QLineEdit;
    port->setFixedWidth(70);
    la=new QLabel(tr("欲执行命令:"));
     la->setEnabled(false);
    lb=new QLabel(tr("执行端口:"));
    lc=new QLabel(tr("导入文本内带有端口就不需要填写"));
    box=new QGroupBox(tr("导入账号"));
    bta=new QPushButton(tr("导入账号"));
    btb=new QPushButton(tr("开始执行"));
    btb->setEnabled(false);
    setFixedWidth(600);


    QStringList  HStrList;
    HStrList.push_back(tr("ID"));
    HStrList.push_back(tr("服务器地址"));
    HStrList.push_back(tr("端口"));
    HStrList.push_back(tr("账号"));
    HStrList.push_back(tr("密码"));
    HStrList.push_back(tr("状态"));
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setEnabled(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setColumnCount(HStrList.count());
    table->setHorizontalHeaderLabels(HStrList);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->resizeSection(0,70);
    table->horizontalHeader()->resizeSection(1,140);
    table->horizontalHeader()->resizeSection(2,49);
    table->horizontalHeader()->resizeSection(3,90);
    table->horizontalHeader()->resizeSection(4,100);
    table->horizontalHeader()->resizeSection(5,90);
    QHBoxLayout *h1=new QHBoxLayout;
    h1->addWidget(table);
    box->setLayout(h1);
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::blue);
    box->setPalette(pe);
    QHBoxLayout *h2=new QHBoxLayout;
    h2->addWidget(la);
    h2->addWidget(commond);

    QHBoxLayout *h3=new QHBoxLayout;
    h3->addWidget(bta);
    h3->addWidget(lb);
    h3->addWidget(port);
    h3->addWidget(lc);
    h3->addStretch(1);
    h3->addWidget(btb);
    QVBoxLayout *v=new QVBoxLayout(this);
    v->addWidget(box);
    v->addLayout(h2);
    v->addLayout(h3);
}
void Widget::importFile()
{
     QString path=QFileDialog::getOpenFileName(this, tr("选择文件"),"",tr("(*.txt)"));
     if(path.length() == 0) {
           QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
     }
     QFile file(path);
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
     {
           return;
     }
     int id=servers.size();
     while (!file.atEnd()) {
            QStringList line = QString(file.readLine()).split(' ');
            if(line.size()!=4)
            {
                continue;
            }
            server s;
            s.id=id+1;
            s.ip=line.at(0);
            s.port=line.at(1).toShort();
            s.username=line.at(2);
            s.password=line.at(3);
            s.password=s.password.remove('\n');
            servers.insert(id,s);
            notExecTask.append(id++);
     }
     noextask=notExecTask.size();
     if(!notExecTask.isEmpty())
     {
         btb->setEnabled(true);
         la->setEnabled(true);
     }
     file.close();
}
void Widget::executTask()
{
    QString cmd=commond->text();
    if(cmd.isEmpty())
    {
        QMessageBox::information(this,"提示","请输入执行命令");
        return;
    }
    btb->setEnabled(false);
    la->setEnabled(false);
    for(int i=0;i<MAXTHREAD;i++)
    {
        threads[i]->setCommond(cmd);
    }
    mute.lock();
    waitco.wakeAll();
    mute.unlock();
}

void Widget::initTable()
{
    table->setRowCount(servers.size());
    QMapIterator<int,server> i(servers);
    while (i.hasNext()) {
         i.next();
         int id=i.key();
         server s=i.value();
         table->setItem(id,0,new QTableWidgetItem(QString::number(s.id)));
         table->setItem(id,1,new QTableWidgetItem(s.ip));
         table->setItem(id,2,new QTableWidgetItem(QString::number(s.port)));
         table->setItem(id,3,new QTableWidgetItem(s.username));
         table->setItem(id,4,new QTableWidgetItem(s.password));
         table->setItem(id,5,new QTableWidgetItem(s.status==login::FAILED?tr("登陆失败"):tr("登陆成功")));
     }
}

void Widget::initThread()
{
    for(int i=0;i<MAXTHREAD;i++)
    {
        threads[i]=new TaskThread();
        threads[i]->setTid(i);
        connect(threads[i],SIGNAL(tasend()),this,SLOT(setTableDate())); 
        threads[i]->start();
    }
}
Widget::~Widget()
{
    mute.lock();
    exitth=1;
    waitco.wakeAll();
    mute.unlock();
    for(int i=0;i<MAXTHREAD;i++)
    {
        delete threads[i];
    }
}
void Widget::setTableDate()
{
    static int te=0;
    te++;
    QString me="已执行 "+QString::number(te)+" 剩余 "+QString::number(noextask-te);
    lc->setText(me);
    if(noextask==te)
    {
        initTable();
        te=0;
    }
}
