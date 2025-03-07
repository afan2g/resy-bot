#include "config.h"
#include "http.h"
#include "utils.h"
#include <iostream>
#include <thread>
#include <curl/curl.h>

int main()
{
    // Initialize CURL globally
    curl_global_init(CURL_GLOBAL_ALL);

    ConfigStruct *config = parse_config("data/booking_configs/booking_config.json");

    print_config(config);

    std::cout << "current time UTC: " << std::chrono::system_clock::now() << "\n";
    std::cout << "release time UTC: " << config->release_time_point << "\n";
    std::cout << "current time local: " << std::chrono::zoned_time{"America/New_York", std::chrono::system_clock::now()} << "\n";
    std::cout << "release time local: " << std::chrono::zoned_time{"America/New_York", config->release_time_point} << "\n";
    std::string option;
    if (config->is_active)
    {
        std::cout << "What funcion would you like to execute? (1: Get slots, 2: Post booking): ";
        std::cin >> option;

        if (option == "1")
        {
            std::thread wait_thread(wait_for_slots, config);
            std::thread countdown_thread(countdown, config);
            wait_thread.join();
            countdown_thread.join();
        }
        else if (option == "2")
        {
            std::thread wait_thread(wait_until_captcha, config);
            std::thread countdown_thread(countdown, config);
            wait_thread.join();
            countdown_thread.join();
        }
        else
        {
            std::cout << "Invalid option\n";
        }
    }
    else
    {
        std::cout << "Config is not active\n";
    }
    delete config;
    curl_global_cleanup();
    return 0;
}