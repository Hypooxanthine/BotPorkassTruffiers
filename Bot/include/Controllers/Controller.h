#pragma once

#include <dpp/dpp.h>

class Controller
{
public:
    Controller(dpp::cluster& bot);

    virtual ~Controller();

    Controller(const Controller&) = delete;

    Controller& operator=(const Controller&) = delete;

    Controller(Controller&&) = default;

    Controller& operator=(Controller&&) = default;

    void init();

    void createCommands() const;

    bool handleSlashCommand(const dpp::slashcommand_t& event);

protected:

    virtual void onInit() = 0;

    virtual void onCreateCommands() const = 0;

    virtual bool onSlashCommand(const dpp::slashcommand_t& event) = 0;

    bool isParamDefined(const dpp::slashcommand_t& event, const std::string& name) const
    {
        return !std::holds_alternative<std::monostate>(event.get_parameter(name));
    }

    template <typename T>
    T getParam(const dpp::slashcommand_t& event, const std::string& name) const
    {
        return std::get<T>(event.get_parameter(name));
    }

    template <typename T>
    T getParamOr(const dpp::slashcommand_t& event, const std::string& name, const T& defaultValue) const
    {
        T value;

        std::visit(
            [&value, &defaultValue](auto&& arg) {
                using Arg = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Arg>)
                    value = arg;
                else
                    value = defaultValue;
            },
            event.get_parameter(name)
        );

        return value;
    }

    template <typename C>
    std::invoke_result_t<C> getParamOrCall(const dpp::slashcommand_t& event, const std::string& name, const C& function) const
    {
        using T = std::invoke_result_t<C>;
        T value;

        std::visit(
            [&value, &function](auto&& arg) {
                using Arg = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Arg>)
                    value = arg;
                else
                    value = function();
            },
            event.get_parameter(name)
        );

        return value;
    }

protected:
    dpp::cluster& m_Bot;
};