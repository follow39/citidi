# Description


# Usage

Basic usage
```c++
disp.Get<SensorData, ID{12}>() -> vector<int>&
disp.Get<SensorData, ID{14}>() -> vector<float>&
disp.Get<SensorCalibrationData, ID{14}>() -> CalibrationData&
disp.Get<Application, ID{12}>() -> App&
disp.Get<MessageProvider, ID{12}>() -> MProvider&
```

Marker groups
```
disp.Get<Marker1, Marker2, ID{16}>() -> vector<int>& -> disp.Get<MarkerGroup, ID{16}>() -> vector<int>&
```
Marker groups combination
disp.Get<MarkerGroup1, MarkerGroup2, ID{16}>() -> vector<int>&
```

Lazy construction?
