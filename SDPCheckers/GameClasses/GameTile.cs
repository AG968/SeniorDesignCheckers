using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media.Imaging;

namespace SDPCheckers.GameClasses
{
    public class GameTile
    {


        public GamePiece tilePiece = null;
        int[] position = new int[2];
        private const int XPOS = 0;
        private const int YPOS = 1;

        public Image tileImage = new Image();

        public GameTile(GamePiece tilePiece, int xPosition, int yPosition)
        {
            position[XPOS] = xPosition;
            position[YPOS] = yPosition;
            this.tilePiece = tilePiece;

            
        }

        public void drawTileImage()
        {
            if (tilePiece == null)
            {
                tileImage.Source = null;
            }else
            {
                
                tileImage.Source = tilePiece.player == GamePiece.Player.PLAYER1
                            ? new BitmapImage(new Uri("pack://application:,,,/SDPCheckers;component/Images/Checkers_Piece_Blue.png", UriKind.Absolute))
                            : new BitmapImage(new Uri("pack://application:,,,/SDPCheckers;component/Images/Checkers_Piece_Orange.png", UriKind.Absolute));
            }
                


        }
    }
}
