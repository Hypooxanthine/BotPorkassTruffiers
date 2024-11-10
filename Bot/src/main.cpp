#include <dpp/dpp.h>
#include <iostream>
#include <filesystem>

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

    TimerController timerController(bot);
    PingController pingController(bot);
    
    bot.on_log(dpp::utility::cout_logger());
    
    /* The event is fired when someone issues your commands */
    bot.on_slashcommand([&bot, &timerController, &pingController](const dpp::slashcommand_t& event) {
        
        if (pingController.handleSlashCommand(event))
            return;
        else if (timerController.handleSlashCommand(event))
            return;
        else
            event.reply(dpp::message("Unknown command").set_flags(dpp::m_ephemeral));
    });
    
    bot.on_ready([&bot, &timerController, &pingController](const dpp::ready_t& event) {

        if (dpp::run_once<struct init_controllers>())
        {
            timerController.init();
            pingController.init();
        }

        if (dpp::run_once<struct clear_bot_commands>())
            bot.global_bulk_command_delete();

        if (dpp::run_once<struct register_bot_commands>())
        {
            pingController.createCommands();
            timerController.createCommands();
        }
    });
    
    try
    {
        bot.start(dpp::st_wait);
    }
    catch(const dpp::invalid_token_exception& e)
    {
        std::cerr << e.what() << '\n';
        std::cerr << "Please check your bot_token.txt file" << std::endl;
        return 1;
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