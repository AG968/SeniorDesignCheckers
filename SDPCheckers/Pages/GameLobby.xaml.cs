using MahApps.Metro.Controls;
using MahApps.Metro.Controls.Dialogs;
using SDPCheckers.Dialogs;
using SDPCheckers.GameClasses;
using SDPCheckers.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace SDPCheckers.Pages
{
    /// <summary>
    /// Interaction logic for GameLobby.xaml
    /// </summary>
    public partial class GameLobby : UserControl
    {
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

            await Task.Run(() =>
            {
                updateLobbyList();
            });
            

            searchingProgressRing.IsActive = false;
            searchingProgressRing.Visibility = Visibility.Hidden;

        }

        private async void createGameClicked(object sender, RoutedEventArgs e)
        {
            NewGameDialog newGameDialog = new NewGameDialog();
            await PageTransitionHelper.showDialog(newGameDialog);
            if (newGameDialog.result)
            {
                GameRoom gameToCreate = new GameRoom(newGameDialog.roomNameInput.Text, newGameDialog.roomPasswordInput.Text);
                var controller = await (App.Current.MainWindow as MetroWindow).ShowProgressAsync("Waiting for opponent...", "Waiting for an opponent to join...", isCancelable: true);
                controller.SetIndeterminate();

                //Wait for opponent on a background thread to not block the UI thread while waiting
                await Task.Run(() =>
                {
                    while (!controller.IsCanceled)
                    {
                        /*
                         * Send created game to DB
                         * Check if player has joined...
                         */
                    }
                });

                //Cancel clicked
                /*
                 * Code to remove created game from DB here
                 */
                await controller.CloseAsync();
            }
        }

        private  void updateLobbyList()
        {
                //Code to request list of games from DB goes here
        }
    }
}
