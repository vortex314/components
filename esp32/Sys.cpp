/*
 * sys.cpp
 *
 *  Created on: Nov 7, 2021
 *      Author: lieven
 */
#include "Sys.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>
#include <esp_mac.h>

#include <string>

#include "Log.h"

std::string __hostname;

uint64_t __millis = 0;

const char *Sys::hostname() {
  if (__hostname.length() == 0) Sys::init();
  return __hostname.c_str();
}
void Sys::hostname(const char *name) { __hostname = name; }

uint64_t Sys::millis() { return Sys::micros() / 1000; }

uint32_t Sys::sec() { return millis() / 1000; }

uint64_t Sys::micros() { return esp_timer_get_time(); }

uint32_t Sys::getFreeHeap() { return xPortGetFreeHeapSize(); };

void Sys::init() {
  std::string hn;
  union {
    uint8_t macBytes[6];
    uint16_t macInt[3];
  };
  if (esp_read_mac(macBytes, ESP_MAC_WIFI_STA) != ESP_OK)
    WARN(" esp_base_mac_addr_get() failed.");
  hn = "ESP32-";
  hn += std::to_string(macInt[2]);
  Sys::hostname(hn.c_str());
}