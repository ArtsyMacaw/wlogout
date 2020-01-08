wlogout(5)

# NAME

wlogout - layout file and options

# LAYOUT

wlogout's buttons consist of four values
- label
- action
- text
- keybind

Label is the css selector by which the buttons may be referred to in a *style.css* file. Action is the shell command to be executed when the button is clicked. Text is the description displayed on the button. Keybind is the key mapped to the button; Note escape is reserved for exiting the application.

# FILE

The buttons values are specified in a JSON formatted file, wherein the values are used as keys and one button corresponds to one JSON object for example:
```
{
    "label" : "foo"
    "action" : "echo 'hello world'"
    "text" : "bar"
    "keybind" : "f"
}
```
Would create a button that has a css label of *foo*, print "hello world" upon being clicked, display "bar" on the button, and be bound to the key 'f'. To create multiple buttons simply create another JSON object.

# AUTHORS

Maintained by Haden Collins <collinshaden@gmail.com> for more information about wlogout, see <https://github.com/ArtsyMacaw/wlogout>.

# SEE ALSO

*wlogout*(1)
