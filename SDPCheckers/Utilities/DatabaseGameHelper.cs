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
        
            string nextPlayer = currentPlayerTurn == GamePiece.Player.PLAYER1 ? "2" : "1";

            using (var client = new HttpClient())
            {
                var values = new Dictionary<string, string>
                {
                    { "source", "PC" },
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
                    { "Request", "createGame" },
                    { "source", "PC" }
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
                    {"source", "PC" },
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
                    {"source", "PC" },
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
                 "http://abugharbieh.com/test/GameLobby.php?Request={0}&source=PC",
                 Uri.EscapeDataString("getListOfGames"));

            using (var client = new HttpClient())
            {

                return await client.GetStringAsync(address);
            }
        }

        public static async Task<string> checkGameStatus(string gameID)
        {
            string address = string.Format(
                "http://abugharbieh.com/test/GameStatus.php?gameID={0}&source=PC",
                 Uri.EscapeDataString(gameID));

            using (var client = new HttpClient())
            {
                return await client.GetStringAsync(address);
            }
        }
    }
}
