#include <dpp/dpp.h>
#include <iostream>
#include <filesystem>

#include "Commands.h"
#include "Controllers/TimerController.h"
    
int main()
{
    auto keysPath = std::filesystem::path("data") / "keys";
    std::ifstream file(keysPath / "bot_token.txt");
    std::string botToken;
    if (file.is_open())
    {
        std::getline(file, botToken);
        file.close();

        if (botToken.empty())
        {
            std::cerr << "Please insert your bot token in file " << (keysPath / "bot_token.txt").string() << std::endl;
            return 1;
        }
    }
    else
    {
        std::cerr << "Could not open bot_token.txt." << std::endl;
        std::filesystem::create_directories(keysPath);
        std::ofstream newFile(keysPath / "bot_token.txt");
        std::cerr << "File " << (keysPath / "bot_token.txt").string() << " created. Please insert your bot token." << std::endl;
        return 1;
    }

    /* Setup the bot */
    dpp::cluster bot(botToken);

    TimerController timerController(bot);
    
    bot.on_log(dpp::utility::cout_logger());
    
    /* The event is fired when someone issues your commands */
    bot.on_slashcommand([&bot, &timerController](const dpp::slashcommand_t& event) {
        
        if (event.command.get_command_name() == "ping")
        {
            event.reply(dpp::message("Pong!").set_flags(dpp::m_ephemeral));
        }
        else if (timerController.handleSlashCommand(event))
            return;
        else
            event.reply(dpp::message("Unknown command").set_flags(dpp::m_ephemeral));
    });
    
    bot.on_ready([&bot, &timerController](const dpp::ready_t& event) {
        if (dpp::run_once<struct clear_bot_commands>())
            bot.global_bulk_command_delete();

        if (dpp::run_once<struct register_bot_commands>())
            timerController.createCommands();

        timerController.init();
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