#pragma once

#include <DPP/dpp.h>

class Controller
{
public:
    Controller(dpp::cluster& bot) : m_Bot(bot) {}

    virtual ~Controller() = default;

    Controller(const Controller&) = delete;

    Controller& operator=(const Controller&) = delete;

    Controller(Controller&&) = delete;

    Controller& operator=(Controller&&) = delete;

    inline void init() { onInit(); }

    inline void createCommands() const { onCreateCommands(); }

    inline bool handleSlashCommand(const dpp::slashcommand_t& event) { return onSlashCommand(event); }

protected:

    virtual void onInit() = 0;

    virtual void onCreateCommands() const = 0;

    virtual bool onSlashCommand(const dpp::slashcommand_t& event) = 0;

protected:
    dpp::cluster& m_Bot;
};