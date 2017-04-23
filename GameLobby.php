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

	//GET Request handler 
	if($_SERVER['REQUEST_METHOD'] === 'GET')
        {
        	if(isset($_GET['Request']))
        	{
        		if($_GET['Request'] == "getListOfGames")
        		{
        			$sql = "SELECT `gameID`,`numOfPlayers` FROM  `Games`";
		        	// Execute SQL statement
		      		$result = mysqli_query($conn,$sql); 
		      		$rows = array();
		      		while($row = $result->fetch_array())
		      		{
		      			array_push($rows,$row); 
		      		}
		      		
		      		echo json_encode($rows); 
        		}
        		else{
        			echo "no get request";
        		}
	        	  
		} 
		else
		{
			
			echo "test";
		}       
       }
        elseif($_SERVER['REQUEST_METHOD'] === 'POST')
        {
        	if(isset($_POST['Request']))
        	{
        		if($_POST['Request'] == "createGame")
        		{
        			//Get max Game ID
        			$sql = "SELECT * FROM Games ORDER BY gameID DESC LIMIT 1";
		        	// Execute SQL statement
		      		$result = mysqli_query($conn,$sql); 
		      		$row = $result->fetch_array();  
		
				$gameID = $row['gameID'] + 1;
				
		  		$sql = "INSERT INTO Games (`gameID`, `numOfPlayers`, `currentPlayerTurn`, `sourceCol`, `sourceRow`, `destCol`, `destRow`) 
		  		VALUES ('".$gameID."', '1', '0', '0', '0', '0', '0')";
		  		
		  		if ($conn->query($sql) === TRUE) 
		  		{		  		
		  			//Return max gameID
					echo $gameID;
				} else {
				    echo "Error: " . $sql . "<br>" . $conn->error;
				}
				
        		}
        		elseif($_POST['Request'] =="deleteGame")
        		{
        			$sql = "DELETE FROM Games WHERE gameID = '".$_POST['gameID']."'";
        			if($conn->query($sql) === TRUE)
        			{
        				echo "Game Deleted";
        			}else{
        			
        				echo "Error: " . $sql . "<br>" . $conn->error;
        			}
        		}
        		elseif($_POST['Request'] == "joinGame")
        		{
        			$sql = 
					"UPDATE 
						Games 
					SET 
						numOfPlayers = 2,
						currentPlayerTurn = 0,
						sourceCol = 0, 
						sourceRow = 0,
						destCol = 0,
						destRow = 0
					WHERE 
						gameID = '".$_POST['gameID']."'";
						
				if($conn->query($sql) === TRUE)
        			{
        				$gameStatusString = 
		  			"{
		  				status: 1,
		  				gameID : '".$_POST['gameID']."',
		  				numOfPlayers : 2,
		  				currentPlayerTurn: 0,
		  				sourceCol: 0,
		  				sourceRow: 0,
		  				destCol: 0,
		  				destRow: 0
		  			 }";
		  			
					echo $gameStatusString;   

        			}else{
        			
        				$gameStatusString = 
		  			"{
		  				status: -1,
		  				gameID : '".$_POST['gameID']."',
		  				numOfPlayers : 1,
		  				currentPlayerTurn: 0,
		  				sourceCol: 0,
		  				sourceRow: 0,
		  				destCol: 0,
		  				destRow: 0
		  			 }";
		  			
					echo $gameStatusString;   
        			}
        		}
        	}
        }


?>