#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include <gtk/gtk.h>
#include "jsmn.h"
#ifdef LAYERSHELL
#include <gtk-layer-shell/gtk-layer-shell.h>
#endif

#ifdef LAYERSHELL
static const int exclusive_level = -1;
#endif
static const int default_size = 100;
static const char *version = "1.2";

typedef struct
{
    char *label;
    char *action;
    char *text;
    float yalign;
    float xalign;
    guint bind;
    gboolean circular;
} button;

static char *command = NULL;
static char *layout_path = NULL;
static char *css_path = NULL;
static button *buttons = NULL;
static GtkWidget *gtk_window = NULL;

static int num_buttons = 0;

static int buttons_per_row = 3;
static int primary_monitor = 0;
static int margin[] = {230, 230, 230, 230};
static int space[] = {0, 0};
static gboolean protocol = TRUE;
static gboolean show_bind = FALSE;
static gboolean no_span = FALSE;

static gboolean process_args(int argc, char *argv[])
{
    static struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"layout", required_argument, NULL, 'l'},
        {"version", no_argument, NULL, 'v'},
        {"css", required_argument, NULL, 'C'},
        {"margin", required_argument, NULL, 'm'},
        {"margin-top", required_argument, NULL, 'T'},
        {"margin-bottom", required_argument, NULL, 'B'},
        {"margin-left", required_argument, NULL, 'L'},
        {"margin-right", required_argument, NULL, 'R'},
        {"buttons-per-row", required_argument, NULL, 'b'},
        {"column-spacing", required_argument, NULL, 'c'},
        {"row-spacing", required_argument, NULL, 'r'},
        {"protocol", required_argument, NULL, 'p'},
        {"show-binds", no_argument, NULL, 's'},
        {"no-span", no_argument, NULL, 'n'},
        {"primary-monitor", required_argument, NULL, 'P'},
        {0, 0, 0, 0}
    };

    const char *help =
        "Usage: wlogout [options] [command]\n"
        "\n"
        "   -h, --help                      Show help message and stop\n"
        "   -l, --layout <layout>           Specify a layout file\n"
        "   -v, --version                   Show version number and stop\n"
        "   -C, --css <css>                 Specify a css file\n"
        "   -b, --buttons-per-row <num>     Set the number of buttons per row\n"
        "   -c  --column-spacing <space>    Set space between buttons columns\n"
        "   -r  --row-spacing <space>       Set space between buttons rows\n"
        "   -m, --margin <padding>          Set margin around buttons\n"
        "   -L, --margin-left <padding>     Set margin for left of buttons\n"
        "   -R, --margin-right <padding>    Set margin for right of buttons\n"
        "   -T, --margin-top <padding>      Set margin for top of buttons\n"
        "   -B, --margin-bottom <padding>   Set margin for bottom of buttons\n"
        "   -p, --protocol <protocol>       Use layer-shell or xdg protocol\n"
        "   -s, --show-binds                Show the keybinds on their corresponding button\n"
        "   -n, --no-span                   Stops from spanning across multiple monitors\n"
        "   -P, --primary-monitor <num>     Set the monitor that buttons appear on\n";

    int c;
    while (TRUE)
    {
        int option_index = 0;
        c = getopt_long(argc, argv, "hl:vc:m:b:T:R:L:B:r:c:p:C:sP:n",
                long_options, &option_index);
        if (c == -1)
        {
            break;
        }
        switch (c)
        {
            case 'm':
                margin[0] = atoi(optarg);
                margin[1] = atoi(optarg);
                margin[2] = atoi(optarg);
                margin[3] = atoi(optarg);
                break;
            case 'L':
                margin[2] = atoi(optarg);
                break;
            case 'T':
                margin[0] = atoi(optarg);
                break;
            case 'B':
                margin[1] = atoi(optarg);
                break;
            case 'R':
                margin[3] = atoi(optarg);
                break;
            case 'c':
                space[1] = atoi(optarg);
                break;
            case 'r':
                space[0] = atoi(optarg);
                break;
            case 'h':
                g_print("%s\n", help);
                return TRUE;
            case 'l':
                layout_path = g_strdup(optarg);
                break;
            case 'v':
                g_print("%s\n", version);
                return TRUE;
            case 'C':
                css_path = g_strdup(optarg);
                break;
            case 'b':
                buttons_per_row = atoi(optarg);
                break;
            case 'p':
                if (strcmp("layer-shell", optarg) == 0) {
                    protocol = TRUE;
                }
                else if (strcmp("xdg", optarg) == 0) {
                    protocol = FALSE;
                }
                else {
                    g_print("%s is an invalid protocol\n", optarg);
                    return TRUE;
                }
                break;
            case 's':
                show_bind = TRUE;
                break;
            case 'P':
                primary_monitor = atoi(optarg);
                break;
            case 'n':
                no_span = TRUE;
                break;
        }
    }
    return FALSE;
}

static gboolean get_layout_path()
{
    char *buf = malloc(default_size * sizeof(char));
    if (!buf)
    {
        fprintf(stderr, "Failed to allocate memory\n");
    }

    char *tmp = getenv("XDG_CONFIG_HOME");
    char *config_path = g_strdup(tmp);
    if (!config_path)
    {
        config_path = getenv("HOME");
        int n = snprintf(buf, default_size, "%s/.config", config_path);
        if (n != 0)
        {
            free(buf);
            buf = malloc((default_size * sizeof(char)) + (sizeof(char) * n));
            snprintf(buf, (default_size * sizeof(char)) + (sizeof(char) * n),
                    "%s/.config", config_path);
        }
        config_path = g_strdup(buf);
    }

    int n = snprintf(buf, default_size, "%s/wlogout/layout", config_path);
    if (n != 0)
    {
        free(buf);
        buf = malloc((default_size * sizeof(char)) + (sizeof(char) * n));
        snprintf(buf, (default_size * sizeof(char)) + (sizeof(char) * n),
                "%s/wlogout/layout", config_path);
    }
    free(config_path);

    if (layout_path)
    {
        free(buf);
        return FALSE;
    }
    else if (access(buf, F_OK) != -1)
    {
        layout_path = g_strdup(buf);
        free(buf);
        return FALSE;
    }
    else if (access("/etc/wlogout/layout", F_OK) != -1)
    {
        layout_path = "/etc/wlogout/layout";
        free(buf);
        return FALSE;
    }
    else if (access("/usr/local/etc/wlogout/layout", F_OK) != -1)
    {
        layout_path = "/usr/local/etc/wlogout/layout";
        free(buf);
        return FALSE;
    }
    else
    {
        free(buf);
        return TRUE;
    }
}

static gboolean get_css_path()
{
    char *buf = malloc(default_size * sizeof(char));
    if (!buf)
    {
        fprintf(stderr, "Failed to allocate memory\n");
    }

    char *tmp = getenv("XDG_CONFIG_HOME");
    char *config_path = g_strdup(tmp);
    if (!config_path)
    {
        config_path = getenv("HOME");
        int n = snprintf(buf, default_size, "%s/.config", config_path);
        if (n != 0)
        {
            free(buf);
            buf = malloc((default_size * sizeof(char)) + (sizeof(char) * n));
            snprintf(buf, (default_size * sizeof(char)) + (sizeof(char) * n),
                    "%s/.config", config_path);
        }
        config_path = g_strdup(buf);
    }

    int n = snprintf(buf, default_size, "%s/wlogout/style.css", config_path);
    if (n != 0)
    {
        free(buf);
        buf = malloc((default_size * sizeof(char)) + (sizeof(char) * n));
        snprintf(buf, (default_size * sizeof(char)) + (sizeof(char) * n),
                "%s/wlogout/style.css", config_path);
    }
    free(config_path);

    if (css_path)
    {
        free(buf);
        return FALSE;
    }
    else if (access(buf, F_OK) != -1)
    {
        css_path = g_strdup(buf);
        free(buf);
        return FALSE;
    }
    else if (access("/etc/wlogout/style.css", F_OK) != -1)
    {
        css_path = "/etc/wlogout/style.css";
        free(buf);
        return FALSE;
    }
    else if (access("/usr/local/etc/wlogout/style.css", F_OK) != -1)
    {
        css_path = "/usr/local/etc/wlogout/style.css";
        free(buf);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static gboolean background_clicked(GtkWidget *widget, GdkEventButton event, gpointer user_data) {
    gtk_main_quit();
    return TRUE;
}

static GtkWidget *get_window()
{
    GdkDisplay *display = gdk_display_get_default();
    int num_of_monitors = gdk_display_get_n_monitors(display);
    GtkWindow **window = malloc(num_of_monitors * sizeof(GtkWindow *));
    GtkWidget **box = malloc(num_of_monitors * sizeof(GtkWidget *));

    for (int i = 0; i < num_of_monitors; i++)
    {
        window[i] = GTK_WINDOW (gtk_window_new (GTK_WINDOW_TOPLEVEL));
        box[i] = gtk_event_box_new();
    }

    GdkScreen *screen = gdk_screen_get_default();
    gboolean layershell = FALSE;
    gint monitor = primary_monitor;

    #ifdef LAYERSHELL
    layershell = gtk_layer_is_supported();
    #else
    printf("wlogout was not compiled with layer shell support\n");
    #endif

    if (protocol && layershell) {
        #ifdef LAYERSHELL
        GdkMonitor **monitors = malloc(num_of_monitors * sizeof(GdkMonitor *));
        for (int i = 0; i < num_of_monitors; i++)
        {
            monitors[i] = gdk_display_get_monitor(display, i);
            gtk_layer_init_for_window (window[i]);
            gtk_layer_set_layer (window[i], GTK_LAYER_SHELL_LAYER_OVERLAY);
            gtk_layer_set_exclusive_zone (window[i], exclusive_level);

            for (int j = 0; j < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; j++) {
                gtk_layer_set_anchor (window[i], j, TRUE);
            }
            gtk_layer_set_monitor(window[i], monitors[i]);
        }

        gtk_layer_set_keyboard_interactivity (window[monitor], TRUE);
        #endif
    }
    else {
        for (int i = 0; i < num_of_monitors; i++)
        {
            gtk_window_fullscreen_on_monitor(window[i], screen, i);
        }
    }

    for (int i = 0; i < num_of_monitors; i++)
    {
        if (i != primary_monitor && !no_span)
        {
            // add event box to exit when clicking the background
            gtk_container_add(GTK_CONTAINER(window[i]), box[i]);
            g_signal_connect(box[i], "button-press-event", G_CALLBACK(background_clicked), NULL);
            gtk_widget_show_all(GTK_WIDGET(window[i]));
        }
    }

    return GTK_WIDGET(window[monitor]);
}

static char *get_substring(char *s, int start, int end, char *buf)
{
    memcpy(s, &buf[start], (end - start));
    s[end - start] = '\0';
    return s;
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
        free(tok);
        g_warning("Failed to parse JSON data\n");
        return TRUE;
    }

    for (int i = 0; i < numtok; i++)
    {
        if (tok[i].type == JSMN_OBJECT)
        {
            num_buttons++;
            buttons[num_buttons - 1].yalign = 0.9;
            buttons[num_buttons - 1].xalign = 0.5;
            buttons[num_buttons - 1].circular = FALSE;
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
                        * (length + 1));
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
                /* Add a small buffer to allocated memory so the keybind
                 * can easily be concatenated later if needed */
                int keybind_buffer = sizeof(guint) + (sizeof(char) * 2);
                buttons[num_buttons - 1].text = malloc((sizeof(char)
                        * (length + 1)) + keybind_buffer);
                strcpy(buttons[num_buttons - 1].text, buf);
            }
            else if (strcmp(tmp, "keybind") == 0)
            {
                if (length != 1)
                {
                    fprintf(stderr, "Invalid keybind\n");
                }
                else
                {
                    buttons[num_buttons - 1].bind = buffer[tok[i].start];
                }
            }
            else if (strcmp(tmp, "height") == 0)
            {
                if (tok[i].type != JSMN_PRIMITIVE || !isdigit(buffer[tok[i].start]))
                {
                    fprintf(stderr, "Invalid height\n");
                }
                else
                {
                    buttons[num_buttons - 1].yalign = buffer[tok[i].start];
                }
            }
            else if (strcmp(tmp, "width") == 0)
            {
                if (tok[i].type != JSMN_PRIMITIVE || !isdigit(buffer[tok[i].start]))
                {
                    fprintf(stderr, "Invalid width\n");
                }
                else
                {
                    buttons[num_buttons - 1].xalign = buffer[tok[i].start];
                }
            }
            else if (strcmp(tmp, "circular") == 0)
            {
                if (tok[i].type != JSMN_PRIMITIVE)
                {
                    fprintf(stderr, "Invalid boolean\n");
                }
                else
                {
                    if (buffer[tok[i].start] == 't') {
                        buttons[num_buttons - 1].circular = TRUE;
                    }
                    else {
                        buttons[num_buttons - 1].circular = FALSE;
                    }
                }
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

static void execute(GtkWidget *widget, char *action)
{
    command = malloc(strlen(action) * sizeof(char) + 1);
    strcpy(command, action);
    gtk_widget_destroy(gtk_window);
    gtk_main_quit();
}

static gboolean check_key(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    if (event->keyval == GDK_KEY_Escape) {
        gtk_main_quit();
        return TRUE;
    }
    for (int i = 0; i < num_buttons; i++)
    {
        if (buttons[i].bind == event->keyval)
        {
            execute(NULL, buttons[i].action);
            return TRUE;
        }
    }
    return FALSE;
}

static void load_buttons(GtkContainer *container)
{
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(container, grid);

    gtk_grid_set_row_spacing(GTK_GRID(grid), space[0]);
    gtk_grid_set_column_spacing(GTK_GRID(grid), space[1]);

    gtk_widget_set_margin_top(grid, margin[0]);
    gtk_widget_set_margin_bottom(grid, margin[1]);
    gtk_widget_set_margin_start(grid, margin[2]);
    gtk_widget_set_margin_end(grid, margin[3]);

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
            if (buttons[count].text && show_bind)
            {
                strcat(buttons[count].text, "[");
                strcat(buttons[count].text, (char *) &buttons[count].bind);
                strcat(buttons[count].text, "]");
            }
            but[i][j] = gtk_button_new_with_label(buttons[count].text);
            gtk_widget_set_name(but[i][j], buttons[count].label);
            gtk_label_set_yalign(GTK_LABEL(
                        gtk_bin_get_child(GTK_BIN(but[i][j]))), buttons[count].yalign);
            gtk_label_set_xalign(GTK_LABEL(
                        gtk_bin_get_child(GTK_BIN(but[i][j]))), buttons[count].xalign);
            if (buttons[count].circular) {
                gtk_style_context_add_class(
                    gtk_widget_get_style_context(but[i][j]), "circular");
            }
            g_signal_connect(but[i][j], "clicked", G_CALLBACK(execute),
                        buttons[count].action);
            gtk_widget_set_hexpand(but[i][j], TRUE);
            gtk_widget_set_vexpand(but[i][j], TRUE);
            gtk_grid_attach(GTK_GRID(grid), but[i][j], i, j, 1, 1);
            count++;
        }
    }
}

static void load_css()
{
    GtkCssProvider *css = gtk_css_provider_new();
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
        g_warning("Failed to find a layout\n");
        return 1;
    }

    if (get_css_path())
    {
        g_warning("Failed to find css file\n");
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
    g_signal_connect(gtk_window, "key_press_event", G_CALLBACK(check_key), NULL);

    GtkWidget *box = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(gtk_window), box);
    g_signal_connect(box, "button-press-event", G_CALLBACK(background_clicked), NULL);

    load_buttons(GTK_CONTAINER(box));
    load_css();
    gtk_widget_show_all(gtk_window);

    gtk_main();
    system(command);

    for (int i = 0; i < num_buttons; i++)
    {
        free(buttons[i].label);
        free(buttons[i].action);
        free(buttons[i].text);
    }
    free(buttons);
    free(command);
}
