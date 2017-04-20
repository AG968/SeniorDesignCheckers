using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SDPCheckers.GameClasses
{
    class Game
    {
        public const int boardHeight = 8;
        public const int boardWidth = 4;

        //Player that this user is
        public GamePiece.Player gamePlayer;

        //4x8 board.  boardTiles are referenced as if referencing a cartesian plain, with the bottom left tile of player1
        //being the origin. EG bottom left of player 1 is boardTiles[0,0], whereas 'bottom left' of Player 2 would be 
        //boardTiles[3,7]
        public GameTile[,] boardTiles = new GameTile[4, 8];
        
        public Game(GamePiece.Player player)
        {
            gamePlayer = player;
            initializeBoard();
        }

        /// <summary>
        /// Initializes the states for all game board tiles 
        /// </summary>
        private void initializeBoard()
        {
        
            //Player 1 tiles
            boardTiles[1, 0] = new GameTile(new GamePiece(GamePiece.Player.PLAYER1), 1, 0);
            boardTiles[3, 0] = new GameTile(new GamePiece(GamePiece.Player.PLAYER1), 3, 0);
            boardTiles[0, 1] = new GameTile(new GamePiece(GamePiece.Player.PLAYER1), 0, 1); 
            boardTiles[2, 1] = new GameTile(new GamePiece(GamePiece.Player.PLAYER1), 2, 1);
            boardTiles[1, 2] = new GameTile(new GamePiece(GamePiece.Player.PLAYER1), 1, 2);
            boardTiles[3, 2] = new GameTile(new GamePiece(GamePiece.Player.PLAYER1), 3, 2);

            //Player 2 tiles
            boardTiles[0, 5] = new GameTile(new GamePiece(GamePiece.Player.PLAYER2), 0, 5);
            boardTiles[2, 5] = new GameTile(new GamePiece(GamePiece.Player.PLAYER2), 2, 5);
            boardTiles[1, 6] = new GameTile(new GamePiece(GamePiece.Player.PLAYER2), 1, 6);
            boardTiles[3, 6] = new GameTile(new GamePiece(GamePiece.Player.PLAYER2), 3, 6);
            boardTiles[0, 7] = new GameTile(new GamePiece(GamePiece.Player.PLAYER2), 0, 7);
            boardTiles[2, 7] = new GameTile(new GamePiece(GamePiece.Player.PLAYER2), 2, 7);

            //Empty Tiles
            for (int x = 0; x < boardWidth; ++x)
            {
                for (int y = 0; y < boardHeight; ++y)
                {
                    if(boardTiles[x,y] == null)
                    {
                        boardTiles[x, y] = new GameTile(null,x,y);
                    }
                }
            }
        }
    }
}
