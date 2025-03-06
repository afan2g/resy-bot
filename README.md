# Resy Booking Automation

A C++ application for automating restaurant reservations on Resy.

## Overview

This application helps automate the process of securing restaurant reservations on Resy as soon as they become available. It handles timing the requests precisely, dealing with captchas if necessary, and processing responses.

## Features

- **Precise Timing**: Schedule reservation attempts with millisecond precision
- **Configurable**: Support for multiple venue configurations
- **Captcha Handling**: Integration with captcha solving services
- **Multi-threaded**: Separate threads for countdown display and reservation logic
- **Detailed Metrics**: Response timing information for performance analysis

## Requirements

- C++20 compatible compiler
- CMake 3.16+
- libcurl
- nlohmann::json


## Installation

1. Clone the repository:
```bash
git clone https://github.com/yourusername/resy-bot.git
cd resy-bot
```

2. Create a build directory and configure with CMake:
```bash
mkdir -p build
cd build
cmake ..
```

3. Build the project:
```bash
make
```

## Usage

1. Configure your booking parameters in `data/booking_configs/your_venue.json`:
```json
{
  "venueId": 12345,
  "venueName": "Restaurant Name",
  "timeslot": "19:00:00",
  "partySize": 2,
  "template": 1,
  "service_type_id": 1,
  "active": true,
  "is_paid": false,
  "feature_recaptcha": false,
  "daysInAdvance": 30,
  "type": "Dining Room",
  "releaseHour": 9,
  "releaseMinute": 0,
  "releaseSecond": 0,
  "struct_payment_method": "{\"id\":12345678}"
}
```

|Key|Value|
|-|-|
|"venueId"|The restaurant's resy venue id|
|"venueName"|The name of the restaurant|
|"timeslot"|Your desired timeslot|
|"partySize"|The size of your party|
|"template"|The template of the booking you want to make|
|"service_type_id"|Resy service type|
|"active"|Enables/disables scheduler|
|"is_paid"|If the booking requires a credit card|
|"feature_recaptcha"|If the booking requires a captcha|
|"daysInAdvance"|How many days out reservations release|
|"type"|What type of seating arrangement|
|"releaseHour", "releaseMinute", "releaseSecond"|The time reservations release at|
|"struct_payment_method"|The payment method to use, if "is_paid" is true|

2. Configure your API headers in `data/headers/`:
   - `GET-slots.json`
   - `POST-rgs.json`
   - `POST-booking.json`

3. Run the application:
```bash
./resy_bot
```

## License

This project is licensed under the MIT License