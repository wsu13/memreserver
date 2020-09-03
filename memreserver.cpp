#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <mutex>
#include <atomic>
#include <thread>

using namespace std;

mutex dataListMtx;
vector<string*> dataList;
atomic<int> counter(0);
size_t size_1mb = 1L * 1024 * 1024;

void doReserve(int num) {
  for (int i = 0; i < num; i++){
    string* bytes = new string();
    bytes->resize(size_1mb);
    unique_lock<mutex> guard(dataListMtx);
    dataList.push_back(bytes);
    int thisRound = ++counter;
    if (thisRound != 0 && thisRound % 1024 == 0) {
      cout << "reserved " << (thisRound/1024) << " gb" << endl;
    }
  }
}


int main(int argc, char** argv) {
  if (argc != 2) {
    cout << "argc != 2" << endl;
    exit(0);
  }
  int gb = stoi(argv[1]);
  int totalMb = gb * 1024;
  int numThread = 10;
  vector<thread*> threads;
  for (int t = 0; t < numThread; t++) {
    if (t == numThread -1) {
      threads.push_back(new thread(doReserve, totalMb / numThread + totalMb % numThread));
    } else {
      threads.push_back(new thread(doReserve, totalMb / numThread));
    }
  }
  for (int t = 0; t < numThread; t++) {
    threads[t]->join();
  }
  cout << "memory reserved. going to sleep.." << endl;
  std::this_thread::sleep_for(std::chrono::hours(7200));
}

