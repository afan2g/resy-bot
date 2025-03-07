#ifndef UTILS_H
#define UTILS_H

#include "config.h"
#include <string>

void countdown(ConfigStruct *config);
void wait_until_prefetch(ConfigStruct *config);
void wait_until_captcha(ConfigStruct *config);
void wait_for_slots(ConfigStruct *config);
std::string read_file(const std::string &filename);

#endif // UTILS_H