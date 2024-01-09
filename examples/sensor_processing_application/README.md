# Description


The example shows how `Citidi` might be used for data processing applications to manage data flows and task scheduling.

The core parts here are the DataDispatcher and the ApplicationDispatcher. They define the data scheme for a Dispatcher object and then these Dispatcher objects can be uses to retrieve data via **Marker Types**.
Also, there is an example of the use of implicit coercion. The required data is described in a function declaration and is automatically provided from a Dispatcher object.


The example contains the following entities:

- Definition of input/output data types
- Definition of `Marker` types for the data types
- Definition of metafunctions for data processing
- Definition of `Marker` types for the metafunctions
- Definition of `Dispatcher` types for the data types
- Definition of `Dispatcher` types for the metafunctions
- Implementation of the main function to run the application

### Input/output data types

In this example dummy data types are used.
Calibration and objects input signals from lidar and ultrasound sensors have different data types and might have different content, but there is only one output data type `HealthData` for all sensors.
```c++
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
```

### Marker types for the data types

The data from the previous part might be **marked** with sensor id and type of signal. The following structs are used as `Marker` types:
```c++
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

```

### Data processing metafunctions

The following metafunction is used to read data from sensor. For the example simplicity it just increase the existing value.

The metafunction object requires the following types as input:
- `SensorType` - sensor which provides information
- `SignalType` - type of provided information
- `T` - `Dispatcher` or `Slice` object type which is used as the base for `Slice<T, With<SensorType, SignalType>>` creation

```c++
template<typename SensorType, typename SignalType, typename T>
struct ReadSignal
{
  void operator()(Slice<T, With<SensorType, SignalType>> slice)
  {
    slice.Single().data.value++;
  }
};
```

The `WriteHealth` metafunction is used to calculate health as a sum of calibration and objects signal from a sensor and write it back to `Dispatcher` object.

```c++
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
```

The `PrintsignalValue` metafunction is used to print actual value to standard output.

```c++
template<typename SensorType, typename SignalType, typename T>
struct PrintSignalValue
{
  void operator()(const Slice<T, With<SensorType, SignalType>>& slice)
  {
    std::cout << "Sensor<" << SensorType::id << "> [" << SignalType::Name()
              << "] value: " << slice.Single().data.value << std::endl;
  }
};

```

### Marker types for the metafunctions

The following data type and constants are used to define the frequency of a job
```c++
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
```

### Dispatcher types for the data types

`Dispatcher` and `Element` objects are used here to make a linkage between input/output data types and `Marker` types:
```c++
using DataDispatcher = Dispatcher<
    // Input
    Element<LidarCalibrationData, Sensor<kLidarId>, Calibration>,
    Element<UltrasoundCalibrationData, Sensor<kUltrasoundId>, Calibration>,
    Element<LidarObjectsData, Sensor<kLidarId>, Objects>,
    Element<UltrasoundObjectsData, Sensor<kUltrasoundId>, Objects>,
    // Output
    Element<HealthData, Sensor<kLidarId>, Health>,
    Element<HealthData, Sensor<kUltrasoundId>, Health>>;
```

### Dispatcher types for the metafunctions

Another `Dispather` object which contains `Element` objects for data processing tasks:
```c++
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
```

### ApplicationProcess

`ApplicationProcess` represents a set of tasks. It queries `ApplicationDispatcher` for tasks with a given `CycleTime` and run them:
```c++
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
```

### Main function

Inside the `main` function `DataDispatcher`, `ApplicationDispatcher` and required thread objecs are initialized:
```c++
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
```
