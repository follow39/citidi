# Description

Citidi (**C**ompile **T**ime **D**ispatch) - a library that allows to query some data with `Marker` types.
Additionally, it provides a way to iterate over some group of data with the same `Marker` types, but possibly with different value types.


# Key concepts

The traditional approach to store some data with different types and meanings is to use variables:
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

Variables is a basic part of almost every programming language.
But there is one inconvenience with C++ - it does not have reflection (until C++26), it does not provide standard ways to iterate over group of variables or query them by part of the name.

Citidi is trying to solve this issue and the main idea here - every value is tagged with some `Marker` types.
This tagging and wide C++ metaprogramming possibilities allow to query data by `Marker` types and iterate over group of data.


## Dispatcher

`Dispatcher` object is used to store all required data and create slices (conditional links) when it is needed.

In the following example, `Dispatcher` object type is defined to store sensor data:
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

`Element` is a simple data cell. It contains actual data and all `Marker` types related to the value.


## Slice

`Slice` object is a link to the base data object (`Dispatcher` or another `Slice`), but scope of the data is decreased by a `Condition` object:
```c++
auto calibration_slice = disp.Get<Condition>();
```

If there is only one element in a slice, it might be taken by `Single` function:
```c++
auto value = slice_object.Single();
```

Or by `Get` function with index or condition:
```c++
auto value = slice_object.Get<3>();
auto value = slice_object.Get<Condition>();
```

Indexing might be used to iterate over a slice of data:
```c++
compile_time_for<slice_object.Size()>(
    [&slice_object](const auto i)
    {
        auto& value = slice_object.template Get<i.value>().data;
        PrintValue(value);
});
```


## Condition

`Condition` object is used to build a logic to choose data for slicing. It must have the following function to be used in a condition chain:
```c++
template<typename MarkersTupleType>
constexpr static bool Evaluate();
```

Where `MarkersTupleType` is a tuple with all marker types related to a value.

### BoolConst

`BoolConst` is used to define some bool constant. It might be used to query all data from a `Dispatcher` object or for prototypes, where the required data is not defined yet.

### Or

`Or` object is used to apply logic **or** operation for results from provided `Condition` objects.

### And

`And` object is used to apply logic **and** operation for results from provided `Condition` objects.

### Not

`Not` object is used to invert the result of a provided `Condition`.

### With

`With` object is used to find given `Marker` types inside `MarkersTupleType` when the `Evaluate` function is called.

### WithExactly

`WithExactly` object is used to find an exact match between tuple with given `Marker` types and `MarkersTupleType` when the `Evaluate` function is called.

### Without

`Without` object is used to find given `Marker` types inside `MarkersTupleType` when the `Evaluate` function is called and invert the result.

