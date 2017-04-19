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
        private Game Game = null;
        private Image currentSelectedPiece = null;
        private Grid[,] boardTileGrids = new Grid[4, 8];
        private List<int[]> highlightedPossibleMoves = new List<int[]>();

        //Constants to set the opacity of a tile 
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
           

        }

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

        public void initializePieceEvents(GameTile tile)
        {
            if (tile.tileImage == null) return;
            tile.tileImage.MouseEnter += initializeMouseEnterPieceEvent;
            tile.tileImage.MouseLeave += initializeMouseLeavePieceEvent;
            tile.tileImage.MouseLeftButtonDown += initializeMouseLeftClickPieceEvent;
        }

        private void initializeMouseEnterPieceEvent(object sender, EventArgs e)
        {
            if (isMyPiece(sender))
            {
                (App.Current.MainWindow).Cursor = Cursors.Hand;
            }
        }

        private void initializeMouseLeavePieceEvent(object sender, EventArgs e)
        {
            if (isMyPiece(sender))
            {
                (App.Current.MainWindow).Cursor = Cursors.Arrow;
            }
        }

        private void initializeMouseLeftClickPieceEvent(object sender, EventArgs e)
        {
            //Check if piece is your piece
            if (!isMyPiece(sender)) return;

            if (currentSelectedPiece != null) currentSelectedPiece.Opacity = 1;

            currentSelectedPiece = sender as Image;
            currentSelectedPiece.Opacity = 0.5;

            //Clear previously highlighted tiles
            foreach(int[] position in highlightedPossibleMoves)
            {
                boardTileGrids[position[0], position[1]].Opacity = NOTPLAYABLE;
            }
            highlightedPossibleMoves.Clear();

            string[] tilePosition = currentSelectedPiece.Uid.Split(',');
           highlightPossibleMoves(Convert.ToInt32(tilePosition[0]), Convert.ToInt32(tilePosition[1]));
           var x = ((((sender as Image).Parent as Grid).Parent) as Border);
        }

        private bool isMyPiece(object sender)
        {
            Image piece = sender as Image;
            string[] position = piece.Uid.Split(',');

            return Game.boardTiles[Convert.ToInt32(position[0]), Convert.ToInt32(position[1])].tilePiece.player == Game.gamePlayer;
            
        }

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
                        //Check if you can jump over a piece to your left
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
                    break;
                default:
                    break;
            }
            
            foreach(int[] position in highlightedPossibleMoves)
            {
                boardTileGrids[position[0], position[1]].Opacity = PLAYABLE;
            }
            //boardTileGrids[3, 4].Opacity = 0.5;
        }

        private void Grid_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (currentSelectedPiece == null) return;
            currentSelectedPiece.Opacity = 1;
            currentSelectedPiece = null;
        }
    }
}
