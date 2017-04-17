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
    /// Interaction logic for IntroductionPage.xaml
    /// </summary>
    public partial class IntroductionPage : UserControl
    {
        public IntroductionPage()
        {
            InitializeComponent();
        }

        private void continueClicked(object sender, RoutedEventArgs e)
        {
            //Set name of player
            (App.Current.MainWindow as MainWindow).playerName = playerNameInput.Text;

            PageTransitionHelper.transitionToPage(new GameLobby());
        }
    }
}
