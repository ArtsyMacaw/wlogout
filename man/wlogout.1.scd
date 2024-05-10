wlogout(1)

# NAME

wlogout - A Wayland logout menu

# SYNOPSIS

*wlogout* [options...]

# OPTIONS

*-h, --help*
	Show help message and stop

*-l, --layout* <layout>
	Specify a custom layout file

*-v, --version*
	Show version number and stop

*-C, --css* <css>
	Specify a custom css file

*-b, --buttons-per-row* <num>
	Set the number of buttons per row

*-c, --column-spacing* <space>
	Set space between buttons columns

*-r, --row-spacing* <space>
	Set space between buttons rows

*-m, --margin* <padding>
	Set margin on all sides

*-L, --margin-left* <padding>
	Set margin for left of buttons

*-R, --margin-right* <padding>
	Set margin for right of buttons

*-T, --margin-right* <padding>
	Set margin for top of buttons

*-B, --margin-right* <padding>
	Set margin for bottom of buttons

*-p, --protocol* <protocol>
	Takes either layer-shell or xdg. The layer-shell allows transparency effects; however, only a few compositors correctly support it. The xdg protocol will work on almost all compositors, but does not allow for transparency.

*-s, --show-binds*
	Will show the keybinds of a button by inserting the key needed to activate the button next to its label.

*-P, --primary-monitor* <num>
	Select the monitor to display buttons on; Monitors are numbered from 0 up.

*-n, --no-span*
	Stops wlogout from spanning across multiple monitors, can be combined with `--primary-monitor` to only appear on one monitor.

# DESCRIPTION

wlogout was created to replace oblogout with a native logout script for Wayland. It also seeks to be a faster alternative that does not rely on deprecated technology such as python 2; while maintaining a small code footprint.

You can run wlogout from any Wayland environment and it should just work.

# CONFIGURATION

Wlogout searches for a layout and style.css file in the following locations, in this order:

. $XDG_CONFIG_HOME/wlogout/
. /etc/wlogout/
. /usr/local/etc/wlogout

If unset, $XDG_CONFIG_HOME defaults to *~/.config/*.

An error is raised when no layout file is found; However, the style.css file is optional. If you would like to customise either it is recommended that you copy the defaults from */etc/wlogout/* into  *~/.config* and make any changes there.

# AUTHORS

Maintained by Haden Collins <collinshaden@gmail.com> for more information about wlogout, see <https://github.com/ArtsyMacaw/wlogout>.

# SEE ALSO

*wlogout*(5)
