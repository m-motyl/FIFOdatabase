# Database with client program using FIFO 

Database containing structures [ID, item], where ID is an integer and item is a string. 
The database is created and statically filled by the server program. The server and clients communicate using FIFO queues.
The server receives from its input queue (common to all clients) a query containing the ID and path to the client's queue 
(each client has its own queue) where to insert the response. The server sends back to the client's queue the name 
corresponding to the identifier sent or the message "None" if there is no such ID in the database.
The server works in an endless loop, the client only once (question - answer).

The server program ensure SIGHUP (terminal disconnected) and SIGTERM signals - prints ignore announcement. SIGUSR1 signal will turn 
off the server work.

## remarks
* run program in separate terminals
* run server program first
