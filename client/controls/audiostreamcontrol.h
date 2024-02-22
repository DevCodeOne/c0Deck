#pragma once

#include <cstdint>
#include <string>

#include <QAbstractListModel>
#include <QObject>

#include "actions.h"
#include "config.h"

struct AudioStreamControlSettings {
    // Filename of background video
    std::filesystem::path background;
    // Space between list entries
    unsigned int spacing;
    // Height in pixels of one singular entry
    unsigned int controlHeight;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AudioStreamControlSettings, background, spacing, controlHeight)

struct AudioStream {
    std::string name;
    uint8_t volume;
};

class AudioStreamList : public QAbstractListModel {
    Q_OBJECT
    Q_ENUMS(DataRole)
    public:
        enum DataRole {
            Name = Qt::UserRole + 1,
            Volume
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
                case DataRole::Name: 
                    return QString::fromStdString(mData[index.row()].name);
                    break;
                case DataRole::Volume:
                    return QVariant::fromValue(mData[index.row()].volume);
                break;
            }

            return QVariant{}; 
        }

        QHash<int, QByteArray> roleNames() const override {
            return {{ Name, "name"}, { Volume, "volume"}};
        }

        void populate(const std::vector<AudioStream> &init) {
            beginResetModel();
            mData = QVector<AudioStream>(init.cbegin(), init.cend());
            endResetModel();
        }

    private:
        QVector<AudioStream> mData;
};

class AudioStreamControl {
    public:
        static inline constexpr std::string_view type = "AudioStreamControl";

        static void initializeComponent();

        AudioStreamControl(const AudioStreamControl &other) = delete;
        AudioStreamControl(AudioStreamControl &&other) = default;
        ~AudioStreamControl() = default;

        AudioStreamControl &operator=(const AudioStreamControl &other) = delete;
        AudioStreamControl &operator=(AudioStreamControl &&other) = default;

        template<typename CreatorType>
        static AudioStreamControl createInstance(CreatorType &creator, const Control &parameters);

        const Actions *getActionHandler() const;

        const AudioStreamList *getStreamList() const;
        AudioStreamList *getStreamList();

    private:
        template<typename Instance>
        AudioStreamControl(Instance *instance);

        std::unique_ptr<AudioStreamList> streamList;
        std::unique_ptr<Actions> actionHandler;
};

template<typename CreatorType>
AudioStreamControl AudioStreamControl::createInstance(CreatorType &creator, const Control &control) {
    typename CreatorType::PropertiesType properties{};

    auto config = control.params.get<AudioStreamControlSettings>();
    properties["spacing"] = config.spacing;
    properties["controlHeight"] = config.controlHeight;
    properties["background"] = config.background;

    // TODO: add config part if needed
    AudioStreamControl instance{creator.getData()};
    instance.getStreamList()->populate({
        {"Firefox", 55},
        {"Discord", 100},
    });

    auto createdComponent = creator.createComponent(control.name, "AudioStreamControl", properties, instance.getStreamList());

    QObject::connect(createdComponent, SIGNAL(doAction(QString)), instance.getActionHandler(), SLOT(handleAction(QString)));

    return instance;
}

template<typename Instance>
AudioStreamControl::AudioStreamControl(Instance *instance) :
    streamList(std::make_unique<AudioStreamList>()),
    actionHandler(std::make_unique<Actions>(instance->getData().getClientInstance())) {
}
