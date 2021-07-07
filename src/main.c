#include <gtk/gtk.h>

// make UI elements globals (ick)
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
GtkSpinButton *sb_dt;
GtkCalendar   *cal;
GtkComboBox   *cb_planet;
GtkRadioButton *rb_TTUT;
GtkRadioButton *rb_TT;
GtkRadioButton *rb_UT;

// Store the GUI variables in user's home directory as ana.ini
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
    if ((fp=fopen(s,"w"))) {
        fprintf(fp, "%f ;Terrestrial east longitude of observer, degrees \n", gtk_spin_button_get_value(sb_lng));
        fprintf(fp, "%f ;Geodetic latitude, degrees\n", gtk_spin_button_get_value(sb_lat));
        fprintf(fp, "%f ;Height above sea level, meters\n", gtk_spin_button_get_value(sb_height));
        fprintf(fp, "%f ;Atmospheric temperature, deg C\n", gtk_spin_button_get_value(sb_temp));
        fprintf(fp, "%f ;Atmospheric pressure, millibars\n", gtk_spin_button_get_value(sb_press));
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rb_TTUT))) {
            fprintf(fp, "%d ; 0 - TDT=UT, 1 - input=TDT, 2 - input=UT\n", 0);
        }
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rb_TT))) {
            fprintf(fp, "%d ; 0 - TDT=UT, 1 - input=TDT, 2 - input=UT\n", 1);
        }
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rb_UT))) {
            fprintf(fp, "%d ; 0 - TDT=UT, 1 - input=TDT, 2 - input=UT\n", 2);
        }
        fprintf(fp, "%f ; Use this deltaT (sec) if nonzero, else compute it.\n", gtk_spin_button_get_value(sb_dt));
        fclose(fp);
        return 0;
    } else {
        return -1;
    }
}

// Store other values aa expects in a temp file.
int store_values() {
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
        fclose(fp);
        return 0;
    } else {
        return -1;
    }
}

// Load the values stored in the ini file as initial widget values.
// Also initialize the calendar and time widgets with the current UTC values.
int initialize_widgets() {
  time_t rawtime;
  struct tm *utc;
  time(&rawtime);
  /* Get GMT time */
  utc = gmtime(&rawtime );
  gtk_spin_button_set_value(sb_hr, utc->tm_hour);
  gtk_spin_button_set_value(sb_min, utc->tm_min);
  gtk_spin_button_set_value(sb_sec, utc->tm_sec);
  gtk_calendar_select_month(cal, utc->tm_mon, utc->tm_year+1900);
  gtk_calendar_select_day(cal, utc->tm_mday);
  FILE *f, *fopen();
  char s[84]; //oddly specific
  double tlong, glat, height, attemp, atpress, dtgiven;
  int jdflag;
  char *t = getenv("HOME");
  strcpy(s, "aa.ini");
  if (t && strlen(t)<70) 
     {
       strcpy(s,t);
       strcat(s,"/.aa.ini");
     }
  if( (f=fopen(s,"r")) ) {
	fgets( s, 80, f );
	sscanf( s, "%lf", &tlong );
    gtk_spin_button_set_value(sb_lng, tlong);
	fgets( s, 80, f );
	sscanf( s, "%lf", &glat );
    gtk_spin_button_set_value(sb_lat, glat);
	fgets( s, 80, f );
	sscanf( s, "%lf", &height );
    gtk_spin_button_set_value(sb_height, height);
	fgets( s, 80, f );
	sscanf( s, "%lf", &attemp );
    gtk_spin_button_set_value(sb_temp, attemp);
	fgets( s, 80, f );
	sscanf( s, "%lf", &atpress );
    gtk_spin_button_set_value(sb_press, atpress);
	fgets( s, 80, f );
	sscanf( s, "%d", &jdflag );
	switch( jdflag )
		{case 0:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(rb_TTUT), TRUE);
		    break;
		case 1:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(rb_TT), TRUE);
			break;
		case 2: 
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(rb_UT), TRUE);
			break;
		default: printf("Illegal jdflag\n" );
		exit(2);
		}
	fgets( s, 80, f );
	sscanf( s, "%lf", &dtgiven );
    gtk_spin_button_set_value(sb_dt, dtgiven);
	fclose(f);
    return 0;
	}
  return -1; // should only get here if bad file read
}

void _clear_results (GtkButton *b) {
    GtkTextIter start;
    GtkTextIter end;
    //gtk_text_buffer_get_start_iter (textbuffer1, start);
    //gtk_text_buffer_get_end_iter(textbuffer1, end);
    //gtk_text_buffer_get_iter_at_line(textbuffer1, start, 0);
    //gtk_text_buffer_get_iter_at_line(textbuffer1, end, 1);
    //end = textbuffer1->get_iter_at_line(textbuffer1->get_line_count());
    gtk_text_buffer_get_bounds (textbuffer1, &start, &end);
    gtk_text_buffer_delete (textbuffer1, &start, &end);
}

void _on_clicked(GtkButton *b) {
  store_ini();
  store_values();
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
    sb_dt  = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "sb11"));
    cal = GTK_CALENDAR(gtk_builder_get_object(builder, "cal")); 
    cb_planet = GTK_COMBO_BOX(gtk_builder_get_object(builder, "cb_planet1"));
    rb_TTUT = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "rb1"));
    rb_TT = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "rb2"));
    rb_UT = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "rb3"));


    // gtk_builder_connect_signals(builder, widgets);
    gtk_builder_connect_signals(builder, NULL);

    // retrieve initial values from .ini file.
    initialize_widgets();

    g_object_unref(builder);

    gtk_widget_show(window);                
    gtk_main();
    
    return 0;
}

// called when window is closed
void on_window1_destroy()
{   
    store_ini();
    gtk_main_quit();
}
