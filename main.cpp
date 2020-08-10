/*
 * Copyright (c) 2017-2020, CATIE
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed.h"
#include "l86.h"

namespace {
#define PERIOD_MS 250
}

DigitalOut led1(LED1);
BufferedSerial uart(UART1_TX, UART1_RX, 9600);
L86 l86(&uart);

int main()
{
    L86::SatelliteSystems satellite_systems;
    satellite_systems[static_cast<size_t>(L86::SatelliteSystem::GPS)] = true;
    satellite_systems[static_cast<size_t>(L86::SatelliteSystem::GLONASS)] = true;
    if (l86.set_satellite_system(satellite_systems)) {
        printf("Satellite Systems OK \n");
    }

    L86::NmeaCommands nmea_commands;
    nmea_commands[static_cast<size_t>(L86::NmeaCommandType::RMC)] = true;
    if (l86.set_nmea_output_frequency(nmea_commands, L86::NmeaFrequency::ONE_POSITION_FIX)) {
        printf("Nmea output frequency OK\n");
    }
    if (l86.set_navigation_mode(L86::NavigationMode::NORMAL_MODE)) {
        printf("Navigation mode OK\n");
    }
    if (l86.set_position_fix_interval(1000)) {
        printf("Position fix interval OK\n");
    }
    if (l86.start(L86::StartMode::HOT_START)) {
        printf("Start Ok\n");
    }

    while (1) {
        ThisThread::sleep_for(PERIOD_MS);
        led1 = !led1;
        time_t current_time = l86.time();
        printf("Time: %s\n", asctime(gmtime(&current_time)));
        printf("Latitude: %f\n", l86.latitude());
        printf("Longitude: %f\n", l86.longitude());
        printf("Speed: %f\n", l86.speed());
    }
}
