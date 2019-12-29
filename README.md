# WLOGOUT
![Example](example.png)
> Wlogout is a logout menu for [wayland](https://wayland.freedesktop.org/) environments that support the layer shell
> protocol. For those wanting a xorg based logout menu look at [oblogout](https://launchpad.net/oblogout).
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
Default buttons are provided in the *layout* file and also serve as a useful example if you would like to create your own.
## Style
Wlogout can be easily styled through style.css file; If you would like to style a button use the label given to it in the layout file, and for other styling refer to the wonderful [GTK Manual](https://developer.gnome.org/gtk3/stable/chap-css-properties.html) which shows all the allowed css.
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
## Todo
- [X] Implement basic functionality
- [X] Implement styling via CSS
- [ ] Implement cli arguements
- [ ] Create man page
## License
Wlogout is licensed under MIT. [Refer to LICENSE for more information](LICENSE)
