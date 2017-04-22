using MahApps.Metro.Controls;
using SDPCheckers.GameClasses;
using SDPCheckers.Pages;
using System.Windows;

namespace SDPCheckers
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow
    {
        public string playerName { get; set; }

        public MainWindow()
        {
            InitializeComponent();
            this.currentPage.Content = new IntroductionPage();
        }
    }
}
