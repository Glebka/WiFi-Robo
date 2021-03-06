#include <QDebug>
#include "CommunicationController.hpp"

CommunicationController::CommunicationController( QObject *parent )
    : QObject( parent )
    , mHost()
    , mPort( 0 )
{
    connectSignals();
}

CommunicationController::CommunicationController( const QJsonObject &config )
    : QObject( NULL )
    , mHost( config["host"].toString() )
    , mPort( config["controlPort"].toInt() )
{

}

CommunicationController::~CommunicationController()
{
   disconnectFromHost();
}

bool CommunicationController::connectToHost(const QString& host, quint16 port)
{
   mSocket.connectToHost( host, port );
   return mSocket.waitForConnected();
}

bool CommunicationController::connectToHost()
{
    return connectToHost( mHost, mPort );
}

void CommunicationController::disconnectFromHost()
{
   Command cmd;
   cmd.code = 0;
   cmd.hiParam = 0;
   cmd.loParam = 0;

   mSocket.write( ( char* )&cmd, sizeof( Command ) );
   mSocket.waitForBytesWritten();
   mSocket.disconnectFromHost();
}

//! Byte swap unsigned short
static uint16_t swap_uint16( uint16_t val )
{
    return (val << 8) | (val >> 8 );
}

void CommunicationController::changeCameraPosition(quint16 rotX, quint16 rotZ)
{
   Command cmd;
   cmd.code = CMD_SERVO_CONTROL;
   cmd.hiParam = rotX;
   cmd.loParam = rotZ;

   mSocket.write( ( char* )&cmd, sizeof( Command ) );
   mSocket.waitForBytesWritten();
}

void CommunicationController::onConnected()
{
   qDebug() << "Connected to robo control server on "
            << mSocket.peerName()
            << ":"
            << mSocket.peerPort();
}

void CommunicationController::onDisconnected()
{
   qDebug() << "Disconnected from robo control server";
}

void CommunicationController::onDataArrived()
{

}

void CommunicationController::connectSignals()
{
    connect( &mSocket, &QTcpSocket::connected, this, &CommunicationController::onConnected );
    connect( &mSocket, &QTcpSocket::disconnected, this, &CommunicationController::onDisconnected );
    connect( &mSocket, &QTcpSocket::readyRead, this, &CommunicationController::onDataArrived );
}

