# NYU Tandon School of Engineering Senior Design Project

### Group Members
- Mahmoud Abugharbieh   - mka299@nyu.edu
- John Vasquez          - jv1065@nyu.edu
- Andreina Vivas        - av1065@nyu.edu


#### Resources
  Checkers piece images in this project were not created, nor do they belong to any member of the group.  The checkers 
  piece images are the work of user **OffbeatDesign** on http://designoutpost.com.
  
  Direct link to thread where the images were found: http://www.designoutpost.com/forums/showthread.php?t=14555
  
  
### Objective
The objective of this project is to create a physical Checkers board with Wi-Fi capability that can connect to another
remote board.  The idea behind this is to create a platform where users can play with actual people regardless of not having
anyone present to play with, while maintaining the feeling of moving the physical pieces as opposed to using a mouse to click
and move a piece.  This project will be done on a half of a checkers board - a 4x8 board instead of an 8x8 due to budget constraints.
Additionally, the second board will be virtualized due to our budget as well. In other words, a move on the physical board will update
a move on a virtualized game of checkers, and the move on the virtualized game of checkers will cause a piece on the physical board
to move.

### Approach
The pieces on each board will be actuated by having magnets in each piece, and having an X, Y, Z motor system with a magnet
attached to it, which will be used to grab and move the piece.  Reed Switches will be used as our sensors, and our software 
will use the reed switches to determine what move is made by the player.  All the board logic will be uploaded to an arduino mega, and
the virtualized board will be created as a C# application.
    
