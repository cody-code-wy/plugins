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

void Dictionary::DICTConnection::getReturn(QString line, return_code_t *return_code){
  *return_code = static_cast<return_code_t>(line.split(' ').first().toInt());
}

Dictionary::DICTConnection::DICTConnection(QString hostname, quint16 port){
  socket = new QTcpSocket();
  this->hostname = hostname;
  this->port = port;
}

Dictionary::DICTConnection::~DICTConnection() {
  socket->close();
}

void Dictionary::DICTConnection::beginConnection(){
  qDebug() << "Connecting to server";
  socket->connectToHost(hostname, port);
  if(socket->waitForConnected(5000)){
    socket->waitForReadyRead(3000);
    QString response(socket->readAll());
    return_code_t return_code;
    getReturn(response, &return_code);
    latestReturnCode = return_code;
    qDebug() << "Server Response: " << return_code;
    if(return_code == TEXT_CAPABILITES){
      qDebug() << "Connected to server";
      connected = true;
    } else {
      connected = false;
    }
  } else {
    connected = false;
  }

}

void Dictionary::DICTConnection::endConnection(){
  qDebug() << "Disconnecting from server";
  socket->disconnectFromHost();
  connected = false;
}

void Dictionary::DICTConnection::define( QString word ){
  qDebug() << "Looking up:" << word;

  QString request("DEFINE * \"");
  request.append(word);
  request.append("\"\r\n");
  socket->write(request.toUtf8());
  socket->waitForBytesWritten(1000);

  processDefineResponse();
}

void Dictionary::DICTConnection::processDefineResponse(){
  bool done = false;
  bool textState = false;
  while (!done){
    if (socket->waitForReadyRead(100)){
      while ( socket->bytesAvailable() > 0 ){
        QString line(socket->readLine());
        if ( textState ){
          processDefineResponseText(line, &textState);
        } else {
          processDefineResponseControl(line, &textState, &done);
        }
      }
    } else {
      qDebug() << "Dictionary DICT read timeout";
      done = true;
      break;
    }
  }
}

void Dictionary::DICTConnection::processDefineResponseControl( QString line, bool *textState, bool *done ){
  /* qDebug() << "PDRC" << line; */
  return_code_t returnCode;
  getReturn(line, &returnCode);
  if(returnCode > 99)
    latestReturnCode = returnCode;
  switch(returnCode){
    case INVALID_DATABASE :
    case NO_MATCH :
    case OK :
      *done = true;
      break;
    case WORD_DATABASE_NAME :
      *textState = true;
      break;
    default : //Squash many warnings
      break;
  }
}

void Dictionary::DICTConnection::processDefineResponseText( QString line, bool *textState ){
  rawDefinition << line.simplified();
  /* qDebug() << "PDRT" << line; */
  if ( line.contains(QRegularExpression("^\\.\r\n$")) ){
    *textState = false;
  }
}

void Dictionary::DICTConnection::splitDefinitions(){
  if (rawDefinition.isEmpty())
    return;
  QRegularExpression linestart("^(?:(\\w+)(?:\\s+))?((?:\\d+:\\s+).*$)");
  foundWord = rawDefinition.takeFirst();
  QString currentWordType;
  QString runningLine;
  qDebug() << "splitting";
  for ( const auto& i : rawDefinition ){
    QRegularExpressionMatch matches;
    if(i.contains(linestart, &matches)){
      if(!runningLine.isEmpty()){
        rawSplitDefinitions << runningLine;
        runningLine = "";
      }
      if(matches.capturedLength(1) > 0){
        currentWordType = matches.captured(1);
        qDebug() << matches;
        qDebug() << "\\w #:" << currentWordType;
        runningLine += i;
      } else {
        qDebug() << "#:" << currentWordType;
        runningLine += currentWordType + " " + i;
      }
    } else {
      runningLine += i;
    }
  }
  if(!runningLine.isEmpty()){
    rawSplitDefinitions << runningLine;
  }
}

int Dictionary::DICTConnection::getDefinitionCount(){
  return rawSplitDefinitions.size();
}

QString Dictionary::DICTConnection::getNextDefinition(){
  if(rawSplitDefinitions.size() >= 1){
    /* qDebug() << rawSplitDefinitions; */
    return rawSplitDefinitions.takeFirst().split(';').first();
  }else{
    return QString("Error");
  }
}

