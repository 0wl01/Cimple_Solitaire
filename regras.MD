# Simple Simon Rules

## 1. Objective
The goal of the game is to move all 52 cards to the 4 foundations, building complete sequences in descending order,
from **King down to Ace**, always of the **same suit**.

## 2. The Board (Setup)
* **Initial Deck:** 1 standard deck of 52 cards.
* **Columns (Tableau):** All cards are dealt face-up into **10 columns**.
    * The first 3 columns receive 8 cards each.
    * The remaining columns receive a decreasing number of cards: the 4th gets 7, the 5th gets 6, all the way down to the 10th column, which receives exactly 1 card.
* **No Leftovers:** There is no stock (draw pile) or waste pile. Every single card is on the table from the very beginning.

## 3. Permitted Moves
* **Stacking:** You can move the top card of a column to another column if its value is exactly **one rank lower** than the destination card, **regardless of the suit** (e.g., you can place a 7 of Hearts on an 8 of Spades).
* **Moving Sequences:** You can move a group of cards together ONLY if they are in a valid descending sequence AND are all of the **same suit** (e.g., a 7, 6, and 5 of Hearts can be moved as a single block).
* **Empty Spaces:** If one of the 10 columns becomes completely empty, you can fill it with any free card or any valid sequence of the same suit.
* **Foundations:** Once a complete sequence from King to Ace of the same suit is formed in a column, it is automatically moved to a foundation pile.