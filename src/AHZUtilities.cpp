#include "PCH.h"

#include "AHZUtilities.h"
#include "windows.h"

std::vector<std::string> CAHZUtilities::SplitString(std::string& str, std::string& token) {
   std::vector<std::string>result;
   while (str.size()) {
      int index = str.find(token);
      if (index != std::string::npos) {
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

std::string & CAHZUtilities::GetSkyrimDataPath()
{
   static std::string s_dataPath;

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

std::string & CAHZUtilities::GetPluginPath()
{
   static std::string s_pluginPath;

   if (s_pluginPath.empty())
   {
      s_pluginPath.append(GetSkyrimDataPath().c_str());
      s_pluginPath.append("SKSE\\Plugins\\");
   }
   return s_pluginPath;
}

// trim from end of string (right)
std::string& CAHZUtilities::rtrim(std::string& s)
{
   s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
   return s;
}

// trim from beginning of string (left)
std::string& CAHZUtilities::ltrim(std::string& s)
{
   s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
   return s;
}

// trim from both ends of string (left & right)
std::string& CAHZUtilities::trim(std::string& s)
{
   return ltrim(rtrim(s));
}


CAHZUtilities::CAHZUtilities()
{
}


CAHZUtilities::~CAHZUtilities()
{
}
