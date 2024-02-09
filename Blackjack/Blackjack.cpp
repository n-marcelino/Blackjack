#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

struct card
{
    string suite;
    string name;
    int position;
    int value;
};

card cards[52];
vector<int> cards_at_play;

struct hand
{
    vector<card> cards;
    int points;
    int bet;
};

struct players
{
    string name;
    int chips = 25;
    hand hands[2];

} dealer, player;

bool end_game = false;
bool round_start = false;
bool turn_end = false;
int total_bet = 0;

void title_screen();
void start_menu();

void new_round();
void reset_values();
void bet(players& p, players& e, int chips_bet, int hand);
void bet_display();
void distribute_chips(players& p, int blackjack_hand);

void open_deck();
void shuffle_deck();

void deal(players& p);
void split_hand(players& p);
void set_ace_value(players& p, int hand, card& c);
void hit(players& p, int hand);

void display_hand(players& p, int hand);
void turn_menu(int hand);

int main()
{
    while (true)
    {
        srand(time(NULL));
        title_screen();
        start_menu();

        if (end_game == true)
        {
            break;
        }
        int input = 0;
        dealer.chips = 100;
        while (true)
        {
            cout << "================================" << endl
                << "[[ " << dealer.name << " has " << dealer.chips << " chips. ]]" << endl
                << "[[ " << player.name << " has " << player.chips << " chips. ]]" << endl
                << "================================" << endl;

            while (round_start == true)
            {
                if (dealer.chips < 50)
                {
                    cout << "[[ Refilling the Dealer's chips...... ]]" << endl;
                    dealer.chips += 50;
                    system("pause");
                    cout << "[[ DEALER now has " << dealer.chips << "chips! ]]" << endl;
                }
                new_round();

                cout << endl;

                cout << "================================" << endl
                    << "IT IS CURRENTLY " << player.name << "'S TURN." << endl
                    << "================================" << endl;
                cout << "[[ The DEALER deals you two cards! ]]" << endl
                    << endl;
                deal(player);
                display_hand(player, 0);
                int num_of_hands = 1;
                if (player.hands[0].cards.at(0).value == player.hands[0].cards.at(1).value || player.hands[0].cards.at(0).name.compare(player.hands[0].cards.at(1).name) == 0)
                {
                    while (true)
                    {
                        cout << "[ DEALER ] You have an equal value pair! Do you want to split hands?" << endl
                            << "[1] Yes" << endl
                            << "[2] No" << endl
                            << "INPUT: ";
                        cin >> input;

                        if (input == 1)
                        {
                            cout << endl
                                << "[[ Hand has been split! The initial bet doubles! ]]" << endl;
                            split_hand(player);
                            bet_display();
                            num_of_hands++;
                            break;
                        }
                        else if (input == 2)
                        {
                            cout << endl
                                << "[[ You have chosen to not split your hand! ]]" << endl;
                            break;
                        }
                        else
                        {
                            cout << endl
                                << "**ERROR: INVALID INPUT**" << endl
                                << "**Please enter only 1 or 2**" << endl
                                << endl;
                            continue;
                        }
                    }
                }

                for (int i = 0; i < num_of_hands; i++)
                {
                    // turn 1:
                    if (num_of_hands > 1)
                    {
                        cout << endl;
                        cout << "================================" << endl
                            << player.name << "'S HAND # " << i + 1 << endl
                            << "================================" << endl;
                    }

                    turn_end = false;
                    int turn = 0;

                    while (turn_end == false)
                    {
                        if (turn != 0 || num_of_hands > 1)
                        { // display only after first turn or after split hand menu
                            display_hand(player, i);
                        }

                        if (player.hands[i].points > 21)
                        {
                            cout << "================================" << endl
                                << "<<  BUST! Your hand has lost! >>" << endl
                                << "================================" << endl
                                << endl;
                            distribute_chips(dealer, 0); // the dealer automatically wins if player gets a bust

                            round_start = false;
                            turn_end = true;
                            break;
                        }
                        else if (player.hands[i].points == 21)
                        {
                            cout << "================================" << endl
                                << "<< BLACKJACK! Your hand wins! >>" << endl
                                << "================================" << endl
                                << endl;
                            distribute_chips(player, i);

                            turn_end = true;
                            break;
                        }

                        turn_menu(i);
                        turn++;
                    }

                    if (num_of_hands == 1 && (player.hands[0].points == 21 || player.hands[0].points > 21))
                    {
                        // checks if there is no split hand
                        // checks if player got a blackjack/bust
                        break;
                    }

                    // THE DEALER'S TURN
                    cout << endl;
                    cout << "================================" << endl
                        << "IT IS CURRENTLY " << dealer.name << "'S TURN." << endl
                        << "================================" << endl;

                    deal(dealer);

                    cout << "[[ The DEALER deals himself two cards! ]]" << endl;
                    display_hand(dealer, 0);

                    turn_end = false;
                    turn = 0;

                    while (turn_end == false)
                    {
                        if (turn != 0 || num_of_hands > 1)
                        { // display only after first turn or after split hand menu
                            display_hand(dealer, i);
                        }

                        if (dealer.hands[i].points > 21)
                        {
                            cout << "================================" << endl
                                << "<<   BUST! The DEALER Loses!  >>" << endl
                                << "================================" << endl
                                << endl;
                            distribute_chips(player, 0);
                            cout << endl;
                            turn_end = true;
                            break;
                        }
                        else if (dealer.hands[i].points == 21)
                        {
                            cout << "================================" << endl
                                << "<< BLACKJACK! The DEALER Wins! >>" << endl
                                << "================================" << endl
                                << endl;
                            distribute_chips(dealer, i);
                            cout << endl;
                            turn_end = true;
                            break;
                        }

                        if (dealer.hands[i].points <= player.hands[i].points)
                        {
                            hit(dealer, i);
                            cout << "[[ The DEALER chose to HIT and drew another card! ]]" << endl;
                            turn++;
                            system("pause");
                            continue;
                        }
                        else
                        {
                            if (!(dealer.hands[i].points > 21) || dealer.hands[i].points != 21)
                            {
                                cout << "[[ The DEALER chose to STAND and ended their turn on " << dealer.hands[i].points << " points! ]]" << endl;

                                if (dealer.hands[i].points > player.hands[i].points)
                                {
                                    cout << "=======================================================" << endl
                                        << "<< The DEALER is closer to BLACKJACK than the PLAYER! >>" << endl
                                        << "========================================================" << endl
                                        << endl;
                                    distribute_chips(dealer, 0);
                                    turn_end = true;
                                }
                                else if (dealer.hands[i].points < player.hands[i].points)
                                {
                                    cout << "=======================================================" << endl
                                        << "<< The PLAYER is closer to BLACKJACK than the DEALER! >>" << endl
                                        << "========================================================" << endl
                                        << endl;
                                    distribute_chips(player, 0);
                                    turn_end = true;
                                }

                                turn_end == true;
                                break;
                            }
                        }
                    }
                }

                round_start = false;
                break;
            }

            input = 0;

            while (player.chips != 0 && dealer.chips != 0)
            {
                cout << endl;
                cout << "=================================" << endl
                    << "<< ROUND HAS ENDED! >>" << endl
                    << "[1] Play Again (Restart the Game) " << endl
                    << "[2] Continue Playing" << endl
                    << "[3] Quit " << endl
                    << "=================================" << endl;
                cout << "INPUT: ";
                cin >> input;

                if (input == 1 || input == 2 || input == 3)
                {
                    break;
                }
                else
                {
                    cout << endl
                        << "**ERROR: INVALID INPUT**" << endl
                        << "**Please enter only 1, 2, or 3**" << endl
                        << endl;
                    continue;
                }
            }

            if (input == 1)
            {
                cout << endl
                    << "[[ You have decided to restart the game! ]]" << endl;
                cout << endl;
                system("pause");
                cout << endl;
                round_start = true;
                break;
            }
            else if (input == 2)
            {
                cout << endl
                    << "[ DEALER ] Great!! Let's continue playing!" << endl;
                cout << endl;
                system("pause");
                cout << endl;
                round_start = true;
                continue;
            }
            else if (input == 3)
            {
                cout << endl
                    << "[ DEALER ] Good game! Till next time! " << endl;
                cout << endl;
                round_start = false;
                break;
            }

            break;
        }

        if (input == 1)
        {
            continue;
        }

        cout << " ================================" << endl
            << "||        GAME HAS ENDED!       ||" << endl
            << " ================================" << endl;

        cout << endl;

        if (input == 3) {
            break;
        }
    }

    return 0;
}

void title_screen()
{
    cout << ">=================================================<" << endl;
    cout << "||             LET'S PLAY BLACKJACK!             ||" << endl;
    cout << "||\t _____                                   ||" << endl;
    cout << "||\t|B .  | _____                            ||" << endl;
    cout << "||\t| /.\\ ||L ^  | _____                     ||" << endl;
    cout << "||\t|(_._)|| / \\ ||A _  | _____              ||" << endl;
    cout << "||\t|  |  || \\ / || ( ) ||C_ _ | _____       ||" << endl;
    cout << "||\t|____J||  .  ||(_'_)||( v )||K .  |      ||" << endl;
    cout << "||\t       |____A||  |  || \\ / || /.\\ |      ||" << endl;
    cout << "||\t              |____C||  .  ||(_._)|      ||" << endl;
    cout << "||\t                     |____K||  |  |      ||" << endl;
    cout << "||\t                            |____*|      ||" << endl;
    cout << "||                                               ||" << endl;
    cout << ">=================================================<" << endl;

    cout << endl;
}

void start_menu()
{
    cout << "First of all, what is your name?" << endl
        << "INPUT: ";
    getline(cin, player.name);
    dealer.name = "DEALER";

    while (true)
    {
        cout << endl
            << "<< WELCOME "
            << player.name
            << " TO THE BLACKJACK TABLE! >>" << endl
            << endl;

        cout << "[1] PLAY" << endl
            << "[2] HOW TO PLAY" << endl
            << "[3] QUIT" << endl
            << endl;

        int choice;
        cout << "INPUT: ";
        cin >> choice;

        if (choice == 1)
        {
            round_start = true;

            cout << endl
                << "<< DEALER >> Let\'s have a good game today!!" << endl
                << endl;
            open_deck();
            break;
        }
        else if (choice == 2)
        {
            cout << "HOW TO PLAY BLACKJACK? " << endl
                << "The goal of BLACKJACK is to have a hand with a value closer to 21!" << endl
                << "Exactly 21 is a BLACKJACK (win!), but over 21 is a BUST (lose!)!" << endl
                << endl
                << "The number cards (1-10) have a value equal to themselves, " << endl
                << "The face cards (King, Queen, Jack) have a value of 10," << endl
                << "And the ACE has a value of either 11 or 1!" << endl
                << "Its value would be whichever would bring the player closer to 21 without getting a BUST!" << endl
                << endl
                << "Players are dealt two cards at the start of their turn." << endl
                << "They can choose to take a HIT and draw more cards" << endl
                << "Or STAND and give up their turn." << endl
                << endl
                << "If the player's initial hand is an equal value pair, they can choose to split it." << endl
                << "They will then be playing normally but with two hands." << endl
                << endl
                << "At the start of every round, the player will need to bet a certain amount of chips." << endl
                << "If the player is playing with a split hand, the initial bet will be doubled." << endl
                << "If the player wins, the dealer will return double of their bet, but" << endl
                << "If the player loses, the dealer will keep their bet." << endl
                << endl
                << "The ROUND will end when: " << endl
                << "\t1) A player gets a BLACKJACK and immediately wins," << endl
                << "\t2) A player gets a BUST and immediately loses." << endl
                << "\t3) A player takes a STAND " << endl
                << "\t\t3a) and wins by having a hand value closer to 21 than the opponent's." << endl
                << "\t\t3b) and loses by having a value farther from 21 than the opponent's. " << endl
                << "\t4) A player runs out of chips." << endl
                << endl;
        }
        else if (choice == 3)
        {
            cout << endl
                << "<< DEALER >> Quitting already? Let\'s play again next time!" << endl;
            end_game = true;
            break;
        }
        else
        {
            cout << endl
                << "**ERROR: Invalid Input**" << endl
                << "**Please input only 1, 2, or 3!**" << endl
                << endl;
        }
    }
}

void new_round()
{
    reset_values();
    int input;

    cout << " ================================" << endl
        << "||  A NEW ROUND IS STARTING.... ||" << endl
        << " ================================" << endl;

    open_deck();
    shuffle_deck();

    cout << "[[ The DEALER opens a new pack of cards and shuffles it! ]]" << endl
        << endl;

    while (true)
    {
        input = 0;
        cout << "[ DEALER ] How much chips do you want to bet this round?" << endl
            << "You currently have " << player.chips << " chips." << endl
            << "INPUT: ";
        cin >> input;

        if (input <= 0)
        {
            cout << endl
                << "**ERROR: INVALID INPUT**" << endl
                << "**Please enter a number greater than 0**" << endl
                << endl;
            continue;
        }

        if (input > player.chips)
        {
            cout << endl
                << "**ERROR: INVALID INPUT**" << endl
                << "**You don't have enough chips!**" << endl
                << endl;
            continue;
        }

        if (input > dealer.chips)
        {
            cout << endl
                << "**ERROR: Unable to Match Bet**" << endl
                << "**" << dealer.name << " does not have enough chips!**" << endl
                << endl;
            continue;
        }

        break;
    }

    cout << endl
        << "The PLAYER has bet [" << input << " CHIPS]! The DEALER matches your bet!" << endl
        << endl;

    bet(player, dealer, input, 0);
    bet(dealer, player, input, 0);
    bet_display();
}

void reset_values()
{
    total_bet = 0;
    cards_at_play.clear();

    for (int i = 0; i < 2; i++)
    {
        dealer.hands[i].cards.clear();
        dealer.hands[i].points = 0;
        dealer.hands[i].bet = 0;

        player.hands[i].cards.clear();
        player.hands[i].points = 0;
        player.hands[i].bet = 0;
    }
}

void bet(players& p, players& e, int chips_bet, int hand)
{
    p.chips -= chips_bet;          // moves chips away from player
    p.hands[hand].bet = chips_bet; // attaches value of bet on hand
    total_bet += chips_bet;        // moves chips to the betting pool
}

void bet_display()
{
    cout << "[[ THE TOTAL BET IS NOW: " << total_bet << " CHIPS ]]" << endl;
    cout << "[[ " << player.name << " now has " << player.chips << " chips! ]]" << endl;
    cout << "[[ " << dealer.name << " now has " << dealer.chips << " chips! ]]" << endl;
}

void distribute_chips(players& p, int blackjack_hand)
{
    int chips_won = (p.hands[blackjack_hand].bet) * 2;

    cout << "[[ " << chips_won << " CHIPS goes to " << p.name << " ]]"
        << endl;

    total_bet -= chips_won;
    p.chips += chips_won;
    bet_display();
}

void open_deck()
{
    string const suite_names[] = { "CLUBS", "DIAMONDS", "HEARTS", "SPADES" };
    string const suite_cards[] = { "ACE", "2", "3", "4", "5",
                                  "6", "7", "8", "9", "10",
                                  "JACK", "QUEEN", "KING" };

    int count = 0;
    for (int s = 0; s < 4; s++) // suite
    {
        for (int c = 0; c < 13; c++) // cards
        {

            cards[count].suite = suite_names[s]; // setting the card's suite
            cards[count].name = suite_cards[c];  // setting the card's name
            cards[count].position = count + 1;

            if (c >= 1 && c <= 9) // sets the values of the number cards
            {
                cards[count].value = c + 1;
            }
            else if (c >= 10 && c <= 12) // sets the values of the face cards
            {
                cards[count].value = 10;
            }
            else
            { // sets the value of ace
                cards[count].value = 11;
            }

            count++;
        }
    }
}

void shuffle_deck()
{
    for (int i = 0; i < 52; i++)
    {
        int r = i + (rand() % 52 - i);

        swap(cards[i], cards[r]);
    }
}

void deal(players& p)
{
    // generate random number between 1 and 52

    hand* hands = p.hands;

    for (int i = 0; i < 2; i++)
    {
        int num;
        while (true)
        {
            num = (rand() % 52) + 1;

            bool card_already_at_play = false;
            for (int j = cards_at_play.size(); j > 0; j--)
            {
                if (cards[num].position == cards_at_play.at(j - 1))
                {
                    card_already_at_play = true;
                }
            }
            if (card_already_at_play == true)
            {
                continue;
            }
            break;
        }

        cards_at_play.push_back(cards[num].position); // marks the card dealt

        hands[0].cards.push_back(cards[num]); // puts the card into a player's hand
        set_ace_value(p, 0, hands[0].cards.at(i));
        hands[0].points = hands[0].points + cards[num].value;
    }
}

void split_hand(players& p)
{

    hand* hands = p.hands;

    int initial_bet = p.hands[0].bet;

    hands[0].points -= hands[0].cards.at(1).value;  // subtracts the value of the card to be moved
    hands[1].cards.push_back(hands[0].cards.at(1)); // puts the second card into split_hand
    set_ace_value(p, 0, hands[1].cards.at(0));      // checks if card2 is an ace
    hands[1].points += hands[1].cards.at(0).value;  // subtracts the value of the card to be moved
    hands[0].cards.erase(hands[0].cards.end());     // removes second card of initial hand

    bet(player, dealer, initial_bet, 1);
    bet(dealer, player, initial_bet, 1);
};

void set_ace_value(players& p, int hand, card& c)
{
    if (c.name.compare("ACE") == 0)
    {
        if ((p.hands[hand].points + c.value) <= 21)
        {
            c.value = 11;
        }
        else
        {
            c.value = 1;
        }
    }
}

void hit(players& p, int hand)
{
    struct hand* hands = p.hands;

    int num;
    while (true)
    {
        num = (rand() % 52) + 1;

        bool card_already_at_play = false;
        for (int j = cards_at_play.size(); j > 0; j--)
        {
            if (cards[num].position == cards_at_play.at(j - 1))
            {
                card_already_at_play = true;
            }
        }
        if (card_already_at_play == true)
        {
            continue;
        }

        break;
    }

    cards_at_play.push_back(cards[num].position);               // marks card as being played
    set_ace_value(p, hand, cards[num]);                         // checks for ace
    hands[hand].cards.push_back(cards[num]);                    // adds card to current hand
    hands[hand].points = hands[hand].points + cards[num].value; // counts current hand's points
}

void display_hand(players& p, int hand)
{

    struct hand* hands = p.hands;

    cout << "<< " << p.name << "\'s HAND # " << hand + 1 << " >>" << endl;
    for (int i = 0; i < hands[hand].cards.size(); i++)
    {
        cout << i + 1 << ": "
            << hands[hand].cards.at(i).name
            << " of "
            << hands[hand].cards.at(i).suite
            << " ["
            << hands[hand].cards.at(i).value
            << "]"
            << endl;
    }
    cout << "<< TOTAL POINTS: " << hands[hand].points << " >>" << endl;

    cout << endl;
};

void turn_menu(int hand)
{
    while (true)
    {
        int input = 0;
        cout << "[ DEALER ] What would you like to do?" << endl
            << "[1] Hit" << endl
            << "[2] Stand" << endl
            << "INPUT: ";
        cin >> input;

        if (input == 1)
        {
            cout << endl
                << "[[ HIT: You have been dealt another card! ]]" << endl;
            hit(player, hand);
            break;
        }
        else if (input == 2)
        {
            cout << endl
                << "[[ STAND: You have ended your turn on " << player.hands[hand].points << " points! ]]" << endl;
            cout << endl;
            turn_end = true;
            break;
        }
        else
        {
            cout << endl
                << "**ERROR: INVALID INPUT**" << endl
                << "**Please enter only 1 or 2**" << endl
                << endl;
            continue;
        }
    }
}