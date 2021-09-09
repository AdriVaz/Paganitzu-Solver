RM = rm -rf

all:
	g++ AstarPaganitzu.cpp -o AstarPaganitzu -Wall -std=c++11 -Wno-deprecated -Werror -pedantic -pedantic-errors

clean:
	$(RM) *.o
	$(RM) AstarPaganitzu
	$(RM) maps/*.output
	$(RM) maps/*.statistics
