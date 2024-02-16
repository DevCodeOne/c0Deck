#include <chrono>
#include <filesystem>
#include <type_traits>
#include <thread>

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
#include <variant>

#include "application_entry.h"
#include "instance.h"
#include "utils.h"
#include "controls/componentcreator.h"

bool MainWindow::initialize(int argc, char *argv[], Instance &instance) {
    QQuickStyle::setStyle("Material");
    Instance::ComponentRegistryType::initializeComponents();

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

    QRect geometry = QGuiApplication::primaryScreen()->availableVirtualGeometry();

    // TODO: actually use values of config
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

    auto &componentRegistry = instance.getComponentRegistry();

    auto qmlCreator = 
    [&engine, &content, &tabbar]
        (const std::string &title, const std::string &componentName, const ComponentPropertiesType &properties, auto *model) {
        auto name = fmt::format("qrc:/qml/{}.qml", componentName);
        spdlog::debug("Adding component : {}", name);
        // The same component can probably be loaded only once -> use hashmap
        QQmlComponent loadedComponent(&engine, QUrl(name.c_str()));
        QQmlComponent tabComponent(&engine, QUrl("qrc:/qml/CustomTabButton.qml"));
        QMap<QString, QVariant> qprops;

        for (const auto &[key, value] : properties) {
            // std::visit([&key](const auto &value) {
            //     spdlog::debug("Adding at key : {} value : {}", key, value);
            // }, value);
            // TODO: improve this -> move to own class
            if (std::holds_alternative<std::string>(value)) {
                spdlog::debug("String input : {}", std::get<std::string>(value));
                qprops[QString::fromStdString(key)] = QString::fromStdString(std::get<std::string>(value));
            } else if (std::holds_alternative<int>(value)) {
                qprops[QString::fromStdString(key)] = std::get<int>(value);
            } else if (std::holds_alternative<unsigned int>(value)) {
                qprops[QString::fromStdString(key)] = std::get<unsigned int>(value);
            } else if (std::holds_alternative<std::filesystem::path>(value)) {
                auto path = std::get<std::filesystem::path>(value);
                spdlog::debug("Path input : {}", path.c_str());

                if (!path.is_absolute()) {
                    path = std::filesystem::absolute(path);
                    spdlog::debug("Converted path to absolute path : {}", path.c_str());
                }

                using namespace std::string_literals;
                std::string pathWithProtoc = fmt::format("file://{}", path.c_str());
                qprops[QString::fromStdString(key)] = QString::fromStdString(pathWithProtoc);
            }
        }

        constexpr bool isVoidPtr = std::is_same_v<void, std::decay_t<std::remove_pointer_t<decltype(model)>>>;
        GenerateIf<!isVoidPtr>::call(
            [](auto &qprops, auto *model){
                spdlog::debug("Setting model to value : {}", (void *) model);
                qprops["model"] = QVariant::fromValue(model);
            }, qprops, model);

        QObject *createdComponent = loadedComponent.createWithInitialProperties(qprops);
        QQuickItem *createdItem = qobject_cast<QQuickItem *>(createdComponent);
        createdComponent->setParent(content);
        createdItem->setParentItem(qobject_cast<QQuickItem *>(content));

        spdlog::debug("Created component : {}", name);

        qprops["text"] = QString::fromStdString(title);
        QObject *createdTab = tabComponent.createWithInitialProperties(qprops);
        QQuickItem *createdTabItem = qobject_cast<QQuickItem *>(createdTab);
        createdTabItem->setParent(tabbar);
        createdTabItem->setParentItem(qobject_cast<QQuickItem *>(tabbar));

        spdlog::debug("Created tab for component : {}", name);

        return createdComponent;
    };

    ComponentCreator creator{qmlCreator};

    for (const auto &currentControl : instance.getConfig().getControls()) {
        componentRegistry.createInstance(currentControl, creator);
    }

    return application.exec();
}