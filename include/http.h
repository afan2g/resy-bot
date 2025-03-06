#ifndef HTTP_H
#define HTTP_H

#include <string>
#include <curl/curl.h>
#include "config.h"

struct MemoryStruct
{
    std::string memory;
};

// Function declarations
struct curl_slist *headers(const std::string &filename);
MemoryStruct *get_slots(const std::string &day, int party_size, int venue_id);
MemoryStruct *get_book_token(const ConfigStruct *config);
MemoryStruct *place_booking(const ConfigStruct *config, const std::string &book_token);
std::string parse_book_token_value(MemoryStruct *response);
void print_response_timing(CURL *curl);
void write_response_to_file(const std::string &json_text, const std::string &filename);

// Helper function for curl memory callbacks
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

#endif // HTTP_H