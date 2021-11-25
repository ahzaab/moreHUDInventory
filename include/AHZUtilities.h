#pragma once

#include <string>
#include <vector>
#include <io.h> 
//using namespace std;

class CAHZUtilities
{
public:
   static std::vector<std::string> SplitString(std::string& str, std::string& token);
   static std::string& GetSkyrimDataPath();
   static std::string& GetPluginPath();
   static std::string& trim(std::string& s);
   static std::string GetConfigOption(const char * section, const char * key);

   CAHZUtilities();
   ~CAHZUtilities();

private:
   static std::string& rtrim(std::string& s);
   static std::string& ltrim(std::string& s);


};

