#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <gtk-layer-shell/gtk-layer-shell.h>
#include <gtk/gtk.h>
#include "jsmn.h"

#define EXCLUSIVE 2
#define MAX_SIZE 1024

static int buttons_per_row = 3;

typedef struct
{
    char label[MAX_SIZE];
    char action[MAX_SIZE];
    char text[MAX_SIZE];
} button;

static button buttons[MAX_SIZE];
static int num_buttons = 0;

static GtkWidget *get_window();
static char *get_string(char *s, int start, int end, char *buf);
static bool get_buttons(FILE *json);
static void display_buttons(GtkWindow *window);

int main (int argc, char *argv[])
{
    g_set_prgname("wlogout");
    gtk_init(&argc, &argv);
    
    FILE *inptr = fopen(argv[1], "r");
    if (!inptr)
    {
        fprintf(stderr, "Failed to open %s\n", argv[1]);
        return 2;
    }
    if (get_buttons(inptr))
    {
        fclose(inptr);
        return 3; 
    }

    GtkWidget *gtk_window = get_window();
    display_buttons(GTK_WINDOW(gtk_window));
    gtk_widget_show_all(gtk_window);

    gtk_main();
}

static void execute(GtkWidget *widget, char *action)
{
    gtk_main_quit();
    system(action);
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
            gtk_widget_set_hexpand(but[i][j], true);
            gtk_widget_set_vexpand(but[i][j], true);
            gtk_grid_attach(GTK_GRID(grid), but[i][j], i, j, 1, 1);
            count++;
        }
    }
}

static bool get_buttons(FILE *json)
{
    fseek(json, 0L, SEEK_END);
    int length = ftell(json);
    rewind(json);

    char *buffer = malloc(length);
    if (!buffer)
    {
        fprintf(stderr, "Failed to allocate memory\n");
        return true;
    }
    fread(buffer, 1, length, json);

    jsmn_parser p;
    jsmntok_t *tok = malloc(MAX_SIZE);
    if (!tok)
    {
        fprintf(stderr, "Failed to allocate memory\n");
        return true;
    }
    jsmn_init(&p);
    int numtok = jsmn_parse(&p, buffer, length, tok, MAX_SIZE);

    if (numtok < 0)
    {
        fprintf(stderr, "Failed to parse json data\n");
        return true;
    }
    
    for (int i = 0; i < (numtok - 1); i++)
    {
        if (tok[i].type == JSMN_OBJECT)
        {
            num_buttons++;
        }
        else if (tok[i].type == JSMN_STRING)
        {
            char tmp[tok[i].end - tok[i].start + 1];
            get_string(tmp, tok[i].start, tok[i].end, buffer);

            if (strcmp(tmp, "label") == 0)
            {
                char buf[tok[i + 1].end - tok[i + 1].start + 1];
                get_string(buf, tok[i + 1].start, tok[i + 1].end, buffer);
                strcpy(buttons[num_buttons - 1].label, buf);
            }
            else if (strcmp(tmp, "action") == 0)
            {
                char buf[tok[i + 1].end - tok[i + 1].start + 1];
                get_string(buf, tok[i + 1].start, tok[i + 1].end, buffer);
                strcpy(buttons[num_buttons - 1].action, buf);
            }
            else if (strcmp(tmp, "text") == 0)
            {
                char buf[tok[i + 1].end - tok[i + 1].start + 1];
                get_string(buf, tok[i + 1].start, tok[i + 1].end, buffer);
                strcpy(buttons[num_buttons - 1].text, buf);
            }
        }
        else
        {
            fprintf(stderr, "Invalid JSON Data\n");
            return true;
        }
    }
    
    free(tok);
    free(buffer);
    fclose(json);
    return false;
}

char *get_string(char *s, int start, int end, char *buf)
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
    gtk_layer_set_exclusive_zone (window, EXCLUSIVE);

    static const gboolean anchors[] = {TRUE, TRUE, TRUE, TRUE};
    for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
        gtk_layer_set_anchor (window, i, anchors[i]);
    }

    return GTK_WIDGET(window);
}
