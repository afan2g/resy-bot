#ifndef CAPTCHA_H
#define CAPTCHA_H

#include <string>

std::string get_captcha_from_local();
std::string get_captcha_token(std::string &url, std::string &websiteKey);

#endif // CAPTCHA_H