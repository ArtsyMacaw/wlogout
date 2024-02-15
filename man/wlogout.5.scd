wlogout(5)

# NAME

wlogout - layout file and options

# LAYOUT

wlogout's buttons can consist of up to six values
- label
- action
- text
- keybind
- height \*
- width \*
- circular \*

\* Optional values

Label is the css selector by which the buttons may be referred to in a *style.css* file, action is the shell command to be executed when the button is clicked, text is the description displayed on the button, keybind is the key mapped to the button (note escape is reserved for exiting the application), height and width are values between 0.0 and 1.0 that control the location of where *text* is displayed the default width 0.5, height 0.9, and circular is a boolean value that makes a button round.

# FILE

The buttons values are specified in a JSON formatted file, wherein the values are used as keys and one button corresponds to one JSON object for example:
```
{
    "label" : "foo",
    "action" : "echo 'hello world'",
    "text" : "bar",
    "keybind" : "f",
    "height" : 1,
    "width" : 1,
    "circular" : true
}
```
Would create a round button that has a css label of *foo*, prints "hello world" upon being clicked, displays "bar" on the button, be bound to the key 'f', and "bar" would be shown at the bottom right corner. To create multiple buttons simply create another JSON object.

# AUTHORS

Maintained by Haden Collins <collinshaden@gmail.com> for more information about wlogout, see <https://github.com/ArtsyMacaw/wlogout>.

# SEE ALSO

*wlogout*(1)
