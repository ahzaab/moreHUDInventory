#pragma once

#include <string>

using namespace std;

class CAHZConfiguration
{
public:

   void Initialize(const char* pluginName);
   int GetIntValue(const char* section, const char * key, int defaultValue = 0);
   float GetFloatValue(const char* section, const char * key, float defaultValue = 0.0);
   bool GetBooleanValue(const char* section, const char * key, bool defaultValue = false);
   string GetStringValue(const char* section, const char * key, string& defaultValue = string(""));
   CAHZConfiguration();
   ~CAHZConfiguration();

private:
   string m_pluginPath;

};

extern CAHZConfiguration g_ahzConfiguration;

