// Copyright (C) 2018 William Young
//
// DICTConnection
//
// For Information about status codes used
// see RFC 2229
//
// This implements a basic QT based
// parser and connection manager for
// DICT connections

#include "dictconnection.h"

Dictionary::DICTConnection::DICTConnection(QString hostname, quint16 port){
  socket = new QTcpSocket();
  socket->connectToHost(hostname, port);
  if(socket->waitForConnected(5000)){
    socket->waitForReadyRead(3000);
    QString response(socket->readAll());
    uint response_code = response.split(' ').first().toUInt();
    qDebug() << "Server Response: " << response_code;
    if(response_code == 220){
      connected = true;
    } else {
      connected = false;
    }
  } else {
    connected = false;
  }
}

Dictionary::DICTConnection::~DICTConnection() {
  socket->close();
}

QString Dictionary::DICTConnection::define( QString word ){
  qDebug() << "Looking up:" << word;

  // Forming the Query
  // see RFC 2229 3.2
  QString request("DEFINE * ");
  request.append(word);
  request.append("\r\n");

  // Sending the Query
  socket->write(request.toUtf8());
  socket->waitForBytesWritten(1000);

  // Read the response
  // see RFC 2229 2.4
  QString response;
  bool done = false;
  uint8_t state = 0;
  while ( !done ) {
    socket->waitForReadyRead(100); // Keep number small
    if ( socket->bytesAvailable() == 0 ){
      done = true;
      break;
    }
    while ( socket->bytesAvailable() > 0 ) {
      QString line(socket->readLine());
      // State 0
      // Normal State
      if (state == 0){
        // Looking for Termination Codes
        // See RFC 2229 2.4.1 & 3.2
        // Looking for code 250
        // Positive Completion Reply - DICT System
        // or code 550,552
        // Permanent Negitive Completion Reply - DICT System
        uint response_code = line.split(' ').first().toUInt();
        if( response_code == 250 ||
            response_code == 550 ||
            response_code == 552  ) {
          done = true;
          break;
        }
        // Looking for Status Codes
        // See RFC 2229 2.4.1 & 3.2
        // Looking for code 150
        // Positive Preliminary Reply - DICT System
        /* if( response_code == 151 ){
          qDebug() << "Found Word";
        } */
        // Looking for Status Codes
        // See RFC 2229 2.4.1 & 3.2
        // Looking for code 151
        // Positive Preliminary Reply - DICT System
        if( response_code == 151 ){
          state = 1;
          break;
        }
      }
      // State 1
      // Text Response State
      // See RFC 2229 2.4.2
      if (state == 1){
        response.append(line);
        if ( QString::compare(line, QString(".\r\n"), Qt::CaseInsensitive) == 0){
          state = 0;
        }
      }
    }
  }
  qDebug() << response;
  return response;
  /* return true; */
}
