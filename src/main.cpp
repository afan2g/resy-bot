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

    ConfigStruct *config = parse_config("data/booking_configs/tatiana.json");

    if (config)
    {
        print_config(config);
        std::cout << "current time UTC: " << std::chrono::system_clock::now() << "\n";
        std::cout << "release time UTC: " << config->release_time_point << "\n";
        std::cout << "captcha fetch time UTC: " << config->release_time_point - std::chrono::seconds(60) << "\n";
        std::cout << "current time local: " << std::chrono::zoned_time{"America/New_York", std::chrono::system_clock::now()} << "\n";
        std::cout << "release time local: " << std::chrono::zoned_time{"America/New_York", config->release_time_point} << "\n";
        std::cout << "captcha fetch time local: " << std::chrono::zoned_time{"America/New_York", config->release_time_point - std::chrono::seconds(60)} << "\n";

        std::thread wait_thread(wait_until_captcha, config);
        std::thread countdown_thread(countdown, config);
        wait_thread.join();
        countdown_thread.join();
    }
    delete config;
    curl_global_cleanup();
    return 0;
}