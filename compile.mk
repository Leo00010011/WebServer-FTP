web_server: web_server.o server_tools.o http_tools.o fd_tools.o structures.o sortbyname sortbysize sortbytimem
	gcc -o web_server web_server.o server_tools.o http_tools.o fd_tools.o structures.o -lm

structures.o: structures.c
	gcc -c structures.c

web_server.o: web_server.c
	gcc -c web_server.c

http_tools.o: http_tools.c 
	gcc -c http_tools.c 

fd_tools.o: fd_tools.c
	gcc -c fd_tools.c

server_tools.o: server_tools.c
	gcc -c server_tools.c

sortbyname: sortbyname.o genhtml.o structures.o http_tools.o sorthtml.o fd_tools.o
	gcc -o sortbyname sortbyname.o structures.o http_tools.o sorthtml.o genhtml.o fd_tools.o -lm 

sortbysize: sortbysize.o genhtml.o structures.o http_tools.o sorthtml.o
	gcc -o sortbysize sortbysize.o structures.o http_tools.o sorthtml.o genhtml.o fd_tools.o -lm
	
sortbytimem: sortbytimem.o genhtml.o structures.o http_tools.o sorthtml.o
	gcc -o sortbytimem sortbytimem.o structures.o http_tools.o sorthtml.o genhtml.o fd_tools.o -lm

sortbyname.o: sortbyname.c 
	gcc -c sortbyname.c

sortbysize.o: sortbysize.c
	gcc -c sortbysize.c

sortbytimem.o: sortbytimem.c
	gcc -c sortbytimem.c

sorthtml.o: sorthtml.c
	gcc -c sorthtml.c

genhtml.o: genhtml.c
	gcc -c genhtml.c


