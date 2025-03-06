#include "utils.h"
#include "http.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <thread>
#include <chrono>

#define LATENCY_OFFSET 15

void countdown(ConfigStruct *config)
{
    // Check if we're already past the prefetch time point
    if (std::chrono::system_clock::now() >= config->prefetch_time_point)
    {
        std::cout << "Prefetch time already reached!\n";
    }
    else
    {
        // Countdown to prefetch
        while (std::chrono::system_clock::now() < config->prefetch_time_point)
        {
            auto time_until_prefetch = config->prefetch_time_point - std::chrono::system_clock::now();

            // Convert to hours, minutes, seconds
            auto hours = std::chrono::duration_cast<std::chrono::hours>(time_until_prefetch).count();
            auto minutes = std::chrono::duration_cast<std::chrono::minutes>(time_until_prefetch % std::chrono::hours(1)).count();
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_until_prefetch % std::chrono::minutes(1)).count();
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time_until_prefetch % std::chrono::seconds(1)).count();
            std::cout << "\rTime until prefetch: "
                      << std::setfill('0') << std::setw(2) << hours << " Hours, "
                      << std::setfill('0') << std::setw(2) << minutes << " minutes, "
                      << std::setfill('0') << std::setw(2) << seconds << "."
                      << std::setfill('0') << std::setw(3) << milliseconds << " seconds" << std::flush;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "\rPrefetch time reached!                                   \n";
    }

    // Check if we're already past the release time point
    if (std::chrono::system_clock::now() >= config->release_time_point)
    {
        std::cout << "Release time already reached!\n";
    }
    else
    {
        // Countdown to release
        while (std::chrono::system_clock::now() < config->release_time_point)
        {
            auto time_until_release = config->release_time_point - std::chrono::system_clock::now();

            // Convert to hours, minutes, seconds
            auto hours = std::chrono::duration_cast<std::chrono::hours>(time_until_release).count();
            auto minutes = std::chrono::duration_cast<std::chrono::minutes>(time_until_release % std::chrono::hours(1)).count();
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_until_release % std::chrono::minutes(1)).count();
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time_until_release % std::chrono::seconds(1)).count();
            std::cout << "\rTime until release: "
                      << std::setfill('0') << std::setw(2) << hours << " Hours, "
                      << std::setfill('0') << std::setw(2) << minutes << " minutes, "
                      << std::setfill('0') << std::setw(2) << seconds << "."
                      << std::setfill('0') << std::setw(3) << milliseconds << " seconds" << std::flush;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "\rRelease time reached!                                   \n";
    }
}

void wait_until_prefetch(ConfigStruct *config)
{
    std::this_thread::sleep_until(config->prefetch_time_point);
    std::cout << "Pre-Release time reached, getting book token...\n";
    MemoryStruct *response = get_book_token(config);
    std::string book_token = parse_book_token_value(response);
    std::cout << "Token: " << book_token << "\n";

    std::chrono::milliseconds latency_offset = std::chrono::milliseconds(LATENCY_OFFSET);
    std::this_thread::sleep_until(config->release_time_point - latency_offset);

    MemoryStruct *booking_response = place_booking(config, book_token);
    if (booking_response)
    {
        write_response_to_file(booking_response->memory, "booking-response.json");
    }
    else
    {
        std::cout << "failed to place booking\n";
    }
    delete booking_response;
    return;
}

// Read an entire file into a std::string.
std::string read_file(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "failed to open file: " << filename << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
