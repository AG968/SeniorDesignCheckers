<?php

	// Prepare variables for database connection
	
	$dbusername = "AG968";  // enter database username
	$dbpassword = "mahmoudag";  // enter database password
	$server = "www.abugharbieh.com"; 
	$dbname = "SeniorDesignProject";
	
	// Create connection
	$conn = new mysqli($server, $dbusername, $dbpassword, $dbname);
	
	// Check connection
	if ($conn->connect_error) {
	    echo "CONNECTION FAILED";
	    die("Connection failed: " . $conn->connect_error);
	} 


      //$dbconnect = mysql_pconnect($server, $dbusername, $dbpassword);
      //$dbselect = mysql_select_db("SeniorDesignProject",$dbconnect);

	//POST request handler
	if ($_SERVER['REQUEST_METHOD'] === 'POST') 
	{
	
		//Make sure all required POST parameters have been passed
		if( isset(
			$_POST['gameID'], 
			$_POST['numOfPlayers'], 
			$_POST['currentPlayerTurn'],
			$_POST['sourceCol'],
			$_POST['sourceRow'],
			$_POST['destCol'],
		$_POST['destRow']
		))
		{
			//$sql = "SELECT * FROM  `Games` WHERE  `gameID` = 1";
			//Update the DB of the game with the proper POST parameters
			$sql = 
			"UPDATE 
				Games 
			SET 
				numOfPlayers = '".$_POST['numOfPlayers']."',
				currentPlayerTurn = '".$_POST['currentPlayerTurn']."',
				sourceCol = '".$_POST['sourceCol']."', 
				sourceRow = '".$_POST['sourceRow']."',
				destCol = '".$_POST['destCol']."',
				destRow = '".$_POST['destRow']."'
			WHERE 
				gameID = '".$_POST['gameID']."'";
		
			if ($conn->query($sql) === TRUE) 
			{
				$sql = "SELECT * FROM  `Games` WHERE  `gameID` = 1";
		        	// Execute SQL statement
		      		$result = mysqli_query($conn,$sql); 
		      		$row = $result->fetch_array(); 
		      		
		  		$gameStatusString = 
		  			"{
		  				status: 1,
		  				gameID : '".$_POST['gameID']."',
		  				numOfPlayers : '".$_POST['numOfPlayers']."',
		  				currentPlayerTurn: '".$_POST['currentPlayerTurn']."',
		  				sourceCol: '".$_POST['sourceCol']."',
		  				sourceRow: '".$_POST['sourceRow']."',
		  				destCol: '".$_POST['destCol']."',
		  				destRow: '".$_POST['destRow']."'
		  			 }";
		  			
				echo $gameStatusString;   
			} 
			else 
			{
			 
		      		$gameStatusString = 
	  				"{
		  				status: -1,
		  				gameID : 0,
		  				numOfPlayers : 0,
		  				currentPlayerTurn: 0,
		  				sourceCol: 0,
		  				sourceRow: 0,
		  				destCol: 0,
		  				destRow: 0
		  			 }";
				echo $gameStatusString;
			}
		
		}
		else{
			echo "ERROR: POST MUST include a gameID, numOfPlayers, currentPlayerTurn, sourceCol, sourceRow, destCol, and destRow field";
		}
	
	}
	//GET Request handler 
	elseif($_SERVER['REQUEST_METHOD'] === 'GET')
        {
        	if(isset($_GET['gameID']))
        	{
        	
	        	$sql = "SELECT * FROM  `Games` WHERE  `gameID` = '".$_GET['gameID']."'";
	        	// Execute SQL statement
	      		$result = mysqli_query($conn,$sql); 
	      		$row = $result->fetch_array();  
	  		
	  		//if invalid gameID
	  		if($row['gameID'] == "" )
	  		{
	  			$gameStatusString = 
  				"{
	  				status: -1,
	  				gameID : 0,
	  				numOfPlayers : 0,
	  				currentPlayerTurn: 0,
	  				sourceCol: 0,
	  				sourceRow: 0,
	  				destCol: 0,
	  				destRow: 0
	  			 }";
  			
			echo $gameStatusString;
	  		}
	  		else
	  		{
		  		$gameStatusString = 
	  			"{
	  				status: 1,
	  				gameID : '".$row['gameID']."',
	  				numOfPlayers : '".$row['numOfPlayers']."',
	  				currentPlayerTurn: '".$row['currentPlayerTurn']."',
	  				sourceCol: '".$row['sourceCol']."',
	  				sourceRow: '".$row['sourceRow']."',
	  				destCol: '".$row['destCol']."',
	  				destRow: '".$row['destRow']."'
	  			 }";
	  			
				echo $gameStatusString;   
			}
		} 
		else
		{
			$gameStatusString = 
  				"{
	  				status: -1,
	  				gameID : 0,
	  				numOfPlayers : 0,
	  				currentPlayerTurn: 0,
	  				sourceCol: 0,
	  				sourceRow: 0,
	  				destCol: 0,
	  				destRow: 0
	  			 }";
  			
			echo $gameStatusString;
		}       
        }

?>