#include "src/lib/trojanmap.h"

#include <map>
#include <vector>

#include "gtest/gtest.h"

#include "src/lib/trojanmap.h"

#include <map>
#include <vector>

#include "gtest/gtest.h"

TEST(TrojanMapTest, Autocomplete) {
  TrojanMap m;
  m.CreateGraphFromCSVFile();
  auto names = m.Autocomplete("sC");
  std::vector<std::string> gt1 = {"Schoenfeld Symphonic Hall"};
  EXPECT_EQ(names, gt1);
  names = m.Autocomplete("sdf");
  std::vector<std::string> gt2 = {};
  EXPECT_EQ(names, gt2);
  names = m.Autocomplete("la");
  std::vector<std::string> gt3 = {"LATTCOrtho Institute", "Lawn"};
  EXPECT_EQ(names, gt3);
}

TEST(TrojanMapTest, FindPosition) {
  TrojanMap m;
  m.CreateGraphFromCSVFile();
  auto position = m.GetPosition("ralphs");
  std::pair<double, double> gt1(-1, -1);
  EXPECT_EQ(position, gt1);
  position = m.GetPosition("Lawn");
  std::pair<double, double> gt2(34.02531, -118.28647);
  // EXPECT_DOUBLE_EQ(position.first, gt2.first);
  // EXPECT_DOUBLE_EQ(position.second, gt2.second);
  EXPECT_NEAR(position.first, gt2.first, 1e-5);
  EXPECT_NEAR(position.second, gt2.second, 1e-5);
  position = m.GetPosition("Starbucks");
  std::pair<double, double> gt3(34.018577, -118.28186);
  // EXPECT_EQ(position, gt3);
  // EXPECT_DOUBLE_EQ(position.first, gt3.first);
  // EXPECT_DOUBLE_EQ(position.second, gt3.second);
  EXPECT_NEAR(position.first, gt3.first, 1e-5);
  EXPECT_NEAR(position.second, gt3.second, 1e-5);

}

TEST(TrojanMapTest, CalculateShortestPath) {
  TrojanMap m;
  m.CreateGraphFromCSVFile();
  auto path = m.CalculateShortestPath_spfa("Ralphs", "Target");
  std::vector<std::string> gt{
      "2578244375", "5559640911", "6787470571", "6808093910", "6808093913", "6808093919", "6816831441",
      "6813405269", "6816193784", "6389467806", "6816193783", "123178876", "2613117895", "122719259",
      "6807243574", "6807243576", "213332111", "441895337", "441895335", "122719255", "2613117893",
      "6813405231", "122719216", "6813405232", "4015372486" ,"7071032399" ,"4015372485","6813379479",
      "5237417650"};
  std::cout << "My path length: "  << m.CalculatePathLength(path) << "miles" << std::endl;
  std::cout << "GT path length: " << m.CalculatePathLength(gt) << "miles" << std::endl;
  EXPECT_EQ(path, gt);
  
  path = m.CalculateShortestPath_spfa("Target", "Ralphs");
  std::reverse(gt.begin(),gt.end());
  std::cout << "My path length: "  << m.CalculatePathLength(path) << "miles" << std::endl;
  std::cout << "GT path length: " << m.CalculatePathLength(gt) << "miles" << std::endl;
  EXPECT_EQ(path, gt);
}

TEST(TrojanMapTest, CalculateShortestPath2) {
  TrojanMap m;
  m.CreateGraphFromCSVFile();
  auto path = m.CalculateShortestPath("Ralphs", "Target");
  std::vector<std::string> gt{
      "2578244375", "5559640911", "6787470571", "6808093910", "6808093913", "6808093919", "6816831441",
      "6813405269", "6816193784", "6389467806", "6816193783", "123178876", "2613117895", "122719259",
      "6807243574", "6807243576", "213332111", "441895337", "441895335", "122719255", "2613117893",
      "6813405231", "122719216", "6813405232", "4015372486" ,"7071032399" ,"4015372485","6813379479",
      "5237417650"};
  std::cout << "My path length: "  << m.CalculatePathLength(path) << "miles" << std::endl;
  std::cout << "GT path length: " << m.CalculatePathLength(gt) << "miles" << std::endl;
  EXPECT_EQ(path, gt);
  
  path = m.CalculateShortestPath_spfa("Target", "Ralphs");
  std::reverse(gt.begin(),gt.end());
  std::cout << "My path length: "  << m.CalculatePathLength(path) << "miles" << std::endl;
  std::cout << "GT path length: " << m.CalculatePathLength(gt) << "miles" << std::endl;
  EXPECT_EQ(path, gt);
}

TEST(TrojanMapTest, TSP) {
  TrojanMap m;
  m.CreateGraphFromCSVFile();
  std::vector<std::string> input{"4015405541", "6813513563", "122719213",  "6815190433","122817161"};
  auto result = m.TravellingTrojan_2opt(input);
  std::cout << "My path length: "  << result.first << "miles" << std::endl;
  std::vector<std::string> gt{"4015405541", "6815190433", "122719213", "6813513563", "122817161", "4015405541"};
  std::cout << "GT path length: "  << m.CalculatePathLength(gt) << "miles" << std::endl;
  bool flag = false;
  if (gt == result.second[result.second.size()-1]) // clockwise
    flag = true;
  std::reverse(gt.begin(),gt.end()); // Reverse the expected order because the counterclockwise result is also correct
  if (gt == result.second[result.second.size()-1]) // counterclockwise
    flag = true;
  
  EXPECT_EQ(flag, true);
  // EXPECT_EQ(gt, result.second[result.second.size()-1]);
}

TEST(TrojanMapTest, TSP2) {
  TrojanMap m;
  m.CreateGraphFromCSVFile();
  std::vector<std::string> input{"6813379493", "6389467804", "269633241",  "6813379465", "2193435026","4015477533"};

  auto result = m.TravellingTrojan_2opt(input);
  std::cout << "My path length: " << result.first << "miles" << std::endl;
  std::vector<std::string> gt{ "6813379493", "6813379465", "6389467804", "2193435026", "4015477533", "269633241", "6813379493" };
  std::cout << "GT path length: " << m.CalculatePathLength(gt) << "miles" << std::endl;
  EXPECT_EQ(gt, result.second[result.second.size()-1]);
}


TEST(TrojanMapTest, TSP3) {
  TrojanMap m;
  m.CreateGraphFromCSVFile();
  std::vector<std::string> input{"4015405541", "6813513563", "122719213",  "6815190433","122817161"};
  auto result = m.TravellingTrojan(input);
  std::cout << "My path length: "  << result.first << "miles" << std::endl;
  std::vector<std::string> gt{"4015405541", "6815190433", "122719213", "6813513563", "122817161", "4015405541"};
  std::cout << "GT path length: "  << m.CalculatePathLength(gt) << "miles" << std::endl;
  bool flag = false;
  if (gt == result.second[result.second.size()-1]) // clockwise
    flag = true;
  std::reverse(gt.begin(),gt.end()); // Reverse the expected order because the counterclockwise result is also correct
  if (gt == result.second[result.second.size()-1]) // counterclockwise
    flag = true;
  
  EXPECT_EQ(flag, true);
  // EXPECT_EQ(gt, result.second[result.second.size()-1]);
}

TEST(TrojanMapTest, TSP4) {
  TrojanMap m;
  m.CreateGraphFromCSVFile();
  std::vector<std::string> input{"6813379493", "6389467804", "269633241",  "6813379465", "2193435026","4015477533"};

  auto result = m.TravellingTrojan(input);
  std::cout << "My path length: " << result.first << "miles" << std::endl;
  std::vector<std::string> gt{ "6813379493", "6813379465", "6389467804", "2193435026", "4015477533", "269633241", "6813379493" };
  std::cout << "GT path length: " << m.CalculatePathLength(gt) << "miles" << std::endl;
  EXPECT_EQ(gt, result.second[result.second.size()-1]);
}
