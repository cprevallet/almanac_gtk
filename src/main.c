#include <gtk/gtk.h>

// globals
GtkTextBuffer *textbuffer1;
GtkSpinButton *sb_hr;
GtkSpinButton *sb_min;
GtkSpinButton *sb_sec;
GtkSpinButton *sb_int;
GtkSpinButton *sb_numint;
GtkSpinButton *sb_lat;
GtkSpinButton *sb_lng;
GtkSpinButton *sb_height;
GtkSpinButton *sb_temp;
GtkSpinButton *sb_press;
GtkCalendar   *cal;
GtkComboBox   *cb_planet;

// Store the GUI variables in user's home directory
// for use with aa.
int store_ini() {
    FILE *fp, *fopen();
    char s[84];
    char *t = getenv("HOME");
    strcpy(s, "aa.ini");
    if (t && strlen(t)<70)
      {
        strcpy(s,t);
        strcat(s,"/.aa.ini");
      }
    if ((fp=fopen(s,"w+"))) {
        fprintf(fp, "%s\n", s);
        fprintf(fp, "%f ;Terrestrial east longitude of observer, degrees \n", gtk_spin_button_get_value(sb_lng));
        fprintf(fp, "%f ;Geodetic latitude, degrees\n", gtk_spin_button_get_value(sb_lat));
        fprintf(fp, "%f ;Height above sea level, meters\n", gtk_spin_button_get_value(sb_height));
        fprintf(fp, "%f ;Atmospheric temperature, deg C\n", gtk_spin_button_get_value(sb_temp));
        fprintf(fp, "%f ;Atmospheric pressure, millibars\n", gtk_spin_button_get_value(sb_press));
        fprintf(fp, "%d ; 0 - TDT=UT, 1 - input=TDT, 2 - input=UT\n", 2);
        fprintf(fp, "%d ; Use this deltaT (sec) if nonzero, else compute it.\n", 0);
    }
    return 0;
}

int get_values() {
    int hr = gtk_spin_button_get_value_as_int(sb_hr);
    int min = gtk_spin_button_get_value_as_int(sb_min);
    int sec = gtk_spin_button_get_value_as_int(sb_sec);
    float interval = gtk_spin_button_get_value(sb_int);
    int num_intervals = gtk_spin_button_get_value_as_int(sb_numint);
    guint year;
    guint mon;
    guint day;
    gtk_calendar_get_date(cal, &year, &mon, &day);
    gint active_item = gtk_combo_box_get_active(cb_planet);
    FILE *fp;
    fp = fopen("/tmp/aa.txt", "w+");
    if(fp) {
        fprintf(fp, "%d\n", year);
        fprintf(fp, "%d\n", mon+1);
        fprintf(fp, "%d\n", day);
        fprintf(fp, "%d\n", hr);
        fprintf(fp, "%d\n", min);
        fprintf(fp, "%d\n", sec);
        fprintf(fp, "%f\n", interval);
        fprintf(fp, "%d\n", num_intervals);
        fprintf(fp, "%d\n", active_item);
    }
    fclose(fp);
        return 0;
}

void _on_clicked(GtkButton *b) {
  store_ini();
  get_values();
  GtkTextMark *mark;
  GtkTextIter iter;
  FILE *fp;
  char line[1035]; //why this big???

  // Open the command for reading.
  fp = popen("/usr/bin/aa < /tmp/aa.txt", "r");

  if (fp == NULL) {
    printf("Failed to run command\n" );
    printf("Astronomical almanac must be installed for this program to function.\n" );
    exit(1);
  }

  // Read the output a line at a time - and display it.
  while (fgets(line, sizeof(line), fp) != NULL) {
    mark = gtk_text_buffer_get_insert (textbuffer1);
    gtk_text_buffer_get_iter_at_mark (textbuffer1, &iter, mark);
    gtk_text_buffer_insert (textbuffer1, &iter, line, -1);
  }

  // close
  pclose(fp);
}

int main(int argc, char *argv[])
{
    GtkBuilder      *builder; 
    GtkWidget       *window;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("glade/example.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    textbuffer1  = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "textbuffer1"));
    sb_hr  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb1"));
    sb_min  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb2"));
    sb_sec  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb3"));
    sb_int  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb4"));
    sb_numint  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb5"));
    sb_lat  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb6"));
    sb_lng  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb7"));
    sb_height  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb8"));
    sb_temp  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb9"));
    sb_press  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb10"));
    cal = GTK_CALENDAR(gtk_builder_get_object(builder, "cal")); 
    cb_planet = GTK_COMBO_BOX(gtk_builder_get_object(builder, "cb_planet1"));

    // gtk_builder_connect_signals(builder, widgets);
    gtk_builder_connect_signals(builder, NULL);


    g_object_unref(builder);

    gtk_widget_show(window);                
    gtk_main();
    
    return 0;
}

// called when window is closed
void on_window1_destroy()
{
    gtk_main_quit();
}
