# magfy
universal configurable desktop magnifier controller.

## Supported
### Vendor
- [x] X11
- [ ] Wayland
- [ ] Windows

### Backend
- [x] gnome
- [ ] windows

## Installation
### Manual build
```sh
git clone https://github.com/waneon/magfy
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DVENDOR="your vendor"
```
vendors: X11, Wayland, Windows

## Usage

## Development
### Dependencies
* spdlog
* yaml-cpp