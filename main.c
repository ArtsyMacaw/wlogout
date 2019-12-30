#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <gtk-layer-shell/gtk-layer-shell.h>
#include <gtk/gtk.h>
#include "jsmn.h"

static const int exclusive_level = 2;
static const int default_size = 100;
static const char *version = "Alpha-0.3\n";

typedef struct
{
    char *label;
    char *action;
    char *text;
} button;

static char *command;
static char *layout_path = NULL;
static char *css_path = NULL;
static button *buttons;
static GtkWidget *gtk_window;

static int num_buttons = 0;
static int buttons_per_row = 3;
static int margin = 500;

static GtkWidget *get_window();
static char *get_substring(char *s, int start, int end, char *buf);
static gboolean get_buttons(FILE *json);
static void display_buttons(GtkWindow *window);
static void load_css();
gboolean process_args(int argc, char *argv[]);
gboolean get_layout_path();

int main (int argc, char *argv[])
{
    buttons = malloc(sizeof(button) * default_size);

    g_set_prgname("wlogout");
    gtk_init(&argc, &argv);
    if (process_args(argc, argv))
    {
        return 0;
    }

    if (get_layout_path())
    {
        g_warning("Failed to find a layout");
        return 1;
    }
    
    FILE *inptr = fopen(layout_path, "r");
    if (!inptr)
    {
        g_warning("Failed to open %s\n", layout_path);
        return 2;
    }
    if (get_buttons(inptr))
    {
        fclose(inptr);
        return 3; 
    }

    gtk_window = get_window();
    gtk_container_set_border_width(GTK_CONTAINER(gtk_window), margin);
    display_buttons(GTK_WINDOW(gtk_window));
    free(buttons);

    load_css();
    gtk_widget_show_all(gtk_window);

    gtk_main();
    system(command);

    free(command);
}

static void execute(GtkWidget *widget, char *action)
{
    command = malloc(strlen(action) * sizeof(char) + 1);
    strcpy(command, action);
    gtk_widget_destroy(gtk_window);
    gtk_main_quit();
}

static void display_buttons(GtkWindow *window)
{
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER (window), grid);

    int num_col = 0;
    if ((num_buttons % buttons_per_row) == 0)
    {
        num_col = (num_buttons / buttons_per_row);
    }
    else
    {
        num_col = (num_buttons / buttons_per_row) + 1;
    }

    GtkWidget *but[buttons_per_row][num_col];

    int count = 0;
    for (int i = 0; i < buttons_per_row; i++)
    {
        for (int j = 0; j < num_col; j++)
        {
            but[i][j] = gtk_button_new_with_label(buttons[count].text);
            gtk_widget_set_name(but[i][j], buttons[count].label);
            gtk_label_set_yalign(GTK_LABEL(
                        gtk_bin_get_child(GTK_BIN(but[i][j]))), 0.9);
            g_signal_connect(but[i][j], "clicked", G_CALLBACK(execute),
                        buttons[count].action);
            gtk_widget_set_hexpand(but[i][j], TRUE);
            gtk_widget_set_vexpand(but[i][j], TRUE);
            gtk_grid_attach(GTK_GRID(grid), but[i][j], i, j, 1, 1);
            count++;
        }
    }
}

static gboolean get_buttons(FILE *json)
{
    fseek(json, 0L, SEEK_END);
    int length = ftell(json);
    rewind(json);

    char *buffer = malloc(length);
    if (!buffer)
    {
        g_warning("Failed to allocate memory\n");
        return TRUE;
    }
    fread(buffer, 1, length, json);

    jsmn_parser p;
    jsmntok_t *tok = malloc(default_size * sizeof(jsmntok_t));
    if (!tok)
    {
        g_warning("Failed to allocate memory\n");
        return TRUE;
    }
    jsmn_init(&p);
    int numtok, i = 1;
    do
    {
        numtok = jsmn_parse(&p, buffer, length, tok, default_size * i);
        if (numtok == JSMN_ERROR_NOMEM)
        {
            i++;
            jsmntok_t *tmp = realloc(tok,
                    ((default_size * i) * sizeof(jsmntok_t)));
            if (!tmp)
            {
                free(tok);
                return FALSE;
            }
            else if (tmp != tok)
            {
                tok = tmp;
            }
        }
        else
        {
            break;
        }
    } while (TRUE);

    if (numtok < 0)
    {
        g_warning("Failed to parse json data\n");
        return TRUE;
    }
    
    for (int i = 0; i < numtok; i++)
    {
        if (tok[i].type == JSMN_OBJECT)
        {
            num_buttons++;
        }
        else if (tok[i].type == JSMN_STRING)
        {
            int length = tok[i].end - tok[i].start;
            char tmp[length + 1];
            get_substring(tmp, tok[i].start, tok[i].end, buffer);
            i++;
            length = tok[i].end - tok[i].start;

            if (strcmp(tmp, "label") == 0)
            {
                char buf[length + 1];
                get_substring(buf, tok[i].start, tok[i].end, buffer);
                buttons[num_buttons - 1].label = malloc(sizeof(char)
                        * length + 1);
                strcpy(buttons[num_buttons - 1].label, buf);
            }
            else if (strcmp(tmp, "action") == 0)
            {
                char buf[length + 1];
                get_substring(buf, tok[i].start, tok[i].end, buffer);
                buttons[num_buttons - 1].action = malloc(sizeof(char)
                        * length + 1);
                strcpy(buttons[num_buttons - 1].action, buf);
            }
            else if (strcmp(tmp, "text") == 0)
            {
                char buf[length + 1];
                get_substring(buf, tok[i].start, tok[i].end, buffer);
                buttons[num_buttons - 1].text = malloc(sizeof(char)
                        * length + 1);
                strcpy(buttons[num_buttons - 1].text, buf);
            }
            else
            {
                g_warning("Invalid key %s\n", tmp);
                return TRUE;
            }
        }
        else
        {
            g_warning("Invalid JSON Data\n");
            return TRUE;
        }
    }
    
    free(tok);
    free(buffer);
    fclose(json);

    return FALSE;
}

char *get_substring(char *s, int start, int end, char *buf)
{
    memcpy(s, &buf[start], (end - start));
    s[end - start] = '\0';
    return s;
}

static GtkWidget *get_window()
{
    GtkWindow *window = GTK_WINDOW (gtk_window_new (GTK_WINDOW_TOPLEVEL));
    gtk_layer_init_for_window (window);
    gtk_layer_set_layer (window, GTK_LAYER_SHELL_LAYER_OVERLAY);
    gtk_layer_set_exclusive_zone (window, exclusive_level);
    gtk_layer_set_keyboard_interactivity (window, FALSE);

    static const gboolean anchors[] = {TRUE, TRUE, TRUE, TRUE};
    for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
        gtk_layer_set_anchor (window, i, anchors[i]);
    }

    return GTK_WIDGET(window);
}

gboolean process_args(int argc, char *argv[])
{
    static struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"layout", required_argument, NULL, 'l'},
        {"version", no_argument, NULL, 'v'},
        {"css", required_argument, NULL, 'C'},
        {"margin", required_argument, NULL, 'm'},
        {"buttons-per-row", required_argument, NULL, 'b'},
        {0, 0, 0, 0}
    };

    const char *help =
        "Usage: wlogout [options]\n"
        "\n"
        "   -h, --help              Show help message and stop\n"
        "   -l, --layout            Specify a layout file\n"
        "   -v, --version           Show version number and stop\n"
        "   -C, --css               Specify a css file\n"
        "   -m, --margin            Set margin around buttons\n"
        "   -b, --buttons-per-row   Set the number of buttons per row\n"
        "\n";

    int c;
    while (TRUE)
    {
        int option_index = 0;
        c = getopt_long(argc, argv, "hl:vc:m:b:", long_options, &option_index);
        if (c == -1)
        {
            break;
        }
        switch (c)
        {
            case 'h':
                g_print(help);
                return TRUE;
            case 'l':
                layout_path = strdup(optarg);
                break;
            case 'v':
                g_print(version);
                return TRUE;
            case 'C':
                css_path = strdup(optarg);
                break;
            case 'm':
                margin = atoi(optarg);
                break;
            case 'b':
                buttons_per_row = atoi(optarg);
                break;
        }
    }
    return FALSE;
}

gboolean get_layout_path()
{
    if (layout_path)
    {
        return FALSE;
    }
    else if (access("/etc/wlogout/layout", F_OK) != -1)
    {
        layout_path = "/etc/wlogout/layout";
        return FALSE;
    }
    else if (access("/usr/local/etc/wlogout/layout", F_OK) != -1)
    {
        layout_path = "/usr/local/etc/wlogout/layout";
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static void load_css()
{
    GtkCssProvider *css = gtk_css_provider_new();

    if (!css_path)
    {
        if (access("/etc/wlogout/style.css", F_OK) != -1)
        {
            css_path = "/etc/wlogout/style.css";
        }
        else if (access("/usr/local/etc/wlogout/style.css", F_OK) != -1)
        {
            css_path = "/usr/local/etc/wlogout/style.css";
        }
    }

    GError *error = NULL;
    gtk_css_provider_load_from_path(css, css_path, &error);
    if (error)
    {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
            GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_USER);
}
