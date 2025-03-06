#include "captcha.h"
#include "http.h"
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

std::string get_captcha_from_local()
{
    MemoryStruct *chunk = new MemoryStruct;
    chunk->memory = "";
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "10.0.0.51:5000/captcha");
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)(&chunk->memory));
        res = curl_easy_perform(curl);
    }
    curl_easy_cleanup(curl);
    std::string token = chunk->memory;
    return token;
}

std::string get_captcha_token(std::string &url, std::string &websiteKey)
{
    CURL *curl;
    CURLcode res;

    char two_captcha_url[] = "https://api.2captcha.com";
    std::string client_key = "API_KEY";

    nlohmann::json json_object = {
        {"clientKey", client_key},
        {"task", {{"type", "RecaptchaV2Task"}, {"websiteURL", url}, {"websiteKey", websiteKey}, {"isInvisible", false}, {"userAgent", "useragent"}, {"cookies", "cookie"}, {"proxyType", "http"}, {"proxyAddress", "10.0.0.1"}, {"proxyPort", "8080"}, {"proxyLogin", "user123"}, {"proxyPassword", "password"}}}};
    std::string post_body = nlohmann::to_string(json_object);

    struct curl_slist *headers_list = NULL;
    curl_slist_append(headers_list, "Content-Type: application/json");
    MemoryStruct *chunk = new MemoryStruct;
    chunk->memory = "";
    curl = curl_easy_init();

    std::string captcha_token;
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, two_captcha_url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_body.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers_list);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)(&chunk->memory));

        res = curl_easy_perform(curl);
        nlohmann::json j = nlohmann::json::parse(chunk->memory);
        captcha_token = j["taskId"];
    }

    curl_slist_free_all(headers_list);
    curl_easy_cleanup(curl);
    return captcha_token;
}
