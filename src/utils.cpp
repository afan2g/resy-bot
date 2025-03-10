#include "utils.h"
#include "http.h"
#include "captcha.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>

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

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
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

void wait_until_captcha(ConfigStruct *config)
{
    // prefetch captcha
    std::this_thread::sleep_until(config->release_time_point - std::chrono::seconds(60));
    std::string captcha_token = get_captcha_from_local();
    std::cout << "Captcha token: " << captcha_token << "\n";

    // wait until release time
    std::this_thread::sleep_until(config->release_time_point);

    // get book token
    MemoryStruct *response = get_book_token(config, captcha_token);
    std::string book_token = parse_book_token_value(response);
    MemoryStruct *booking_response = place_booking(config, book_token);
    if (booking_response)
    {
        write_response_to_file(booking_response->memory, "booking-response.json");
    }
    else
    {
        std::cout << "failed to place booking\n";
    }

    delete response;
    delete booking_response;
    return;
}

void wait_for_slots(ConfigStruct *config)
{
    std::this_thread::sleep_until(config->release_time_point);
    std::cout << "Release time reached, retrieving slots...\n";

    MemoryStruct *response = get_slots(config);

    // Retry loop for fetching slots
    for (int i = 0; i < 5; i++)
    {
        if (!response || response->memory.empty())
        {
            std::cerr << "Failed to get slots (null response or empty memory)\n";
        }
        else
        {
            try
            {
                nlohmann::json j = nlohmann::json::parse(response->memory);

                // Ensure structure is valid before accessing nested fields
                if (j.contains("results") && j["results"].contains("venues") &&
                    !j["results"]["venues"].empty() && j["results"]["venues"][0].contains("slots") &&
                    !j["results"]["venues"][0]["slots"].empty())
                {
                    std::cout << "Slots found!\n";
                    break; // Exit loop if slots are found
                }
                else
                {
                    std::cerr << "No slots available\n";
                }
            }
            catch (const nlohmann::json::parse_error &e)
            {
                std::cerr << "JSON parse error: " << e.what() << "\n";
            }
            catch (const nlohmann::json::type_error &e)
            {
                std::cerr << "JSON type error: " << e.what() << "\n";
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
        response = get_slots(config); // Retry fetching slots
    }

    // Ensure response is valid before writing to file
    if (response && !response->memory.empty())
    {
        write_response_to_file(response->memory, "slots-response.json");
    }
    else
    {
        std::cerr << "No valid response to write to file.\n";
    }
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
