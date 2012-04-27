/*
 * Original author of parts of gtkpreview David D Lowe <daviddlowe.flimm@gmail.com>
 * Parts of cursor preveiw Copyright (c) 2008 Nick Schermer <nick@xfce.org> & Jannis Pohlmann <jannis@xfce.org>
 * from xfce4-settings-4.6.5/dialogs/mouse-settings
 *
 * Parts of xwm4 4.10pre
 *
 * oroborus - (c) 2001 Ken Lynch
 * xfwm4    - (c) 2002-2011 Olivier Fourdan
 *
 * Seriously mucked about by:
 *
 * K.D.Hedger 2012 <kdheger@yahoo.co.uk>
 *
 */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>

#include "globals.h"
#include "database.h"
#include "thumbnails.h"
#include "gui.h"
#include "callbacks.h"

bool			whoops=false;
GtkWidget*		progressWindow;
GtkWidget*		progressBar;

// RESET THEME
void resetTheme(GtkWidget* widget,gpointer data)
{
	char		command[256];
	GtkSettings *settings=gtk_settings_get_default();;

	sprintf(command,"%s\"%s\"",XCONFSETCONTROLS,currentGtkTheme);
	system(command);
	sprintf(command,"%s\"%s\"",XCONFSETFRAME,currentWmTheme);
	system(command);
	sprintf(command,"%s\"%s\"",XCONFSETICONS,currentIconTheme);
	system(command);
	sprintf(command,"%s\"%s\"",XCONFSETCURSOR,currentCursorTheme);
	system(command);
	sprintf(command,"%s\"%s\"",XCONFSETPAPER,currentWallPaper);
	system(command);
	sprintf(command,"%s%i",XCONFSETSTYLE,currentWallStyle);
	system(command);

	g_object_set(settings,"gtk-theme-name",currentGtkTheme,"gtk-color-scheme","default",NULL);
	gtk_combo_box_set_active((GtkComboBox*)styleComboBox,currentWallStyle);
}

void shutdown(GtkWidget* window,gpointer data)
{
	gtk_main_quit();
}

void rerunAndBuild(GtkWidget* window,gpointer data)
{
	char*	datax[]={(char*)"xfce-theme-manager",(char*)"-m",NULL};

	gtk_main_quit();
	execvp("xfce-theme-manager",datax);
}

void showAdvanced(GtkWidget* widget,gpointer data)
{
	gboolean	state=gtk_toggle_button_get_active((GtkToggleButton*)widget);

	if (state==true)
		gtk_notebook_set_current_page(advanced,1);
	else
		gtk_notebook_set_current_page(advanced,0);
}

void init(void)
{
	gchar	*stdout;

	asprintf(&themesArray[0],"%s/.themes",getenv("HOME"));
	asprintf(&themesArray[1],"%s",GLOBALTHEMES);
	
	asprintf(&iconsArray[0],"%s/.icons",getenv("HOME"));
	asprintf(&iconsArray[1],"%s",GLOBALICONS);

	asprintf(&papersArray[0],"%s/.local/share/xfce4/backdrops",getenv("HOME"));
	asprintf(&papersArray[1],"%s",GLOBALWALLPAPERS);


	asprintf(&dbFolder,"%s/.config/XfceThemeManager",getenv("HOME"));
	asprintf(&metaFolder,"%s/.config/XfceThemeManager/meta",getenv("HOME"));
	asprintf(&framesFolder,"%s/.config/XfceThemeManager/frames",getenv("HOME"));
	asprintf(&controlsFolder,"%s/.config/XfceThemeManager/controls",getenv("HOME"));
	asprintf(&iconsFolder,"%s/.config/XfceThemeManager/icons",getenv("HOME"));
	asprintf(&cursorsFolder,"%s/.config/XfceThemeManager/cursors",getenv("HOME"));
	asprintf(&wallpapersFolder,"%s/.config/XfceThemeManager/wallpapers",getenv("HOME"));

	g_spawn_command_line_sync("xfconf-query -c xfce4-desktop -vp /backdrop/screen0/monitor0/image-style",&stdout,NULL,NULL,NULL);
	stdout[strlen(stdout)-1]=0;
	currentWallStyle=atol(stdout);
	g_free(stdout);

	g_spawn_command_line_sync(XCONFGETCONTROLS,&currentGtkTheme,NULL,NULL,NULL);
	currentGtkTheme[strlen(currentGtkTheme)-1]=0;
	
	g_spawn_command_line_sync(XCONFGETICONS,&currentIconTheme,NULL,NULL,NULL);
	currentIconTheme[strlen(currentIconTheme)-1]=0;

	g_spawn_command_line_sync(XCONFGETCURSOR,&currentCursorTheme,NULL,NULL,NULL);
	currentCursorTheme[strlen(currentCursorTheme)-1]=0;

	g_spawn_command_line_sync(XCONFGETFRAME,&currentWmTheme,NULL,NULL,NULL);
	currentWmTheme[strlen(currentWmTheme)-1]=0;

	g_spawn_command_line_sync(XCONFGETPAPER,&currentWallPaper,NULL,NULL,NULL);
	currentWallPaper[strlen(currentWallPaper)-1]=0;

	g_spawn_command_line_sync(XCONFGETLAYOUT,&currentButtonLayout,NULL,NULL,NULL);
	currentButtonLayout[strlen(currentButtonLayout)-1]=0;

	g_spawn_command_line_sync(XCONFGETTITLEPOS,&currentTitlePos,NULL,NULL,NULL);
	currentTitlePos[strlen(currentTitlePos)-1]=0;

	g_spawn_command_line_sync(XCONFGETWMFONT,&currentWMFont,NULL,NULL,NULL);
	currentWMFont[strlen(currentWMFont)-1]=0;

	g_spawn_command_line_sync(XCONFGETAPPFONT,&currentAppFont,NULL,NULL,NULL);
	currentAppFont[strlen(currentAppFont)-1]=0;

}

void makeProgressBar(void)
{
	GtkWidget*		vbox;

	progressWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size((GtkWindow*)progressWindow,400,40);
	gtk_window_set_title((GtkWindow*)progressWindow,"Re-Building Database, Please Wait...");
	vbox=gtk_vbox_new(FALSE, 0);
	progressBar=gtk_progress_bar_new();
	gtk_progress_bar_pulse((GtkProgressBar*)progressBar);

	gtk_progress_bar_set_orientation((GtkProgressBar*)progressBar,GTK_PROGRESS_LEFT_TO_RIGHT);

	gtk_box_pack_start(GTK_BOX(vbox),progressBar,false,false,8);
	gtk_container_add(GTK_CONTAINER(progressWindow),vbox);

	gtk_widget_show_all(progressWindow);
}

gboolean updateBarTimer(gpointer data)
{	
	if(GTK_IS_PROGRESS_BAR((GtkProgressBar*)progressBar))
		{
			gtk_progress_bar_pulse((GtkProgressBar*)progressBar);
			return(true);
		}
	else
		return(false);
}

int main(int argc,char **argv)
{
	GtkWidget*		vbox;
	GtkNotebook*	notebook;
	GtkWidget*		buttonHbox;
	GtkWidget*		label;
	GtkWidget*		button;

//	GtkWidget*	advancedVbox;
	GtkWidget*	advancedScrollBox;
//	GtkWidget*	advancedHbox;
//	GtkWidget*	advancedRange;
//	GtkWidget*	advancedEntry;

	g_thread_init(NULL);
	gdk_threads_init();
	gtk_init(&argc, &argv);

	init();

	if (argc==2 && strcasecmp(argv[1],"-m")==0)
		{
		makeProgressBar();

		gdk_threads_enter();
			g_timeout_add (100, updateBarTimer, NULL);
			g_thread_create(rebuildDB,NULL,false,NULL);
			gtk_main();
		gdk_threads_leave();
		gtk_widget_destroy(progressWindow);
		}

	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size((GtkWindow*)window,400,470);
	g_signal_connect_after(G_OBJECT(window),"destroy",G_CALLBACK(shutdown),NULL);

//main window vbox
	vbox=gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window),(GtkWidget*)vbox);

	buildPages();

//notebook
	notebook=(GtkNotebook*)gtk_notebook_new();
	advanced=(GtkNotebook*)gtk_notebook_new();
 	gtk_notebook_set_show_tabs(advanced,false);
 
//pages
	label=gtk_label_new("Themes");
	gtk_notebook_append_page(notebook,themesScrollBox,label);

	label=gtk_label_new("Window Borders");
	gtk_notebook_append_page(notebook,framesScrollBox,label);

	label=gtk_label_new("Controls");
	gtk_notebook_append_page(notebook,controlsScrollBox,label);

	label=gtk_label_new("Icons");
	gtk_notebook_append_page(notebook,iconsScrollBox,label);

	label=gtk_label_new("Cursors");
	gtk_notebook_append_page(notebook,cursorsScrollBox,label);

	label=gtk_label_new("Wallpapers");
	gtk_notebook_append_page(notebook,wallpapersScrollBox,label);

	gtk_notebook_append_page(advanced,(GtkWidget*)notebook,NULL);

//do advanced gui
	advancedScrollBox=gtk_scrolled_window_new(NULL,NULL);
	buildAdvancedGtui(advancedScrollBox);
	gtk_notebook_append_page(advanced,advancedScrollBox,NULL);

#if 0	
	
	
	advancedVbox=gtk_vbox_new(FALSE, 0);

//comp ed
	gtk_box_pack_start(GTK_BOX(advancedVbox),gtk_label_new("Launch Xfce-Composite-Editor"),false,false,2);
	button=gtk_button_new_with_label("Xfce-Composite-Editor");
	g_signal_connect_after(G_OBJECT(button),"clicked",G_CALLBACK(launchCompEd),NULL);
	gtk_box_pack_start(GTK_BOX(advancedVbox),button,false,false,8);
	gtk_box_pack_start(GTK_BOX(advancedVbox),gtk_hseparator_new(),false,false,4);

	gtk_scrolled_window_add_with_viewport((GtkScrolledWindow*)advancedScrollBox,advancedVbox);
	gtk_notebook_append_page(advanced,advancedScrollBox,NULL);

//back drop aadj
	gtk_box_pack_start(GTK_BOX(advancedVbox),gtk_label_new("Backdrop Adjustments"),false,false,2);
	advancedHbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(advancedHbox),gtk_label_new("Brightness"), false,false,4);

//bright
	advancedRange=gtk_hscale_new_with_range(-128,127,1);
	gtk_scale_set_value_pos((GtkScale*)advancedRange,GTK_POS_LEFT);
	gtk_range_set_value((GtkRange*)advancedRange,0);
	g_signal_connect_after(G_OBJECT(advancedRange),"value-changed",G_CALLBACK(setBright),NULL);
	gtk_range_set_update_policy((GtkRange*)advancedRange,GTK_UPDATE_DISCONTINUOUS);
	gtk_box_pack_start(GTK_BOX(advancedHbox),advancedRange, true,true,0);
	gtk_box_pack_start(GTK_BOX(advancedVbox),advancedHbox, false,false,2);

	button=gtk_button_new_with_label("Reset");
	g_signal_connect_after(G_OBJECT(button),"clicked",G_CALLBACK(resetBright),(gpointer)advancedRange);
	gtk_box_pack_start(GTK_BOX(advancedHbox),button, false,false,8);

//satu
	advancedHbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(advancedHbox),gtk_label_new("Saturation"), false,false,4);

	advancedRange=gtk_hscale_new_with_range(-10.0,10,0.1);
	gtk_scale_set_value_pos((GtkScale*)advancedRange,GTK_POS_LEFT);
	gtk_range_set_value((GtkRange*)advancedRange,1.0);
	g_signal_connect_after(G_OBJECT(advancedRange),"value-changed",G_CALLBACK(setSatu),NULL);
	gtk_range_set_update_policy((GtkRange*)advancedRange,GTK_UPDATE_DISCONTINUOUS);
	gtk_box_pack_start(GTK_BOX(advancedHbox),advancedRange, true,true,0);
	gtk_box_pack_start(GTK_BOX(advancedVbox),advancedHbox, false,false,2);

	button=gtk_button_new_with_label("Reset");
	g_signal_connect_after(G_OBJECT(button),"clicked",G_CALLBACK(resetSatu),(gpointer)advancedRange);
	gtk_box_pack_start(GTK_BOX(advancedHbox),button, false,false,8);

	gtk_box_pack_start(GTK_BOX(advancedVbox),gtk_hseparator_new(),false,false,4);

//buton layout
	advancedHbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(advancedHbox),gtk_label_new("Button Layout"), false,false,4);

	advancedEntry=gtk_entry_new();
	gtk_entry_set_text((GtkEntry*)advancedEntry,currentButtonLayout);
	g_signal_connect_after(G_OBJECT(advancedEntry),"key-release-event",G_CALLBACK(changeLayout),NULL);
	gtk_box_pack_start(GTK_BOX(advancedHbox),advancedEntry, true,true,2);

	button=gtk_button_new_with_label("Reset");
	g_signal_connect_after(G_OBJECT(button),"clicked",G_CALLBACK(resetLayout),(gpointer)advancedEntry);
	gtk_box_pack_start(GTK_BOX(advancedHbox),button, false,false,8);

	gtk_box_pack_start(GTK_BOX(advancedVbox),advancedHbox, false,false,2);
	gtk_box_pack_start(GTK_BOX(advancedVbox),gtk_hseparator_new(),false,false,4);

//buildTitlePos
	gtk_box_pack_start(GTK_BOX(advancedVbox),buildTitlePos(), false,false,2);
	gtk_box_pack_start(GTK_BOX(advancedVbox),gtk_hseparator_new(),false,false,4);

//fonts
//wmfont
	gtk_box_pack_start(GTK_BOX(advancedVbox),gtk_label_new("Font Selection"), false,false,4);
	advancedHbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(advancedHbox),gtk_label_new("WM Font "), false,false,1);

	wmFontButton=gtk_font_button_new_with_font(currentWMFont);
	g_signal_connect_after(G_OBJECT(wmFontButton),"font-set",G_CALLBACK(setFont),(void*)0);
	gtk_box_pack_start(GTK_BOX(advancedHbox),wmFontButton, true,true,1);

	button=gtk_button_new_with_label("Reset");
	g_signal_connect_after(G_OBJECT(button),"clicked",G_CALLBACK(resetFont),(void*)0);
	gtk_box_pack_start(GTK_BOX(advancedHbox),button, false,false,1);

	gtk_box_pack_start(GTK_BOX(advancedVbox),advancedHbox, false,false,4);

//appfont
	advancedHbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(advancedHbox),gtk_label_new("App Font"), false,false,1);

	appFontButton=gtk_font_button_new_with_font(currentAppFont);
	g_signal_connect_after(G_OBJECT(appFontButton),"font-set",G_CALLBACK(setFont),(void*)1);
	gtk_box_pack_start(GTK_BOX(advancedHbox),appFontButton, true,true,1);

	button=gtk_button_new_with_label("Reset");
	g_signal_connect_after(G_OBJECT(button),"clicked",G_CALLBACK(resetFont),(void*)1);
	gtk_box_pack_start(GTK_BOX(advancedHbox),button, false,false,1);

	gtk_box_pack_start(GTK_BOX(advancedVbox),advancedHbox, false,false,4);

	gtk_box_pack_start(GTK_BOX(advancedVbox),gtk_hseparator_new(),false,false,4);
#endif

//add notebook to window
	gtk_container_add(GTK_CONTAINER(vbox),(GtkWidget*)advanced);

	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),false,false,4);

//do buttons
	buttonHbox=gtk_hbox_new(true,0);

	button=gtk_button_new_with_label("Reset Theme");
	gtk_box_pack_start(GTK_BOX(buttonHbox),button, false,false,0);
	g_signal_connect_after(G_OBJECT(button),"clicked",G_CALLBACK(resetTheme),NULL);

	button=gtk_button_new_with_label("Rebuild DB");
	gtk_box_pack_start(GTK_BOX(buttonHbox),button, false,false,0);
	g_signal_connect_after(G_OBJECT(button),"clicked",G_CALLBACK(rerunAndBuild),NULL);

	button=gtk_toggle_button_new_with_label("Advanced");
	gtk_box_pack_start(GTK_BOX(buttonHbox),button, false,false,0);
	g_signal_connect_after(G_OBJECT(button),"clicked",G_CALLBACK(showAdvanced),NULL);

	gtk_box_pack_start(GTK_BOX(vbox),buttonHbox, false,false, 8);

	buttonHbox=gtk_hbox_new(true,0);
	button=gtk_button_new_from_stock(GTK_STOCK_QUIT);
	g_signal_connect_after(G_OBJECT(button),"clicked",G_CALLBACK(shutdown),NULL);

	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),false,false,0);

	gtk_box_pack_start(GTK_BOX(buttonHbox),button, false,false,8);
	gtk_box_pack_start(GTK_BOX(vbox),buttonHbox, false,false,8);

//do dnd
	gtk_drag_dest_set(vbox,GTK_DEST_DEFAULT_ALL,NULL,0,GDK_ACTION_COPY);
	gtk_drag_dest_add_uri_targets(vbox);
	g_signal_connect (G_OBJECT(vbox),"drag_data_received",G_CALLBACK(dropUri), NULL);

	gtk_widget_show_all(window);
	gtk_main();
}
//
//gtkprev [border] /path/to/border /out/path/to/png
//gtkprev [controls] gtkthemename /out/path/to/png
//gtkprev [theme] gtkthemename /path/to/border /out/path/to/png
//gtkprev [cursors] cursortheme /out/path/to/png
//gtkprev [icons] icontheme /out/path/to/png
//gtkprev [custom] gtkthemename cursortheme icontheme /path/to/border /out/path/to/png
//gtkprev [fontpicker] "fontname"


