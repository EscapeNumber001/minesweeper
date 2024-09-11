#include "game.hpp"

Point::Point(int x, int y)
{
  this->x = x;
  this->y = y;
}

Point::Point()
{
  Point(0, 0);
}


Tile::Tile(int x, int y, Board* board)
{
  isMine = false;
  isRevealed = false;
  isFlagged = false;
  surroundingMineCount = -1;  // This value is set by TryRevealTile().
  position = (Point){x, y};
  this->board = board;
}


Point Tile::GetPosition()
{
  return (Point){position.x, position.y};
}

Tile* Board::GetTileAtPoint(Point p)
{
  for (Tile* t : tiles)
  {
    if (t->GetPosition().x == p.x && t->GetPosition().y == p.y)
    {
      return t;
    }
  }
  return nullptr;
}

void Board::Init(int sizeX, int sizeY, int mineCount)
{
  // Create tiles
  for (int y = 0; y < sizeY; y++)
  {
    for (int x = 0; x < sizeX; x++)
    {
      Tile* t = new Tile(x, y, this);
      tiles.push_back(t);
    }
  }

  int square_size = sizeX * sizeY;
  for (int i = 0; i < mineCount; i++)
  {
    int index = rand() % square_size;
    tiles[index]->isMine = true;
  }

  turnCount = 0;
  this->mineCount = mineCount;
  minesFlagged = 0;
}

void Board::DestroyAllTiles()
{
  for (Tile* t : tiles)
  {
    delete t;
  }
  tiles.clear();
  mineCount = 0;
  minesFlagged = 0;
}

void Tile::TryRevealTile(bool isBeingCalledRecursively)
{
  if (isRevealed)
    return;

  if (!isMine)
    surroundingMineCount = CountAdjacentMines();

  isRevealed = true;
  if (!isBeingCalledRecursively)  // Prevents the turn count from incrementing with every automatic reveal
    board->turnCount++;

  if (surroundingMineCount == 0)
    RevealAdjacentZeroTiles();
}

void Tile::ToggleFlag()
{
  isFlagged = !isFlagged;
  if (isMine)
    if (isFlagged)
      board->minesFlagged++;
    else
      board->minesFlagged--;
}

void Tile::RevealAdjacentZeroTiles()
{
  Tile* adjacentTiles[4] = {
    board->GetTileAtPoint((Point){position.x + 1, position.y}),
    board->GetTileAtPoint((Point){position.x - 1, position.y}),
    board->GetTileAtPoint((Point){position.x, position.y + 1}),
    board->GetTileAtPoint((Point){position.x, position.y - 1}),
  };

  for (Tile* t : adjacentTiles)
  {
    if (t == nullptr || t->isMine || t->isRevealed)
      continue;

    t->TryRevealTile(true);
  }
}

// TODO: this needs re-indented
int Tile::CountAdjacentMines()
  {
    int minesCounted = 0;

    for (Tile* tile : board->tiles)
    {
      if (tile == this)
	continue;

      // Ensure the tile is adjacent
      if (tile->position.x > position.x + 1 || tile->position.x < position.x - 1
	  || tile->position.y > position.y + 1 || tile->position.y < position.y - 1
	  )
	continue;

      if (tile->isMine)
	minesCounted++;
    }

    return minesCounted;
  }

