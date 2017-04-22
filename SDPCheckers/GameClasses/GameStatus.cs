using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SDPCheckers.GameClasses
{
    class GameStatus
    {

        private string _status;
        public int status {
            get {
                return Convert.ToInt32(_status);
            }
            set {
                _status = value.ToString();
            }
        }

        private string _gameID;
        public int gameID {
            get
            {
                return Convert.ToInt32(_gameID);
            }
            set
            {
                _gameID = value.ToString();
            }
        }

        private string _numOfPlayers;
        public int numOfPlayers {
            get
            {
                return Convert.ToInt32(_numOfPlayers);
            }
            set
            {
                _numOfPlayers = value.ToString();
            }
        }

        private string _currentPlayerTurn;
        public int currentPlayerTurn {
            get
            {
                return Convert.ToInt32(_currentPlayerTurn);
            }
            set
            {
                _currentPlayerTurn = value.ToString();
            }
        }

        public string _sourceCol;
        public int sourceCol {
            get
            {
                return Convert.ToInt32(_sourceCol);
            }
            set
            {
                _sourceCol = value.ToString();
            }
        }

        public string _sourceRow;
        public int sourceRow {
            get
            {
                return Convert.ToInt32(_sourceRow);
            }
            set
            {
                _sourceRow = value.ToString();
            }
        }

        public string _destCol;
        public int destCol {
            get
            {
                return Convert.ToInt32(_destCol);
            }
            set
            {
                _destCol = value.ToString();
            }
        }

        public string _destRow;
        public int destRow {
            get
            {
                return Convert.ToInt32(_destRow);
            }
            set
            {
                _destRow = value.ToString();
            }
        }

        public GameStatus()
        {

        }
    }
}
