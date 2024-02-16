#pragma once 

#include <filesystem>
#include <string>
#include <string_view>
#include <memory>

#include "nlohmann/json.hpp"

#include "spdlog/spdlog.h"

#include <QAbstractListModel>
#include <QObject>

#include "config.h"
#include "controls/componentcreator.h"

struct AnimatedButton {
    std::filesystem::path icon;
    std::string action;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnimatedButton, icon, action)

class AnimatedButtonList : public QAbstractListModel {
    Q_OBJECT
    Q_ENUMS(DataRole)
    public:
        enum DataRole {
            Icon = Qt::UserRole + 1,
            Action
        };

        using QAbstractListModel::QAbstractListModel;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override { return mData.size(); }

        int columnCount(const QModelIndex &parent = QModelIndex()) const override { return 2; }

        QVariant data(const QModelIndex &index, int role) const override { 
            if (!index.isValid()) {
                return QVariant{};
            }
            DataRole roleAsEnum = static_cast<DataRole>(role);

            switch (roleAsEnum) {
                case DataRole::Icon: 
                {
                    using namespace std::string_literals;
                    // TODO: Add conversion from std::filesystem to QString and always generate absolute path
                    auto absolutePath = std::filesystem::absolute(mData[index.row()].icon);
                    auto withProtoc = "file://"s + absolutePath.c_str();
                    spdlog::debug("Absolute path for icon : {}", withProtoc);
                    return QString::fromStdString(withProtoc);
                    break;
                }
                case DataRole::Action:
                return QString::fromStdString(mData[index.row()].action);
                break;
            }

            return QVariant{}; 
        }

        QHash<int, QByteArray> roleNames() const override {
            return {{ Icon, "icon"}, { Action, "action"}};
        }

        void populate(const std::vector<AnimatedButton> &init) {
            beginResetModel();
            mData = QVector<AnimatedButton>(init.cbegin(), init.cend());
            endResetModel();
        }
    private:
        QVector<AnimatedButton> mData;
};

// TODO: probably replace this with a central class to handle all possible actions, 
// so that all possible actions can be triggered with every component
class ButtonActionHandler : public QObject {
    Q_OBJECT
    public slots:
        void handleAction(const QString &msg);
};

struct ButtonControlConfig {
    // Size of icons, might change to string in the future for different approach e.g. 8x8 (8 rows by 8 columns -> make it fit)
    unsigned int iconSize; 
    // Space between buttons
    unsigned int spacing;
    // Filename of background video
    std::filesystem::path background;
    // List of actions, every action will create a new button
    std::vector<AnimatedButton> actions;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ButtonControlConfig, iconSize, spacing, actions, background)

class ButtonControl {
    public:
        static inline constexpr std::string_view type = "ButtonView";

        static void initializeComponent();

        ButtonControl(const ButtonControl &other) = delete;
        ButtonControl(ButtonControl &&other) = default;
        ~ButtonControl() = default;

        ButtonControl &operator=(const ButtonControl &other) = delete;
        ButtonControl &operator=(ButtonControl &&other) = default;

        template<typename CreatorType>
        static ButtonControl createInstance(const Control &parameters, CreatorType &creator);

        const ButtonActionHandler *getActionHandler() const;

        const AnimatedButtonList *getButtonList() const;
        AnimatedButtonList *getButtonList();
    private:
        ButtonControl();

        std::unique_ptr<AnimatedButtonList> buttonList;
        std::unique_ptr<ButtonActionHandler> actionHandler;
};

template<typename CreatorType>
ButtonControl ButtonControl::createInstance(const Control &control, CreatorType &creator) {
    ComponentPropertiesType properties{};

    auto config = control.params.get<ButtonControlConfig>();
    properties["iconSize"] = config.iconSize;
    properties["spacing"] = config.spacing;
    properties["background"] = config.background;
    ButtonControl instance{};
    instance.getButtonList()->populate(config.actions);

    auto createdComponent = creator.template createComponent<AnimatedButtonList>(
        control.name, 
        "ButtonView", 
        properties, 
        reinterpret_cast<AnimatedButtonList *>(instance.getButtonList()));

    // TODO: put this code in the ButtonActionHandler class
    QObject::connect(createdComponent, SIGNAL(doAction(QString)), instance.getActionHandler(), SLOT(handleAction(QString)));

    return instance;
}