#include <chrono>
#include <iostream>
#include <thread>

#include "include/condition.hpp"
#include "include/dispatcher.hpp"
#include "include/element.hpp"
#include "include/slice.hpp"

struct LidarObjectsData
{
  std::size_t value {0U};
};

struct LidarCalibrationData
{
  std::size_t value {0U};
};

struct UltrasoundObjectsData
{
  std::size_t value {0U};
};

struct UltrasoundCalibrationData
{
  std::size_t value {0U};
};

struct HealthData
{
  std::size_t value {0U};
};

template<std::size_t Id>
struct Sensor
{
  constexpr static std::size_t id = Id;
};

struct Health
{
  constexpr static const char* Name() { return "Health"; }
};

struct Calibration
{
  constexpr static const char* Name() { return "Calibration"; }
};

struct Objects
{
  constexpr static const char* Name() { return "Objects"; }
};

constexpr static std::size_t kLidarId {0U};
constexpr static std::size_t kUltrasoundId {1U};

template<std::size_t CycleTimeValueMs>
struct CycleTime
{
  constexpr static std::size_t value_ms = CycleTimeValueMs;
  constexpr static std::size_t ValueMs() { return value_ms; }
};

constexpr static std::size_t k40ms {40U};
constexpr static std::size_t k80ms {80U};
constexpr static std::size_t k250ms {250U};
constexpr static std::size_t k1000ms {1000U};

template<typename SensorType, typename SignalType, typename T>
struct ReadSignal
{
  void operator()(Slice<T, With<SensorType, SignalType>> slice)
  {
    slice.Single().data.value++;
  }
};

template<typename SensorType, typename T>
struct WriteHealth
{
  void operator()(Slice<T, With<SensorType>> disp)
  {
    const std::size_t result =
        disp.template Get<With<SensorType, Calibration>>().Single().data.value
        + disp.template Get<With<SensorType, Objects>>().Single().data.value;
    disp.template Get<With<SensorType, Health>>().Single().data.value = result;
  }
};

template<typename SensorType, typename SignalType, typename T>
struct PrintSignalValue
{
  void operator()(const Slice<T, With<SensorType, SignalType>>& slice)
  {
    std::cout << "Sensor<" << SensorType::id << "> [" << SignalType::Name()
              << "] value: " << slice.Single().data.value << std::endl;
  }
};

namespace detail
{

template<std::size_t N>
struct Number
{
  static const constexpr auto value = N;
};

template<class F, std::size_t... Is>
void for_(F func, std::index_sequence<Is...>)
{
  using expander = int[];
  (void)expander {0, ((void)func(Number<Is> {}), 0)...};
}

template<std::size_t N, typename F>
void for_(F func)
{
  for_(func, std::make_index_sequence<N>());
}

}  // namespace detail

template<typename CTime, typename T, typename... Args>
struct ApplicationProcess
{
  static void Run(Slice<T, With<CTime>> tasks, Args&... args)
  {
    while (true) {
      detail::for_<tasks.Size()>(
          [&tasks, &args...](const auto i)
          {
            auto& task = tasks.template Get<i.value>().data;
            task(args...);
          });
      std::this_thread::sleep_for(std::chrono::milliseconds {CTime::ValueMs()});
    }
  }
};

using DataDispatcher = Dispatcher<
    // Input
    Element<LidarCalibrationData, Sensor<kLidarId>, Calibration>,
    Element<UltrasoundCalibrationData, Sensor<kUltrasoundId>, Calibration>,
    Element<LidarObjectsData, Sensor<kLidarId>, Objects>,
    Element<UltrasoundObjectsData, Sensor<kUltrasoundId>, Objects>,
    // Output
    Element<HealthData, Sensor<kLidarId>, Health>,
    Element<HealthData, Sensor<kUltrasoundId>, Health>>;

using ApplicationDispatcher = Dispatcher<
    // 40 ms
    Element<ReadSignal<Sensor<kLidarId>, Calibration, DataDispatcher>,
            CycleTime<k40ms>>,
    Element<ReadSignal<Sensor<kLidarId>, Objects, DataDispatcher>,
            CycleTime<k40ms>>,
    // 80 ms
    Element<ReadSignal<Sensor<kUltrasoundId>, Calibration, DataDispatcher>,
            CycleTime<k80ms>>,
    Element<ReadSignal<Sensor<kUltrasoundId>, Objects, DataDispatcher>,
            CycleTime<k80ms>>,
    // 250 ms
    Element<WriteHealth<Sensor<kLidarId>, DataDispatcher>, CycleTime<k250ms>>,
    Element<WriteHealth<Sensor<kUltrasoundId>, DataDispatcher>,
            CycleTime<k250ms>>,
    // 1000ms
    Element<PrintSignalValue<Sensor<kLidarId>, Health, DataDispatcher>,
            CycleTime<k1000ms>>,
    Element<PrintSignalValue<Sensor<kUltrasoundId>, Health, DataDispatcher>,
            CycleTime<k1000ms>>>;

int main()
{
  DataDispatcher disp {};
  ApplicationDispatcher app {};

  std::thread thread_40ms {[&app, &disp]() -> void
                           {
                             ApplicationProcess<CycleTime<k40ms>,
                                                ApplicationDispatcher,
                                                DataDispatcher>::Run(app, disp);
                           }};

  std::thread thread_80ms {[&app, &disp]() -> void
                           {
                             ApplicationProcess<CycleTime<k80ms>,
                                                ApplicationDispatcher,
                                                DataDispatcher>::Run(app, disp);
                           }};

  std::thread thread_250ms {[&app, &disp]() -> void
                            {
                              ApplicationProcess<CycleTime<k250ms>,
                                                 ApplicationDispatcher,
                                                 DataDispatcher>::Run(app,
                                                                      disp);
                            }};

  std::thread thread_1000ms {[&app, &disp]() -> void
                             {
                               ApplicationProcess<CycleTime<k1000ms>,
                                                  ApplicationDispatcher,
                                                  DataDispatcher>::Run(app,
                                                                       disp);
                             }};

  while (true) {
    std::this_thread::yield();
  }

  return 0;
}
