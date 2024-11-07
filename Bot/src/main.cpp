#include <dpp/dpp.h>
#include <iostream>

#include "Commands.h"
#include "Timer.h"
	 
const std::string BOT_TOKEN = "MTMwMzc5ODg3NzE0MDQxODU3MA.GbzGeD.9wfe7F4cb3xdyswms3Z33QaQH8myV62AoaP-Z4";
constexpr auto CHANNEL_GENERAL = 1303418258661179547;
    
int main()
{
    std::list<Timer> timers;

    /* Setup the bot */
    dpp::cluster bot(BOT_TOKEN);
    
    bot.on_log(dpp::utility::cout_logger());
    
    /* The event is fired when someone issues your commands */
    bot.on_slashcommand([&bot, &timers](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "ping")
        {
            event.reply(dpp::message("Pong!").set_flags(dpp::m_ephemeral));
        }
        else if (event.command.get_command_name() == "set_timer")
        {
            dpp::snowflake channel;
            std::visit(
                [&channel, &event](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, dpp::snowflake>)
                        channel = arg;
                    if constexpr (std::is_same_v<T, std::monostate>)
                        channel = event.command.channel_id;
                },
                event.get_parameter("channel")
            );

            int64_t interval = std::get<int64_t>(event.get_parameter("interval"));
            std::string message = std::get<std::string>(event.get_parameter("message"));
            std::string start = std::get<std::string>(event.get_parameter("start"));
            std::string end = std::get<std::string>(event.get_parameter("end"));

            auto startTime = Timer::ParseTime(start);
            auto endTime = Timer::ParseTime(end);

            if (!startTime || !endTime)
            {
                event.reply(dpp::message("Invalid time format.").set_flags(dpp::m_ephemeral));
                return;
            }

            auto& timer = timers.emplace_back(bot, channel, interval, message, *startTime, *endTime);
            auto it = std::prev(timers.end());
            timer.onEnd([it, &timers]() {
                timers.erase(it);
            });
            timer.start();

            std::string msg = "Timer started with message \"" + message + "\".\n";
            msg += " Start: " + Timer::GetFormattedTime(*startTime) + '\n';
            msg += " End: " + Timer::GetFormattedTime(*endTime) + '\n';
            msg += " Interval: " + std::to_string(interval) + " seconds.";

            event.reply(dpp::message(msg).set_flags(dpp::m_ephemeral));
        }
    });
    
    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct clear_bot_commands>())
            bot.global_bulk_command_delete();

        if (dpp::run_once<struct register_bot_commands>())
        {
            bot.global_command_create(dpp::slashcommand("ping", "Ping the bot", bot.me.id));

            dpp::slashcommand set_timer("set_timer", "Set a timer", bot.me.id);
            set_timer.add_option(dpp::command_option(dpp::co_integer, "interval", "Interval in seconds between each message.", true));
            set_timer.add_option(dpp::command_option(dpp::co_string, "message", "Message to send.", true));
            set_timer.add_option(dpp::command_option(dpp::co_string, "start", "Start time of the timer in dd/mm/yy hh:mm:ss format.", true));
            set_timer.add_option(dpp::command_option(dpp::co_string, "end", "End time of the timer in dd/mm/yy hh:mm:ss format.", true));
            set_timer.add_option(dpp::command_option(dpp::co_channel, "channel", "Channel to send the message to. Default: this channel.", false));
            bot.global_command_create(set_timer);
        }
    });
    
    bot.start(dpp::st_wait);
    
    return 0;
}