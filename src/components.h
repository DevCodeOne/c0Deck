#pragma once

#include <bits/utility.h>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>
#include <utility>

#include "build_config.h"
#include "config.h"

template<typename T>
struct Components;

template<typename ... T>
struct Components<std::tuple<T ...>>;

namespace Detail {
    template<typename ComponentList>
    class ComponentRegistry;

    template<typename ... ComponentTypes>
    class ComponentRegistry<Components<std::tuple<ComponentTypes ...>>> {
        public:
            using ComponentType = std::variant<ComponentTypes ..., std::monostate>;

            static void initializeComponents();
            template<typename Callable>
            bool createInstance(const Control &controlDescription, Callable callable);

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
    void ComponentRegistry<Components<std::tuple<ComponentTypes ...>>>::initializeComponents() {
        iterateWithIndex([](auto Index) constexpr {
            std::tuple_element_t<decltype(Index)::value, std::tuple<ComponentTypes ...>>::initializeComponent();
        }, std::make_index_sequence<sizeof...(ComponentTypes)>{});
    }

    template<typename ... ComponentTypes>
    template<typename ComponentCreator>
    bool ComponentRegistry<Components<std::tuple<ComponentTypes ...>>>::createInstance(const Control &control, ComponentCreator creator) {
        ComponentType createdComponent{std::monostate{}};

        auto initFunc = [&control, &createdComponent, &creator](auto Index) {
            using CurrentType = std::tuple_element_t<decltype(Index)::value, std::tuple<ComponentTypes ...>>;
            if (CurrentType::type == control.type) {
                // Yes create this type
                createdComponent = CurrentType::createInstance(control, creator);
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

using ComponentRegistry = Detail::ComponentRegistry<Components<ComponentList>>;