#include "http.h"
#include "utils.h"
#include "captcha.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <nlohmann/json.hpp>

// This callback is used by libcurl to write incoming data into our std::string
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    std::string *mem = reinterpret_cast<std::string *>(userp);
    mem->append(reinterpret_cast<char *>(contents), realsize);
    return realsize;
}

// Print timing information from a CURL handle
void print_response_timing(CURL *curl)
{
    double namelookup_time, connect_time, appconnect_time, pretransfer_time, starttransfer_time, total_time;
    curl_easy_getinfo(curl, CURLINFO_NAMELOOKUP_TIME, &namelookup_time);
    curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME, &connect_time);
    curl_easy_getinfo(curl, CURLINFO_APPCONNECT_TIME, &appconnect_time);
    curl_easy_getinfo(curl, CURLINFO_PRETRANSFER_TIME, &pretransfer_time);
    curl_easy_getinfo(curl, CURLINFO_STARTTRANSFER_TIME, &starttransfer_time);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);

    std::cout << "\n--- Timing Metrics ---\n";
    std::cout << "DNS Lookup Time       : " << namelookup_time << " seconds\n";
    std::cout << "TCP Connect Time      : " << connect_time << " seconds\n";
    std::cout << "SSL Handshake Time    : " << appconnect_time << " seconds\n";
    std::cout << "Pre-transfer Time     : " << pretransfer_time << " seconds\n";
    std::cout << "Time Until First Byte : " << starttransfer_time << " seconds (Latency)\n";
    std::cout << "Total Request Time    : " << total_time << " seconds\n";
}

// Write a string (typically a JSON response) to a file
void write_response_to_file(const std::string &json_text, const std::string &filename)
{
    std::ofstream file(filename);
    if (!file)
    {
        std::cerr << "Failed to open file: " << filename << "\n";
        return;
    }
    file << json_text;
    if (!file)
    {
        std::cerr << "Failed to write to file: " << filename << "\n";
    }
    else
    {
        std::cout << "JSON data written to " << filename << " successfully.\n";
    }
}

// Parse a JSON file of headers and return a curl_slist pointer
struct curl_slist *headers(const std::string &filename)
{
    std::string data = read_file(filename);
    if (data.empty())
    {
        return nullptr;
    }

    nlohmann::json json_object = nlohmann::json::parse(data);
    struct curl_slist *headersList = nullptr;

    for (auto &el : json_object.items())
    {
        std::string header = el.key() + ": " + el.value().get<std::string>();
        headersList = curl_slist_append(headersList, header.c_str());
    }

    return headersList;
}

// Get available slots (performs a GET request) and returns the response wrapped in MemoryStruct
MemoryStruct *get_slots(ConfigStruct *config)
{
    struct curl_slist *headers_list = headers("data/headers/GET-slots.json");
    if (!headers_list)
    {
        std::cerr << "Failed to load headers\n";
        return nullptr;
    }

    MemoryStruct *chunk = new MemoryStruct();
    chunk->memory = "";

    std::string url = "https://api.resy.com/4/find?lat=0&long=0&day=" + config->target_date +
                      "&party_size=" + std::to_string(config->party_size) +
                      "&venue_id=" + std::to_string(config->venue_id);

    CURL *curl = curl_easy_init();
    CURLcode res;

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers_list);
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)(&chunk->memory));

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
            delete chunk;
            curl_slist_free_all(headers_list);
            curl_easy_cleanup(curl);
            return nullptr;
        }
        else
        {
            print_response_timing(curl);
            std::cout << chunk->memory.size() << " bytes retrieved\n";
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "Failed to initialize curl\n";
        delete chunk;
        curl_slist_free_all(headers_list);
        return nullptr;
    }

    curl_slist_free_all(headers_list);
    return chunk;
}

// Get a booking token by sending a POST request
MemoryStruct *get_book_token(const ConfigStruct *config, const std::string &captcha_token)
{
    struct curl_slist *headers_list = headers("data/headers/POST-rgs.json");
    if (!headers_list)
    {
        std::cerr << "Failed to load headers\n";
        return nullptr;
    }

    MemoryStruct *chunk = new MemoryStruct();
    chunk->memory = "";
    std::string url = "https://api.resy.com/3/details";

    int commit = 1;
    std::string post_body;

    nlohmann::json json_object = {
        {"config_id", config->rgs_string},
        {"commit", commit},
    };
    if (!captcha_token.empty())
    {
        json_object["captcha_token"] = captcha_token;
    }
    else if (config->feature_recaptcha)
    {
        json_object["captcha_token"] = get_captcha_from_local();
    }

    post_body = json_object.dump();
    std::cout << "post body: " << post_body << "\n";

    CURL *curl = curl_easy_init();
    CURLcode res;

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers_list);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_body.c_str());
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)(&chunk->memory));

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
            delete chunk;
            curl_slist_free_all(headers_list);
            curl_easy_cleanup(curl);
            return nullptr;
        }
        else
        {
            print_response_timing(curl);
            std::cout << chunk->memory.size() << " bytes retrieved\n";
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "Failed to initialize curl\n";
        delete chunk;
        curl_slist_free_all(headers_list);
        return nullptr;
    }

    curl_slist_free_all(headers_list);
    return chunk;
}

// Place a booking by sending a POST request with the booking token
MemoryStruct *place_booking(const ConfigStruct *config, const std::string &book_token)
{
    struct curl_slist *headers_list = headers("data/headers/POST-booking.json");

    MemoryStruct *chunk = new MemoryStruct();
    chunk->memory = "";

    CURL *curl = curl_easy_init();
    CURLcode res;

    if (curl)
    {
        // Escape the book token to be URL safe
        char *escaped_book_token = curl_easy_escape(curl, book_token.c_str(), book_token.length());
        std::string post_data;

        if (config->is_paid)
        {
            char *escaped_struct_payment_method = curl_easy_escape(curl,
                                                                   config->struct_payment_method.c_str(),
                                                                   config->struct_payment_method.length());
            post_data = "book_token=" + std::string(escaped_book_token) +
                        "&struct_payment_method=" +
                        std::string(escaped_struct_payment_method) +
                        "&source_id=resy.com-venue-details" +
                        "&venue_marketing_opt_in=0";
            curl_free(escaped_struct_payment_method);
        }
        else
        {
            post_data = "book_token=" + std::string(escaped_book_token) +
                        "&source_id=resy.com-venue-details"
                        "&venue_marketing_opt_in=0";
        }
        curl_free(escaped_book_token);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post_data.length());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers_list);
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.resy.com/3/book");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)(&chunk->memory));

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
            delete chunk;
            curl_slist_free_all(headers_list);
            curl_easy_cleanup(curl);
            return nullptr;
        }
        else
        {
            print_response_timing(curl);
            std::cout << chunk->memory.size() << " bytes retrieved\n";
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "Failed to initialize curl\n";
        delete chunk;
        curl_slist_free_all(headers_list);
        return nullptr;
    }

    curl_slist_free_all(headers_list);
    return chunk;
}

// Parse the booking token value from a JSON response.
std::string parse_book_token_value(MemoryStruct *response)
{
    write_response_to_file(response->memory, "book-token-response.json");
    nlohmann::json j = nlohmann::json::parse(response->memory);
    std::string token = j["book_token"]["value"];
    delete response;
    return token;
}