# :dollar: **Monopoly: For Ages 6 and Under and 85 and Over** :dollar:
Systems Project 02: The Final Frontier
Brian Leung, Kristin Lin, Sabrina Wen | Pd. 10

#### Broad Description ####
Wotcher Mr. DW (and friends)! Welcome to a special edition of *Monopoly: For Ages 6 and Under and 85 and Over*! The objective of this game is similar to regular Monopoly: you will work against 3 other players to buy property (or not!), and compete to see who can survive the longest in a ruthless capitalist world. We have also decided to set this game in the UK (for aesthetic reasons).

#### Libraries Required (and how to install them!)####
1. libsdl2-dev `sudo apt-get install libsdl2-dev`

2. libsdl-image1.2 `sudo apt-get install libsdl-image1.2-dev`
3. libsdl2-2.0 `sudo apt-get install libsdl2-2.0`

#### How to Play: ####
1. Clone this repository and cd into it (turbo-system).
1. Install above libraries.
2. In your terminal, type in the following:
```C
make
./server
```
3. Make four new terminal windows or tabs, and making sure you are in this directory (turbo-system), type in `./client`. Our game requires exactly four players/clients.
4. The computer will choose a random player/client to start, and "roll" the dice for you. You will move 1 to 12 spaces from your starting position and start off with $1500. Possible spaces you can land on include:
  1. **Property/Railroad space**
      * This is the most common space. The terminal will ask you if you want to purchase a property and how much rent you stand to gain from it. Type "yes" or "YES" if you wish to, type anything other than "yes" or "YES" if you do not.
      * If you do buy the property, you have the option of buying 1-4 houses (or no houses at all). The terminal will tell you how much each house costs and how much rent you will earn based on how many houses you buy. You must type in an integer from 0 to 4.
      * Rent is a fixed amount of cash collected anytime a player other than you lands on your property.
      * If you choose not to pay for a property and/or houses for that property, nothing will happen to you.

  2. **Chance space**

      * If you land on a chance space, the computer will randomly draw a card from the chance deck for you.

      *  You have no choice but to follow the actions of the chance card. You might earn/lose money, and/or be transported to another location on the board.

  3. **Jail, luxury tax, free parking, etc.**

      * These are special spaces, where you might be charged a fee, with the amount depending on where you land, or nothing might happen to you, depending on where you land.

5. The game only ends when all but one player is bankrupt. The last non-bankrupt player standing is the winner.

6. If you cannot pay rent, you are automatically deemed bankrupt and lose.

7. Although this game is mostly conducted through the terminal, the board will tell you where you are and where everyone else is.

Have fun! :grinning:
