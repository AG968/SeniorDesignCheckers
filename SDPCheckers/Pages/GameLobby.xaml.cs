using MahApps.Metro.Controls;
using MahApps.Metro.Controls.Dialogs;
using Newtonsoft.Json;
using SDPCheckers.GameClasses;
using SDPCheckers.Utilities;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

namespace SDPCheckers.Pages
{
    /// <summary>
    /// Interaction logic for GameLobby.xaml
    /// </summary>
    public partial class GameLobby : UserControl
    {
        List<GameRoom> listOfGames = new List<GameRoom>();

        public GameLobby()
        {
            InitializeComponent();
            greetingLabel.Text = "Welcome, " + (App.Current.MainWindow as MainWindow).playerName + "!";
        }


        /// <summary>
        /// Sets the progress bar to spinning while a request to update the lobby list is sent to the backend 
        /// </summary>
        private async void refreshLobbyClicked(object sender, RoutedEventArgs e)
        {
            searchingProgressRing.IsActive = true;
            searchingProgressRing.Visibility = Visibility.Visible;

            await Task.Run(async () =>
            {
                listOfGames = await updateLobbyList();
            });
            GameList.ItemsSource = null;
            GameList.ItemsSource = listOfGames;

            searchingProgressRing.IsActive = false;
            searchingProgressRing.Visibility = Visibility.Hidden;

        }

        /// <summary>
        /// Shows an input dialog box requesting a room name and optional room password, and creates a game if the user presses OK.  
        /// Once the room is created, the DB is updated with the newly created room and waits for a player to join before creating a game.
        /// </summary>
        private async void createGameClicked(object sender, RoutedEventArgs e)
        {

            var controller = await (App.Current.MainWindow as MetroWindow).ShowProgressAsync("Waiting for opponent...", "Waiting for an opponent to join...", isCancelable: true);
            controller.SetIndeterminate();

            string statusString = await DatabaseGameHelper.createGame();
            GameStatus createGameStatus = JsonConvert.DeserializeObject<GameStatus>(statusString);
            if(createGameStatus.status == -1)
            {
                await controller.CloseAsync();
                await (App.Current.MainWindow as MetroWindow).ShowMessageAsync("Error while hosting game", "Please try again");
            }
            else
            {
                Game newGame = new Game(GamePiece.Player.PLAYER1, Convert.ToInt32(createGameStatus.gameID));

                await Task.Run(async () =>
                {

                    string gameStatusString = await DatabaseGameHelper.checkGameStatus(createGameStatus.gameID.ToString());
                    //Game status will give you the status of the game that was created, and check if anyone has joined before starting the game
                    GameStatus gameStatus = JsonConvert.DeserializeObject<GameStatus>(gameStatusString);
                    while (newGame.numOfPlayers == 1)
                    {
                        if (gameStatus.status == 1)
                        {
                            newGame.numOfPlayers = gameStatus.numOfPlayers;
                        }
                        else
                        {
                            await controller.CloseAsync();
                            await DatabaseGameHelper.deleteGame(Convert.ToInt32(createGameStatus.gameID));
                            await (App.Current.MainWindow as MetroWindow).ShowMessageAsync("Error while hosting game", "Please try again");
                            return;
                        }
                        if (controller.IsCanceled)
                        {
                            await controller.CloseAsync();
                            await DatabaseGameHelper.deleteGame(Convert.ToInt32(createGameStatus.gameID));
                            return;
                        }

                        await Task.Delay(5000);
                        gameStatusString = await DatabaseGameHelper.checkGameStatus(createGameStatus.gameID.ToString());
                        gameStatus = JsonConvert.DeserializeObject<GameStatus>(gameStatusString);
                    }

                });

                if (controller.IsOpen && !controller.IsCanceled)
                {
                    await controller.CloseAsync();

                    PageTransitionHelper.transitionToPage(newGame);
                }

            }


        }

        /// <summary>
        /// Sends a request to DB to update the list of existing games.
        /// </summary>
        private async Task<List<GameRoom>> updateLobbyList()
        {
            //Code to request list of games from DB goes here
            string listOfGamesString = await DatabaseGameHelper.getListOfGames();
            List<GameRoom> newListOfGames = JsonConvert.DeserializeObject<List<GameRoom>>(listOfGamesString);
            if (listOfGames == null) return new List<GameRoom>();

            return newListOfGames;
        }

        private async void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            listOfGames = await updateLobbyList();
            GameList.ItemsSource = null;
            GameList.ItemsSource = listOfGames;
        }

        private async void GameList_MouseDoubleClick(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            var gameRoom = (sender as DataGrid).SelectedItem as GameRoom;
            if(Convert.ToInt32(gameRoom.numOfPlayers)> 1)
            {
                await (App.Current.MainWindow as MetroWindow).ShowMessageAsync("Game is full", "The game is full, please join another match.");
                return;
            }
            string gameStatusString = await DatabaseGameHelper.joinGame(gameRoom.gameID);
            GameStatus gameStatus = JsonConvert.DeserializeObject<GameStatus>(gameStatusString);
            if(gameStatus.status == -1)
            {
                await (App.Current.MainWindow as MetroWindow).ShowMessageAsync("Error", "There was an error joining the game, please try again.");
                return;
            }else
            {
                Game newGame = new Game(GamePiece.Player.PLAYER2, Convert.ToInt32(gameRoom.gameID));
                newGame.numOfPlayers = 2;
                PageTransitionHelper.transitionToPage(newGame);
            }
        }
    }
}
