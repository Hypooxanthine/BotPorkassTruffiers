#include <iostream>
#include <filesystem>
#include <vector>

#include <dpp/dpp.h>

#include "Commands.h"
#include "Controllers/TimerController.h"
#include "Controllers/PingController.h"

/**
 * @brief Get the bot token from file.
 * 
 * @return std::string The bot token.
 * 
 * @throw std::runtime_error if the file could not be opened.
 */
std::string GetBotToken();
    
int main()
{
    /* Setup the bot */
    dpp::cluster bot(GetBotToken());

    std::vector<std::unique_ptr<Controller>> controllers;
    controllers.push_back(std::make_unique<PingController>(bot));
    controllers.push_back(std::make_unique<TimerController>(bot));
    
    bot.on_log(dpp::utility::cout_logger());
    
    /* The event is fired when someone issues your commands */
    bot.on_slashcommand([&bot, &controllers](const dpp::slashcommand_t& event) {

        for (const auto& controller : controllers)
        {
            if (controller->handleSlashCommand(event))
                return;
        }

        event.reply(dpp::message("Unknown command").set_flags(dpp::m_ephemeral));
    });
    
    bot.on_ready([&bot, &controllers](const dpp::ready_t& event) {

        if (dpp::run_once<struct init_controllers>())
        {
            for (const auto& controller : controllers)
                controller->init();

            bot.log(dpp::ll_info, "Controllers initialized");
        }

        if (dpp::run_once<struct clear_bot_commands>())
        {
            bot.global_bulk_command_delete();
            bot.log(dpp::ll_info, "Commands cleared");
        }

        if (dpp::run_once<struct register_bot_commands>())
        {
            for (const auto& controller : controllers)
                controller->createCommands();

            bot.log(dpp::ll_info, "Commands registered");
        }
    });
    
    try
    {
        bot.start(dpp::st_wait);
    }
    catch(...)
    {
        std::cerr << "Invalid token. Please check bot_token.txt" << std::endl;
        throw;
    }
    
    return 0;
}

std::string GetBotToken()
{
    auto keysPath = std::filesystem::path("data") / "keys";
    std::ifstream file(keysPath / "bot_token.txt");
    std::string botToken;
    if (file.is_open())
    {
        std::getline(file, botToken);
        file.close();

        if (botToken.empty())
            throw std::runtime_error("Please insert your bot token in file " + (keysPath / "bot_token.txt").string());
    }
    else
    {
        std::filesystem::create_directories(keysPath);
        std::ofstream newFile(keysPath / "bot_token.txt");

        throw std::runtime_error("Could not open bot_token.txt. File " + (keysPath / "bot_token.txt").string() + " created. Please insert your bot token.");
    }

    return botToken;
}