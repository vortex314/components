
#include "Log.h"

#include <assert.h>
#include <string.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
std::string time_in_HH_MM_SS_MMM()
{
	using namespace std::chrono;

	// get current time
	auto now = system_clock::now();

	// get number of milliseconds for the current second
	// (remainder after division into seconds)
	auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

	// convert to std::time_t in order to convert to std::tm (broken time)
	auto timer = system_clock::to_time_t(now);

	// convert to broken time
	std::tm bt = *std::localtime(&timer);

	std::ostringstream oss;

	oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
	oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

	return oss.str();
}

Log::Log()
{
}

Log &Log::printf(const char *format, ...)
{
	if (txBusy)
		return *this;
	if (offset > sizeof(_buffer))
		return *this;
	va_list args;
	va_start(args, format);
	offset +=
		vsnprintf(&Log::_buffer[offset], sizeof(Log::_buffer) - offset, format, args);
	va_end(args);
	return *this;
}

void Log::flush()
{
	if (txBusy)
		return;
	if (offset >= (sizeof(_buffer) - 2))
		return;
	Log::_buffer[offset++] = '\r';
	Log::_buffer[offset++] = '\n';
	txBusy = true;
	fwrite((uint8_t *)Log::_buffer, offset, 1, stdout);
	offset = 0;
	txBusy = false;
}

Log &Log::tfl(const char *file, const uint32_t line)
{
	if (txBusy)
		return *this;
	uint32_t t = Sys::millis();
	uint32_t sec = t / 1000;
	uint32_t min = sec / 60;
	uint32_t hr = min / 60;
	offset =
		snprintf(_buffer, sizeof(_buffer), "%2.2u:%2.2u:%2.2u.%3.3u | %s:%4u | ", hr % 24, min % 60, sec % 60, t % 1000, file, line);
	return *this;
}

LogWriter *Log::setWriter(LogWriter f)
{
	return 0;
}

void Log::setLevel(char l)
{
	switch (l)
	{
	case 'I':
	{
		_level = L_INFO;
		break;
	}
	case 'D':
	{
		_level = L_DEBUG;
		break;
	}
	case 'E':
	{
		_level = L_ERROR;
		break;
	}
	case 'W':
	{
		_level = L_WARN;
		break;
	}
	default:
	{
		WARN(" unknwown log level");
	}
	}
}
