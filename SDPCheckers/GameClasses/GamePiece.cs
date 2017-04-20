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
        public enum PieceType
        {
            CHECKER,
            KING
        }

        public Player player;
        public PieceType pieceType;
        public GamePiece(Player player)
        {
            this.player = player;
            pieceType = PieceType.CHECKER;
        }

    }
}
