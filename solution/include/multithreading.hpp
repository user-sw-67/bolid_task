#ifndef MULTITHREADING_HPP
#define MULTITHREADING_HPP

#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <string>

#include "url_parser.hpp"
#include "client.hpp"


extern std::mutex mutex_cout;


class MultithreadingWorker{
private:
    std::atomic<int> url_counter;
    const std::vector<UrlParser>& urls;
    const std::string& dir;
    int count_thread;

public:
    MultithreadingWorker(const std::vector<UrlParser>& urls, 
        const std::string& dir, int count_thread) : 
            urls(urls), dir(dir), count_thread(count_thread), url_counter(0) {}

    void run() {
        std::vector<std::thread> threads;
        for (int i = 0; i < count_thread; ++i){
            threads.emplace_back([this](){
                while (true){
                    int url_idx = url_counter.fetch_add(1);
                    if (url_idx >= urls.size()) break;
                    download(urls[url_idx]);
                }
            });
        }

        for (auto& t : threads){
            t.join();
        }
    }

    void download(const UrlParser& url) {
        if(url.protocol == "http"){
            RequestHTTP rq(url);
            rq.fetch(dir);
        } else {
            RequestHTTPS rq(url);
            rq.fetch(dir);
        }
    }   
};

#endif