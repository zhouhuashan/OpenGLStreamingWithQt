/**************************************************************************
**   Author: Oleksiy Kasilov, bbv Software Services AG
**   Date:   10/17/2015
**   Year:   2015
**************************************************************************/

#ifndef OPENGLSERVER_H
#define OPENGLSERVER_H

#include <QObject>
#include <QList>
#include <QPair>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QByteArray)

class OpenGLServer : public QObject
{
    Q_OBJECT

public:
    explicit OpenGLServer(quint16 port, bool debug = false, QObject *parent = Q_NULLPTR);
    typedef QPair<QWebSocket *, bool> ClientEntry;
    typedef QList<ClientEntry> Clients;
    ~OpenGLServer();

signals:
    void closed();

private slots:
    void onNewConnection();
    void processBinaryMessage(const QByteArray &message);
    void socketDisconnected();
    void sendBinaryMessage(const QByteArray &message);
    void onFrameEnd();

private:
    QWebSocketServer *mpWebSocketServer;
    Clients mClients;
    bool mDebug;
};

#endif //OPENGLSERVER_H
