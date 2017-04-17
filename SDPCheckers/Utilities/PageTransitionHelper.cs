using MahApps.Metro.Controls;
using MahApps.Metro.Controls.Dialogs;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace SDPCheckers.Utilities
{
    class PageTransitionHelper
    {
        /// <summary>
        /// Transitions the content of the current page to the given page
        /// </summary>
        public static void transitionToPage(UserControl page)
        {
            (App.Current.MainWindow as MainWindow).currentPage.Content = page;
        }

        /// <summary>
        /// Displays the dialog and waits for the dialog to be closed before returning, so that any input needed
        /// from the dialog can be acquired by the time showDialog returns.
        /// </summary>
        public async static Task<bool> showDialog<T>(T dialog)
        {
            await (App.Current.MainWindow as MetroWindow).ShowMetroDialogAsync(dialog as BaseMetroDialog);
            await (dialog as BaseMetroDialog).WaitUntilUnloadedAsync();
            return true;
        }


        public async static void closeDialog<T>(T dialog)
        {
            await (App.Current.MainWindow as MetroWindow).HideMetroDialogAsync(dialog as BaseMetroDialog);
        }
    }
}
