// Copyright (C) 2018 William Young

#pragma once
#include <QObject>
#include <memory>
#include "core/extension.h"
#include "core/queryhandler.h"
#include "dictconnection.h"

namespace Dictionary {

class Private;

class Extension final :
        public Core::Extension,
        public Core::QueryHandler
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ALBERT_EXTENSION_IID FILE "metadata.json")

public:

    Extension();
    ~Extension();

    QString name() const override { return "Dictionary"; }
    QWidget *widget(QWidget *parent = nullptr) override;
    void setupSession() override;
    void teardownSession() override;
    void handleQuery(Core::Query * query) const override;

private:

    std::unique_ptr<Private> d;

};
}
