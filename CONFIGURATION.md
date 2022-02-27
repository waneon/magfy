# config.yaml
configuration file must be located in the path below.  
* **X11** `~/.config/magfy/config.yaml`  
* **Wayland** to be supported  
* **Windows** to be supported

# Example
```sh
---
magnifier:
  backend: gnome
  mag-factor: 1.5
  mag-enlarge-factor: 0.5
keyboard-shortcut:
  toggle: Alt+F1
  exit: Alt+F2
mouse-shortcut:
  toggle: Side2
  enlarge: Side1
```

# magnifier (must be specified)
Settings related to magnifier.
## backend (must be specified)
Setting which backend to use.  
**default**: `None`  
**available**: `gnome`, `windows`
## mag-factor
The mag-factor when magnifier toggled-on.  
**default**: `1.5`
## mag-enlarge-factor
The enlargement factor when magnifier enlarged.  
**default**: `0.5`
## mag-shrink-factor
The shrinking factor when magnifier shrinked.  
**default**: `0.5`

# keyboard-shortcut
Settings related to keyboard shortcut.


**Combinations**: [`MODIFIER` +]* `KEY`  
**MODIFIER**: Ctrl | Alt | Shift  
**KEY**: A-Z | 0-9 | F1-F12
## toggle
Toggle the magnifier.  
**default**: `None`
## shrink
Shrink the mag-factor.  
**default**: `None`
## enlarge
Enlarge the mag-factor.  
**default**: `None`
## exit
Exit the program.  
**default**: `None`

# mouse-shortcut
Settings related to mouse shortcut.


**Combinations**: [`MODIFIER` +]* `BUTTOON`  
**MODIFIER**: Ctrl | Alt | Shift  
**BUTTON**:  
* `Left`: Mouse left click  
* `Middle`: Mouse middle click  
* `Right`: Mouse right click  
* `WheelUp`: Mouse wheel up  
* `WheelDown`: Mouse wheel down  
* `WheelLeft`: Mouse wheel left  
* `WheelRight`: Mosue wheel right  
* `Side1`: Mouse side1 click  
* `Side2`: Mouse side2 click
## toggle
Toggle the magnifier.  
**default**: `None`
## shrink
Shrink the mag-factor.  
**default**: `None`
## enlarge
Enlarge the mag-factor.  
**default**: `None`
## exit
Exit the program.  
**default**: `None`