// Copyright (C) 2018 William Young

#include <QDebug>
#include <QPointer>
#include <QTcpSocket>
#include <stdexcept>
#include "util/standarditem.h"
#include "xdg/iconlookup.h"
#include "dictconnection.h"
#include "configwidget.h"
#include "extension.h"
using namespace Core;
using namespace std;

class Dictionary::Private
{
public:
    QPointer<ConfigWidget> widget;
};


/** ***************************************************************************/
Dictionary::Extension::Extension()
    : Core::Extension("org.albert.extension.connectionionary"), // Must match the id in metadata
      Core::QueryHandler(Core::Plugin::id()),
      d(new Private){

    registerQueryHandler(this);

}

/** ***************************************************************************/
Dictionary::Extension::~Extension() {

}



/** ***************************************************************************/
QWidget *Dictionary::Extension::widget(QWidget *parent) {
    if (d->widget.isNull()) {
        d->widget = new ConfigWidget(parent);
    }
    return d->widget;
}



/** ***************************************************************************/
void Dictionary::Extension::setupSession() {
}



/** ***************************************************************************/
void Dictionary::Extension::teardownSession() {
}



/** ***************************************************************************/
void Dictionary::Extension::handleQuery(Core::Query * query) const {

    DICTConnection connection("localhost");

    connection.beginConnection();

    connection.define(query->string());
    connection.splitDefinitions();
    qDebug() << connection.latestReturnCode;
    qDebug() << "definition count: " << connection.getDefinitionCount();
    uint priority = UINT_MAX;
    while(connection.getDefinitionCount() > 0){
      if(connection.latestReturnCode == OK){
        auto item = make_shared<StandardItem>("connectionionary");
        item->setText(QString("Defenition of: ") + query->string());
        item->setCompletion(item->text());
        item->setSubtext(connection.getNextDefinition());
        query->addMatch(move(item), (priority--));
      }
    }

    connection.endConnection();
}

