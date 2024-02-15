#pragma once

#include <functional>
#include <utility>
#include <map>
#include <string>
#include <variant>
#include <filesystem>

using ComponentPropertiesType = std::map<std::string, std::variant<std::string, std::filesystem::path, int, unsigned int>>;

// TODO: add parameter check
template<typename ComponentFactoryMethod>
class ComponentCreator {
    public:

        ComponentCreator(ComponentFactoryMethod func);

        template<typename T = void>
        auto createComponent(
            const std::string &title,
            const std::string &componentName,
            const ComponentPropertiesType &properties,
            T *model = nullptr);
    private:
        ComponentFactoryMethod creationFunc;
};

template<typename ComponentFactory>
ComponentCreator(ComponentFactory func) -> ComponentCreator<ComponentFactory>;

template<typename ComponentFactory>
ComponentCreator<ComponentFactory>::ComponentCreator(ComponentFactory func) : creationFunc(func) { }

template<typename ComponentFactory>
template<typename T>
auto ComponentCreator<ComponentFactory>::createComponent(
    const std::string &title, 
    const std::string &componentName, 
    const ComponentPropertiesType &properties, 
    T *model) {

    return creationFunc(title, componentName, properties, model);
}