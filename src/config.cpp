#include "config.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <nlohmann/json.hpp>
void print_config(const ConfigStruct *config)
{
    std::cout << "release_time_point: " << config->release_time_point << "\n";
    std::cout << "target_date: " << config->target_date << "\n";
    std::cout << "venue_id: " << config->venue_id << "\n";
    std::cout << "venue_name: " << (config->venue_name.empty() ? "NULL" : config->venue_name) << "\n";
    std::cout << "party_size: " << config->party_size << "\n";
    std::cout << "timeslot: " << config->timeslot << "\n";
    std::cout << "template_id: " << config->template_id << "\n";
    std::cout << "service_type_id: " << config->service_type_id << "\n";
    std::cout << "is_active: " << config->is_active << "\n";
    std::cout << "is_paid: " << config->is_paid << "\n";
    std::cout << "feature_recaptcha: " << config->feature_recaptcha << "\n";
    std::cout << "days_in_advance: " << config->days_in_advance << "\n";
    std::cout << "type: " << config->type << "\n";
    std::cout << "rgs: " << config->rgs_string << "\n";
    std::cout << "struct_payment_method: " << config->struct_payment_method << "\n";
}

// Parse the configuration JSON file and return a pointer to a ConfigStruct.
ConfigStruct *parse_config(const std::string &filename)
{
    ConfigStruct *config = new ConfigStruct();

    std::ifstream f(filename, std::ios::in);
    nlohmann::json json_data = nlohmann::json::parse(f);
    f.close();

    config->venue_id = json_data["venueId"];
    config->venue_name = json_data["venueName"];
    config->timeslot = json_data["timeslot"];
    config->type = json_data["type"];
    config->struct_payment_method = json_data["struct_payment_method"];
    config->days_in_advance = json_data["daysInAdvance"];
    config->release_time_point = compute_release_time(json_data["releaseHour"], json_data["releaseMinute"], json_data["releaseSecond"], 0);
    config->is_active = json_data["active"];
    config->feature_recaptcha = json_data["feature_recaptcha"];
    config->is_paid = json_data["is_paid"];
    config->party_size = json_data["partySize"];
    config->template_id = json_data["template"];
    config->service_type_id = json_data["service_type_id"];

    std::chrono::system_clock::time_point target_tp = config->release_time_point + std::chrono::days(config->days_in_advance);
    config->prefetch_time_point = config->release_time_point - std::chrono::minutes(3);
    config->target_date = std::format("{:%Y-%m-%d}", target_tp);
    std::string rgs_buffer = "rgs://resy/" + std::to_string(config->venue_id) + "/" +
                             std::to_string(config->template_id) + "/" +
                             std::to_string(config->service_type_id) + "/" +
                             config->target_date + "/" +
                             config->target_date + "/" +
                             config->timeslot + "/" +
                             std::to_string(config->party_size) + "/" +
                             config->type;
    config->rgs_string = rgs_buffer;

    return config;
}

std::chrono::system_clock::time_point compute_release_time(int release_hour, int release_minute, int release_second, int release_millisecond)
{
    using namespace std::chrono;
    // Locate the New York time zone.
    auto ny_zone = locate_zone("America/New_York");

    // Get the current time (UTC) and create a zoned_time for New York.
    system_clock::time_point now = system_clock::now();
    zoned_time ny_time{ny_zone, now};

    // Round down New York local time to today's midnight.
    auto local_midnight = floor<days>(ny_time.get_local_time());

    // Build the release time in New York local time.
    auto release_local = local_midnight + hours{release_hour} + minutes{release_minute} + seconds{release_second} + milliseconds{release_millisecond};

    // If the release time is already past in New York, schedule it for the next day.
    if (release_local < ny_time.get_local_time())
        release_local += days{1};

    // Convert the New York local time to a system_clock (UTC) time_point.
    system_clock::time_point release_utc = ny_zone->to_sys(release_local);
    return release_utc;
}