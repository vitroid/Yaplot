#include <cairo.h>
#include <gtk/gtk.h>
#include <pango/pango.h>

static void do_drawing(cairo_t *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, 
    gpointer user_data)
{      
  do_drawing(cr);  

  return FALSE;
}

static void do_drawing(cairo_t *cr)
{
   cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
   cairo_paint(cr);
   cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
}

static void setup(GtkWidget *win)
{        
  gtk_widget_set_app_paintable(win, TRUE);
  gtk_window_set_type_hint(GTK_WINDOW(win), GDK_WINDOW_TYPE_HINT_DOCK);
  gtk_window_set_keep_below(GTK_WINDOW(win), TRUE);
  
  GdkScreen *screen = gdk_screen_get_default();
  GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
  
  if (visual != NULL && gdk_screen_is_composited(screen)) {
      gtk_widget_set_visual(win, visual);
  }  
}

int main (int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *lbl;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  
  setup(window);
  
  lbl = gtk_label_new("ZetCode, tutorials for programmers");
  
  PangoFontDescription *fd = pango_font_description_from_string("Serif 20");
  gtk_widget_modify_font(lbl, fd);  
  gtk_container_add(GTK_CONTAINER(window), lbl);  
  
  GdkColor color;
  gdk_color_parse("white", &color);
  gtk_widget_modify_fg(lbl, GTK_STATE_NORMAL, &color);
  
  g_signal_connect(G_OBJECT(window), "draw", 
      G_CALLBACK(on_draw_event), NULL); 
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 350, 250); 

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
