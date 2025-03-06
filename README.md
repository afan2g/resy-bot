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

## Project Structure

```
resy-booking/
├── include/                  # Header files
│   ├── config.h              # Configuration structures and functions
│   ├── http.h                # HTTP request handling and CURL wrappers
│   ├── utils.h               # Utility functions
│   └── captcha.h             # Captcha handling functionality
├── src/                      # Source files
│   ├── config.cpp            # Configuration parsing implementation
│   ├── http.cpp              # HTTP request implementation
│   ├── utils.cpp             # Utility functions implementation
│   ├── captcha.cpp           # Captcha service implementation
│   └── main.cpp              # Main application entry point
├── data/                     # Data files
│   ├── headers/              # HTTP headers for various requests
│   │   ├── GET-slots.json
│   │   ├── POST-rgs.json
│   │   └── POST-booking.json
│   └── booking_configs/      # Booking configuration files
│       └── massara.json      # Specific venue configuration
├── build/                    # Build output directory
├── CMakeLists.txt            # CMake configuration
└── README.md                 # This file
```

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