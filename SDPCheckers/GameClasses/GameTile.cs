using SDPCheckers.Pages;
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
        private static BitmapImage PLAYER1CHECKERPIECE = new BitmapImage(new Uri("pack://application:,,,/SDPCheckers;component/Images/Checkers_Piece_White.png", UriKind.Absolute));
        private static BitmapImage PLAYER2CHECKERPIECE = new BitmapImage(new Uri("pack://application:,,,/SDPCheckers;component/Images/Checkers_Piece_Red.png", UriKind.Absolute));
        private static BitmapImage PLAYER1KINGPIECE = new BitmapImage(new Uri("pack://application:,,,/SDPCheckers;component/Images/Checkers_Piece_White_King.png", UriKind.Absolute));
        private static BitmapImage PLAYER2KINGPIECE = new BitmapImage(new Uri("pack://application:,,,/SDPCheckers;component/Images/Checkers_Piece_Red_King.png", UriKind.Absolute));

        public GamePiece tilePiece = null;

        //position[0] = position[XPOS] = x position of the tile
        //position[1] = position[YPOS] = y position of the tile
        public int[] position = new int[2];
        private const int XPOS = 0;
        private const int YPOS = 1;

        //Upon initialization of a game board, each GameTile is assigned the tile image associated with the tiles position.
        //I.E a GameTile with position {1 , 2} would be given the tile image at that coordinate
        public Image tileImage = new Image();

        public GameTile(GamePiece tilePiece, int xPosition, int yPosition)
        {
            position[XPOS] = xPosition;
            position[YPOS] = yPosition;
            this.tilePiece = tilePiece;

            
        }

        /// <summary>
        /// Checks what piece is currently in the tile.  If there is no piece, the tile image source is cleared, otherwise,
        /// the image source is updated based on which player the piece in the tile belongs to.
        /// </summary>
        public void drawTileImage()
        {
            if (tilePiece == null)
            {
                tileImage.Source = null;
            }else
            {
                if(tilePiece.player == GamePiece.Player.PLAYER1)
                {
                    tileImage.Source = tilePiece.pieceType == GamePiece.PieceType.CHECKER
                        ? PLAYER1CHECKERPIECE
                        : PLAYER1KINGPIECE;
                }else
                {
                    tileImage.Source = tilePiece.pieceType == GamePiece.PieceType.CHECKER
                        ? PLAYER2CHECKERPIECE
                        : PLAYER2KINGPIECE;
                }
            }
        }

        /// <summary>
        /// Takes in the state of the boardTiles and the player number of the game.
        /// Based on the player that the piece belongs to, it determines the
        /// possible moves that can be made by the specified piece.
        /// </summary>
        public List<int[]> getPossibleMoves(GameTile[,] boardTiles, GamePiece.Player gamePlayer)
        {
            List<int[]> possibleMoves = new List<int[]>();
            //Player 1's and Player 2's left and right possible diagonals are different
            switch (gamePlayer)
            {
                case GamePiece.Player.PLAYER1:
                    //Player 1 left diagonal, forward
                    if (position[XPOS] - 1 >= 0 && position[YPOS] + 1 < Game.getBoardHeight())
                    {
                        if (boardTiles[position[XPOS] - 1, position[YPOS] + 1].tilePiece == null)
                        {
                            possibleMoves.Add(new int[] { position[XPOS] - 1, position[YPOS] + 1 });
                        }
                        //Check if you can jump over a piece to your left
                        else if (boardTiles[position[XPOS] - 1, position[YPOS] + 1].tilePiece.player != gamePlayer)
                        {
                            if (position[XPOS] - 2 >= 0 && position[YPOS] + 2 < Game.getBoardHeight())
                            {
                                if (boardTiles[position[XPOS] - 2, position[YPOS] + 2].tilePiece == null)
                                {
                                    possibleMoves.Add(new int[] { position[XPOS] - 2, position[YPOS] + 2 });
                                }
                            }
                        }
                    }
                    //Player 1 left diagonal, backward, for when a piece is a king
                    if (boardTiles[position[XPOS], position[YPOS]].tilePiece.pieceType == GamePiece.PieceType.KING)
                    {
                        if (position[XPOS] - 1 >= 0 && position[YPOS] - 1 >= 0)
                        {
                            if (boardTiles[position[XPOS] - 1, position[YPOS] - 1].tilePiece == null)
                            {
                                possibleMoves.Add(new int[] { position[XPOS] - 1, position[YPOS] - 1 });
                            }
                            //Check if you can jump over a piece to your left, backwards
                            else if (boardTiles[position[XPOS] - 1, position[YPOS] - 1].tilePiece.player != gamePlayer)
                            {
                                if (position[XPOS] - 2 >= 0 && position[YPOS] - 2 >= 0)
                                {
                                    if (boardTiles[position[XPOS] - 2, position[YPOS] - 2].tilePiece == null)
                                    {
                                        possibleMoves.Add(new int[] { position[XPOS] - 2, position[YPOS] - 2 });
                                    }
                                }
                            }
                        }
                    }
                    //Player 1 right diagonal
                    if (position[XPOS] + 1 < Game.getBoardWidth() && position[YPOS] + 1 < Game.getBoardHeight())
                    {
                        if (boardTiles[position[XPOS] + 1, position[YPOS] + 1].tilePiece == null)
                        {
                            possibleMoves.Add(new int[] { position[XPOS] + 1, position[YPOS] + 1 });
                        }
                        //Check if you can jump over a piece to your right
                        else if (boardTiles[position[XPOS] + 1, position[YPOS] + 1].tilePiece.player != gamePlayer)
                        {
                            if (position[XPOS] + 2 < Game.getBoardWidth() && position[YPOS] + 2 < Game.getBoardHeight())
                            {
                                if (boardTiles[position[XPOS] + 2, position[YPOS] + 2].tilePiece == null)
                                {
                                    possibleMoves.Add(new int[] { position[XPOS] + 2, position[YPOS] + 2 });
                                }
                            }
                        }
                    }
                    //Player 1's right diagonal, backward, if piece is a King
                    if (boardTiles[position[XPOS], position[YPOS]].tilePiece.pieceType == GamePiece.PieceType.KING)
                    {
                        if (position[XPOS] + 1 < Game.getBoardWidth() && position[YPOS] - 1 >= 0)
                        {
                            if (boardTiles[position[XPOS] + 1, position[YPOS] - 1].tilePiece == null)
                            {
                                possibleMoves.Add(new int[] { position[XPOS] + 1, position[YPOS] - 1 });
                            }
                            //Check if you can jump over a piece to your right
                            else if (boardTiles[position[XPOS] + 1, position[YPOS] - 1].tilePiece.player != gamePlayer)
                            {
                                if (position[XPOS] + 2 < Game.getBoardWidth() && position[YPOS] - 2 >= 0)
                                {
                                    if (boardTiles[position[XPOS] + 2, position[YPOS] - 2].tilePiece == null)
                                    {
                                        possibleMoves.Add(new int[] { position[XPOS] + 2, position[YPOS] - 2 });
                                    }
                                }
                            }
                        }
                    }
                    break;
                case GamePiece.Player.PLAYER2:
                    //Player 2's left diagonal
                    if (position[XPOS] + 1 < Game.getBoardWidth() && position[YPOS] - 1 >= 0)
                    {
                        if (boardTiles[position[XPOS] + 1, position[YPOS] - 1].tilePiece == null)
                        {
                            possibleMoves.Add(new int[] { position[XPOS] + 1, position[YPOS] - 1 });
                        }
                        //Check if you can jump over a piece to your left
                        else if (boardTiles[position[XPOS] + 1, position[YPOS] - 1].tilePiece.player != gamePlayer)
                        {
                            if (position[XPOS] + 2 < Game.getBoardWidth() && position[YPOS] - 2 >= 0)
                            {
                                if (boardTiles[position[XPOS] + 2, position[YPOS] - 2].tilePiece == null)
                                {
                                    possibleMoves.Add(new int[] { position[XPOS] + 2, position[YPOS] - 2 });
                                }
                            }
                        }
                    }
                    //Player 2's left diagonal, backwards, if piece is a King
                    if (boardTiles[position[XPOS], position[YPOS]].tilePiece.pieceType == GamePiece.PieceType.KING)
                    {
                        if (position[XPOS] + 1 < Game.getBoardWidth() && position[YPOS] + 1 < Game.getBoardHeight())
                        {
                            if (boardTiles[position[XPOS] + 1, position[YPOS] + 1].tilePiece == null)
                            {
                                possibleMoves.Add(new int[] { position[XPOS] + 1, position[YPOS] + 1 });
                            }
                            //Check if you can jump over a piece to your left
                            else if (boardTiles[position[XPOS] + 1, position[YPOS] + 1].tilePiece.player != gamePlayer)
                            {
                                if (position[XPOS] + 2 < Game.getBoardWidth() && position[YPOS] + 2 < Game.getBoardHeight())
                                {
                                    if (boardTiles[position[XPOS] + 2, position[YPOS] + 2].tilePiece == null)
                                    {
                                        possibleMoves.Add(new int[] { position[XPOS] + 2, position[YPOS] + 2 });
                                    }
                                }
                            }
                        }
                    }
                    //Player 2's right diagonal
                    if (position[XPOS] - 1 >= 0 && position[YPOS] - 1 >= 0)
                    {
                        if (boardTiles[position[XPOS] - 1, position[YPOS] - 1].tilePiece == null)
                        {
                            possibleMoves.Add(new int[] { position[XPOS] - 1, position[YPOS] - 1 });
                        }
                        //Check if you can jump over a piece to your right
                        else if (boardTiles[position[XPOS] - 1, position[YPOS] - 1].tilePiece.player != gamePlayer)
                        {
                            if (position[XPOS] - 2 >= 0 && position[YPOS] - 2 >= 0)
                            {
                                if (boardTiles[position[XPOS] - 2, position[YPOS] - 2].tilePiece == null)
                                {
                                    possibleMoves.Add(new int[] { position[XPOS] - 2, position[YPOS] - 2 });
                                }
                            }
                        }
                    }
                    //Player 2's right diagonal, backwards, if piece is a King
                    if (boardTiles[position[XPOS], position[YPOS]].tilePiece.pieceType == GamePiece.PieceType.KING)
                    {
                        if (position[XPOS] - 1 >= 0 && position[YPOS] + 1 < Game.getBoardHeight())
                        {
                            if (boardTiles[position[XPOS] - 1, position[YPOS] + 1].tilePiece == null)
                            {
                                possibleMoves.Add(new int[] { position[XPOS] - 1, position[YPOS] + 1 });
                            }
                            //Check if you can jump over a piece to your right
                            else if (boardTiles[position[XPOS] - 1, position[YPOS] + 1].tilePiece.player != gamePlayer)
                            {
                                if (position[XPOS] - 2 >= 0 && position[YPOS] + 2 < Game.getBoardHeight())
                                {
                                    if (boardTiles[position[XPOS] - 2, position[YPOS] + 2].tilePiece == null)
                                    {
                                        possibleMoves.Add(new int[] { position[XPOS] - 2, position[YPOS] + 2 });
                                    }
                                }
                            }
                        }
                    }
                    break;
                default:
                    break;

            }

            return possibleMoves;

        }

    }
}
