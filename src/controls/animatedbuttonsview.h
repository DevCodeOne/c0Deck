#pragma once 

#include <string>
#include <string_view>

#include <QAbstractListModel>

#include "config.h"
#include "controls/componentcreator.h"

class AnimatedButton {
    public:
        AnimatedButton() = default;
        AnimatedButton(const QString &gifName, const QString &action) : mGifName(gifName), mAction(action) {}
    private:
        QString mGifName;
        QString mAction;
};

class ButtonList : public QAbstractListModel {
    Q_OBJECT
    public:
        ButtonList(QObject *parent = nullptr) {}
        int rowCount(const QModelIndex &parent = QModelIndex()) const { return 1; }
        int columnCount(const QModelIndex &parent = QModelIndex()) const { return 1; }
        QVariant data(const QModelIndex &index, int role) const { return QVariant{}; }
        void populate() { }
    private:
        QList<AnimatedButton> mData;
};

class ButtonControl {
    public:
        static inline constexpr std::string_view type = "ButtonControl";

        static void initializeComponent();

        ButtonControl(const ButtonControl &other) = delete;
        ButtonControl(ButtonControl &&other) = default;
        ~ButtonControl() = default;

        ButtonControl &operator=(const ButtonControl &other) = delete;
        ButtonControl &operator=(ButtonControl &&other) = default;

        static ButtonControl createInstance(const Control &parameters, ComponentCreator &creator);
    private:
        ButtonControl() = default;
};