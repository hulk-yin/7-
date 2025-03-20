#ifndef AUTH_UTILS_H
#define AUTH_UTILS_H

#include <Arduino.h>
#include <Crypto.h>
#include <SHA256.h>
#include <string.h>

class AuthUtils
{
public:
    static String assembleAuthUrl(const String &host,const String &path
        , const String &apiKey, const String &apiSecret, const struct tm &timeinfo);
    static String generateHmacSha256Signature(const String& data, const String& key);

private:
    static String _buildHmacSha256Signature(const String &secret, const String &signString);
    static String _buildSignature(const String &secret, const String &signString);
    static String _urlEncode(const String &value);
    static String _timeToString(const struct tm &timeinfo);
    static String bytesToHex(const uint8_t* bytes, size_t length);
};

#endif // AUTH_UTILS_H