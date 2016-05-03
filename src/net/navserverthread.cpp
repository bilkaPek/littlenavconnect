/*****************************************************************************
* Copyright 2015-2016 Alexander Barthel albar965@mailbox.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "navserver.h"
#include "navserverthread.h"

#include <QtNetwork>

NavServerThread::NavServerThread(qintptr socketDescriptor, NavServer *parent)
  : QThread(parent), socket(socketDescriptor), server(parent)
{
  server->log("NavServerThread created " + objectName(), QtInfoMsg);
}

NavServerThread::~NavServerThread()
{
  server->log("NavServerThread deleted " + objectName(), QtInfoMsg);
}

void NavServerThread::run()
{
  QTcpSocket tcpSocket;
  if(!tcpSocket.setSocketDescriptor(socket))
  {
    server->log("Error creating socket " + tcpSocket.errorString(), QtCriticalMsg);
    return;
  }

  server->log("Connection from " + tcpSocket.peerAddress().toString() +
              " port " + QString::number(tcpSocket.peerPort()), QtInfoMsg);

  while(true)
  {
    QString msg;
    mutex.lock();
    waitCondition.wait(&mutex);
    msg = message;
    mutex.unlock();

    if(terminate)
    {
      server->log("Termination requested", QtWarningMsg);
      break;
    }

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << msg;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    if(!tcpSocket.isOpen())
    {
      server->log("Connection closed by peer", QtWarningMsg);
      break;
    }

    tcpSocket.write(block);
    tcpSocket.flush();
    sleep(1);
  }
  tcpSocket.disconnectFromHost();

  if(tcpSocket.state() != QAbstractSocket::UnconnectedState)
    tcpSocket.waitForDisconnected(10000);
}

void NavServerThread::postMessage(const QString& msg)
{
  QMutexLocker locker(&mutex);
  message = msg;
  waitCondition.wakeAll();
}

void NavServerThread::setTerminate()
{
  terminate = true;
  waitCondition.wakeAll();
}
