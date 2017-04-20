using SDPCheckers.GameClasses;
using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media.Imaging;

namespace SDPCheckers.Pages
{
    /// <summary>
    /// Interaction logic for GameBoard.xaml
    /// </summary>
    public partial class GameBoard : UserControl
    {
        //Will initialize state of all game board tiles once game is created
        private Game Game = null;

        //Keep track of the piece the user has selected to know which tiles should be highlighted and what moves can be done.
        private GameTile currentSelectedPiece = null;

        //Used to reference the grid columns/rows that need to be highlighted when a piece is clicked
        private Grid[,] boardTileGrids = new Grid[4, 8];

        //Remember the tiles that were highlighted, so we know which to unhighlight when a move is made without having to loop through all the tiles
        private List<int[]> highlightedPossibleMoves = new List<int[]>();

        //Constants to set the opacity of a tile.  The opacity of a tile is how a tile is 'highlighted', but it is also used
        //to determine whether that specific tile is playable or not.
        private const double PLAYABLE = 0.5;
        private const double NOTPLAYABLE = 1;

        public GameBoard(GamePiece.Player player)
        {
            Game = new Game(player);
    
            InitializeComponent();
            initializeBoardTileImageSources();
            initializeTileIndexReferences();
            drawGameBoardPieces();

            //Rotate the board UI if you're player 2
            boardAngle.Angle = player == GamePiece.Player.PLAYER1 ? 0 : 180;

        }

        /// <summary>
        /// Sets the boardTileGrids array with the respective grid tile for each index.  Also attaches MouseEnter, MouseLeave, and
        /// MouseLeftButtonDown event handlers for each playable grid tile
        /// </summary>
        private void initializeTileIndexReferences()
        {
            boardTileGrids[1, 0] = G10;
            boardTileGrids[3, 0] = G30;
            boardTileGrids[0, 1] = G01;
            boardTileGrids[2, 1] = G21;
            boardTileGrids[1, 2] = G12;
            boardTileGrids[3, 2] = G32;
            boardTileGrids[0, 3] = G03;
            boardTileGrids[2, 3] = G23;
            boardTileGrids[1, 4] = G14;
            boardTileGrids[3, 4] = G34;
            boardTileGrids[0, 5] = G05;
            boardTileGrids[2, 5] = G25;
            boardTileGrids[1, 6] = G16;
            boardTileGrids[3, 6] = G36;
            boardTileGrids[0, 7] = G07;
            boardTileGrids[2, 7] = G27;

            for(int x = 0; x < Game.boardWidth; ++x)
            {
                for(int y = 0; y < Game.boardHeight; ++y)
                {
                    //If the tile is not a playable tile, go to the next tile
                    if(boardTileGrids[x, y] == null) continue;

                    //Attach event handlers to each playable grid tile
                    boardTileGrids[x, y].MouseEnter += boardTileMouseEnter;
                    boardTileGrids[x, y].MouseLeave += boardTileMouseExit;
                    boardTileGrids[x, y].MouseLeftButtonDown += boardTileMouseClick;
                }
            }
        }

        /// <summary>
        /// This event is fired after a player selects a piece and clicks on the tile they want to move to.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void boardTileMouseClick(object sender, EventArgs e)
        {
            Grid tileToMoveTo = (sender as Grid);
            if(tileToMoveTo.Opacity == PLAYABLE && currentSelectedPiece != null)
            {
                //Each grid tile will only ever have one child, which is the image of the piece, so indexing the  0th child of the grid
                //will get the appropriate tile
                string[] positionToMoveTo = (tileToMoveTo.Children[0] as Image).Uid.Split(',');
                
                //Move the piece from the current position to the selected position
                movePiece(Game.boardTiles[currentSelectedPiece.position[0], currentSelectedPiece.position[1]],
                    Game.boardTiles[Convert.ToInt32(positionToMoveTo[0]), Convert.ToInt32(positionToMoveTo[1])]);
        

                //Update player turn
                Game.gamePlayer = Game.gamePlayer == GamePiece.Player.PLAYER1 ? GamePiece.Player.PLAYER2 : GamePiece.Player.PLAYER1;

                /*
                    Update DB of move that was done here
                */
            }
        }

        /// <summary>
        /// Handles the UI updating associated with moving a piece.  That is, if you're doing a regular move, it draws the piece in the new
        /// position.  If you're eating a piece, it removes the piece that was killed from the board and updates the position of the piece
        /// that ate it.
        /// </summary>
        private void movePiece(GameTile sourceTile, GameTile destinationTile)
        {
            //If you moved more than one tile horizontally, that means you ate a piece, so we have to remove that eaten piece from the board
            if(Math.Abs(destinationTile.position[0] - sourceTile.position[0]) > 1)
            {
                int xPosOfEnemyPiece = (destinationTile.position[0] + sourceTile.position[0]) / 2;
                
                int yPosOfEnemyPiece = (destinationTile.position[1] + sourceTile.position[1]) / 2;

                //Kill enemy's piece and redraw that tile
                GameTile enemyPieceTile = Game.boardTiles[xPosOfEnemyPiece, yPosOfEnemyPiece];
                enemyPieceTile.tilePiece = null;
                enemyPieceTile.drawTileImage();
            }
            //Update position of piece that moved
            destinationTile.tilePiece = sourceTile.tilePiece;
            sourceTile.tilePiece = null;

            //Update images of piece that moved
            sourceTile.drawTileImage();
            destinationTile.drawTileImage();

            //Unhighlight the possible moves that had been highlighted
            foreach (int[] position in highlightedPossibleMoves)
            {
                boardTileGrids[position[0], position[1]].Opacity = NOTPLAYABLE;
            }
            highlightedPossibleMoves.Clear();

            //Update cursor hand to arrow
            (App.Current.MainWindow).Cursor = Cursors.Arrow;

        }

        /// <summary>
        /// If the sender was a playable tile, the cursor is updated to a hand.
        /// </summary>
        /// <param name="sender">  A Grid tile on the game board</param>
        /// <param name="e"></param>
        private void boardTileMouseEnter(object sender, EventArgs e)
        {
            if((sender as Grid).Opacity == PLAYABLE)
            {
                (App.Current.MainWindow).Cursor = Cursors.Hand;
            }
        }

        /// <summary>
        /// IF the sender was a playable tile, the cursor is updated back to an arrow.
        /// </summary>
        /// <param name="sender"> A Grid tile on the game board</param>
        /// <param name="e"></param>
        private void boardTileMouseExit(object sender, EventArgs e)
        {
            if ((sender as Grid).Opacity == PLAYABLE)
            {
                (App.Current.MainWindow).Cursor = Cursors.Arrow;
            }
        }


        /// <summary>
        /// Initializes the image objects that should be referenced by each Game.boardTile to be matched with its respective
        /// GameBoard grid tile.
        /// </summary>
        private void initializeBoardTileImageSources()
        {
            //Only initializing the tiles that are playable on checkers
            Game.boardTiles[1, 0].tileImage = T10;
            Game.boardTiles[3, 0].tileImage = T30;

            Game.boardTiles[0, 1].tileImage = T01;
            Game.boardTiles[2, 1].tileImage = T21;

            Game.boardTiles[1, 2].tileImage = T12;
            Game.boardTiles[3, 2].tileImage = T32;

            Game.boardTiles[0, 3].tileImage = T03;
            Game.boardTiles[2, 3].tileImage = T23;

            Game.boardTiles[1, 4].tileImage = T14;
            Game.boardTiles[3, 4].tileImage = T34;

            Game.boardTiles[0, 5].tileImage = T05;
            Game.boardTiles[2, 5].tileImage = T25;

            Game.boardTiles[1, 6].tileImage = T16;
            Game.boardTiles[3, 6].tileImage = T36;

            Game.boardTiles[0, 7].tileImage = T07;
            Game.boardTiles[2, 7].tileImage = T27;
        }

        /// <summary>
        /// Attaches event handlers to each game board tile and draws them onto the gameboard.s
        /// </summary>
        private void drawGameBoardPieces()
        {
            for(int x = 0; x < Game.boardWidth; ++x)
            {
                for(int y = 0; y< Game.boardHeight; ++y)
                {
                    initializePieceEvents(Game.boardTiles[x, y]);
                    Game.boardTiles[x, y].drawTileImage();
                }
            }
        }

        /// <summary>
        /// Attaches a MouseEnter, MouseLeave, and MouseLeftButtonDown event handler for the GameTile image (the piece).
        /// </summary>
        /// <param name="tile"></param>
        public void initializePieceEvents(GameTile tile)
        {
            if (tile.tileImage == null) return;
            tile.tileImage.MouseEnter += initializeMouseEnterPieceEvent;
            tile.tileImage.MouseLeave += initializeMouseLeavePieceEvent;
            tile.tileImage.MouseLeftButtonDown += initializeMouseLeftClickPieceEvent;
        }

        /// <summary>
        /// If the sender is your piece, change the cursor to a hand
        /// </summary>
        private void initializeMouseEnterPieceEvent(object sender, EventArgs e)
        {
            if (isMyPiece(sender))
            {
                (App.Current.MainWindow).Cursor = Cursors.Hand;
            }
        }

        //If the sender was your piece, change the cursor back to an arrow
        private void initializeMouseLeavePieceEvent(object sender, EventArgs e)
        {
            if (isMyPiece(sender))
            {
                (App.Current.MainWindow).Cursor = Cursors.Arrow;
            }
        }

        //If the sender was your piece, unhighlight the last selected piece and highlight the newly selected piece, then highlight the possible
        //moves that can be made by the newly selected piece.
        private void initializeMouseLeftClickPieceEvent(object sender, EventArgs e)
        {
            //Check if piece is your piece
            if (!isMyPiece(sender)) return;

            if (currentSelectedPiece != null) currentSelectedPiece.tileImage.Opacity = 1;

            string[] tilePosition = (sender as Image).Uid.Split(',');
            currentSelectedPiece = Game.boardTiles[Convert.ToInt32( tilePosition[0]),Convert.ToInt32( tilePosition[1])];
            currentSelectedPiece.tileImage.Opacity = 0.5;

            //Clear previously highlighted tiles
            foreach(int[] position in highlightedPossibleMoves)
            {
                boardTileGrids[position[0], position[1]].Opacity = NOTPLAYABLE;
            }
            highlightedPossibleMoves.Clear();
            
           highlightPossibleMoves(currentSelectedPiece.position[0], currentSelectedPiece.position[1]);
        }


        /// <summary>
        /// Returns true if the sender was your piece, false otherwise
        /// </summary>]
        private bool isMyPiece(object sender)
        {
            Image piece = sender as Image;
            string[] position = piece.Uid.Split(',');

            return Game.boardTiles[Convert.ToInt32(position[0]), Convert.ToInt32(position[1])].tilePiece.player == Game.gamePlayer;
            
        }

        /// <summary>
        /// Takes in the position of the piece that is selected.  Based on the player that the piece belongs to, it determines the
        /// possible moves that can be made by the specified piece.
        /// </summary>
        /// <param name="xPos">x position of piece selected</param>
        /// <param name="yPos">y position of piece selected</param>
        private void highlightPossibleMoves(int xPos, int yPos)
        {
            //Player 1's and Player 2's left and right possible diagonals are different
            switch (Game.gamePlayer)
            {
                case GamePiece.Player.PLAYER1:
                    //Player 1 left diagonal
                    if(xPos - 1 >= 0 && yPos + 1 < Game.boardHeight)
                    {
                        if(Game.boardTiles[xPos-1,yPos+1].tilePiece == null)
                        {
                            highlightedPossibleMoves.Add(new int[] { xPos - 1, yPos + 1 });
                        }
                        //Check if you can jump over a piece to your left
                        else if(Game.boardTiles[xPos-1,yPos+1].tilePiece.player != Game.gamePlayer)
                        {
                            if(xPos - 2 >= 0 && yPos + 2 < Game.boardHeight)
                            {
                                if(Game.boardTiles[xPos-2,yPos+2].tilePiece == null)
                                {
                                    highlightedPossibleMoves.Add(new int[] { xPos - 2, yPos + 2 });
                                }
                            }
                        }
                    }
                    //Player 1 right diagonal
                    if (xPos + 1 < Game.boardWidth && yPos + 1 < Game.boardHeight)
                    {
                        if (Game.boardTiles[xPos + 1, yPos + 1].tilePiece == null)
                        {
                            highlightedPossibleMoves.Add(new int[] { xPos + 1, yPos + 1 });
                        }
                        //Check if you can jump over a piece to your right
                        else if (Game.boardTiles[xPos + 1, yPos + 1].tilePiece.player != Game.gamePlayer)
                        {
                            if (xPos + 2 < Game.boardWidth && yPos + 2 < Game.boardHeight)
                            {
                                if (Game.boardTiles[xPos + 2, yPos + 2].tilePiece == null)
                                {
                                    highlightedPossibleMoves.Add(new int[] { xPos + 2, yPos + 2 });
                                }
                            }
                        }
                    }
                    break;
                case GamePiece.Player.PLAYER2:
                    //Player 2's left diagonal
                    if (xPos + 1 < Game.boardWidth && yPos - 1 >= 0)
                    {
                        if (Game.boardTiles[xPos + 1, yPos - 1].tilePiece == null)
                        {
                            highlightedPossibleMoves.Add(new int[] { xPos + 1, yPos - 1 });
                        }
                        //Check if you can jump over a piece to your left
                        else if (Game.boardTiles[xPos + 1, yPos - 1].tilePiece.player != Game.gamePlayer)
                        {
                            if (xPos + 2 < Game.boardWidth && yPos - 2 >= 0)
                            {
                                if (Game.boardTiles[xPos + 2, yPos - 2].tilePiece == null)
                                {
                                    highlightedPossibleMoves.Add(new int[] { xPos + 2, yPos - 2 });
                                }
                            }
                        }
                    }
                    //Player 2's right diagonal
                    if (xPos - 1 >= 0 && yPos - 1 >= 0)
                    {
                        if (Game.boardTiles[xPos - 1, yPos - 1].tilePiece == null)
                        {
                            highlightedPossibleMoves.Add(new int[] { xPos - 1, yPos - 1 });
                        }
                        //Check if you can jump over a piece to your right
                        else if (Game.boardTiles[xPos - 1, yPos - 1].tilePiece.player != Game.gamePlayer)
                        {
                            if (xPos - 2 >= 0 && yPos - 2 >= 0)
                            {
                                if (Game.boardTiles[xPos - 2, yPos - 2].tilePiece == null)
                                {
                                    highlightedPossibleMoves.Add(new int[] { xPos - 2, yPos - 2 });
                                }
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
            
            foreach(int[] position in highlightedPossibleMoves)
            {
                boardTileGrids[position[0], position[1]].Opacity = PLAYABLE;
            }
        }

        /// <summary>
        /// Unhighlights the selected piece and possible moves
        /// </summary>
        private void Grid_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (currentSelectedPiece == null) return;
            currentSelectedPiece.tileImage.Opacity = 1;
            currentSelectedPiece = null;

            foreach(int[] position in highlightedPossibleMoves)
            {
                boardTileGrids[position[0], position[1]].Opacity = NOTPLAYABLE;
            }
            highlightedPossibleMoves.Clear();
        }
    }
}
