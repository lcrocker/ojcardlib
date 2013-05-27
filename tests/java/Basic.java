/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 */

import com.onejoker.cardlib.*;

public class Basic {
    static {
        System.loadLibrary("ojcard");
    }

    private static int cards_and_decks() {
        if (13 != (new Card(Card.FIVE, Card.CLUB)).intValue()) return 1;
        if (30 != Card.intValue(Card.NINE, Card.DIAMOND)) return 2;
        if (43 != (new Card(Card.QUEEN, Card.HEART)).intValue()) return 3;
        if (52 != Card.intValue(Card.ACE, Card.SPADE)) return 4;
        if (54 != Card.REDJOKER) return 5;

        if (Card.TREY != Card.rank(7)) return 6;
        if (Card.DIAMOND != (new Card(26)).suit()) return 7;
        if (Card.JACK != Card.rank(new Card(38))) return 8;
        if (Card.SPADE != Card.suit(new Card(48))) return 9;
        if (Card.RANK_JOKER != (new Card(54)).rank()) return 10;

        if (52 != DeckType.deckSize(DeckType.STANDARD.value())) return 11;
        if (54 != DeckType.TWOJOKERS.size()) return 12;
        if (40 != DeckType.deckSize(DeckType.STRIPPED40)) return 13;

        CardList deck = DeckType.getDeck(DeckType.ONEJOKER);
        if (53 != deck.size()) return 14;
        if (! (new Card(Card.DEUCE, Card.CLUB)).equals(deck.get(0)))
            return 15;
        if (Card.JOKER != deck.getInt(52)) return 16;

        deck = DeckType.getDeck(DeckType.STRIPPED32.value());
        if (32 != deck.size()) return 17;
        if ((new Card(Card.SEVEN, Card.CLUB)).intValue() != deck.getInt(0))
            return 18;
        if (! (new Card(Card.ACE, Card.SPADE)).equals(deck.get(31)))
            return 19;

        deck = DeckType.getDeck(DeckType.STRIPPED40J);
        if (41 != deck.size()) return 20;
        if (! (new Card(Card.SEVEN, Card.SPADE)).equals(deck.get(23)))
            return 21;
        if (Card.intValue(Card.JACK, Card.CLUB) != deck.getInt(24))
            return 22;
        if (Card.JOKER != deck.getInt(40)) return 23;

        return 0;
    }

    private static int text_functions() {
        return 0;
    }

    public static void main(String[] args) {
        int failed = 0;

        failed = cards_and_decks();
        failed = 100 * failed + text_functions();

        System.out.printf("Basic Java tests ");
        if (0 != failed) {
            System.out.printf("failed (code = %d).\n", failed);
            System.exit(1);
        } else {
            System.out.printf("passed.\n");
        }
    }
}
