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
cmake .. -DCMAKE_BUILD_TYPE=Release -DVENDOR="X11 | Wayland | Windows"
```

## Usage
Read [CONFIGURATION.md](CONFIGURATION.md) for configurations.

## Development
### Dependencies
* spdlog
* yaml-cpp