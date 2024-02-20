#pragma once

#include <string>

// TODO: add parameter check
// TODO: should UserData be really used, or is another mechanism better suited ?
template<typename ComponentFactoryMethod, typename UserData, typename P>
class ComponentCreator {
    public:
        using PropertiesType = P;

        ComponentCreator(ComponentFactoryMethod func, UserData *user) : creationFunc(func), data(user) {}

        auto getData() { return data; }

        template<typename T = void>
        auto createComponent(
            const std::string &title,
            const std::string &componentName,
            const PropertiesType &properties,
            T *model = nullptr) {
                return creationFunc(title, componentName, properties, model);
            }
    private:
        ComponentFactoryMethod creationFunc;
        UserData *data; 
};

template<typename PropertiesType>
auto makeComponentCreator(auto callable, auto *userData) {
    return ComponentCreator<decltype(callable), std::remove_pointer_t<decltype(userData)>, PropertiesType>{callable, userData};
}