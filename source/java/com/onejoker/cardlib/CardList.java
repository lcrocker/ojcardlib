/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * CardList is designed to look more or less like a Java List class, but it is
 * really just a wrapper for the C structure, so you can't do everything you
 * can do with a list. But it enables you to call the fast functions of the C
 * library like the shuffler, hand evaluator, and combinatorics.
 */
package com.onejoker.cardlib;

import java.nio.*;
import java.util.ArrayList;
import java.util.Iterator;

public class CardList {
    private ByteBuffer mBuf;
    private static int mStructSize;

    private static native int nStructSize();
    static { mStructSize = nStructSize(); }

    public CardList(int size) {
        mBuf = ByteBuffer.allocateDirect(mStructSize + 4 * size);
        nNew(mBuf, size);
    }
    public CardList(int size, ArrayList<Card> cards) {
        this(size);
        this.add(cards);
    }
    public CardList(ArrayList<Card> cards) {
        this(cards.size(), cards);
    }
    public CardList(int size, String s) {
        this(size);
        this.add(s);
    }
    public CardList(String s) {
        ArrayList<Card> cards = Card.listFromString(s);
        int size = cards.size();
        mBuf = ByteBuffer.allocateDirect(mStructSize + 4 * size);
        nNew(mBuf, size);
        this.add(cards);
    }

    private static final int ERDONLY = -2;
    private static final int EFULL = -3;
    private static final int EDUPLICATE = -4;
    private static final int EBADINDEX = -5;

    private static final int handleError(int e) {
        if (ERDONLY == e) throw new SecurityException();
        if (EFULL == e || EBADINDEX == e)
            throw new IndexOutOfBoundsException();
        if (EDUPLICATE == e) throw new ArrayStoreException();
        return e;
    }

    public boolean add(int c) { return handleError(nAppend(mBuf, c)) >= 0; }
    public boolean add(Card c) { return this.add(c.intValue()); }
    public boolean add(ArrayList<Card> cards) {
        for (Card c : cards) { this.add(c.intValue()); }
        return true;
    }
    public boolean add(String s) {
        ArrayList<Card> cards = Card.listFromString(s);
        return this.add(cards);
    }
    public boolean add(int i, int c) {
        return handleError(nInsert(mBuf, i, c)) >= 0;
    }

    public boolean addAll(CardList s) {
        return handleError(nExtend(mBuf, s.mBuf, 0)) > 0;
    }

    public void clear() { handleError(nClear(mBuf)); }

    public boolean contains(int c) { return -1 != nIndex(mBuf, c); }
    public boolean contains(Card c) { return this.contains(c.intValue()); }
    public boolean contains(String s) {
        return this.contains(Card.parseInt(s));
    }

    public boolean equals(CardList s) { return nEquals(mBuf, s.mBuf); }
    public boolean equals(String s) { return this.equals(new CardList(s)); }

    /* TODO: get/set flags
     */

    public int getInt(int i) { return handleError(nGet(mBuf, i)); }
    public Card get(int i) { return new Card(this.getInt(i)); }

    public int fill(int c, int t) { return handleError(nFill(mBuf, c, t)); }
    public int fill(int c, DeckType t) { return this.fill(c, t.value()); }
    public int fill(DeckType t) { return this.fill(t.size(), t); }
    public int fill(int c) { return this.fill(c, DeckType.STANDARD); }
    public int fill() { return this.fill(52, DeckType.STANDARD); }

    public int hashCode() { return nHash(mBuf); }

    public int indexOf(int c) { return nIndex(mBuf, c); }
    public int indexOf(Card c) { return this.indexOf(c.intValue()); }

    public boolean isEmpty() { return 0 == size(); }

    public Iterator<Card> iterator() { return new CardListIterator(this); }

    public int popInt() { return handleError(nPop(mBuf)); }
    public Card pop() { return new Card(this.popInt()); }

    public int removeIndex(int i) { return handleError(nDelete(mBuf, i)); }
    public boolean removeCard(int c) {
        return handleError(nRemove(mBuf, c)) >= 0;
    }
    public boolean removeCard(Card c) { return this.removeCard(c.intValue()); }

    public void reverse() { handleError(nReverse(mBuf)); }

    public int set(int i, int c) { return handleError(nSet(mBuf, i, c)); }
    public Card set(int i, Card c) {
        return new Card(this.set(i, c.intValue()));
    }

    public void shuffle() { handleError(nShuffle(mBuf)); }

    public int size() { return nSize(mBuf); }

    public void sort() { handleError(nSort(mBuf)); }

    public ArrayList<Card> toList() {
        ArrayList<Card> cards = new ArrayList<Card>();
        for (int i = 0; i < this.size(); ++i) { cards.add(this.get(i)); }
        return cards;
    }

    public String toString() {
        String s = "(";
        for (int i = 0; i < this.size(); ++i) {
            if (0 != i) s += " ";
            s += Card.name(this.get(i));
        }
        return s + ")";
    }

    public void truncate(int s) { handleError(nTruncate(mBuf, s)); }

    private static native int nAppend(ByteBuffer b, int c);
    private static native int nClear(ByteBuffer b);
    private static native int nDelete(ByteBuffer b, int i);
    private static native boolean nEquals(ByteBuffer b, ByteBuffer c);
    private static native int nExtend(ByteBuffer d, ByteBuffer s, int c);
    private static native int nFill(ByteBuffer b, int c, int t);
    private static native int nGet(ByteBuffer b, int i);
    private static native int nHash(ByteBuffer b);
    private static native int nIndex(ByteBuffer b, int c);
    private static native int nInsert(ByteBuffer b, int i, int c);
    private static native void nNew(ByteBuffer b, int s);
    private static native int nPop(ByteBuffer b);
    private static native int nRemove(ByteBuffer b, int c);
    private static native int nReverse(ByteBuffer b);
    private static native int nSet(ByteBuffer b, int i, int c);
    private static native int nShuffle(ByteBuffer b);
    private static native int nSize(ByteBuffer b);
    private static native int nSort(ByteBuffer b);
    private static native int nTruncate(ByteBuffer b, int s);
}

class CardListIterator implements Iterator<Card> {
    private int mIndex;
    private CardList mCards;

    CardListIterator(CardList cards) {
        mIndex = 0;
        mCards = cards;
    }

    public boolean hasNext() { return mIndex != mCards.size(); }

    public Card next() {
        Card c = mCards.get(mIndex);
        ++mIndex;
        return c;
    }

    public void remove() {
        if (0 == mIndex || 0 == mCards.size()) return;
        --mIndex;
        mCards.removeIndex(mIndex);
    }
}
