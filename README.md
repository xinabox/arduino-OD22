# arduino-OD22
Library for OD22: Riverdi Display

In all the examples, the line

```C++
FTImpl.Init(FT_DISPLAY_RESOLUTION);
```
needs to be changed to
```C++
FTImpl.Init(FT_DISPLAY_RESOLUTION,0);
```

### Board Support
Library is currently only compatible with the following Cores:
* [CW02](https://xinabox.cc/products/cw02)
* [CW01](https://xinabox.cc/products/cw01)
* [CC03](https://xinabox.cc/products/cc03)
* [CS11](https://xinabox.cc/products/cs11)

### Software Support

* SD Card examples are currently not supported
