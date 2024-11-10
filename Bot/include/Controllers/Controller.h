#pragma once

#include <DPP/dpp.h>

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

protected:
    dpp::cluster& m_Bot;
};