#pragma once 

#include <QAbstractListModel>

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