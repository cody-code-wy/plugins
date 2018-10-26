// Copyright (C) 2018 William Young

#include <QDebug>
#include <QPointer>
#include <QTcpSocket>
#include <stdexcept>
#include "util/standarditem.h"
#include "xdg/iconlookup.h"
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
void Dictionary::Extension::handleQuery(Core::Query *) const {

    /*
     * Things change so often I wont maintain this tutorial here. Check the relevant headers.
     *
     * - core/extension.h
     * - core/queryhandler.h
     * - core/query.h
     * - core/item.h
     * - core/action.h
     * - util/standarditem.h
     * - util/offlineindex.h
     * - util/standardindexitem.h
     *
     * Use
     *
     *   query->addMatch(my_item)
     *
     * to add matches. If you created a throw away item MOVE it instead of
     * copying e.g.:
     *
     *   query->addMatch(std::move(my_tmp_item))
     *
     * The relevance factor is optional. (Defaults to 0) its a usigned integer depicting the
     * relevance of the item 0 mean not relevant UINT_MAX is totally relevant (exact match).
     * E.g. it the query is "it" and your items name is "item"
     *
     *   my_item.name().startswith(query->string)
     *
     * is a naive match criterion and
     *
     *   UINT_MAX / ( query.searchterm().size() / my_item.name().size() )
     *
     * a naive match factor.
     *
     * If you have a lot of items use the iterator versions addMatches, e.g. like that
     *
     *   query->addMatches(my_items.begin(), my_items.end());
     *
     * If the items in the container are temporary object move them to avoid uneccesary
     * reference counting:
     *
     *   query->addMatches(std::make_move_iterator(my_tmp_items.begin()),
     *                     std::make_move_iterator(my_tmp_items.end()));
     */
}

