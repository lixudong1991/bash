#include "taskthread.h"
#include <libssh2.h>
#include <QHostAddress>
TaskThread::TaskThread()
{
}
void TaskThread::run()
{
    QTcpSocket sock;
    forever
     {
        int Taskid=0;
        { 
            mute.lock();
            if(notExecTask.isEmpty())
            { 
                waitco.wait(&mute);
            }
            if(exitth)
            {
               mute.unlock();
               return;
            }
            Taskid=notExecTask.first();
            notExecTask.removeFirst();
            mute.unlock();
        }
        runCommond(Taskid,sock);
        emit tasend();
     }
 }
 void TaskThread::runCommond(int Taskid,QTcpSocket &sock)
 {

        server s=servers.value(Taskid);
        if(sock.state()!=QAbstractSocket::UnconnectedState)
        {
            sock.disconnectFromHost();
        }
        int libssh2_error = libssh2_init(0);
        if(libssh2_error)
        {
             return;
        }
        sock.connectToHost(QHostAddress(s.ip),s.port);
        if(!sock.waitForConnected())
        {
             return;
        }
        LIBSSH2_SESSION *session_ = libssh2_session_init();
        if(!session_)
        {
             return ;
        }
        libssh2_error = libssh2_session_startup(session_, sock.socketDescriptor());
        if(libssh2_error)
        {
             return;
        }
        libssh2_userauth_list(session_, s.username.toLocal8Bit().constData(), s.username.toLocal8Bit().length());
        if(!libssh2_userauth_password(
                session_,
                s.username.toLocal8Bit().constData(),
                s.password.toLocal8Bit().constData()
        ))
        {
            servers.find(Taskid).value().status=login::SUCCESS;
            LIBSSH2_CHANNEL *channel = libssh2_channel_open_session(session_);
            if (channel)
            {
                libssh2_channel_handle_extended_data(channel,LIBSSH2_CHANNEL_EXTENDED_DATA_NORMAL);
                libssh2_channel_set_blocking(channel, 1);
                int rc;
                while ((rc=libssh2_channel_exec(channel, cmd.toLocal8Bit().constData()))==LIBSSH2_ERROR_EAGAIN );
                libssh2_channel_close(channel);
                libssh2_channel_free(channel);
            }
        }

        libssh2_session_disconnect(session_, "Client disconnecting for Normal");
        libssh2_session_free(session_);
        libssh2_exit();
 }
 void TaskThread::setCommond(QString c)
 {
     cmd=c;
 }
 void TaskThread::setTid(int id)
  {
      tid=id;
  }

 TaskThread::~TaskThread()
 {
     {
         mute.lock();
          notExecTask.clear();
          mute.unlock();
     }
     wait();
 }

