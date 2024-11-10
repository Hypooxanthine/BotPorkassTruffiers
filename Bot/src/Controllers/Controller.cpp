#include "Controllers/Controller.h"

Controller::Controller(dpp::cluster& bot)
    : m_Bot(bot)
{
}

Controller::~Controller()
{
}

void Controller::init()
{
    onInit();
}

void Controller::createCommands() const
{
    onCreateCommands();
}

bool Controller::handleSlashCommand(const dpp::slashcommand_t& event)
{
    return onSlashCommand(event);
}