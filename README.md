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

## Handling Captcha

There are two ways to handle captchas, if the reservation requires them.

### Locally Hosted Captcha Solver

You can run [this python solver](https://github.com/sarperavci/GoogleRecaptchaBypass/tree/selenium) on a local flask server. The bot will query the server for a captcha token when needed. You will need to change the IP and port to your own configuration specifications.

### Using a third-party Captcha Solver

There exists a few third-party captcha solving service APIs. Included in this source is an untested and incomplete implementation of 2captcha's API.

## Installation

1. Clone the repository:
```bash
git clone https://github.com/afan2g/resy-bot.git
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

## Booking Paramters

Several booking parameters are needed to preprocess your reservation. You should use the network monitor to obtain the response data. Below are the routes where you can find the data required.

|Key|Value|Route|
|-|-|-|
|**venueId**|The restaurant's resy venue id||
|**venueName**|The name of the restaurant||
|**timeslot**|Your desired timeslot||
|**partySize**|The size of your party||
|**template**|The booking template|api.resy.com/4/find?=|
|**service_type_id**|Resy service type|api.resy.com/4/find?=|
|**active**|Enables/disables scheduler||
|**is_paid**|If the booking requires a credit card|api.resy.com/4/find?=|
|**feature_recaptcha**|If the booking requires a captcha|api.resy.com/4/find?=|
|**daysInAdvance**|How many days out reservations release||
|**type**|The type of seating arrangement|api.resy.com/4/find?=|
|**releaseHour**, **releaseMinute**, **releaseSecond**|The time reservations are released|
|**struct_payment_method**|The payment method to use|api.resy.com/2/user|

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