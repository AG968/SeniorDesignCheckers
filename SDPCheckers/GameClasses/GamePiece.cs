using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SDPCheckers.GameClasses
{
    public class GamePiece
    {
        public enum Player
        {
            PLAYER1,
            PLAYER2
        }

        public Player player;
   
        public GamePiece(Player player)
        {
            this.player = player;
        }
    }
}
