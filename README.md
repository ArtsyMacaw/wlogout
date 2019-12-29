# WLOGOUT
## Purpose
Wlogout is a logout menu for [wayland](https://wayland.freedesktop.org/) environments that support the layer shell protocol. For those wanting a xorg based logout menu look at [oblogout](https://launchpad.net/oblogout).

![Example](example.png)
### Table of Contents
[Status](#status)

[TODO](#todo)

[Usage](#usage)

[Compiling from Source](#compile)

<a name="status"/>

## Status
Version 0.2 Alpha

<a name="todo"/>

### TODO
- [X] Implement basic functionality
- [X] Implement styling via CSS
- [ ] Implement cli arguements
- [ ] Create man page

<a name="usage"/>

## Usage 
Simply call
```
./wlogout (path to json file)
```
Where the json file contains a list of however many buttons in json format e.g.
```
{
    "label" : "CSS Label",
    "action" : "command to execute when clicked",
    "text" : "text displayed on button"
}
```
Default buttons are provided in the [layout](layout) file and also serve as a useful example if you would like to create your own.

<a name="compile"/>

## Compiling from Source
Install dependencies:
* GTK+  
* [gtk-layer-shell](https://github.com/wmww/gtk-layer-shell) 
* meson
* systemd (optional: default buttons)
* swaylock (optional: default buttons)

Run these commands:
```
git clone https://github.com/ArtsyMacaw/wlogout.git
cd wlogout
meson build
ninja -C build
```
