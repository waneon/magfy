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
### Windows
1. [Download](https://ds.waneon.me/drive/d/s/nfvmZn2mVoHUJNx51xhYl7q2AR8pkHl7/lJgerMJC5MpXbPlYTiqvS76Lc4yeNKKW-xbfgC6AaVQk) the latest `magfy.zip` and `magfy.crt`.
2. Extract `magfy.zip` to `C:\Program Files`.
3. Install `magfy.crt`: `Install Certificate..` -> `Local Machine` -> `Place all certificates in the following store` -> `Browse..` -> `Trusted Root Certification Authorities`.
4. Make config.yaml in the `%localappdata%/magfy/`, and [configure](CONFIGURATION.md) it.  

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