/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
 * Globals.cpp
 */
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>

#include "globals.h"

//current stuff
int			currentWallStyle;
char*			currentIconTheme;
char*			currentWmTheme;
char*			currentCursorTheme;
char*			currentWallPaper;
char*			currentGtkTheme;
char*			currentButtonLayout;
char*			currentTitlePos;
char*			currentWMFont;
char*			currentAppFont;
int			currentBright;
double		currentSatu;
int			currentCursSize;
char*			currentMetaTheme;

char*			lastGtkTheme;
char*			lastIconTheme;
char*			lastWmTheme;
char*			lastCursorTheme;
char*			lastWallPaper;
char*			lastMetaTheme;
char*		homeThemesHash;

boxStruct		previewBox[6]={{NULL,NULL,NULL,0,NULL,NULL}};

//db
char*			dbFolder;
char*			metaFolder;
char*			framesFolder;
char*			controlsFolder;
char*			iconsFolder;
char*			cursorsFolder;
char*			wallpapersFolder;
char*			customFolder;

//folders
char*			themesArray[2];
char*			iconsArray[2];
char*			papersArray[2];
char*			homeFolder;

char*			cursorTheme;
char*			iconTheme;

//widgets etc
GtkWidget*		appFontButton;
GtkWidget*		wmFontButton;
GtkWidget*		window;
GtkComboBoxText*	styleComboBox;
GtkComboBoxText*	previewComboBox;
GdkWindow*		gdkWindow;
GdkCursor*		watchCursor;

int 			wallStyle;
GtkWidget*		layoutEntry;
GtkComboBoxText*	titlePos;
GtkWidget*		briteRange;
GtkWidget*		satuRange;
GtkWidget*		cursorSize;

GtkWidget*		wallpapersMainBox=NULL;

//advanced
GtkNotebook*	advanced;
GtkWidget*		systemCheck;
GtkWidget*		metaCheck;
GtkWidget*		borderCheck;
GtkWidget*		gtkCheck;
GtkWidget*		iconsCheck;
GtkWidget*		cursorsCheck;
GtkWidget*		paperCheck;
GtkWidget*		onlyCustomCheck;

//panels
GtkWidget*		panelSelect;
GtkWidget*		panelStyleWidget;
GtkWidget*		panelSizeWidget;

GtkWidget*		panelImageBox;
GtkWidget*		panelColourBox;
GtkWidget*		panelAlphaBox;

GtkWidget*		panelAlphaWidget;
GtkWidget*		panelColourWidget;
GtkWidget*		panelImagePathWidget;

int				numOfPanels=-1;
panelData*		panels[10];
int				currentPanel=1;

int			controlHeight=64;
int			controlWidth=256;
GdkPixbuf*		controlsPixbuf;

GtkNotebook*	notebook;

char			generalBuffer[4096];

GdkPixbuf*		missingImage;
GdkPixbuf*		blankImage;
int			langID;

//config stuff
int			showGlobal=1;
int			showMeta=1;
int			showGtk=1;
int			showDecs=1;
int			showIcons=1;
int			showCursors=1;
int			showBackdrop=1;
int			showOnlyCustom=0;
int			winWid=420;
int			winHite=470;
int			gotXCE=0;
int			previewSize=128;

bool			noGui=false;
bool			updateDb=false;
bool			rebuildDb=false;

char*			cliTheme=NULL;
char*			cliControls=NULL;
char*			cliBorder=NULL;
char*			cliIcons=NULL;
char*			cliCursors=NULL;
char*			cliWallpaper=NULL;
char*			listParts=NULL;
char*			cliFileName=NULL;

const char* folders[]={"meta","frames","controls","icons","cursors","wallpapers"};

const char* langStr[][STOPWORD]={
{"Themes","Window Borders","Controls","Icons","Cursors","Wallpapers","Re-Build The Database","Rebuild DB","Launch Xfce-Composite-Editor","Backdrop Adjustments","Brightness","Saturation","Reset","Button Layout","Font Selection","WM Font","App Font","Cursor Size","Reset Theme","Save Theme","Advanced","Title Position","Enter Name for Theme...","Left","Centre","Right","Auto","Centered","Tiled","Stretched","Scaled","Zoomed","Custom","View Options","Show System Themes","Show Only Custom Themes","Delete Custom Theme","Integrated theme manager","An intergrated theme manager for Xfce 4.10","Huge Previews","Large Previews","Medium Previews","Small Previews","Panels","Panel Size:","None","Solid Colour","Image","Style:\t","File:\t\t","Alpha:\t","Colour:\t"},

{"Temas","Bordes De Ventana","Controles","Iconos","Cursores","Fondos De Pantalla","Re-Crear La Base De Datos","Reconstruir DB","Lanzar Xfce-Composite-Editor","Ajustes De Fondo De Escritorio","Brillo","Saturación","Reiniciar","Orden De Los Botones","Selección De Fuentes","Fuente De WM","Fuentes De Aplicación","Tamaño Del Cursor","Restablecer Tema","Guardar Tema","Avanzado","Posición Del Título","Ingrese el nombre del Tema...","Izquierda","Centro","Derecho","Auto","Centrado","Mosaico","Estirado","Escalado","Zoomed","Costumbre","Ver Opciones","Mostrar Temas del Sistema","Mostrar Sólo Temas Personalizados","Borrar Tema Personalizado","Un gestor integrado de temas","Un gestor de temas integrada para Xfce 4.10","Vista previa enorme","Vista previa grande","Vista previa mediana","Vista previa pequeña","Panels","Panel Size:","None","Solid Colour","Image","Style:\t","File:\t\t","Alpha:\t","Colour:\t"},

{"Oberfläche","Fenster","Schaltflächen","Symbole","Mauszeiger","Hintergründe","Datenbank neu anlegen","DB erneuern","Xfce-Composite-Editor starten","Hintergrund-Anpassung","Helligkeit","Sättigung","Zurücksetzen","Anordnung der Schaltflächen","Schrift","Fensterschrift","Anwendungsschrift","Größe des Mauszeigers","Oberfläche Zurücksetzen","Speichern Oberfläche","Erweitert","Position des Fenstertitels","Name für Oberfläche eingeben...","Links","Mitte","Rechts","Automatisch","Zentriert","Gekachelt","Gestreckt","Skaliert","Vergrößert","Benutzerdefiniert","Anzeige-Optionen","System-Oberflächen Anzeigen","Nur benutzerdefinierte Oberflächen anzeigen","Benutzerdefinierte Oberfläche löschen","Eine integrierte Oberflächen-Verwaltung","Eine integrierte Oberflächen-Verwaltung für Xfce 4.10","Riesige Vorschaubilder","Große Vorschaubilder","Mittlere Vorschaubilder","Kleine Vorschaubilder","Panels","Panel Size:","None","Solid Colour","Image","Style:\t","File:\t\t","Alpha:\t","Colour:\t"}
};

void freeAndNull(char** ptr)
{
	if (*ptr!=NULL)
		{
			free(*ptr);
			*ptr=NULL;
		}
}

int positionToInt(char* pos)
{
	gchar	*stdout=NULL;
	gchar	*stderr=NULL;
	gint   retval=0;

	if (pos!=NULL)
		{
			if(g_ascii_strcasecmp(pos,"left")==0)
				return(0);

			if(g_ascii_strcasecmp(pos,"center")==0)
				return(1);

			if(g_ascii_strcasecmp(pos,"right")==0)
				return(2);
		}
	else
		{
			asprintf(&currentTitlePos,"%s","center");
			sprintf(generalBuffer,"%s\"%s\"",XCONFSETTITLEPOS,currentTitlePos);
			g_spawn_command_line_sync(generalBuffer,&stdout,&stderr,&retval,NULL);
		}
	return(1);
}

char* doubleToStr(double num)
{
	char*		doublestr=NULL;
	char*		comma=NULL;

	asprintf(&doublestr,"%f",num);
	comma=strchr(doublestr,',');
	if(comma!=NULL)
		*comma='.';	

	return(doublestr);
}

void setValue(const char* command,dataType type,void* ptr)
{
	gchar	*stdout=NULL;
	gchar	*stderr=NULL;
	gint   retval=0;

	g_spawn_command_line_sync(command,&stdout,&stderr,&retval,NULL);
	if (retval==0)
		{
			switch(type)
				{
					case INT:
						stdout[strlen(stdout)-1]=0;
						*(int*)ptr=atoi(stdout);
						break;

					case STRING:
						stdout[strlen(stdout)-1]=0;
						asprintf((char**)ptr,"%s",stdout);
						break;

					case FLOAT:
						stdout[strlen(stdout)-1]=0;
						*(double*)ptr=atof(stdout);
						break;
				}
		}
	freeAndNull(&stdout);
	freeAndNull(&stderr);
}

int sizeDrop(bool toDrop,int data)
{
	if (toDrop==TRUE)
		{
			switch(data)
				{
					case PREHUGE:
						return(0);
						break;

					case 	PRELARGE:
						return(1);
						break;

					case PREMEDIUM:
						return(2);
						break;

					case PRESMALL:
						return(3);
						break;
				}
		}
	else
		{
			switch(data)
				{
					case 0:
						return(PREHUGE);
						break;

					case 1:
						return(PRELARGE);
						break;

					case 2:
						return(PREMEDIUM);
						break;

					case 3:
						return(PRESMALL);
						break;
				}
		}
	return(-1);
}

