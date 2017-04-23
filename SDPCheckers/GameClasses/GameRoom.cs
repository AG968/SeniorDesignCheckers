using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SDPCheckers.GameClasses
{
    class GameRoom
    {
        private int _gameID;
        public string gameID {
            get
            {
                return _gameID.ToString();
            }
            set
            {
                _gameID = Convert.ToInt32(value);
            }
        }
        private int _numOfPlayers;
        public string numOfPlayers {
            get
            {
                return _numOfPlayers.ToString();
            }
            set
            {
                _numOfPlayers = Convert.ToInt32(value);
            }
        }

        /// <summary>
        /// The GameRoom will be what is returned by the database to show the list of games currently available to join
        /// </summary>
        public GameRoom()
        {
            
        }
    }
}
