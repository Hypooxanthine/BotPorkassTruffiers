#pragma once

#include "Controllers/Controller.h"

class PingController final : public Controller
{
public:
    PingController(dpp::cluster& bot);

protected:
    void onInit() override;

    void onCreateCommands() const override;

    bool onSlashCommand(const dpp::slashcommand_t& event) override;
};