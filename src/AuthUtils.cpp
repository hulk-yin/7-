#include "AuthUtils.h"
#include <base64.h>
#include <Crypto.h>    

  

String AuthUtils::assembleAuthUrl(
    const String &host,
    const String &path,
    const String &apiKey,
    const String &apiSecret,
    const struct tm &timeinfo)
{
  // 解析主机和路径
  

  // 生成时间字符串
  String date = _timeToString(timeinfo);
  printf("date: %s\n", date.c_str());
  // 构建签名字符串
  String signString = "host: " + host + "\n" + "date: " + date + "\n" + "GET " + path + " HTTP/1.1";

  // 生成签名
  String signature = _buildSignature(apiSecret, signString);
  // 构建认证头
  String authParams = "api_key=\"" + apiKey + "\", algorithm=\"hmac-sha256\"" + ", headers=\"host date request-line\"" + ", signature=\"" + signature + "\"";
  String authorization = base64::encode(authParams);
  
  // 生成最终URL
  return "?host=" + _urlEncode(host) +
         "&date=" + _urlEncode(date) +
         "&authorization=" + _urlEncode(authorization);

}


String AuthUtils::_buildHmacSha256Signature(
  const String &secret,
  const String &signString)
{
  SHA256HMAC hmac(reinterpret_cast<const unsigned char *>(secret.c_str()), secret.length());
  hmac.doUpdate(reinterpret_cast<const unsigned char *>(signString.c_str()), signString.length());
  byte hmacResult[32];
  hmac.doFinal(hmacResult);
  String signature = base64::encode(hmacResult, 32);
  signature.replace("\n", "");
  return signature;
}


String AuthUtils::_buildSignature(
    const String &secret,
    const String &signString)
{
  byte hmacResult[32];
  mbedtls_md_context_t ctx;
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
  mbedtls_md_hmac_starts(&ctx,
                         reinterpret_cast<const unsigned char *>(secret.c_str()),
                         secret.length());
  mbedtls_md_hmac_update(&ctx,
                         reinterpret_cast<const unsigned char *>(signString.c_str()),
                         signString.length());
  mbedtls_md_hmac_finish(&ctx, hmacResult);
  mbedtls_md_free(&ctx);

  String signature = base64::encode(hmacResult, 32);
  signature.replace("\n", "");
  return signature;
  // byte hmacResult[32];
  // mbedtls_md_context_t ctx;

  // mbedtls_md_init(&ctx);
  // mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
  // mbedtls_md_hmac_starts(&ctx,
  //                        reinterpret_cast<const unsigned char *>(secret.c_str()),
  //                        secret.length());
  // mbedtls_md_hmac_update(&ctx,
  //                        reinterpret_cast<const unsigned char *>(signString.c_str()),
  //                        signString.length());
  // mbedtls_md_hmac_finish(&ctx, hmacResult);
  // mbedtls_md_free(&ctx);

  // String signature = base64::encode(hmacResult, 32);
  // signature.replace("\n", "");
  // return signature;
}

String AuthUtils::_urlEncode(const String &value)
{
  static const char *hex = "0123456789ABCDEF";
  String encoded = "";
  for (unsigned int i = 0; i < value.length(); i++)
  {
    char c = value[i];
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
    {
      encoded += c;
    }
    else
    {
      encoded += '%';
      encoded += hex[(c >> 4) & 0xF];
      encoded += hex[c & 0xF];
    }
  }
  return encoded;
}

String AuthUtils::_timeToString(const struct tm &timeinfo)
{
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", &timeinfo);
  return String(buffer);
}