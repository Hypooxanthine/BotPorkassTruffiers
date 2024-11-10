#include "Controllers/PingController.h"

PingController::PingController(dpp::cluster& bot)
    : Controller(bot)
{
}

PingController::~PingController()
{
}

void PingController::onInit()
{
    m_Bot.log(dpp::ll_info, "PingController initialized");
}

void PingController::onCreateCommands() const
{
    m_Bot.global_command_create(dpp::slashcommand("ping", "Ping the bot", m_Bot.me.id));
    
    m_Bot.log(dpp::ll_info, "Commands created");
}

bool PingController::onSlashCommand(const dpp::slashcommand_t& event)
{
    bool handled = true;
    
    if (event.command.get_command_name() == "ping")
    {
        event.reply(dpp::message("Pong!").set_flags(dpp::m_ephemeral));
    }
    else
        handled = false;

    return handled;
}