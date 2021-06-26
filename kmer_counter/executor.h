#ifndef KMER_COUNTER_EXECUTOR_H
#define KMER_COUNTER_EXECUTOR_H

#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <future>
#include <condition_variable>
#include <concurrentqueue.h>

namespace kcoss {

    /**
     * 一个能够提交一批任务，然后等待这批任务完成的定长的线程池，
     * 工作线程可从队列中获取参数然后执行业务逻辑.
     * @tparam Args 参数类型
     */
    template<typename Args>
    class Executor {
    protected:
        typedef std::vector<std::thread> worker_list_t;
        typedef std::atomic_size_t counter_t;
        typedef std::mutex mutex_t;
        typedef std::condition_variable cv_t;
        typedef moodycamel::ConcurrentQueue<Args> queue_t;
        typedef Args arg_t;

        const function<void(Args)> _main;
        volatile bool run;

        counter_t to_process{};
        queue_t arg_queue{};
        worker_list_t workers;

        mutex_t workers_mutex;
        mutex_t cv_mutex;
        cv_t cv_clear;

    public:
        explicit Executor(const function<void(Args)> &func, size_t workers): _main(func) {
            run = true;
            for (size_t i = 0; i < workers; ++i) {
                _add_worker();
            }
        }

        virtual ~Executor() {
            run = false;
            for (auto &t: workers) {
                t.join();
            }
        }

        void enqueue(const arg_t &arg) {
            to_process.fetch_add(1);
            arg_queue.enqueue(arg);
        }

        void join() {
            std::unique_lock<mutex_t> lock(cv_mutex);
            cv_clear.wait(lock, [this]() { return to_process.load() == 0; });
        }

    protected:
        void _add_worker() {
            std::unique_lock<mutex_t> lock(workers_mutex);
            workers.emplace_back(std::thread([this]() { _worker_main(); }));
        }

        void _worker_main() {
            arg_t item;
            bool not_empty;
            while ((not_empty = arg_queue.try_dequeue(item)) || run) {
                if (!not_empty) {
                    continue;
                }
                _main(item);

                size_t prev = to_process.fetch_add(-1);
                if (prev == 1) {
                    cv_clear.notify_all();
                }
            }
        }
    };
}

#endif //KMER_COUNTER_EXECUTOR_H
