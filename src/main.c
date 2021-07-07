#include <gtk/gtk.h>

typedef struct {
    GtkTextBuffer *textbuffer1;
} app_widgets;

// globals
GtkTextBuffer *textbuffer1;
GtkSpinButton *sb_hr;
GtkSpinButton *sb_min;
GtkSpinButton *sb_sec;
GtkSpinButton *sb_int;
GtkSpinButton *sb_numint;
/*
int get_values() {
    gdouble hr = gtk_spin_button_get_value (sb_hr);
	//sprintf(tmp, "spin=%d", (int) );
    printf("hr=%d\n", (int)hr);
    return 0;
}
*/
void _on_clicked(GtkButton *b, app_widgets *w) {
  printf("clicked.\n" );

  //get_values();
  GtkTextMark *mark;
  GtkTextIter iter;
  FILE *fp;
  char path[1035];

  /* Open the command for reading. */
  fp = popen("/usr/bin/aa < /home/craig/rust/astronomical-almanac/aa.que", "r");

  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit(1);
  }

  /* Read the output a line at a time - output it. */
  while (fgets(path, sizeof(path), fp) != NULL) {
    mark = gtk_text_buffer_get_insert (w->textbuffer1);
    gtk_text_buffer_get_iter_at_mark (w->textbuffer1, &iter, mark);
    gtk_text_buffer_insert (w->textbuffer1, &iter, path, -1);
  }

  /* close */
  pclose(fp);
}

void hours_changed() {
}

int main(int argc, char *argv[])
{
    GtkBuilder      *builder; 
    GtkWidget       *window;
    app_widgets     *widgets = g_slice_new(app_widgets);

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("glade/example.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    widgets->textbuffer1  = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "textbuffer1"));
    sb_hr  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb_hr"));
    sb_min  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb_min"));
    sb_sec  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb_sec"));
    sb_int  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb_int"));
    sb_numint  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb_numint"));

    gtk_builder_connect_signals(builder, widgets);


    g_object_unref(builder);

    gtk_widget_show(window);                
    gtk_main();
    
    g_slice_free(app_widgets, widgets);

    return 0;
}

// called when window is closed
void on_window1_destroy()
{
    gtk_main_quit();
}
