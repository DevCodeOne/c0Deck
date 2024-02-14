#pragma once 

#include <string>
#include <string_view>
#include <memory>

#include <QAbstractListModel>
#include <QObject>

#include "nlohmann/json.hpp"

#include "config.h"
#include "controls/componentcreator.h"

struct AnimatedButton {
    std::string icon;
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
                return QString::fromStdString(mData[index.row()].icon);
                break;
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

struct ButtonControlConfig {
    // Size of icons, might change to string in the future for different approach e.g. 8x8 (8 rows by 8 columns -> make it fit)
    unsigned int iconSize; 
    // Space between buttons
    unsigned int spacing;
    // List of actions, every action will create a new button
    std::vector<AnimatedButton> actions;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ButtonControlConfig, iconSize, spacing, actions)

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

        const AnimatedButtonList *getButtonList() const;
        AnimatedButtonList *getButtonList();
    private:
        ButtonControl();

        std::unique_ptr<AnimatedButtonList> buttonList;
};

template<typename CreatorType>
ButtonControl ButtonControl::createInstance(const Control &control, CreatorType &creator) {
    ComponentPropertiesType properties{};

    auto config = control.params.get<ButtonControlConfig>();
    properties["iconSize"] = config.iconSize;
    properties["spacing"] = config.spacing;
    ButtonControl instance{};
    instance.getButtonList()->populate(config.actions);

    creator.template createComponent<AnimatedButtonList>(
        control.name, 
        "ButtonView", 
        properties, 
        reinterpret_cast<AnimatedButtonList *>(instance.getButtonList()));
    return instance;
}