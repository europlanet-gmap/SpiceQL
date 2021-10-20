#include <fstream>

#include <gtest/gtest.h>

#include "Fixtures.h"

#include "query.h"
#include "utils.h"

using namespace std;
using namespace SpiceQL;


TEST(QueryTests, UnitTestGetLatestKernel) {
  vector<string> kernels = {
    "iak.0001.ti",
    "iak.0003.ti",
    "different/place/iak.0002.ti",
    "test/iak.0004.ti"
  };

  EXPECT_EQ(getLatestKernel(kernels),  "test/iak.0004.ti");
}


TEST(QueryTests, UnitTestGetLatestKernelError) {
  vector<string> kernels = {
    "iak.0001.ti",
    "iak.0003.ti",
    "different/place/iak.0002.ti",
    "test/iak.4.ti",
    // different extension means different filetype and therefore error
    "test/error.tf"
  };

  try {
    getLatestKernel(kernels);
    FAIL() << "expected invalid argument error";
  }
  catch(invalid_argument &e) {
    SUCCEED();
  }
}


TEST_F(KernelDataDirectories, FunctionalTestSearchMissionKernelsAllMess) {
  string dbPath = getMissionConfigFile("mess");

  ifstream i(dbPath);
  nlohmann::json conf;
  i >> conf;

  MockRepository mocks;
  mocks.OnCallFunc(ls).Return(paths);

  nlohmann::json res = searchMissionKernels("/isis_data/", conf);

  ASSERT_EQ(res["mdis"]["ck"]["reconstructed"]["kernels"].size(), 4);
  ASSERT_EQ(res["mdis"]["ck"]["smithed"]["kernels"].size(), 4);
  ASSERT_EQ(res["mdis"]["ck"]["deps"]["objs"].size(), 4);
  ASSERT_EQ(res["mdis"]["spk"]["reconstructed"]["kernels"].size(), 2);
  ASSERT_EQ(res["mdis"]["tspk"]["kernels"].size(), 1);
  ASSERT_EQ(res["mdis"]["fk"]["kernels"].size(), 2);
  ASSERT_EQ(res["mdis"]["ik"]["kernels"].size(), 2);
  ASSERT_EQ(res["mdis"]["iak"]["kernels"].size(), 2);
  ASSERT_EQ(res["mdis"]["pck"]["na"]["kernels"].size(), 2);

  ASSERT_EQ(res["mdis_att"]["ck"]["reconstructed"]["kernels"].size(), 4);
  ASSERT_EQ(res["mdis_att"]["ck"]["deps"]["objs"].size(), 2);

  ASSERT_EQ(res["mess"]["ck"]["reconstructed"]["kernels"].size(), 5);
  ASSERT_EQ(res["mess"]["sclk"]["kernels"].size(), 2);
  ASSERT_EQ(res["mess"]["ck"]["deps"]["objs"].size(), 2);
}


TEST_F(KernelDataDirectories, FunctionalTestSearchMissionKernelsClem1) {
  fs::path dbPath = getMissionConfigFile("clem1");

  ifstream i(dbPath);
  nlohmann::json conf;
  i >> conf;

  MockRepository mocks;
  mocks.OnCallFunc(ls).Return(paths);

  nlohmann::json res = searchMissionKernels("/isis_data/", conf);

  ASSERT_EQ(res["clem1"]["ck"]["reconstructed"]["kernels"].size(), 4);
  ASSERT_EQ(res["clem1"]["ck"]["smithed"]["kernels"].size(), 1);
  ASSERT_EQ(res["clem1"]["ck"]["deps"]["objs"].size(), 2);
  ASSERT_EQ(res["clem1"]["spk"]["reconstructed"]["kernels"].size(), 2);
  ASSERT_EQ(res["clem1"]["fk"]["kernels"].size(), 1);
  ASSERT_EQ(res["clem1"]["sclk"]["kernels"].size(), 2);

  ASSERT_EQ(res["uvvis"]["ik"]["kernels"].size(), 1);
  ASSERT_EQ(res["uvvis"]["iak"]["kernels"].size(), 2);

  ASSERT_EQ(res["uvvis"]["iak"]["kernels"].size(), 2);
}


TEST_F(KernelDataDirectories, FunctionalTestSearchMissionKernelsGalileo) {
  fs::path dbPath = getMissionConfigFile("galileo");

  ifstream i(dbPath);
  nlohmann::json conf;
  i >> conf;

  MockRepository mocks;
  mocks.OnCallFunc(ls).Return(paths);

  nlohmann::json res = searchMissionKernels("/isis_data/", conf);

  ASSERT_EQ(res["galileo"]["ck"]["reconstructed"]["kernels"].size(), 4);
  ASSERT_EQ(res["galileo"]["ck"]["reconstructed"]["deps"].size(), 0);
  ASSERT_EQ(res["galileo"]["ck"]["smithed"]["kernels"].size(), 3);
  ASSERT_EQ(res["galileo"]["ck"]["smithed"]["deps"]["objs"].size(), 1);
  ASSERT_EQ(res["galileo"]["ck"]["deps"]["objs"].size(), 2);
  ASSERT_EQ(res["galileo"]["spk"]["reconstructed"]["kernels"].size(), 2);
  ASSERT_EQ(res["galileo"]["iak"]["kernels"].size(), 1);
  ASSERT_EQ(res["galileo"]["pck"]["smithed"]["kernels"].size(), 2);
  ASSERT_EQ(res["galileo"]["pck"]["smithed"]["deps"].size(), 0);
  ASSERT_EQ(res["galileo"]["pck"]["na"]["kernels"].size(), 1);
  ASSERT_EQ(res["galileo"]["pck"]["na"]["deps"].size(), 0);
  ASSERT_EQ(res["galileo"]["sclk"]["kernels"].size(), 1);
}


TEST_F(KernelDataDirectories, FunctionalTestSearchMissionKernelsApollo16) {
  fs::path dbPath = getMissionConfigFile("apollo16");


  ifstream i(dbPath);
  nlohmann::json conf;
  i >> conf;

  MockRepository mocks;
  mocks.OnCallFunc(ls).Return(paths);

  nlohmann::json res = searchMissionKernels("/isis_data/", conf);

  cout << res << endl;

  ASSERT_EQ(res["apollo16"]["sclk"]["kernels"].size(), 1);
  ASSERT_EQ(res["apollo16"]["ck"]["reconstructed"]["kernels"].size(), 4);

  ASSERT_EQ(res["apollo16"]["spk"]["reconstructed"]["kernels"].size(), 4);
  ASSERT_EQ(res["apollo16"]["fk"]["kernels"].size(), 2);
  ASSERT_EQ(res["metric"]["ik"]["kernels"].size(), 2);
  ASSERT_EQ(res["metric"]["iak"]["kernels"].size(), 1);
  ASSERT_EQ(res["panoramic"]["ik"]["kernels"].size(), 1);
  ASSERT_EQ(res["apollo_pan"]["iak"]["kernels"].size(), 1);
}


TEST_F(KernelDataDirectories, FunctionalTestSearchMissionKernelsJuno) {
  fs::path dbPath = getMissionConfigFile("jno");
  string base = "/isis_data/juno";
  regex base_reg(fmt::format("({})", fmt::join(base, "")));
  vector<string> paths_with_base;

  for (auto path: paths) {
    if (regex_search(path, base_reg)) {
      paths_with_base.push_back(path);
    }
  }

  MockRepository mocks;
  mocks.OnCallFunc(ls).Return(paths_with_base);

  nlohmann::json res = searchMissionKernels(base, conf);
  std::cout << res << '\n';

  ASSERT_EQ(res["juno"]["ck"]["reconstructed"]["kernels"].size(), 4);
  ASSERT_EQ(res["juno"]["ck"]["deps"]["objs"].size(), 2);
  ASSERT_EQ(res["juno"]["spk"]["reconstructed"]["kernels"].size(), 2);
  ASSERT_EQ(res["juno"]["spk"]["deps"]["objs"].size(), 1);
  ASSERT_EQ(res["juno"]["sclk"]["kernels"].size(), 1);
  ASSERT_EQ(res["juno"]["tspk"]["kernels"].size(), 3);
  ASSERT_EQ(res["juno"]["fk"]["kernels"].size(), 1);
  ASSERT_EQ(res["juno"]["ik"]["kernels"].size(), 1);
  ASSERT_EQ(res["juno"]["iak"]["kernels"].size(), 1);
  ASSERT_EQ(res["juno"]["pck"]["na"]["kernels"].size(), 1);
}
