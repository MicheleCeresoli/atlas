
#include "settings.h"

WorldOptions::WorldOptions() : WorldOptions("", "") {}

WorldOptions::WorldOptions(const std::string& dem, const std::string& dom) : 
    WorldOptions(std::vector<std::string>{dem}, std::vector<std::string>{dom}) {}

WorldOptions::WorldOptions(const std::vector<std::string>& dem, const std::string& dom) : 
    WorldOptions(dem, std::vector<std::string>{dom}) {}

WorldOptions::WorldOptions(const std::string& dem, const std::vector<std::string>& dom) : 
    WorldOptions(std::vector<std::string>{dem}, dom) {}

WorldOptions::WorldOptions(
    const std::vector<std::string>& dem, const std::vector<std::string>& dom
) : demFiles(dem), domFiles(dom) {}
