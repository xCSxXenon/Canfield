/*********************************************************************
  Author:        Dana Vrajitoru, IUSB, CS
  Class:         C243 Data Structures
  File name:     canfield.cc
  Last updated:  September 2019
  Description:   Implementation of the card game of Canfield using 
                 stacks and queues.
**********************************************************************/

#include "Canfield.h"
#include "general.h"
#include <iostream>
#include <iomanip>
using namespace std;
#include <cstdlib>

// Default constructor
Canfield::Canfield()
    : deck(), reserve() // calling default constructors
{
    waste  = -1;        // non existing card, empty marker
    quit   = false;
    from   = '\0'; 
    to     = '\0';
    fromId = 0; 
    toId   = 0;
    score  = 0;
} // Canfield() 

// Destructor
Canfield::~Canfield()
{
    emptyAll();
} // ~Canfield()

// Empty all the data structures
void Canfield::emptyAll()
{
    int i;
    deck.makeEmpty();
    reserve.makeEmpty();
    for (i = 0; i < NUM_SUITS; i++) 
    {
        foundation[i].makeEmpty();
        tableau[i].makeEmpty();
    }
} // Canfield::emptyAll()

// The main loop of the game.
void Canfield::play()
{
    char answer;
    newGame();
    printGame();
    while (!quit) 
    {
        printMenu();
        cin >> answer;
        performAction(answer);
    }
} // canfield()

//////////////////////// New game ///////////////////////////////////////////

// Starting a new game. It must reset all the data structures,
// generate a new set of 52 random cards and place them in the
// appropriate places in the game.
void Canfield::newGame()
{
    int i, card;
    int cardsLeft = NUM_CARDS;
    int newDeck[NUM_CARDS];

    // Empty all the data structures
    emptyAll();

    // Create a new deck and shuffle it
    newShuffledPack(newDeck);

    // Now place the cards in the data structures
    for (i = 0; i < NUM_RANKS; i++)  // fill in the reserve with 13 cards
    {
        card = newDeck[--cardsLeft]; // use a card and decrease the total count
        reserve.push(card);
    }
    for (i = 0; i < NUM_SUITS; i++)  // draw a card for each tableau
    {
        card = newDeck[--cardsLeft]; // use a card and decrease the total count
        tableau[i].push(card);
    }
    for (i = 0; i < NUM_SUITS; i++)  // Initialize the foundations with 0
        foundation[i].push(0);
    waste = newDeck[--cardsLeft];    // the card to play
    while (cardsLeft > 0)            // the remaining cards go into the deck
    {
        card = newDeck[--cardsLeft];
        deck.enqueue(card);
    }
} // Canfield::newGame()

// Create a new pack of cards and shuffles them
void Canfield::newShuffledPack(int *newDeck)
{
    int i, j;
    for (i = 0; i < NUM_CARDS; i++)
        newDeck[i] = 1 + i % NUM_RANKS; // add the cards in order

    for (i = 0; i < NUM_CARDS-1; i++)   // then shuffle them
    {
        j = i + rand() % (NUM_CARDS-i); // random other card
        swap(newDeck[i], newDeck[j]);   // swap them
    }
} // Canfield::newShuffledPack()

///////////////////// Interface ///////////////////////////////////////////

// Prints the choices of actions for the player.
void Canfield::printMenu()
{
    cout << "[q] quit [N] new game [m] move [d] deal a card [r] repeat move" 
         << endl;
    cout << "Enter your choice: ";
} // Canfield::printMenu()

// Performs an action based on the user's choice.
void Canfield::performAction(char answer)
{
    switch (answer) 
    {
    case 'q':
    case 'Q':
        quit = true;
    break;
    //case 'n': this causes problems because it's next to m
    case 'N':
        newGame();
        break;
    case 'm':
    case 'M':
        move();
    break;
    case 'd':
    case 'D':
        deal();
    break;
    case 'r':
        repeatMove();
    break;
    }
    if (!quit)
        printGame();
} // Canfield::performAction()

// Displays the entire content of the game.
void Canfield::printGame()
{
    clearScreen();
    cout << "Deck:    ";
    if (waste != -1)
        cout << waste;
    cout << endl << endl
         << "Reserve: ";
    if (!reserve.isEmpty())
        cout << reserve.top();
    cout << endl << endl;
    cout << "Score: " << getScore();
    cout << endl << endl;
    for (int i = 0; i < NUM_SUITS; i++) 
    {
        cout << "F" << i << "      ";
        if (foundation[i].top() > 0)
            cout << setw(2) << foundation[i].top() << "   ";
        else
            cout << "     ";
        cout << "T" << i << ' ';
        tableau[i].print();
        cout << endl << endl;
    }
    cout << endl << endl;
} // Canfield::printGame()

/////////////////////////// Menu actions ////////////////////////////////////

// Deals one card from the deck.
void Canfield::deal()
{
    if (!deck.isEmpty()) 
    {
        deck.enqueue(waste);
        waste = deck.dequeue();
    }
} // Canfield::deal()

// Performs one move.
void Canfield::move()
{
    int card;

    // Input the move information from the user
    cout << "From:" << endl
         << "[t] top of the tableau [T] whole tableau [d] deck [r] reserve : ";
    cin >> from;
    if (from == 't' || from == 'T') 
    {
        cout << "number : ";
        getNumber(fromId, NUM_SUITS);
    }
    cout << "To: [t] tableau [f] foundation : ";
    cin >> to;
    cout << "number : ";
    getNumber(toId, NUM_SUITS);

    // make the move
    card = getCard(); // card to be moved
    cout << "card to move: " << card << endl;
    if (card != -1 && checkMove(card)) // is the move possible?
    {
        removeCard();
        addCard(card);
        checkWin();
        if (tableau[fromId].isEmpty())
	{
	    if (!reserve.isEmpty())
	    {
	        from = 'r';
	        to = 't';
                card = getCard();
                if (card != -1 && checkMove(card))
                {
	            removeCard();
                    addCard(card);
		}
	    }
	}
    }
    else 
    {
        cout << "Movement not permitted. Strike any key to continue."; 
        cin.get();
        while (cin.get() != '\n') ; // Clear the line
    }
} // Canfield::move()

//////////////////////////////////////////////////////////////////////
// Functions that perform the actions for a move, in this order:
// get the card to move, check if it can be moved, 
// remove it from the old structure, add it to the new one
//////////////////////////////////////////////////////////////////////

// Determines what card is to be moved. Accessor method.
int Canfield::getCard()
{
    switch (from) 
    {
    case 'd':
    case 'D':
        return waste;
    case 'r':
    case 'R':
        if (!reserve.isEmpty())
            return reserve.top();
        else
            return -1;
    case 't':
        if (!tableau[fromId].isEmpty())
            return tableau[fromId].top();
        else
            return -1;
    case 'T':
        if (!tableau[fromId].isEmpty())
            return tableau[fromId].bottom();
        else
            return -1;
    default:
        return -1;
    }  
} // Canfield::getCard()

// Checks if the requested movement is possible. Accessor method.
bool Canfield::checkMove(int card)
{
    if (to == 't' || to == 'T') 
        if (tableau[toId].isEmpty()) // one can always move anything to an
                                     // empty tableau
            return true;
        else
            return (tableau[toId].top() == card+1 || 
                    tableau[toId].top() == 1 && card == NUM_RANKS) ;

	// One can place a card on the top of the tableau if it's one
	// card smaller than the top of that tableau. One can also
	// place a 13 over a 1.
    else if (to == 'f' || to == 'F') 
    {
        if (from == 'T' && tableau[fromId].stored() != 1)
            return false;
        return (foundation[toId].top() == card-1);
    }
    return false; // Anything else is not permitted.
} // Canfield::checkMove()

// Removes the card to be moved. Mutator method.
void Canfield::removeCard()
{
    int error=0;
    switch (from) 
    {
    case 'd':
    case 'D':
        if (!deck.isEmpty())
            waste=deck.dequeue();
        else
            waste=-1;
        break;
    case 'r':
    case 'R':
        if (!reserve.isEmpty())
            reserve.pop();
        else 
            error = 1;
        break;
    case 't':
        if (!tableau[fromId].isEmpty()) 
            tableau[fromId].pop();
        else 
            error = 2;
        break;
    case 'T':
        if (!tableau[fromId].isEmpty())
            ;
        else 
            error = 3;
        break;
    }
    if (error) 
    {
        cout << "Error in the code: " << error << endl;
        exit(1);
    }
} // Canfield::removeCard()

// Adds the card or the entire tableau to its new destination. 
// Mutator method.
void Canfield::addCard(int card)
{
    if (from == 'T' && (to == 't' || to == 'T')) 
    {
        // add the stack onto which we move at the end of the one we move.
        tableau[fromId].concatenate(tableau[toId]);
        // move that stack back into the destination stack.
        tableau[toId].concatenate(tableau[fromId]);
    }
    else 
        if (to == 't' || to == 'T') 
            tableau[toId].push(card);
        else 
	{
            foundation[toId].push(card);
            addScore();
        }
} // Canfield::addCard()

void Canfield::repeatMove()
{
    int card = getCard();
    cout << "card to move: " << card << endl;
    if (card != -1 && checkMove(card)) // is the move possible?
    {
        removeCard();
        addCard(card);
    }
    else
    {
        cout << "Movement not permitted. Strike any key to continue.";
        cin.get();
        while (cin.get() != '\n') ; // Clear the line
    }
} // Canfield::repeatMove()

void Canfield::printWin()
{
    cout << "You won!" << endl;
    exit;
} // Canfield::printWin()

void Canfield::addScore()
{
    this->score += 1;
} //Canfield::addScore

void Canfield::checkWin()
{
    if (this->tableau[0].stored() == 13)
        if (this->tableau[1].stored() == 13)
            if (this->tableau[2].stored() == 13)
                if (this->tableau[3].stored() ==13)
                    printWin();
} // Canfield::checkWin()

int Canfield::getScore()
{
  return this->score;
} // Canfield::getScore;
