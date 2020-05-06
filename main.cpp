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
#include "swo.h"
#include "l86.h"

using namespace sixtron;

namespace {
#define PERIOD_MS 1000
}

DigitalOut led1(LED1);
SWO swo;
RawSerial uart(UART1_TX, UART1_RX, 9600);
char buf[100];
L86 l86(&uart);

int main()
{
    L86::SatelliteSystems satellite_systems;
    satellite_systems[static_cast<size_t>(L86::SatelliteSystem::GPS)] = true;
    satellite_systems[static_cast<size_t>(L86::SatelliteSystem::GLONASS)] = true;
    l86.set_satellite_system(satellite_systems);

    L86::NmeaCommands nmea_commands;
    nmea_commands[static_cast<size_t>(L86::NmeaCommandType::RMC)] = true;
    l86.set_nmea_output_frequency(nmea_commands, L86::NmeaFrequency::ONE_POSITION_FIX);

    l86.set_navigation_mode(L86::NavigationMode::NORMAL_MODE);
    l86.set_position_fix_interval(10000);
    l86.start(L86::StartMode::HOT_START);

    swo.printf("GPS fixing ...");
    /* Wait that gnss module fixes the communication with satellites */
    while (l86.positionning_mode() == L86::PositionningMode::NO_FIX || l86.positionning_mode() == L86::PositionningMode::UNKNOWN) {
        swo.printf(".");
        ThisThread::sleep_for(500);
    }
    swo.printf("Success\n");

    while (1) {
        ThisThread::sleep_for(50);
        led1 != led1;
        swo.printf("\nLast frame time : %s\n", l86.time());
        swo.printf("Latitude : %s\n", l86.latitude());
        swo.printf("Longitude : %s\n", l86.longitude());
        swo.printf("Speed : %.2f knots\n", l86.speed(L86::SpeedUnit::KNOTS));
    }
}
