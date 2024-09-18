/*
 * =====================================================================================
 *
 *       Filename:  interface.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/01/24 14:23:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "interface.hpp"
#include "global.hpp"
#include <string>

// === Global Variables ===
const char* tileDisplayChars[] = {
  " ",  // 0
  "1",
  "2",
  "3",
  "4",
  "5",
  "6",
  "7",
  "8",
  "!",  // Flagged
  " ",  // Unrevealed tile
  "X"   // Revealed mine
};

GtkBuilder* builder;
GtkWidget* window;  // Initialized in on_activate().
std::vector<UITileButton*>* tileButtons;  // Initialized in on_activate().
Board* board = nullptr;
int mode = REVEAL_MODE;

// Whether tiles can be revealed or flagged.
// This is set to true when receiving a game over or winning, and set to false when starting a new game.
bool preventTileRevealing = false;

std::string exeRootDir = "";

GtkWidget* grid;
// ==========================

// TODO: Implement this system more elegantly
const char* GetDisplayCharacterForTile(Tile* t)
{
  if (t->isFlagged)
    return tileDisplayChars[9];

  if (!t->isRevealed)
    return tileDisplayChars[10];

  if (!t->isMine)
    return tileDisplayChars[t->CountAdjacentMines()];
  
  return tileDisplayChars[11];
}

void interface_init(Board* gameBoard)
{
  board = gameBoard;
}

UITileButton::UITileButton(GtkWidget* button, Tile* tile)
{
  this->button = button;
  this->associatedTile = tile;
}

void updateAllTileButtons(std::vector<UITileButton*>* buttons)
{
  for (UITileButton* b : *buttons)
  {
    gtk_button_set_label(GTK_BUTTON(b->button), GetDisplayCharacterForTile(b->associatedTile));
    if (b->associatedTile->isRevealed)
    {
      gtk_widget_set_sensitive(GTK_WIDGET(b->button), false);
      gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(b->button)), "empty_tile");
    }
  }
}

void setModeToReveal(GtkButton* button, gpointer user_data)
{
  mode = REVEAL_MODE;
}

void setModeToFlag(GtkButton* button, gpointer user_data)
{
  mode = FLAG_MODE;
}

static void dismissButtonClicked(GtkButton* btn, gpointer messageDialog)
{
  gtk_widget_set_visible(GTK_WIDGET(messageDialog), false);
}

// Applicable to both win and lose screen buttons
static void restartButtonClicked(GtkButton* btn, gpointer messageDialog)
{ 
  gtk_widget_set_visible(GTK_WIDGET(messageDialog), false);

  // These values get clobbered by Board::Init() so we want to preserve them here
  int sizeX = board->sizeX;
  int sizeY = board->sizeY;
  int mineCount = board->mineCount;

  board->DestroyAllTiles();
  board->Init(sizeX, sizeY, mineCount);
  clear_buttons();
  create_buttons();
  preventTileRevealing = false;
}

static void gameOverGtk()
{
  GtkMessageDialog* dialog = GTK_MESSAGE_DIALOG(gtk_builder_get_object(builder, "youLoseWindow"));
  g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "replayButtonLoseWindow")), "clicked", G_CALLBACK(restartButtonClicked), dialog);
  g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "dismissButtonLoseWindow")), "clicked", G_CALLBACK(dismissButtonClicked), dialog);
  preventTileRevealing = true;
  gtk_widget_set_visible(GTK_WIDGET(dialog), true);
}

static void revealTileGtk(GtkButton* btn, gpointer userdata)
{
  if (mode != REVEAL_MODE || preventTileRevealing)
    return;

  Tile* t = (Tile*)userdata;
  t->TryRevealTile(false);
  updateAllTileButtons(tileButtons);

  if (t->isMine)
  {
    gameOverGtk();
  }
}


static void checkForWin()
{
  if (board->minesFlagged == board->mineCount && board->innocentTilesFlagged == 0)
  {
    GtkMessageDialog* dialog = GTK_MESSAGE_DIALOG(gtk_builder_get_object(builder, "youWinWindow"));
    g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "replayButtonWinWindow")), "clicked", G_CALLBACK(restartButtonClicked), dialog);
    g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "dismissButtonWinWindow")), "clicked", G_CALLBACK(dismissButtonClicked), dialog);
    gtk_widget_set_visible(GTK_WIDGET(dialog), true);
    preventTileRevealing = true;
  }
}


void tileClicked(GtkButton* btn, gpointer userdata)
{
  if (preventTileRevealing)
    return;

  if (mode == REVEAL_MODE)
  {
    revealTileGtk(btn, userdata);
  }
  else if (mode == FLAG_MODE)
  {
    Tile* t = (Tile*)userdata;
    if (t->isRevealed)
      return;

    t->ToggleFlag();
    updateAllTileButtons(tileButtons);
    checkForWin();
  }
}

void create_buttons()
{
  for (Tile* t : board->tiles)
  {
    Point pos = t->GetPosition();
    GtkWidget* button = gtk_button_new_with_label(GetDisplayCharacterForTile(t));
    gtk_widget_set_visible(button, true);
    g_signal_connect(button, "clicked", G_CALLBACK(tileClicked), t);
    gtk_grid_attach(GTK_GRID(grid), button, pos.x, pos.y, 1, 1);

    UITileButton* newButton = new UITileButton(button, t);
    tileButtons->push_back(newButton);
  }
}

void clear_buttons()
{
  for (UITileButton* b : *tileButtons)
  {
    gtk_widget_destroy(b->button);
    delete b;
  }
  tileButtons->clear();
}

static void on_new_game_clicked(GtkButton* button, gpointer* userdata)
{
  GtkPopoverMenu* popover = GTK_POPOVER_MENU(gtk_builder_get_object(builder, "newGamePopover"));
  gtk_popover_popup(GTK_POPOVER(popover));
}

static void on_about_button_clicked(GtkButton* button, gpointer* userdata)
{
  const char* authors[] = {
    "EscapeNumber001\nhttps://github.com/EscapeNumber001",
    NULL
  };

  const char* artists[] = {
    "Nicubunu @ Openclipart\nhttps://openclipart.org/detail/22065/broom",
    NULL
  };

  // TODO: Logo on about screen
  std::string path = exeRootDir + "/assets/openclipart_broom.png";
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(path.c_str(), NULL);
    if (!pixbuf) {
        g_printerr("Failed to load image\n");
	return;
    }
    pixbuf = gdk_pixbuf_scale_simple(pixbuf, 128, 128, GDK_INTERP_BILINEAR); 

  gtk_show_about_dialog(NULL, "program-name", "Minesweeper",
	"title", "Minesweeper",
	"comments", "A simple Minesweeper clone written in C++.",
	"authors", authors,
	"artists", artists,
	"logo", pixbuf,
	"version", VERSION,
	NULL
      );
}

static void new_game_confirm_button_clicked(GtkButton* button, gpointer* userdata)
{
  clear_buttons();
  GtkWidget* xSizeInp = GTK_WIDGET(gtk_builder_get_object(builder, "boardSizeXInput"));
  GtkWidget* ySizeInp = GTK_WIDGET(gtk_builder_get_object(builder, "boardSizeYInput"));
  GtkWidget* minesInp = GTK_WIDGET(gtk_builder_get_object(builder, "mineCountInput"));
  int sizeX = gtk_spin_button_get_value(GTK_SPIN_BUTTON(xSizeInp));
  int sizeY = gtk_spin_button_get_value(GTK_SPIN_BUTTON(ySizeInp));
  int mineCount = gtk_spin_button_get_value(GTK_SPIN_BUTTON(minesInp));

  clear_buttons();
  board->DestroyAllTiles();
  board->Init(sizeX, sizeY, mineCount);
  create_buttons();
  preventTileRevealing = false;
}

static void on_activate(GtkApplication* app) {
  builder = gtk_builder_new();
  GError *error = NULL;
  std::string interfaceFilepath = exeRootDir + "/assets/interface.ui";
  gtk_builder_add_from_file(builder, interfaceFilepath.c_str(), &error);  // FIXME: This local filepath won't work once the program is moved to any other directory!
  if (error) {
     g_printerr("Error loading UI file: %s\n", error->message);
     g_clear_error(&error);
      return;
  }

  GtkCssProvider* css = gtk_css_provider_new();
  std::string cssFilepath = exeRootDir + "/assets/interface_style.css";
  gtk_css_provider_load_from_path(css, cssFilepath.c_str(), &error);
  if (error) {
     g_printerr("Error loading CSS file: %s\n", error->message);
     g_clear_error(&error);
      return;
  }
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_USER);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  gtk_window_set_application(GTK_WINDOW(window), app);
  grid = GTK_WIDGET(gtk_builder_get_object(builder, "buttonsGrid"));
  g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "flagModeButton")), "clicked", G_CALLBACK(setModeToFlag), NULL);  // TODO: Connect these from Glade directly because it's a bit hacky doing it this way
  g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "revealModeButton")), "clicked", G_CALLBACK(setModeToReveal), NULL);

  g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "newGameButton")), "clicked", G_CALLBACK(on_new_game_clicked), NULL);
  g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "confirmNewGameSettingsButton")), "clicked", G_CALLBACK(new_game_confirm_button_clicked), NULL);
  g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "aboutButton")), "clicked", G_CALLBACK(on_about_button_clicked), NULL);

  tileButtons = new std::vector<UITileButton*>;
  gtk_window_present(GTK_WINDOW(window));
}

int interface_run_game(int argc, char* argv[])
{
  GtkApplication* app = gtk_application_new("com.EscapeNumber001.Minesweeper", \
      G_APPLICATION_DEFAULT_FLAGS);

  std::string s = argv[0];
  std::string delimiter = "/";
  exeRootDir = s.substr(0, s.find_last_of(delimiter));

  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
  return g_application_run(G_APPLICATION(app), argc, argv);
}
