/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Enumeration of deck types. Uses the fill function from CardList,
 * which calls into the C library, to get the cards.
 */
package com.onejoker.cardlib;

public enum DeckType {
    STANDARD(0, 52), ONEJOKER(1, 53), TWOJOKERS(2, 54),
    STRIPPED32(3, 32), STRIPPED40(4, 40), STRIPPED40J(5, 41),
    PINOCHLE(6, 24);

    private final static int NTYPES = DeckType.nTypes();

    private final int mValue;
    private final int mSize;
    private DeckType(int value, int size) {
        this.mValue = value;
        this.mSize = size;
    }
    public int value() { return this.mValue; }
    public int size() { return this.mSize; }

    private static final CardList[] mDecks = new CardList[NTYPES];
    static {
        for (int i = 0; i < NTYPES; ++i) {
            int c = DeckType.nCards(i);
            mDecks[i] = new CardList(c);
            mDecks[i].fill(c, i);
        }
    }
    public static CardList getDeck(int type) { return mDecks[type]; }
    public static CardList getDeck(DeckType t) { return mDecks[t.mValue]; }
    public static int deckSize(int type) { return DeckType.nCards(type); }
    public static int deckSize(DeckType t) { return DeckType.nCards(t.value()); }

    private static native int nCards(int type);
    private static native int nTypes();
}
