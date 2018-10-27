// Copyright (C) 2018 William Young

#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QRegularExpressionMatch>
#include "dictreturncodes.h"

namespace Dictionary {

  class DICTConnection final {
    public:

    DICTConnection( QString hostname, quint16 port = 2628 );
    ~DICTConnection();

    quint8
      getReturnCode();

    void
      beginConnection(),
      endConnection(),
      splitDefinitions(),
      define( QString word );

    bool
      connected;


    return_code_t
      latestReturnCode;

    int
      getDefinitionCount();

    QString
      getNextDefinition();

    private:

    void
      processDefineResponse(),
      processDefineResponseControl( QString, bool *textState, bool *done ),
      processDefineResponseText( QString, bool *textState ),
      getReturn(QString line, return_code_t *return_code);

    QTcpSocket* socket;
    QStringList
      rawDefinition,
      rawSplitDefinitions;
    QString
      foundWord,
      hostname;
    quint16 port;
  };
}
