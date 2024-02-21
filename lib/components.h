#pragma once

#include <string_view>
#include <tuple>
#include <variant>
#include <vector>
#include <utility>

template<typename T>
struct Components;

template<typename ... T>
struct Components<std::tuple<T ...>>;

namespace Detail {
    template<typename ComponentList>
    class ComponentRegistry;

    // TODO: do this differently don't use std::tuple for this, instead use custom type
    template<typename ... ComponentTypes>
    class ComponentRegistry<Components<std::tuple<ComponentTypes ...>>> {
        public:
            using ComponentType = std::variant<ComponentTypes ..., std::monostate>;

            static void initializeComponents(auto && ... arguments);
            template<typename Parameter, typename Callable>
            bool createInstance(std::string_view name, const Parameter &controlDescription, Callable callable);

            ComponentType get(std::string_view name);
        private:
            std::vector<ComponentType> componentInstances;

            template<typename Callable, size_t ... Indices>
            static void iterateWithIndex(Callable callable, std::index_sequence<Indices ...> indices) {
                (callable(std::integral_constant<size_t, Indices>{}), ...);
            }
    };

    template<typename Callable>
    struct IgnoreMonoState {
        void operator()(std::monostate) {};
        template<typename T>
        void operator()(T arg) { callable(arg); }

        Callable callable;
    };

    template<typename ... ComponentTypes>
    void ComponentRegistry<Components<std::tuple<ComponentTypes ...>>>::initializeComponents(auto && ... arguments) {
        iterateWithIndex([&](auto Index) constexpr {
            using CurrentType = std::tuple_element_t<decltype(Index)::value, std::tuple<ComponentTypes ...>>;
            CurrentType::initializeComponent(std::forward<decltype(arguments)>(arguments) ...);
        }, std::make_index_sequence<sizeof...(ComponentTypes)>{});
    }

    // TODO: do the same parameter forward trick as used in the initializeComponent func
    template<typename ... ComponentTypes>
    template<typename Parameter, typename ComponentCreator>
    bool ComponentRegistry<Components<std::tuple<ComponentTypes ...>>>::createInstance(
        std::string_view typeName, 
        const Parameter &param, ComponentCreator creator) {
        ComponentType createdComponent{std::monostate{}};

        auto initFunc = [&param, &typeName, &createdComponent, &creator](auto Index) {
            using CurrentType = std::tuple_element_t<decltype(Index)::value, std::tuple<ComponentTypes ...>>;
            if (CurrentType::type == typeName) {
                // Yes create this type
                createdComponent = CurrentType::createInstance(param, creator);
            }
        };

        IgnoreMonoState<decltype(initFunc)> handler{ .callable = initFunc };
        iterateWithIndex(handler, std::make_index_sequence<sizeof...(ComponentTypes)>{});

        if (std::holds_alternative<std::monostate>(createdComponent)) {
            return false;
        }

        componentInstances.push_back(std::move(createdComponent));
        return true;
    }
}

template<typename T>
struct Components { };

template<typename ... T>
struct Components<std::tuple<T ...>> { };