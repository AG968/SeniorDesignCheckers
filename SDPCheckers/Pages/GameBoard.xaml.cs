using SDPCheckers.GameClasses;
using System;
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
        private Game Game = new Game();
        private Image currentSelectedPiece = null;
        private Grid[,] boardTileGrids = new Grid[4, 8];

        public GameBoard()
        {
            InitializeComponent();
            initializeBoardTileImageSources();
            initializeTileIndexReferences();
            drawGameBoardPieces();
            
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
            (App.Current.MainWindow).Cursor = Cursors.Hand;
        }

        private void initializeMouseLeavePieceEvent(object sender, EventArgs e)
        {
            (App.Current.MainWindow).Cursor = Cursors.Arrow;
        }

        private void initializeMouseLeftClickPieceEvent(object sender, EventArgs e)
        {
            if (currentSelectedPiece != null) currentSelectedPiece.Opacity = 1; ;
            currentSelectedPiece = sender as Image;
            currentSelectedPiece.Opacity = 0.5;

            string[] tilePosition = currentSelectedPiece.Uid.Split(',');
           highlightPossibleMoves(Convert.ToInt32(tilePosition[0]), Convert.ToInt32(tilePosition[1]));
           var x = ((((sender as Image).Parent as Grid).Parent) as Border);
        }

        private void highlightPossibleMoves(int xPos, int yPos)
        {
            boardTileGrids[3, 4].Opacity = 0.5;
        }

        private void Grid_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (currentSelectedPiece == null) return;
            currentSelectedPiece.Opacity = 1;
            currentSelectedPiece = null;
        }
    }
}
