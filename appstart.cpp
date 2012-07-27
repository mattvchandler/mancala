//appstart.cpp
//main routines for mancala game
//Copyright Matthew Chandler 2012

#include <iostream>

#include "mancala.h"

int main()
{
    srand(time(0));
    Board b;
    //b.bowls=std::vector<Bowl>({Bowl(1,1,12), Bowl(0,2,11), Bowl(0,3,10), Bowl(0,4,9), Bowl(2,5,8), Bowl(1,6,7), Bowl(0,7,0), Bowl(0,8,5), Bowl(0,9,4), Bowl(0,10,3), Bowl(0,11,2), Bowl(0,12,1), Bowl(1,13,0), Bowl(0,0,0)});
    char nextmove = '\0';
    int player = 1;
    while(std::cin && !b.finished() && nextmove != 'q' && nextmove != 'Q')
    {
        std::cout<<"Player "<<player<<std::endl;
        b.crapprint();
        std::cout<<"Best move: "<<choosemove(b)<<std::endl;
        std::cout<<"Next move: ";
        std::cin>>nextmove;
        std::cout<<std::endl;
        if(nextmove == 'S' || nextmove == 's')
        {
            b.swapsides();
            player = (player == 1) ? 2 : 1;
        }
        if(nextmove >= '0' && nextmove <= '5')
        {
            if(!b.move(nextmove - '0'))
            {
                b.swapsides();
                player = (player == 1) ? 2 : 1;
            }
        }
    }
    if(b.finished())
    {
        std::cout<<"Player "<<player<<std::endl;
        b.crapprint();
        if(b.bowls[b.p1_store].count == b.bowls[b.p2_store].count)
            std::cout<<"Tie"<<std::endl;
        else if(player == 1)
            if(b.bowls[b.p1_store].count > b.bowls[b.p2_store].count)
                std::cout<<"Player 1 wins"<<std::endl;
            else
                std::cout<<"Player 2 wins"<<std::endl;
        else
            if(b.bowls[b.p1_store].count > b.bowls[b.p2_store].count)
                std::cout<<"Player 2 wins"<<std::endl;
            else
                std::cout<<"Player 1 wins"<<std::endl;
        if(abs(b.bowls[b.p1_store].count - b.bowls[b.p2_store].count) >= 10)
            std::cout<<"FATALITY"<<std::endl;
    }
    return 0;
}
