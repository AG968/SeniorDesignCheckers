using SDPCheckers.GameClasses;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;

namespace SDPCheckers.Utilities
{
    public class DatabaseGameHelper
    {
        public static async Task<string> sendMove(int gameID, int numOfPlayers, GamePiece.Player currentPlayerTurn, int sourceCol, int sourceRow, int destCol, int destRow)
        {
            //Database uses 0 for player 1, and 1 for player 2, so if it's player 1's turn, the next value for the next player should be updated to 1,
            //and if it's player 2's turn, the value for the next player should be updated to 0;
            string nextPlayer = currentPlayerTurn == GamePiece.Player.PLAYER1 ? "1" : "0";

            using (var client = new HttpClient())
            {
                var values = new Dictionary<string, string>
                {
                    { "gameID", gameID.ToString() },
                    { "numOfPlayers", numOfPlayers.ToString() },
                    { "currentPlayerTurn", nextPlayer },
                    { "sourceCol", sourceCol.ToString() },
                    { "sourceRow", sourceRow.ToString() },
                    { "destCol", destCol.ToString() },
                    { "destRow", destRow.ToString() }
                };

                var content = new FormUrlEncodedContent(values);

                var response = await client.PostAsync("http://abugharbieh.com/test/GameStatus.php", content);

                return await response.Content.ReadAsStringAsync();
            }
        }

    

        public static async Task<string> createGame()
        {
            using (var client = new HttpClient())
            {
                var values = new Dictionary<string, string>
                {
                    { "Request", "createGame" }
                };

                var content = new FormUrlEncodedContent(values);

                var response = await client.PostAsync("http://abugharbieh.com/test/GameLobby.php", content);

                return await response.Content.ReadAsStringAsync();
                
            }
        }

        public static async Task<string> deleteGame(int gameID)
        {
            using (var client = new HttpClient())
            {
                var values = new Dictionary<string, string>
                {
                    {"Request", "deleteGame" },
                    {"gameID", gameID.ToString() }
                };

                var content = new FormUrlEncodedContent(values);
                var response = await client.PostAsync("http://abugharbieh.com/test/GameLobby.php", content);

                var x = await response.Content.ReadAsStringAsync();
                return x;
            }
        }

        public static async Task<string> joinGame(string gameID)
        {
            using (var client = new HttpClient())
            {
                var values = new Dictionary<string, string>
                {
                    {"Request", "joinGame" },
                    {"gameID", gameID }
                };

                var content = new FormUrlEncodedContent(values);
                var response = await client.PostAsync("http://abugharbieh.com/test/GameLobby.php", content);

                return await response.Content.ReadAsStringAsync();
            }
        }

        public static async Task<string> getListOfGames()
        {
            string address = string.Format(
                 "http://abugharbieh.com/test/GameLobby.php?Request={0}",
                 Uri.EscapeDataString("getListOfGames"));

            using (var client = new HttpClient())
            {

                return await client.GetStringAsync(address);
            }
        }

        public static async Task<string> checkGameStatus(string gameID)
        {
            string address = string.Format(
                "http://abugharbieh.com/test/GameStatus.php?gameID={0}",
                 Uri.EscapeDataString(gameID));

            using (var client = new HttpClient())
            {
                return await client.GetStringAsync(address);
            }
        }
    }
}
