# magfy
universal configurable desktop magnifier.

## Supported
### Vendor
- [x] X11
- [x] Windows
- [ ] Wayland

### Backend
- [x] gnome
- [x] windows

## Installation
### Manual build
```sh
git clone https://github.com/waneon/magfy
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DVENDOR="X11 | Wayland | Windows"
make
```

## Usage
Read [CONFIGURATION.md](CONFIGURATION.md) for configurations.

## Development
### Dependencies
* spdlog
* yaml-cpp