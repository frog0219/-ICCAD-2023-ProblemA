#include<chrono>

class Timer{
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
public:
    Timer() : start_time(std::chrono::high_resolution_clock::now()){};
    void reset(){
        start_time = std::chrono::high_resolution_clock::now();
    }
    double GetElapsedMillseconds() const{
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double , std::milli> elapsed = end_time - start_time;
        return elapsed.count();
    }
};