using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SDPCheckers.GameClasses
{
    class GameRoom
    {
        public string roomName { get; set; }
        public string roomPassword { get; set; }
        public int numberOfPlayers { get; set; }

        /// <summary>
        /// The GameRoom is a medium to connect two players together via the database before initializing an actual Game
        /// </summary>
        /// <param name="roomName"></param>
        /// <param name="roomPassword"></param>
        public GameRoom(string  roomName, string roomPassword)
        {
            this.roomName = roomName;
            this.roomPassword = roomPassword;

            //initialize number of players to 1 to account for the host of the game
            numberOfPlayers = 1;
        }
    }
}
