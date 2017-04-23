using MahApps.Metro.Controls;
using MahApps.Metro.Controls.Dialogs;
using Newtonsoft.Json;
using SDPCheckers.GameClasses;
using SDPCheckers.Utilities;
using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using System.Windows.Threading;

namespace SDPCheckers.Pages
{
    /// <summary>
    /// Interaction logic for Game.xaml
    /// </summary>
    public partial class Game : UserControl
    {
        private const int boardHeight = 8;
        private const int boardWidth = 4;

        public int numOfPlayers = 1;
        private int gameID=1;
        public bool gameIsDone = false;
        public GamePiece.Player currentPlayerTurn = GamePiece.Player.PLAYER1;
        //Player that this user is
        public GamePiece.Player gamePlayer;

        //4x8 board.  boardTiles are referenced as if referencing a cartesian plain, with the bottom left tile of player1
        //being the origin. EG bottom left of player 1 is boardTiles[0,0], whereas 'bottom left' of Player 2 would be 
        //boardTiles[3,7]
        public GameTile[,] boardTiles = new GameTile[4, 8];

        private bool opponentsMoveWasUpdated = false;

        //Keep track of the piece the user has selected to know which tiles should be highlighted and what moves can be done.
        private GameTile currentSelectedPiece = null;

        //Used to reference the grid columns/rows that need to be highlighted when a piece is clicked
        private Grid[,] boardTileGrids = new Grid[4, 8];

        //Remember the tiles that were highlighted, so we know which to unhighlight when a move is made without having to loop through all the tiles
        private List<int[]> possibleMoves = new List<int[]>();

        //Constants to set the opacity of a tile.  The opacity of a tile is how a tile is 'highlighted', but it is also used
        //to determine whether that specific tile is playable or not.
        private const double PLAYABLE = 0.5;
        private const double NOTPLAYABLE = 1;


        private DispatcherTimer gameTimer = new DispatcherTimer();

        public Game(GamePiece.Player player, int gameID)
        {
            InitializeComponent();
            gamePlayer = player;
            if(gamePlayer == GamePiece.Player.PLAYER1)
            {
                waitingForPlayer(false);
            }
            this.gameID = gameID;
            
            //Initialize the board after the component and player have been initialized
            initializeBoard();

        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            initializeGameTimer();
        }

        private async void UserControl_Unloaded(object sender, RoutedEventArgs e)
        {
            destroyGameTimer();
            await DatabaseGameHelper.deleteGame(gameID);
        }

        private void initializeGameTimer()
        {
            gameTimer.Interval = new TimeSpan(0, 0, 5);
            gameTimer.Tick += updateGame;
            gameTimer.Start();
        }

        private void destroyGameTimer()
        {
            if (gameTimer == null) return;
            gameTimer.Stop();
            gameTimer.Tick -= updateGame;
            gameTimer = null;
        }

        public async void updateGame(object sender, EventArgs e)
        {
            if (!gameIsDone)
            {
                //If it's not your turn, check to see if the opponent has went, and update the board/game player if they have
                if (currentPlayerTurn != gamePlayer)
                {
                    string gameStatusString = await DatabaseGameHelper.checkGameStatus(gameID.ToString());
                    GameStatus gameStatus = JsonConvert.DeserializeObject<GameStatus>(gameStatusString);
                    if(gameStatus.status == 1)
                    {
                        //If it became your turn
                        GamePiece.Player statusPlayerTurn = gameStatus.currentPlayerTurn == 0 ? GamePiece.Player.PLAYER1 : GamePiece.Player.PLAYER2;
                        if(statusPlayerTurn == gamePlayer)
                        {
                            currentPlayerTurn = gamePlayer;
                            movePiece(boardTiles[gameStatus.sourceCol, gameStatus.sourceRow], boardTiles[gameStatus.destCol, gameStatus.destRow]);
                            waitingForPlayer(false);
                        }

                    }else
                    {
                        //Game connection has been lost
                        destroyGameTimer();
                        await (App.Current.MainWindow as MetroWindow).ShowMessageAsync("Your opponent has left the game", "It appears that your opponent has left the match.");
                        PageTransitionHelper.transitionToPage(new GameLobby());
                    }
                }
            }
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
                    if (boardTiles[x, y] == null)
                    {
                        boardTiles[x, y] = new GameTile(null, x, y);
                    }
                }
            }

            initializeBoardTileImageSources();
            initializeTileIndexReferences();
            drawGameBoardPieces();


            //Rotate the board UI if you're player 2
            boardAngle.Angle = gamePlayer == GamePiece.Player.PLAYER1 ? 0 : 180;
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

            for(int x = 0; x < boardWidth; ++x)
            {
                for(int y = 0; y < boardHeight; ++y)
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
        private async void boardTileMouseClick(object sender, EventArgs e)
        {
            if (currentPlayerTurn != gamePlayer) return;
            Grid tileToMoveTo = (sender as Grid);
            if(tileToMoveTo.Opacity == PLAYABLE && currentSelectedPiece != null)
            {
                //Each grid tile will only ever have one child, which is the image of the piece, so indexing the  0th child of the grid
                //will get the appropriate tile
                string[] positionToMoveTo = (tileToMoveTo.Children[0] as Image).Uid.Split(',');


                string gameStatusString = await DatabaseGameHelper.sendMove(gameID, numOfPlayers, currentPlayerTurn, currentSelectedPiece.position[0], currentSelectedPiece.position[1], Convert.ToInt32(positionToMoveTo[0]), Convert.ToInt32(positionToMoveTo[1]));
                GameStatus gameStatus = JsonConvert.DeserializeObject<GameStatus>(gameStatusString);
                if(gameStatus.status != 1)
                {
                    //Request for move to be made was not successful so don't do the move
                    await (App.Current.MainWindow as MetroWindow).ShowMessageAsync("Move could not be sent.", "Your move was not sent to the other player. Try again");
                    return;
                }

                //Move the piece from the current position to the selected position
                movePiece(boardTiles[currentSelectedPiece.position[0], currentSelectedPiece.position[1]],
                    boardTiles[Convert.ToInt32(positionToMoveTo[0]), Convert.ToInt32(positionToMoveTo[1])]);
        

                //Update player turn
                currentPlayerTurn = gamePlayer == GamePiece.Player.PLAYER1 ? GamePiece.Player.PLAYER2 : GamePiece.Player.PLAYER1;

                //Show waiting for player
                waitingForPlayer(true);
                /*
                    Update DB of move that was done here
                */
            }
        }

        private void waitingForPlayer(bool isOtherPlayersTurn)
        {
            waitingForOtherPlayer.Visibility = isOtherPlayersTurn ? Visibility.Visible : Visibility.Hidden;
            waitingForPlayerText.Visibility = isOtherPlayersTurn ? Visibility.Visible : Visibility.Hidden;
        }

        /// <summary>
        /// Handles the UI updating associated with moving a piece.  That is, if you're doing a regular move, it draws the piece in the new
        /// position.  If you're eating a piece, it removes the piece that was killed from the board and updates the position of the piece
        /// that ate it.
        /// </summary>
        private void movePiece(GameTile sourceTile, GameTile destinationTile)
        {
            //If player 1 just reached the opposite side of the board
            if (destinationTile.position[1] == boardHeight - 1 && sourceTile.tilePiece.player == GamePiece.Player.PLAYER1)
            {
                sourceTile.tilePiece.pieceType = GamePiece.PieceType.KING;
            }
            //If player 2 just reached the opposite side of the board
            else if (destinationTile.position[1] == 0 && sourceTile.tilePiece.player == GamePiece.Player.PLAYER2)
            {
                sourceTile.tilePiece.pieceType = GamePiece.PieceType.KING;
            }

            //If you moved more than one tile horizontally, that means you ate a piece, so we have to remove that eaten piece from the board
            if (Math.Abs(destinationTile.position[0] - sourceTile.position[0]) > 1)
            {
                int xPosOfEnemyPiece = (destinationTile.position[0] + sourceTile.position[0]) / 2;
                
                int yPosOfEnemyPiece = (destinationTile.position[1] + sourceTile.position[1]) / 2;

                //Kill enemy's piece and redraw that tile
                GameTile enemyPieceTile = boardTiles[xPosOfEnemyPiece, yPosOfEnemyPiece];
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
            foreach (int[] position in possibleMoves)
            {
                boardTileGrids[position[0], position[1]].Opacity = NOTPLAYABLE;
            }
            possibleMoves.Clear();

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
            if (currentPlayerTurn != gamePlayer) return;
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
            if (currentPlayerTurn != gamePlayer) return;
            if ((sender as Grid).Opacity == PLAYABLE)
            {
                (App.Current.MainWindow).Cursor = Cursors.Arrow;
            }
        }


        /// <summary>
        /// Initializes the image objects that should be referenced by each boardTile to be matched with its respective
        /// Game grid tile.
        /// </summary>
        private void initializeBoardTileImageSources()
        {
            //Only initializing the tiles that are playable on checkers
            boardTiles[1, 0].tileImage = T10;
            boardTiles[3, 0].tileImage = T30;

            boardTiles[0, 1].tileImage = T01;
            boardTiles[2, 1].tileImage = T21;

            boardTiles[1, 2].tileImage = T12;
            boardTiles[3, 2].tileImage = T32;

            boardTiles[0, 3].tileImage = T03;
            boardTiles[2, 3].tileImage = T23;

            boardTiles[1, 4].tileImage = T14;
            boardTiles[3, 4].tileImage = T34;

            boardTiles[0, 5].tileImage = T05;
            boardTiles[2, 5].tileImage = T25;

            boardTiles[1, 6].tileImage = T16;
            boardTiles[3, 6].tileImage = T36;

            boardTiles[0, 7].tileImage = T07;
            boardTiles[2, 7].tileImage = T27;
        }

        /// <summary>
        /// Attaches event handlers to each game board tile and draws them onto the Game.s
        /// </summary>
        private void drawGameBoardPieces()
        {
            for(int x = 0; x < boardWidth; ++x)
            {
                for(int y = 0; y< boardHeight; ++y)
                {
                    initializePieceEvents(boardTiles[x, y]);
                    boardTiles[x, y].drawTileImage();
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
            if (currentPlayerTurn != gamePlayer) return;
            if (isMyPiece(sender))
            {
                (App.Current.MainWindow).Cursor = Cursors.Hand;
            }
        }

        //If the sender was your piece, change the cursor back to an arrow
        private void initializeMouseLeavePieceEvent(object sender, EventArgs e)
        {
            if (currentPlayerTurn != gamePlayer) return;
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
            currentSelectedPiece = boardTiles[Convert.ToInt32( tilePosition[0]),Convert.ToInt32( tilePosition[1])];
            currentSelectedPiece.tileImage.Opacity = 0.5;

            //Clear previously highlighted tiles
            foreach(int[] position in possibleMoves)
            {
                boardTileGrids[position[0], position[1]].Opacity = NOTPLAYABLE;
            }
            possibleMoves.Clear();

           possibleMoves = currentSelectedPiece.getPossibleMoves(boardTiles, gamePlayer);

            //Highlight new possible moves
            foreach (int[] position in possibleMoves)
            {
                boardTileGrids[position[0], position[1]].Opacity = PLAYABLE;
            }
        }


        /// <summary>
        /// Returns true if the sender was your piece, false otherwise
        /// </summary>]
        private bool isMyPiece(object sender)
        {
            Image piece = sender as Image;
            string[] position = piece.Uid.Split(',');

            return boardTiles[Convert.ToInt32(position[0]), Convert.ToInt32(position[1])].tilePiece.player == gamePlayer;
            
        }

        /// <summary>
        /// Unhighlights the selected piece and possible moves
        /// </summary>
        private void Grid_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (currentPlayerTurn != gamePlayer) return;
            if (currentSelectedPiece == null) return;
            currentSelectedPiece.tileImage.Opacity = 1;
            currentSelectedPiece = null;

            foreach(int[] position in possibleMoves)
            {
                boardTileGrids[position[0], position[1]].Opacity = NOTPLAYABLE;
            }
            possibleMoves.Clear();
        }

        public static int getBoardWidth()
        {
            return boardWidth;
        }

        public static int getBoardHeight()
        {
            return boardHeight;
        }

       
        
        private async void Button_Click_1(object sender, RoutedEventArgs e)
        {
   
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            PageTransitionHelper.transitionToPage(new GameLobby());
        }
    }
}
