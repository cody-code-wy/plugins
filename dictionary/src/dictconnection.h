// Copyright (C) 2018 William Young

#pragma once
#include <QObject>
#include <QTcpSocket>

namespace Dictionary {
  class DICTConnection final {
    public:

    DICTConnection( QString hostname, quint16 port = 2628 );
    ~DICTConnection();

    quint8
      getReturnCode();

    QString
      define( QString word );

    bool
      connected;

    private:

    QTcpSocket* socket;
  };
}
