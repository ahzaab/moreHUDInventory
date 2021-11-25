#include "PCH.h"

#include "AHZUtilities.h"
#include "windows.h"

vector<string> CAHZUtilities::SplitString(string& str, string& token) {
   vector<string>result;
   while (str.size()) {
      int index = str.find(token);
      if (index != string::npos) {
         result.push_back(str.substr(0, index));
         str = str.substr(index + token.size());
         if (str.size() == 0)result.push_back(str);
      }
      else {
         result.push_back(str);
         str = "";
      }
   }
   return result;
}

string & CAHZUtilities::GetSkyrimDataPath()
{
   static string s_dataPath;

   if (s_dataPath.empty())
   {
      auto hModule = GetModuleHandle(nullptr);
      if (hModule != nullptr)
      {
         char skyrimPath[_MAX_PATH];
         char skyrimDir[_MAX_DIR];
         char skyrimDrive[_MAX_DRIVE];
         // Use GetModuleFileName() with module handle to get the path
         GetModuleFileNameA(hModule, skyrimPath, (sizeof(skyrimPath)));

         _splitpath_s(
            (const char*)skyrimPath,
            &skyrimDrive[0],
            (size_t)sizeof(skyrimDrive),
            &skyrimDir[0],
            (size_t)sizeof(skyrimDir),
            NULL,
            0,
            NULL,
            0
         );

         s_dataPath.append(skyrimDrive);
         s_dataPath.append(skyrimDir);
         s_dataPath.append("Data\\");
      }
   }
   return s_dataPath;
}

string & CAHZUtilities::GetPluginPath()
{
   static string s_pluginPath;

   if (s_pluginPath.empty())
   {
      s_pluginPath.append(GetSkyrimDataPath().c_str());
      s_pluginPath.append("SKSE\\Plugins\\");
   }
   return s_pluginPath;
}

// trim from end of string (right)
string& CAHZUtilities::rtrim(string& s)
{
   s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
   return s;
}

// trim from beginning of string (left)
string& CAHZUtilities::ltrim(string& s)
{
   s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
   return s;
}

// trim from both ends of string (left & right)
string& CAHZUtilities::trim(string& s)
{
   return ltrim(rtrim(s));
}


CAHZUtilities::CAHZUtilities()
{
}


CAHZUtilities::~CAHZUtilities()
{
}
