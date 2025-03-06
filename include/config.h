#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <chrono>

struct ConfigStruct
{
    std::chrono::system_clock::time_point release_time_point;
    std::chrono::system_clock::time_point prefetch_time_point;
    std::string target_date;
    int venue_id = 0;
    std::string venue_name;
    int party_size = 0;
    int template_id = 0;
    int service_type_id = 0;
    bool is_active = false;
    bool is_paid = false;
    bool feature_recaptcha = false;
    int days_in_advance = 0;
    std::string type;
    std::string timeslot;
    std::string rgs_string;
    std::string struct_payment_method;
};

// Function declarations
ConfigStruct *parse_config(const std::string &filename);
void print_config(const ConfigStruct *config);
std::chrono::system_clock::time_point compute_release_time(int release_hour,
                                                           int release_minute,
                                                           int release_second,
                                                           int release_millisecond);

#endif // CONFIG_H