#include "mainwindow.h"

#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>
#include <QGuiApplication>
#include <QScreen>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QStyleHints>
#include <QtQml/QQmlContext>

#include <qtwebengineglobal.h>

bool MainWindow::initialize(int argc, char *argv[]) {
    QQuickStyle::setStyle("Material");
    QtWebEngine::initialize();

    QGuiApplication application(argc, argv);
    QGuiApplication::setApplicationDisplayName(QCoreApplication::translate("main", "Default"));
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    QCommandLineParser parser;
    parser.setApplicationDescription(QGuiApplication::applicationDisplayName());
    parser.addHelpOption();
    parser.addVersionOption();

    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();
    context->setContextProperty(QStringLiteral("initialUrl"), QString("https://www.google.de"));

    QRect geometry = QGuiApplication::primaryScreen()->availableVirtualGeometry();
    if (!QGuiApplication::styleHints()->showIsFullScreen()) {
        const QSize windowDimensions = geometry.size() / 2;
        const QSize windowOffset = (geometry.size() - windowDimensions) * 0.5;
        const QPoint offset = geometry.topLeft() + QPoint(windowOffset.width(), windowOffset.height());

        geometry = QRect(offset, windowDimensions);
    }

    context->setContextProperty(QStringLiteral("initialX"), geometry.x());
    context->setContextProperty(QStringLiteral("initialY"), geometry.y());
    context->setContextProperty(QStringLiteral("initialWidth"), 1200);
    context->setContextProperty(QStringLiteral("initialHeight"), 400);
    context->setContextProperty(QStringLiteral("iconSize"), 100);
    context->setContextProperty(QStringLiteral("landScape"), false);

    engine.load(QUrl(QStringLiteral("qrc:/qml/MainWindow.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return application.exec();
}