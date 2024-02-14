#pragma once

#include <functional>
#include <utility>
#include <map>
#include <string>
#include <variant>

using ComponentPropertiesType = std::map<std::string, std::variant<std::string, int, unsigned int>>;
// using ComponentCreationFunc = std::function<bool (const std::string &title, const std::string &componentName, const ComponentPropertiesType &properties)>;

// TODO: add parameter check
template<typename ComponentFactoryMethod>
class ComponentCreator {
    public:

        ComponentCreator(ComponentFactoryMethod func);

        template<typename T = void>
        void createComponent(
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
void ComponentCreator<ComponentFactory>::createComponent(
    const std::string &title, 
    const std::string &componentName, 
    const ComponentPropertiesType &properties, 
    T *model) {

    creationFunc(title, componentName, properties, model);
}