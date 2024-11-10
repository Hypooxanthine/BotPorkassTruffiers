#include <dpp/dpp.h>
#include <iostream>
#include <filesystem>

#include "Commands.h"
#include "Controllers/TimerController.h"
    
int main()
{
    std::filesystem::path keysPath("data/keys");
    std::ifstream file(keysPath / "bot_token.txt");
    std::string botToken;
    if (file.is_open())
    {
        std::getline(file, botToken);
        file.close();
    }
    else
    {
        std::cerr << "Could not open bot_token.txt." << std::endl;
        std::filesystem::create_directories(keysPath);
        std::ofstream newFile(keysPath / "bot_token.txt");
        std::cerr << "File " + (keysPath / "bot_token.txt").string() + " created. Please insert your bot token." << std::endl;
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
        {
            bot.global_command_create(dpp::slashcommand("ping", "Ping the bot", bot.me.id));

            dpp::slashcommand set_timer("set_timer", "Set a timer", bot.me.id);
                set_timer.add_option(dpp::command_option(dpp::co_string, "name", "Timer name. Must be unique.", true));
                set_timer.add_option(dpp::command_option(dpp::co_integer, "interval", "Interval in seconds between each message.", true));
                set_timer.add_option(dpp::command_option(dpp::co_string, "message", "Message to send.", true));
                set_timer.add_option(dpp::command_option(dpp::co_string, "end", "End time of the timer in dd/mm/yy hh:mm:ss format.", true));
                set_timer.add_option(dpp::command_option(dpp::co_string, "start", "Start time of the timer in dd/mm/yy hh:mm:ss format. Default: now.", false));
                set_timer.add_option(dpp::command_option(dpp::co_channel, "channel", "Channel to send the message to. Default: this channel.", false));
            bot.global_command_create(set_timer);
            bot.global_command_create(dpp::slashcommand("list_timers", "List running timers.", bot.me.id));
            dpp::slashcommand stop_timer("stop_timer", "Stop a running timer.", bot.me.id);
                stop_timer.add_option(dpp::command_option(dpp::co_string, "name", "Name of the timer to stop.", true));
            bot.global_command_create(stop_timer);
        }

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