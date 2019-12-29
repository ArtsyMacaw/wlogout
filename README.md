# WLOGOUT
## Purpose
Wlogout is a logout menu for [wayland](https://wayland.freedesktop.org/) environments that support the layer shell protocol. For those wanting a xorg based logout menu look at [oblogout](https://launchpad.net/oblogout).
### Table of Contents
[Status](#status)

[TODO](#todo)

[Usage](#usage)

[Compiling from Source](#compile)

<a name="status"/>

## Status
Version 0.1 Alpha

<a name="todo"/>

### TODO
- [X] Implement basic functionality
- [ ] Implement styling via CSS
- [ ] Implement cli arguements
- [ ] Create man page

<a name="usage"/>

## Usage 
Simply call
```
./wlogout (path to json file)
```
where the json file contains a list of however many buttons in json format e.g.
```
{
    "label" : "CSS Label",
    "action" : "command to execute when clicked",
    "text" : "text displayed on button"
}
```

<a name="compile"/>

## Compiling from Source
Install dependencies:
* GTK+  
* [gtk-layer-shell](https://github.com/wmww/gtk-layer-shell) 
* meson

Run these commands:
```
git clone https://github.com/ArtsyMacaw/wlogout.git
cd wlogout
meson build
ninja -C build
```
