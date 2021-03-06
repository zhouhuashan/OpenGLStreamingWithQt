/**************************************************************************
**   Author: Oleksiy Kasilov, bbv Software Services AG
**   Date:   10/17/2015
**   Year:   2015
**************************************************************************/

#include "openglserver.h"

#include <trace_writer_local.hpp>

#include <QWebSocketServer>
#include <QCoreApplication>
#include <QWebSocket>
#include <QByteArray>
#include <QDebug>

#include <tuple>
#include <algorithm>
#include <cassert>

QT_USE_NAMESPACE

OpenGLServer::OpenGLServer(quint16 port, bool debug, QObject *parent) :
    QObject(parent),
    mpWebSocketServer(new QWebSocketServer(QStringLiteral("OpenGL Server"),
                                            QWebSocketServer::NonSecureMode, this)),
    mClients(),
    mDebug(debug)
{
    if (mpWebSocketServer->listen(QHostAddress::Any, port)) {
        if (mDebug)
            qDebug() << "OpenGL server listening on port" << port;
        connect(mpWebSocketServer, &QWebSocketServer::newConnection,
                this, &OpenGLServer::onNewConnection);
        connect(mpWebSocketServer, &QWebSocketServer::closed, this, &OpenGLServer::closed);
    }
}

OpenGLServer::~OpenGLServer()
{
    mpWebSocketServer->close();
    foreach (auto pClient, mClients) {
        delete pClient.first;
    }
}

void OpenGLServer::onNewConnection()
{
    QWebSocket *pSocket = mpWebSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &OpenGLServer::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &OpenGLServer::socketDisconnected);
    connect(&trace::localWriter, &trace::LocalWriter::frameEnd, this, &OpenGLServer::onFrameEnd);


    mClients << qMakePair(pSocket, false);
    pSocket->sendBinaryMessage(trace::localWriter.getInitFrame());
}

void OpenGLServer::sendBinaryMessage(const QByteArray &message)
{
    foreach (auto &pClient, mClients) {
        if (pClient.second) {
            pClient.first->sendBinaryMessage(message);
            if (mDebug)
                qDebug() << "Message sent. Size:" << message.size();

        }
    }
}

void OpenGLServer::onFrameEnd()
{
    for (auto &pClient :mClients) {
        if (!pClient.second) {
            trace::localWriter.resetSignatures();
            pClient.second = true;
            QByteArray data;
            data.append(trace::EVENT_RESET);
            sendBinaryMessage(data);
            connect(&trace::localWriter, &trace::LocalWriter::glFrameSerialized, this, &OpenGLServer::sendBinaryMessage,
                    static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection));
            break;
        }
    }
}

void OpenGLServer::processBinaryMessage(const QByteArray &message)
{
    /*Archive ar(message);

    if (QObject *obj = findOpenGLWidget(ar.getData().constData()))
    {
        QEvent* event = mSerializer.deserialize(ar);
        assert(event != nullptr);
        if (mDebug)
            qDebug() << "Posting event to" << obj->metaObject()->className();
        qApp->postEvent(obj, event);
    }*/
}

void OpenGLServer::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (mDebug)
        qDebug() << "socketDisconnected:" << pClient;
    if (pClient) {
        Clients::iterator it = std::find_if(mClients.begin(), mClients.end(), [pClient](const ClientEntry &clientEntry) {
            return clientEntry.first == pClient;
        });
        if (it != mClients.end() ) {
            mClients.removeAll(*it);
        }
        pClient->deleteLater();
    }
}
