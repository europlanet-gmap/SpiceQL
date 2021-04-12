/**
  * @file
  *
  *
  *
  *
 **/

#include "query.h"
#include "utils.h"

using json = nlohmann::json;
using namespace std;

json searchMissionKernels(fs::path root, string mission) {

  /** 
    * Lambda for globbing files from a regular expression stored 
    * in json. As they can wither be a single expression or a 
    * list, we need to massage the json a little. 
   **/
  auto getPathsFromRegex = [&root](json r) -> vector<fs::path> {
      vector<string> regexes = jsonArrayToVector(r); 
      regex reg(fmt::format("({})", fmt::join(regexes, "|")));
      vector<fs::path> paths = glob(root, reg, true);

      return paths;
  };

  /**
    * Lambda for parsing a CK json object, returns a json object 
    * with a similar structure, but regexes replaces with a path list
   **/
  auto globCks = [&](json category) -> json {
    if(!category.contains("ck")) {
      return (json){};
    }

    category = category["ck"];
    json ret;
    
    for(auto qual: Kernel::QUALITIES) {
      if(!category.contains(qual)) {
        continue; 
      }
      ret[qual] = getPathsFromRegex(category[qual]);
    }

    // pass deps through
    if (category.at("deps").contains("objs")) {
      ret["deps"]["objs"] = category.at("deps").at("objs");
    }
    
    ret["deps"]["sclk"] = getPathsFromRegex(category.at("deps").at("sclk"));
    ret["deps"]["lsk"] = getPathsFromRegex(category.at("deps").at("lsk"));
    return ret;
  };

  /**
    * Lambda for parsing a SPK json object, returns a json object 
    * with a similar structure, but regexes replaces with a path list
   **/
  auto globSpks = [&](json category) -> json {
    if(!category.contains("spk")) {
      return (json){};
    }
    
    category = category["spk"];
    json ret; 

    for(auto qual: Kernel::QUALITIES) {
      if(!category.contains(qual)) {
        continue; 
      }
      ret[qual] = getPathsFromRegex(category[qual]);
    }

    // pass deps through
    if (category.at("deps").contains("objs")) {
      ret["deps"]["objs"] = category.at("deps").at("objs");
    }

    ret["deps"]["sclk"] = getPathsFromRegex(category.at("deps").value("sclk", "$^"));
    ret["deps"]["lsk"] = getPathsFromRegex(category.at("deps").value("lsk", "$^"));

    return ret; 
  };


  /**
    * Lambda for parsing a SPK json object, returns a json object 
    * with a similar structure, but regexes replaces with a path list
   **/
  auto globTspks = [&](json category) -> json {
    if(!category.contains("tspk")) {
      return (json){};
    }
    
    category = category["tspk"];
    json ret; 

    for(auto qual: Kernel::QUALITIES) {
      if(!category.contains(qual)) {
        continue; 
      }
      ret[qual] = getPathsFromRegex(category[qual]);
    }

    if (category.contains("deps")) {
        // pass deps through
        if (category.at("deps").contains("objs")) {
            ret["deps"]["objs"] = category.at("deps").at("objs");
        }
        
        ret["deps"]["sclk"] = getPathsFromRegex(category.at("deps").value("sclk", "$^"));
        ret["deps"]["lsk"] = getPathsFromRegex(category.at("deps").value("lsk", "$^"));
    }

    return ret; 
  };


  /**
    * Lambda for parsing a FK json object, returns a json object 
    * with a similar structure, but regexes replaces with a path list
   **/
  auto globFks = [&](json category) -> json {
    json ret; 
    return getPathsFromRegex(category.value("fk", "$^"));
  };

  /**
    * Lambda for parsing a IK json object, returns a json object 
    * with a similar structure, but regexes replaces with a path list
   **/
  auto globIks = [&](json category) -> json {
    json ret; 
    return getPathsFromRegex(category.value("ik", "$^"));
  };

  /**
    * Lambda for parsing a IAK json object, returns a json object 
    * with a similar structure, but regexes replaces with a path list
   **/
  auto globIaks = [&](json category) -> json {
    json ret; 
    return getPathsFromRegex(category.value("iak", "$^"));
  };

  /**
    * Lambda for parsing a PCK json object, returns a json object 
    * with a similar structure, but regexes replaces with a path list
   **/
  auto globPcks = [&](json category) -> json {
    json ret; 
    return getPathsFromRegex(category.value("pck", "$^"));
  };

  fs::path dbPath = getMissionConfigFile(mission);
  
  ifstream i(dbPath);
  json db;
  i >> db;
  
  // first get any dependencies 
  // string deps = jsonArrayToVector(db[instrument][sType]); 

  json kernels; 

  // iterate the categories (e.g. missions)
  for (auto& cat: db.items()) {
      kernels[cat.key()]["ck"] = globCks(cat.value());
      kernels[cat.key()]["spk"] = globSpks(cat.value());
      kernels[cat.key()]["tspk"] = globTspks(cat.value());
      kernels[cat.key()]["fk"] = globFks(cat.value());
      kernels[cat.key()]["ik"] = globIks(cat.value());
      kernels[cat.key()]["iak"] = globIaks(cat.value());
      kernels[cat.key()]["pck"] = globPcks(cat.value());
  }

  return kernels;
}