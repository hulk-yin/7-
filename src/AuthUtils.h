#ifndef AUTH_UTILS_H
#define AUTH_UTILS_H

#include <Arduino.h>
#include <mbedtls/md.h>

class AuthUtils
{
public:
    static String assembleAuthUrl(const String &hosturl, const String &apiKey, const String &apiSecret, const struct tm &timeinfo);

private:
    static String _buildSignature(const String &secret, const String &signString);
    static String _urlEncode(const String &value);
    static String _timeToString(const struct tm &timeinfo);
};

#endif // AUTH_UTILS_H