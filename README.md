# Description



# To do

- [x] Marker groups
- [x] Marker groups combination
- [x] Orderless markers
- [ ] Get slice function
- [ ] View object
- [ ] Lazy construction

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
disp.Get<Marker1, Marker2, ID{16}>() -> vector<int>&
-> disp.Get<MarkerGroup, ID{16}>() -> vector<int>&
```

Marker groups combination
```
disp.Get<MarkerGroup1, MarkerGroup2, ID{16}>() -> vector<int>&
```

Orderless markers
```
disp.Get<SensorData, ID{12}>() == disp.Get<ID{12}, SensorData>() 
```

Get slice function
```
disp.Get<SensorData>() -> tuple<Element<SensorData, ID{12}>&, Element<SensorData, ID{12}>&>
```
