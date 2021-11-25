#include "PCH.h"

#include "AHZConfiguration.h"
#include "AHZUtilities.h"
#include "windows.h"

std::string m_pluginPath;

void CAHZConfiguration::Initialize(const char * pluginName)
{
   m_pluginPath.clear();
   m_pluginPath.append(CAHZUtilities::GetPluginPath().c_str());
   m_pluginPath.append(pluginName);
   m_pluginPath.append(".ini");
}

int CAHZConfiguration::GetIntValue(const char * section, const char * key, int defaultValue)
{
   string strDefault;
   char szDefault[255];
   szDefault[0] = 0;
   int intResult;
   sprintf_s(szDefault, (size_t)255, "%d", defaultValue);
   strDefault.append(szDefault);
   string result = GetStringValue(section, key, strDefault);
   intResult = atoi(result.c_str());
   return intResult;
}

float CAHZConfiguration::GetFloatValue(const char * section, const char * key, float defaultValue)
{
   std::string strDefault;
   char szDefault[255];
   szDefault[0] = 0;
   float_t fltResult;
   sprintf_s(szDefault, (size_t)255, "%f", defaultValue);
   strDefault.append(szDefault);
   string result = GetStringValue(section, key, strDefault);
   fltResult = atof(result.c_str());
   return fltResult;
}

bool CAHZConfiguration::GetBooleanValue(const char * section, const char * key, bool defaultValue)
{
   string strDefault;
   strDefault.append(defaultValue ? "1" : "0");
   string result = GetStringValue(section, key, strDefault);
   return result == "1" ? true : false;
}

std::string CAHZConfiguration::GetStringValue(const char * section, const char * key, std::string& defaultValue)
{
   std::string result = defaultValue;

   char	resultBuf[256];
   resultBuf[0] = 0;

   if (_access_s(m_pluginPath.c_str(), 0) == 0)
   {
      uint32_t	resultLen = GetPrivateProfileStringA(section, key, defaultValue.c_str(), resultBuf, sizeof(resultBuf), m_pluginPath.c_str());
      result = resultBuf;
   }
   return result;
}

CAHZConfiguration::CAHZConfiguration()
{
}

CAHZConfiguration::~CAHZConfiguration()
{
}