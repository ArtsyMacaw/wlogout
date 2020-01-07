# wlogout
![Example](example.png)
> wlogout is a logout menu for [wayland](https://wayland.freedesktop.org/) environments that support the layer shell protocol
## Running
Run `wlogout` from within any wayland environment that supports layer shell protocol; Any window manager using [wlroots](https://github.com/swaywm/wlroots) will likely work, as well as an up to date version of KDE
## Layout
Custom buttons can be defined and edited in a layout file. The format is as follows:
```
{
    "label" : "CSS Label",
    "action" : "command to execute when clicked",
    "text" : "text displayed on button",
    "keybind" : "character to be bound"
}
```
Check the default [layout file](layout) for examples and inspiration.
## Style
wlogout can be easily styled through a css file; If you would like to style a button use the label given to it in the layout file, and for other styling refer to the [GTK Manual](https://developer.gnome.org/gtk3/stable/chap-css-properties.html) which shows all the allowed css.
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
sudo ninja -C build install
```
## Todo
- [X] Implement basic functionality
- [X] Implement styling via CSS
- [X] Implement cli arguements
- [X] Implement keyboard interaction
- [ ] Create man page
## License
Wlogout is licensed under MIT. [Refer to LICENSE for more information](LICENSE)
