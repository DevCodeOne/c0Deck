#include <chrono>

#include <qnamespace.h>
#include <spdlog/spdlog.h>

#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>
#include <QGuiApplication>
#include <QScreen>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QStyleHints>
#include <QtQml/QQmlContext>
#include <QtWebEngine>
#include <QQmlComponent>
#include <QMap>
#include <thread>

#include "application_entry.h"
#include "instance.h"

bool MainWindow::initialize(int argc, char *argv[], Instance &instance) {
    QQuickStyle::setStyle("Material");
    QtWebEngine::initialize();

    QGuiApplication application(argc, argv);
    QGuiApplication::setApplicationDisplayName(QCoreApplication::translate("main", "Default"));
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    QCommandLineParser parser;
    parser.setApplicationDescription(QGuiApplication::applicationDisplayName());
    parser.addHelpOption();
    parser.addVersionOption();

    auto keys = instance.getArgumentKeys();
    QList<QCommandLineOption> options;

    for (const auto &currentKey : keys) {
        auto asQString = QString::fromStdString(currentKey);
        QCommandLineOption currentOption(asQString, QString("Set") + asQString, asQString);
        options.append(currentOption);
    }

    parser.addOptions(options);
    parser.process(application);

    for (auto &currentOption : options) {
        const auto value = parser.value(currentOption);
        if (value != "") {
            instance.setArgumentValue(currentOption.valueName().toStdString(), value.toStdString());
        }
    }

    instance.initRuntime();

    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();
    context->setContextProperty(QStringLiteral("initialUrl"), QString("https://www.google.de"));

    QRect geometry = QGuiApplication::primaryScreen()->availableVirtualGeometry();

    context->setContextProperty(QStringLiteral("initialWidth"), 1200);
    context->setContextProperty(QStringLiteral("initialHeight"), 400);
    context->setContextProperty(QStringLiteral("iconSize"), 100);
    context->setContextProperty(QStringLiteral("landScape"), false);

    engine.load(QUrl(QStringLiteral("qrc:/qml/MainWindow.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }


    QObject *content = engine.rootObjects().first()->findChild<QObject *>("content");
    QObject *tabbar = engine.rootObjects().first()->findChild<QObject *>("tabs");

    spdlog::debug("ContentPtr : {}, TabbarPtr : {}", (void *) content, (void *) tabbar);

    QQmlComponent component(&engine, QUrl("qrc:/qml/WebControl.qml"));
    QMap<QString, QVariant> properties;
    properties["intialUrl"] = "https://www.google.de";
    QObject *createdComponent = component.createWithInitialProperties(properties);
    QQuickItem *createdItem = qobject_cast<QQuickItem *>(createdComponent);
    createdComponent->setParent(content);
    createdItem->setParentItem(qobject_cast<QQuickItem *>(content));

    QQmlComponent tab(&engine, QUrl("qrc:/qml/CustomTabButton.qml"));
    properties["text"] = "WebControl";
    QObject *createdComponent2 = tab.createWithInitialProperties(properties);
    QQuickItem *createdItem2 = qobject_cast<QQuickItem *>(createdComponent2);
    createdComponent2->setParent(tabbar);
    createdItem2->setParentItem(qobject_cast<QQuickItem *>(tabbar));

    return application.exec();
}