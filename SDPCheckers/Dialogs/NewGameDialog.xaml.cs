using MahApps.Metro.Controls.Dialogs;
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

namespace SDPCheckers.Dialogs
{
    /// <summary>
    /// Interaction logic for NewGameDialog.xaml
    /// </summary>
    public partial class NewGameDialog : CustomDialog
    {
        //Used as a flag to determine whether or not the room name and room password UI textboxes should be checked after the dialog is closed
        public bool result { get; set; }

        public NewGameDialog()
        {
            InitializeComponent();
        }

        private void cancelClicked(object sender, RoutedEventArgs e)
        {
            result = false;
            PageTransitionHelper.closeDialog(this);
        }

        private void createRoomClicked(object sender, RoutedEventArgs e)
        {
            result = true;
            PageTransitionHelper.closeDialog(this);
        }
    }
}
