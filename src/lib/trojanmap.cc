#include "trojanmap.h"

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <fstream>
#include <locale>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <utility>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"

//-----------------------------------------------------
// TODO (Students): You do not and should not change the following functions:
//-----------------------------------------------------

/**
 * PrintMenu: Create the menu
 * 
 */
void TrojanMap::PrintMenu() {

  std::string menu =
      "**************************************************************\n"
      "* Select the function you want to execute.                    \n"
      "* 1. Autocomplete                                             \n"
      "* 2. Find the position                                        \n"
      "* 3. CalculateShortestPath                                    \n"
      "* 4. Travelling salesman problem                              \n"
      "* 5. Exit                                                     \n"
      "**************************************************************\n";
  std::cout << menu << std::endl;
  std::string input;
  getline(std::cin, input);
  char number = input[0];
  switch (number)
  {
  case '1':
  {
    menu =
        "**************************************************************\n"
        "* 1. Autocomplete                                             \n"
        "**************************************************************\n";
    std::cout << menu << std::endl;
    menu = "Please input a partial location:";
    std::cout << menu;
    getline(std::cin, input);
    auto results = Autocomplete(input);
    menu = "*************************Results******************************\n";
    std::cout << menu;
    if (results.size() != 0) {
      for (auto x : results) std::cout << x << std::endl;
    } else {
      std::cout << "No matched locations." << std::endl;
    }
    menu = "**************************************************************\n";
    std::cout << menu << std::endl;
    PrintMenu();
    break;
  }
  case '2':
  {
    menu =
        "**************************************************************\n"
        "* 2. Find the position                                        \n"
        "**************************************************************\n";
    std::cout << menu << std::endl;
    menu = "Please input a location:";
    std::cout << menu;
    getline(std::cin, input);
    auto results = GetPosition(input);
    menu = "*************************Results******************************\n";
    std::cout << menu;
    if (results.first != -1) {
      std::cout << "Latitude: " << results.first
                << " Longitude: " << results.second << std::endl;
      PlotPoint(results.first, results.second);
    } else {
      std::cout << "No matched locations." << std::endl;
    }
    menu = "**************************************************************\n";
    std::cout << menu << std::endl;
    PrintMenu();
    break;
  }
  case '3':
  {
    menu =
        "**************************************************************\n"
        "* 3. CalculateShortestPath                                            "
        "      \n"
        "**************************************************************\n";
    std::cout << menu << std::endl;
    menu = "Please input the start location:";
    std::cout << menu;
    std::string input1;
    getline(std::cin, input1);
    menu = "Please input the destination:";
    std::cout << menu;
    std::string input2;
    getline(std::cin, input2);
    std::string sw;
    std::cout << "**************************************************************" << std::endl;
    std::cout << "*1.Dijkstra" << std::endl << "*2.spfa" << std::endl;
    std::cout << "**************************************************************" << std::endl;
    getline(std::cin, sw);
    std::vector<std::string> results;
    if (sw == "1") results = CalculateShortestPath(input1, input2);
    else results = CalculateShortestPath_spfa(input1, input2);
    menu = "*************************Results******************************\n";
    std::cout << menu;
    if (results.size() != 0) {
      for (auto x : results) std::cout << x << std::endl;
      PlotPath(results);
    } else {
      std::cout << "No route from the start point to the destination."
                << std::endl;
    }
    menu = "**************************************************************\n";
    std::cout << menu << std::endl;
    PrintMenu();
    break;
  }
  case '4':
  {
    menu =
        "**************************************************************\n"
        "* 4. Travelling salesman problem                              \n"
        "**************************************************************\n";
    std::cout << menu << std::endl;
    menu = "In this task, we will select N random points on the map and you need to find the path to travel these points and back to the start point.";
    std::cout << menu << std::endl << std::endl;
    menu = "Please input the number of the places:";
    std::cout << menu;
    getline(std::cin, input);
    std::string sw;
    std::cout << "**************************************************************" << std::endl;
    std::cout << "*1.DFS" << std::endl << "*2.2opt" << std::endl;
    std::cout << "**************************************************************" << std::endl;
    getline(std::cin, sw);
    std::pair<double, std::vector<std::vector<std::string>>>  results;

    int num = std::stoi(input);
    std::vector<std::string> keys;
    for (auto x : data) {
      keys.push_back(x.first);
    }
    std::vector<std::string> locations;
    srand(time(NULL));
    for (int i = 0; i < num; i++)
      locations.push_back(keys[rand() % keys.size()]);
    PlotPoints(locations);
    std::cout << "Calculating ..." << std::endl;
    if (sw == "1") results = TravellingTrojan(locations);
    else results = TravellingTrojan_2opt(locations);
    // auto results = TravellingTrojan(locations);
    menu = "*************************Results******************************\n";
    std::cout << menu;
    CreateAnimation(results.second);
    if (results.second.size() != 0) {
      for (auto x : results.second[results.second.size()-1]) std::cout << x << std::endl;
      menu = "**************************************************************\n";
      std::cout << menu;
      std::cout << "The distance of the path is:" << results.first << std::endl;
      PlotPath(results.second[results.second.size()-1]);
    } else {
      std::cout << "The size of the path is 0" << std::endl;
    }
    menu = "**************************************************************\n"
           "You could find your animation at src/lib/output.avi.          \n";
    std::cout << menu << std::endl;
    PrintMenu();
    break;
  }
  case '5':
    break;
  default:
    std::cout << "Please select 1 - 5." << std::endl;
    PrintMenu();
    break;
  }
}


/**
 * CreateGraphFromCSVFile: Read the map data from the csv file
 * 
 */
void TrojanMap::CreateGraphFromCSVFile() {
  std::fstream fin;
  fin.open("src/lib/map.csv", std::ios::in);
  std::string line, word;

  getline(fin, line);
  while (getline(fin, line)) {
    std::stringstream s(line);

    Node n;
    int count = 0;
    while (getline(s, word, ',')) {
      word.erase(std::remove(word.begin(), word.end(), '\''), word.end());
      word.erase(std::remove(word.begin(), word.end(), '"'), word.end());
      word.erase(std::remove(word.begin(), word.end(), '['), word.end());
      word.erase(std::remove(word.begin(), word.end(), ']'), word.end());
      if (count == 0)
        n.id = word;
      else if (count == 1)
        n.lat = stod(word);
      else if (count == 2)
        n.lon = stod(word);
      else if (count == 3)
        n.name = word;
      else {
        word.erase(std::remove(word.begin(), word.end(), ' '), word.end());
        n.neighbors.push_back(word);
      }
      count++;
    }
    data[n.id] = n;
  }
  fin.close();
}

/**
 * PlotPoint: Given a location id, plot the point on the map
 * 
 * @param  {std::string} id : location id
 */
void TrojanMap::PlotPoint(std::string id) {
  std::string image_path = cv::samples::findFile("src/lib/input.jpg");
  cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);
  auto result = GetPlotLocation(data[id].lat, data[id].lon);
  cv::circle(img, cv::Point(result.first, result.second), DOT_SIZE,
             cv::Scalar(0, 0, 255), cv::FILLED);
  cv::imshow("TrojanMap", img);
  cv::waitKey(1);
}
/**
 * PlotPoint: Given a lat and a lon, plot the point on the map
 * 
 * @param  {double} lat : latitude
 * @param  {double} lon : longitude
 */
void TrojanMap::PlotPoint(double lat, double lon) {
  std::string image_path = cv::samples::findFile("src/lib/input.jpg");
  cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);
  auto result = GetPlotLocation(lat, lon);
  cv::circle(img, cv::Point(int(result.first), int(result.second)), DOT_SIZE,
             cv::Scalar(0, 0, 255), cv::FILLED);
  cv::startWindowThread();
  cv::imshow("TrojanMap", img);
  cv::waitKey(1);
}

/**
 * PlotPath: Given a vector of location ids draws the path (connects the points)
 * 
 * @param  {std::vector<std::string>} location_ids : path
 */
void TrojanMap::PlotPath(std::vector<std::string> &location_ids) {
  std::string image_path = cv::samples::findFile("src/lib/input.jpg");
  cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);
  auto start = GetPlotLocation(data[location_ids[0]].lat, data[location_ids[0]].lon);
  cv::circle(img, cv::Point(int(start.first), int(start.second)), DOT_SIZE,
             cv::Scalar(0, 0, 255), cv::FILLED);
  for (auto i = 1; i < location_ids.size(); i++) {
    auto start = GetPlotLocation(data[location_ids[i - 1]].lat, data[location_ids[i - 1]].lon);
    auto end = GetPlotLocation(data[location_ids[i]].lat, data[location_ids[i]].lon);
    cv::circle(img, cv::Point(int(end.first), int(end.second)), DOT_SIZE,
               cv::Scalar(0, 0, 255), cv::FILLED);
    cv::line(img, cv::Point(int(start.first), int(start.second)),
             cv::Point(int(end.first), int(end.second)), cv::Scalar(0, 255, 0),
             LINE_WIDTH);
  }
  cv::startWindowThread();
  cv::imshow("TrojanMap", img);
  cv::waitKey(1);
}

/**
 * PlotPoints: Given a vector of location ids draws the points on the map (no path).
 * 
 * @param  {std::vector<std::string>} location_ids : points
 */
void TrojanMap::PlotPoints(std::vector<std::string> &location_ids) {
  std::string image_path = cv::samples::findFile("src/lib/input.jpg");
  cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);
  for (auto x : location_ids) {
    auto result = GetPlotLocation(data[x].lat, data[x].lon);
    cv::circle(img, cv::Point(result.first, result.second), DOT_SIZE,
               cv::Scalar(0, 0, 255), cv::FILLED);
  }
  cv::imshow("TrojanMap", img);
  cv::waitKey(1);
}


/**
 * CreateAnimation: Create the videos of the progress to get the path
 * 
 * @param  {std::vector<std::vector<std::string>>} path_progress : the progress to get the path
 */
void TrojanMap::CreateAnimation(std::vector<std::vector<std::string>> path_progress){
  cv::VideoWriter video("src/lib/output.avi", cv::VideoWriter::fourcc('M','J','P','G'), 10, cv::Size(1248,992));
  for(auto location_ids: path_progress) {
    std::string image_path = cv::samples::findFile("src/lib/input.jpg");
    cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);
    auto start = GetPlotLocation(data[location_ids[0]].lat, data[location_ids[0]].lon);
    cv::circle(img, cv::Point(int(start.first), int(start.second)), DOT_SIZE,
              cv::Scalar(0, 0, 255), cv::FILLED);
    for (auto i = 1; i < location_ids.size(); i++) {
      auto start = GetPlotLocation(data[location_ids[i - 1]].lat, data[location_ids[i - 1]].lon);
      auto end = GetPlotLocation(data[location_ids[i]].lat, data[location_ids[i]].lon);
      cv::circle(img, cv::Point(int(end.first), int(end.second)), DOT_SIZE,
                cv::Scalar(0, 0, 255), cv::FILLED);
      cv::line(img, cv::Point(int(start.first), int(start.second)),
              cv::Point(int(end.first), int(end.second)), cv::Scalar(0, 255, 0),
              LINE_WIDTH);
    }
    video.write(img);
    cv::imshow("TrojanMap", img);
    cv::waitKey(1);
  }
	video.release();
}
/**
 * GetPlotLocation: Transform the location to the position on the map
 * 
 * @param  {double} lat         : latitude 
 * @param  {double} lon         : longitude
 * @return {std::pair<double, double>}  : position on the map
 */
std::pair<double, double> TrojanMap::GetPlotLocation(double lat, double lon) {
  std::pair<double, double> bottomLeft(34.01001, -118.30000);
  std::pair<double, double> upperRight(34.03302, -118.26502);
  double h = upperRight.first - bottomLeft.first;
  double w = upperRight.second - bottomLeft.second;
  std::pair<double, double> result((lon - bottomLeft.second) / w * 1248,
                                   (1 - (lat - bottomLeft.first) / h) * 992);
  return result;
}

//-----------------------------------------------------
// TODO: Student should implement the following:
//-----------------------------------------------------
/**
 * GetLat: Get the latitude of a Node given its id.
 * 
 * @param  {std::string} id : location id
 * @return {double}         : latitude
 */
double TrojanMap::GetLat(std::string id) { 
  init();
  return data[id].lat;
  }

/**
 * GetLon: Get the longitude of a Node given its id. 
 * 
 * @param  {std::string} id : location id
 * @return {double}         : longitude
 */
double TrojanMap::GetLon(std::string id) { 
  init();
  return data[id].lon;  
}

/**
 * GetName: Get the name of a Node given its id.
 * 
 * @param  {std::string} id : location id
 * @return {std::string}    : name
 */
std::string TrojanMap::GetName(std::string id) { 
  init();
  return data[id].name;
}

/**
 * GetNeighborIDs: Get the neighbor ids of a Node.
 * 
 * @param  {std::string} id            : location id
 * @return {std::vector<std::string>}  : neighbor ids
 */
std::vector<std::string> TrojanMap::GetNeighborIDs(std::string id) {
    std::vector<std::string> result;
    result = data[id].neighbors;
    return result;
}


/**
 * CalculateDistance: Get the distance between 2 nodes. 
 * 
 * @param  {Node} a  : node a
 * @param  {Node} b  : node b
 * @return {double}  : distance in mile
 */
double TrojanMap::CalculateDistance(const Node &a, const Node &b) {
  // TODO: Use Haversine Formula:
  // dlon = lon2 - lon1;
  // dlat = lat2 - lat1;
  // a = (sin(dlat / 2)) ^ 2 + cos(lat1) * cos(lat2) * (sin(dlon / 2)) ^ 2;
  // c = 2 * arcsin(min(1, sqrt(a)));
  // distances = 3961 * c;

  // where 3961 is the approximate radius of the earth at the latitude of
  // Washington, D.C., in miles
  double lat1 = a.lat, lon1 = a.lon, lat2 = b.lat, lon2 = b.lon;
  double dLat = (lat2 - lat1) * M_PI / 180.0; 
  double dLon = (lon2 - lon1) * M_PI / 180.0; 

  lat1 = (lat1) * M_PI / 180.0; 
  lat2 = (lat2) * M_PI / 180.0; 

  double aa = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2); 
  double rad = 3961; 
  double c = 2 * asin(sqrt(aa)); 
  return rad * c; 
}

/**
 * CalculatePathLength: Calculates the total path length for the locations inside the vector.
 * 
 * @param  {std::vector<std::string>} path : path
 * @return {double}                        : path length
 */
double TrojanMap::CalculatePathLength(const std::vector<std::string> &path) {
  double sum = 0;
  if (path.size() < 2) return sum;
  for (int i = 1; i < path.size(); ++i)
    sum += CalculateDistance(data[path[i - 1]], data[path[i]]);
  return sum;
}

void TrojanMap::init() {
  if (name2node.empty()) {
    for (const auto& [k, v] : data) {
        name2node[v.name] = v;
    }
    // std::cout << name2node.size() << std::endl;
    int id = 1;
    for (const auto& [k, v] : data) {
      id2num[k] = id;
      num2id[id] = k;
      ++id;
    }
    // std::cout << id2num.size() << std::endl;
    // std::cout << "here" << std::endl;
    G = std::vector<std::vector<double>>(3000, std::vector<double>(3000, 100000000.0));
    for (const auto& [k, v] : data) {
      int a = id2num[k];
      for (const auto& ne : v.neighbors) {
        int b = id2num[ne];
        // std::cout << a << ' ' << b << std::endl;
        G[a][b] = G[b][a] = CalculateDistance(v, data[ne]);
      }
    }
  }
}

/**
 * Autocomplete: Given a parital name return all the possible locations with
 * partial name as the prefix.
 *
 * @param  {std::string} name          : partial name
 * @return {std::vector<std::string>}  : a vector of full names
 */
std::vector<std::string> TrojanMap::Autocomplete(std::string name) {
  std::vector<std::string> results;
  init();
  auto starttime = clock();
  for (char &c : name) c = std::tolower(c);

  for (const auto& [s, v] : name2node) {
    bool flag = true;
    for (int i = 0, j = 0; i < name.size(); ++i, ++j) {
      if (j >= s.size()) {
        flag = false;
        break;
      }
      if (std::tolower(s[j]) != name[i]) {
        flag = false;
        break;
      }
    }
    if (flag) results.push_back(s);
  }
  auto endtime = clock();
  std::cout << "autocomplete runtime: " << (double)(endtime - starttime) / CLOCKS_PER_SEC << 's' <<std::endl;
  return results;
}

/**
 * GetPosition: Given a location name, return the position.
 *
 * @param  {std::string} name          : location name
 * @return {std::pair<double,double>}  : (lat, lon)
 */
std::pair<double, double> TrojanMap::GetPosition(std::string name) {
  std::pair<double, double> results(-1, -1);
  init();
  auto starttime = clock();
  if (name2node.find(name) != name2node.end()) {
    auto t = name2node[name];
    results = std::make_pair(t.lat, t.lon);
  }
  auto endtime = clock();
  std::cout << "Getposition runtime: " << (double)(endtime - starttime) / CLOCKS_PER_SEC << 's' <<std::endl;
  return results;
}

/**
 * CalculateShortestPath: Given 2 locations, return the shortest path which is a
 * list of id.
 *
 * @param  {std::string} location1_name     : start
 * @param  {std::string} location2_name     : goal
 * @return {std::vector<std::string>}       : path
 */
std::vector<std::string> TrojanMap::CalculateShortestPath(
    std::string location1_name, std::string location2_name) {
  
  init();

  auto starttime = clock();
  
  if (name2node.find(location1_name) == name2node.end() || name2node.find(location2_name) == name2node.end()) return {};
  std::vector<std::string> x;
  std::vector<double> dist(3000, 100000000.0);
  std::vector<bool> st(3000, false);
  std::vector<int> pre(3000, -1);
  int start = id2num[name2node[location1_name].id];
  int end = id2num[name2node[location2_name].id];
  // std::cout << start << ' ' << end << std::endl;
  dist[start] = 0;
  for (int i = 0; i < 2237; ++i) {
    int j = -1;
    for (int k = 1; k <= 2237; ++k) {
      if (!st[k] && (j == -1 || dist[k] < dist[j]))
        j = k;
    }
    if (j == -1) break;
    // std::cout << j << std::endl;
    for (int k = 1; k <= 2237; ++k)
      if (dist[j] + G[j][k] < dist[k]) {
        pre[k] = j;
        dist[k] = dist[j] + G[j][k];
      }
    st[j] = true;
  }
  // std::cout << "here" << std::endl;
  if (dist[end] > 100000000.0 / 2) return {};
  int tmp = end;
  while (tmp != start) {
    x.push_back(num2id[tmp]);
    tmp = pre[tmp];
  }
  x.push_back(num2id[start]);
  std::reverse(x.begin(), x.end());

  auto endtime = clock();
  std::cout << "Dijikstra runtime: " << (double)(endtime - starttime) / CLOCKS_PER_SEC << 's' <<std::endl;
  
  return x;
}

std::vector<std::string> TrojanMap::CalculateShortestPath_spfa(
    std::string location1_name, std::string location2_name) {
  init();

  auto starttime = clock();
  if (name2node.find(location1_name) == name2node.end() || name2node.find(location2_name) == name2node.end()) return {};
  std::vector<std::string> x;
  std::vector<double> dist(3000, 100000000.0);
  std::vector<bool> st(3000, false);
  std::vector<int> pre(3000, -1);
  int start = id2num[name2node[location1_name].id];
  int end = id2num[name2node[location2_name].id];


  dist[start] = 0;
  int hh = 0, tt = -1;
  int q[30000] = {};
  q[++tt] = start;
  st[start] = true;
  // std::cout << start << ' ' << end << std::endl;
  while (hh <= tt) {
    int t = q[hh++];
    st[t] = false;
    for (const auto& ne : data[num2id[t]].neighbors) {
        int j = id2num[ne];
        // std::cout << j << std::endl;
        if (dist[j] > dist[t] + G[t][j]) {
          dist[j] = dist[t] + G[t][j];
          pre[j] = t;
          if (!st[j]) {
            q[++tt] = j;
            st[j] = true;
          }
        }
    }
  }
  // std::cout << "here" << std::endl;
  if (dist[end] > 100000000.0 / 2) return {};
  int tmp = end;
  while (tmp != start) {
    x.push_back(num2id[tmp]);
    tmp = pre[tmp];
  }
  x.push_back(num2id[start]);
  std::reverse(x.begin(), x.end());

  auto endtime = clock();
  std::cout << "SPFA runtime: " << (double)(endtime - starttime) / CLOCKS_PER_SEC << 's' <<std::endl;
  
  return x;
}



void TrojanMap::brute_dfs(int u, double cur, double& res, std::vector<bool> state, std::vector<int> path, const std::vector<int> &location_nums, std::vector<std::vector<int>> &all_path) {
  if (u == 0) {
    path.push_back(location_nums[0]);
    state[location_nums[0]] = true;
    brute_dfs(u + 1, 0, res, state, path, location_nums, all_path);
  } 

  if (cur > res) return;
  if (u == location_nums.size()) {
    cur += G_TSP[location_nums[0]][path.back()];


    if (cur < res) {
      res = cur;
      path.push_back(location_nums[0]);
      all_path.push_back(path);
    }
  }
  for (int i = 0; i < location_nums.size(); ++i) {
    if (state[location_nums[i]]) continue;
    state[location_nums[i]] = true;
    double tmp = cur + G_TSP[path.back()][location_nums[i]];
    path.push_back(location_nums[i]);
    brute_dfs(u + 1, tmp, res, state, path, location_nums, all_path);
    state[location_nums[i]] = false;
    path.pop_back();
    
  }
  

}

void TrojanMap::init_TSP(std::vector<std::string> &location_id) {
  id2num_TSP.clear();
  num2id_TSP.clear();
  G_TSP.clear();
  int idx = 1;
  for (const auto& id : location_id) {
    id2num_TSP[id] = idx;
    num2id_TSP[idx] = id;
    idx++;
  }

  G_TSP = std::vector<std::vector<double>>(id2num_TSP.size() + 1, std::vector<double>(id2num_TSP.size() + 1, 100000000.0));
  for (int i = 0; i < location_id.size(); ++i)
    for (int j = 0; j < location_id.size(); ++j) {
      if (i == j) continue;
      int id1 = id2num_TSP[location_id[i]];
      int id2 = id2num_TSP[location_id[j]];
      G_TSP[id1][id2] = G_TSP[id2][id1] = CalculateDistance(data[location_id[i]], data[location_id[j]]);
    }
  
}


/**
 * Travelling salesman problem: Given a list of locations, return the shortest
 * path which visit all the places and back to the start point.
 *
 * @param  {std::vector<std::string>} input : a list of locations needs to visit
 * @return {std::pair<double, std::vector<std::vector<std::string>>} : a pair of total distance and the all the progress to get final path
 */
std::pair<double, std::vector<std::vector<std::string>>> TrojanMap::TravellingTrojan(std::vector<std::string> &location_ids) {
  init_TSP(location_ids);
  auto starttime = clock();
  std::pair<double, std::vector<std::vector<std::string>>> results;
  double res = 1000000000.0;
  double cur = 0;
  int u = 0;
  std::vector<int> path;
  std::vector<bool> state(location_ids.size() + 1, false);
  std::vector<std::vector<int>> all_path;
  std::vector<int> location_nums;
  for (const auto &s : location_ids) location_nums.push_back(id2num_TSP[s]);
    // std::cout << "here" << std::endl;
  brute_dfs(u, cur, res, state, path, location_nums, all_path);

  std::vector<std::vector<std::string>> temp;
  for (const auto &a : all_path) {
    std::vector<std::string> s;
    for (int x : a)
      s.push_back(num2id_TSP[x]);
    temp.push_back(s);
  }
  results = std::make_pair(res, temp);

  auto endtime = clock();
  std::cout << "TSP_DFS runtime: " << (double)(endtime - starttime) / CLOCKS_PER_SEC << 's' <<std::endl;

  return results;
} 


std::vector<std::string> twoopt(const std::vector<std::string> &path, int i, int k) {
  std::vector<std::string> res(path);
  int l = i, r = k;
  while (l < r) {
    swap(res[l], res[r]);
    ++l, --r;
  }
  return res;
}


std::pair<double, std::vector<std::vector<std::string>>> TrojanMap::TravellingTrojan_2opt(std::vector<std::string> &location_ids) {
  init_TSP(location_ids);
  auto starttime = clock();
  std::pair<double, std::vector<std::vector<std::string>>> results;
  std::vector<std::string> path = location_ids;
  std::vector<std::vector<std::string>> all_path;
  path.push_back(path[0]);
  double cur = CalculatePathLength(path);
  double res = cur;
  int n = path.size();
  all_path.push_back(path);
  int count = 0;
  while (1) {
    bool flag = false;
    bool tobreak = false;
    for (int i = 1; i < n - 2; i++) {
      if (flag) break;
      for (int k = i + 1; k <= n - 2; k++) {
        auto new_path = twoopt(path, i, k);
        cur = CalculatePathLength(new_path);
        if (cur < res) {
          ++count;
          res = cur;
          path = new_path;
          all_path.push_back(path);
          flag = true;
          break;
        }
      }
    }
    if (!flag || count > 100000) break;
  }
  results = std::make_pair(res, all_path);

  auto endtime = clock();
  std::cout << "TSP_2opt runtime: " << (double)(endtime - starttime) / CLOCKS_PER_SEC << 's' <<std::endl;
  return results;


}


