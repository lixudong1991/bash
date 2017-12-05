#include "server.h"

QMutex mute;
QWaitCondition waitco;
QMap<int,server> servers;
QLinkedList<int> notExecTask;
int exitth=0;
