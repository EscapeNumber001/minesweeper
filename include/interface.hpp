/*
 * =====================================================================================
 *
 *       Filename:  interface.hpp
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  09/01/24 14:20:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _INTERFACE_HPP_
#define _INTERFACE_HPP_

#include <gtk/gtk.h>
#include "game.hpp"

#define REVEAL_MODE 0
#define FLAG_MODE 1

struct UITileButton
{
public:
  GtkWidget* button;
  Tile* associatedTile;
public:
  UITileButton(GtkWidget* button, Tile* tile);
};

extern const char* tileDisplayChars[];
extern GtkWidget* window;
extern std::vector<UITileButton*>* tileButtons;
extern Board* board;
extern int mode;

void interface_init(Board* board);
int interface_run_game(int argc, char* argv[]);

// Private functions
void clear_buttons();
void create_buttons();
void tileClicked(GtkButton* btn, gpointer userdata);
#endif
