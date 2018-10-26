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
    : Core::Extension("org.albert.extension.dictionary"), // Must match the id in metadata
      Core::QueryHandler(Core::Plugin::id()),
      d(new Private) {

    registerQueryHandler(this);

    Dictionary::DICTConnection dict("localhost");

    if (dict.connected) {
      dict.define(QString("test"));
      dict.define(QString("nonword"));
    }else{
      throw std::runtime_error("Could not connect to DICT server");
    }

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

    auto item = make_shared<StandardItem>("dictionary");
    /* item->setIconPath(d->iconPath);
    d->locale.setNumberOptions(settings().value(CFG_SEPS, CFG_SEPS_DEF).toBool()
                               ? d->locale.numberOptions() & ~QLocale::OmitGroupSeparator
                               : d->locale.numberOptions() | QLocale::OmitGroupSeparator ); */
    item->setText(QString("Dictionary"));
    item->setCompletion(item->text());
    /* item->addAction(make_shared<ClipAction>("Copy result to clipboard",
                                            d->locale.toString(result, 'G', 16)));
    item->addAction(make_shared<ClipAction>("Copy equation to clipboard",
                                            QString("%1 = %2").arg(query->string(), item->text()))); */

    Dictionary::DICTConnection dict("localhost");

    if (dict.connected) {
      item->setSubtext(dict.define(query->string()));
    }else{
      throw std::runtime_error("Could not connect to DICT server");
    }

    query->addMatch(move(item), UINT_MAX);
}

