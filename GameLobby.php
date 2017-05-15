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
        	if(isset(
        		$_GET['Request'], 
        		$_GET['source']
        		)
        	  )
        	{
			//Get the list of games
        		if($_GET['Request'] == "getListOfGames")
        		{
        			$sql = "SELECT `gameID`,`numOfPlayers` FROM  `Games` where `numOfPlayers` = 1";
		        	// Execute SQL statement
		      		$result = mysqli_query($conn,$sql); 
		      		$rows = array();
		      		while($row = $result->fetch_array())
		      		{
		      			array_push($rows,$row); 
		      		}
		      		
		      		if($_GET['source'] == "PC")
		      		{
		      			echo json_encode($rows); 
		      		}
		      		else
		      		{
		      		//If the source is the arduino, return a string in the format : {numOfGames,gameId1,gameId2,gameIdN}
		      		  	$response = "{" . count($rows) . ",";
		      		  	for($game = 0; $game < count($rows); ++$game)
		      		  	{	
		      		  		$response = $response . $rows[$game]['gameID'];
		      		  		if($game != count($rows) - 1)
		      		  		{
		      		  			$response = $response . ",";
		      		  		}
		      		  	}
		      		  	$response = $response . "}";
		      		  	echo $response;
		      		}
		      		
        		}
        		
        		else{
				echo 
		  			"{
		  				status: -1
		  			 }";
        		}
	        	  
		} 
		else
		{
			
			echo 
				"{
					status: -1
				}";
		}       
       }
        elseif($_SERVER['REQUEST_METHOD'] === 'POST')
        {
        	if(isset($_POST['Request'], $_POST['source']))
        	{
        		//Create a game with the next valid gameID
        		if($_POST['Request'] == "createGame")
        		{
        			//Get max Game ID
        			$sql = "SELECT * FROM Games ORDER BY gameID DESC LIMIT 1";
		        	// Execute SQL statement
		      		$result = mysqli_query($conn,$sql); 
		      		$row = $result->fetch_array();  
		
				$gameID = $row['gameID'] + 1;
				
				//Query to add game to DB with new gameID
		  		$sql = "INSERT INTO Games (`gameID`, `numOfPlayers`, `currentPlayerTurn`, `sourceCol`, `sourceRow`, `destCol`, `destRow`) 
		  		VALUES ('".$gameID."', '1', '1', '0', '0', '0', '0')";
		  		
		  		
		  		if ($conn->query($sql) === TRUE) 
		  		{		  		
		  			//Return max gameID and positive status
		  			if($_POST['source']  == "PC")
		  			{
		  			
						echo 
							"{
								status: 1,
								gameID: '".$gameID."'
							}";
					}
					else
					{
						echo "{1," . $gameID . ",1,0,0,0,0,0}";
					
					}
				} else {
				    	echo 
						"{
							status: -1
						}";
				}
				
        		}
        		elseif($_POST['Request'] =="deleteGame")
        		{
        			$sql = "DELETE FROM Games WHERE gameID = '".$_POST['gameID']."'";
        			if($conn->query($sql) === TRUE)
        			{
        				if($_POST['source'] == "PC"){
        				
        					echo "Game Deleted";
        				}
        				else
        				{
        				//all we are about is status being 1 to know that the game was deleted. the return parameters are status,gameID,numOfPlayers,currentPlayerTurn,sourceCol,sourceRow,destCol,destRow)
        					echo "{1,0,0,0,0,0,0,0}";
        				}
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
						currentPlayerTurn = 1,
						sourceCol = 0, 
						sourceRow = 0,
						destCol = 0,
						destRow = 0
					WHERE 
						gameID = '".$_POST['gameID']."'";
						
				if($conn->query($sql) === TRUE)
        			{
        				if($_POST['source'] == "PC")
        				{
	        				$gameStatusString = 
			  			"{
			  				status: 1,
			  				gameID : '".$_POST['gameID']."',
			  				numOfPlayers : 2,
			  				currentPlayerTurn: 1,
			  				sourceCol: 0,
			  				sourceRow: 0,
			  				destCol: 0,
			  				destRow: 0
			  			 }";
			  			
						  
        				}
        				else
        				{
        					$gameStatusString = "{1," . $_POST['gameID'] . ",2,1,0,0,0,0}";
        					
        				}
        			
		  			
					echo $gameStatusString;   

        			}else{
        				if($_POST['source'] == "PC")
        				{
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
        				}
        				else
        				{
        					$gameStatusString = "{-1," . $_POST['gameID'] . ",1,0,0,0,0,0}";
        				}
		  			
					echo $gameStatusString;   
        			}
        		}
        	}
        }


?>