wlogout(1)

# NAME

wlogout - A Wayland logout menu

# SYNOPSIS

*wlogout* [options] [command]

# OPTIONS

*-h, --help*
	Show help message and stop

*-l, --layout* <layout>
	Specify a custom layout file

*-v, --version*
	Show version number and stop

*-C, -css* <css>
	Specify a custom css file

*-b, --buttons-per-row* <num>
	Set the number of buttons per row

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

# DESCRIPTION

wlogout was created to replace oblogout with a native logout script for Wayland. It also seeks to be a faster alternative that does not rely on depracated technology such as python 2; while maintaining a small code footprint.

You can run wlogout from any Wayland environment that supports the layer-shell protocol

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