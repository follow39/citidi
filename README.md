# Description

Citidi (Compile Time Dispatch) - a library that allows to query some data with `Marker` types.
Additionally it provides a way to iterate over some group of data with the same `Marker` types, but possibly with different value types.


# Key concepts

Traditional approach to store some data with different types and meanings is to use variables:
```c++
// Input
LidarCalibrationData lidar_calibration_data{};
UltrasoundCalibrationData ultrasound_calibration_data{};
LidarObjectsData lidar_objects_data{};
UltrasoundObjectsData ultrasound_objects_data{};
// Output
LidarHealthData lidar_health_data{};
UltrasoundHealthData ultrasound_health_data{};
```

Variables is a basic part of almost every programming languige.
But there is an one inconvinience with C++ - it does not have reflection (until C++26).

Citidi is trying to solve this issue and the main idea here - every value is tagged with some `Marker` types.
This tagging and wide C++ metaprogramming possibilities allow to query a data by `Marker` types and even iterate over some group of data (eg it is useful to iterate over all calibration values).


## Dispatcher

`Dispatcher` object is used to store all required data and create slices if needed.

In the following example `Dispatcher` object type is defined to store sensors data:
```c++
using DataDispatcher = Dispatcher<
    // Input
    Element<LidarCalibrationData, Sensor<kLidarId>, Calibration>,
    Element<UltrasoundCalibrationData, Sensor<kUltrasoundId>, Calibration>,
    Element<LidarObjectsData, Sensor<kLidarId>, Objects>,
    Element<UltrasoundObjectsData, Sensor<kUltrasoundId>, Objects>,
    // Output
    Element<HealthData, Sensor<kLidarId>, Health>,
    Element<HealthData, Sensor<kUlTrasound>, Health>>;
    
DataDispatcher disp{};
```


## Element

`Element` is a data cell, it contains actual data and all `Marker` types related to the value.


## Slice

`Slice` object is a link to the `Dispatcher` object, but scope of the data might be decreased with some `Condition`:
```c++
auto calibration_slice = disp.Get<Condition>();
```

If there is only one element in slice, it might be taken by `Single` function:
```c++
auto value = slice_object.Single();
```

Or by `Get` function with index or condition:
```c++
auto value = slice_object.Get<3>();
auto value = slice_object.Get<Condition>();
```

Indexing is used to iterate over slice data:
```c++
compile_time_for<slice_object.Size()>(
    [&slice_object](const auto i)
    {
        auto& value = slice_object.template Get<i.value>().data;
        PrintValue(value);
});
```


## Condition

`Condition` object is used to build a logic to choose data for slicing.

Condition object must have following function to be used in a condition chain:
```c++
template<typename MarkersTupleType>
constexpr static bool Evaluate();
```

Where `MarkersTupleType` is a tuple with all marker types related to a value.


### BoolConst

`BoolConst` is used to define some bool constant. It might be used tp query all data from a `Dispatcher` object or for prototyphs, where requered data is not defined yet.


### Or

`Or` object is used to apply logic **or** operation for results from provided `Condition` objects

### And

`And` object is used to apply logic **and** operation for results from provided `Condition` objects

### Not

### With

### WithExactly

### Without



