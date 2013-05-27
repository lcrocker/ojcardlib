/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Immutable class Card is basically a wrapper around a single integer value
 * like Integer, but with some specialized utilities.
 */
package com.onejoker.cardlib;

import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.Hashtable;
import java.util.ArrayList;

public final class Card {
    private final int mValue;

    public Card(int v) { mValue = v; }
    public Card(int r, int s) { mValue = (r << 2) + s + 1; }
    public Card(String s) { mValue = parseInt(s); }

    public int intValue() { return mValue; }
    public int hashCode() { return mValue; }
    public int rank() { return (mValue - 1) >>> 2; }
    public int suit() { return (mValue - 1) & 3; }
    public String name() { return cardnames[mValue]; }
    public boolean equals(Card c) { return this.mValue == c.mValue; }
    public boolean equals(int c) { return this.mValue == c; }
    public boolean equals(String s) { return this.mValue == Card.parseInt(s); }

    public static int intValue(int r, int s) { return (r << 2) + s + 1; }
    public static int rank(int c) { return (c - 1) >>> 2; }
    public static int rank(Card c) { return (c.mValue - 1) >>> 2; }
    public static int suit(int c) { return (c - 1) & 3; }
    public static int suit(Card c) { return (c.mValue - 1) & 3; }
    public static String name(int c) { return cardnames[c]; }
    public static String name(Card c) { return cardnames[c.mValue]; }

    public static int parseInt(String str) {
        Matcher m = mPat.matcher(str);
        if (m.find()) {
            String j = m.group(2);
            if (null != j) {
                if ('2' == j.charAt(1)) return 54;
                return 53;
            }
            String r = m.group(4).toLowerCase();
            String s = m.group(5).toLowerCase();
            return (mRanks.get(r) << 2) + mSuits.get(s) + 1;
        }
        throw new NumberFormatException("Unrecognized card value");
    }

    public static ArrayList<Card> listFromString(String str) {
        ArrayList<Card> cards = new ArrayList<Card>();
        Matcher m = mPat.matcher(str);
        while (m.find()) {
            String j = m.group(2);
            if (null != j) {
                if ('2' == j.charAt(1)) cards.add(new Card(54));
                else cards.add(new Card(53));
            } else {
                String r = m.group(4).toLowerCase();
                String s = m.group(5).toLowerCase();
                cards.add(new Card((mRanks.get(r) << 2) + mSuits.get(s) + 1));
            }
        }
        return cards;
    }

    public static final int DEUCE = 0;
    public static final int TREY = 1;
    public static final int FOUR = 2;
    public static final int FIVE = 3;
    public static final int SIX = 4;
    public static final int SEVEN = 5;
    public static final int EIGHT = 6;
    public static final int NINE = 7;
    public static final int TEN = 8;
    public static final int JACK = 9;
    public static final int QUEEN = 10;
    public static final int KING = 11;
    public static final int ACE = 12;
    public static final int RANK_JOKER = 13;

    public static final int CLUB = 0;
    public static final int DIAMOND = 1;
    public static final int HEART = 2;
    public static final int SPADE = 3;

    public static final int JOKER = 53;
    public static final int REDJOKER = 54;

    public static final String[] cardnames = { "XX",
        "2c","2d","2h","2s", "3c","3d","3h","3s", "4c","4d","4h","4s",
        "5c","5d","5h","5s", "6c","6d","6h","6s", "7c","7d","7h","7s",
        "8c","8d","8h","8s", "9c","9d","9h","9s", "Tc","Td","Th","Ts",
        "Jc","Jd","Jh","Js", "Qc","Qd","Qh","Qs", "Kc","Kd","Kh","Ks",
        "Ac","Ad","Ah","As", "JK","JR"
    };
    private static final Hashtable<String, Integer> mRanks =
        new Hashtable<String, Integer>();
    static {
        mRanks.put("2", DEUCE); mRanks.put("3", TREY); mRanks.put("4", FOUR);
        mRanks.put("5", FIVE);  mRanks.put("6", SIX);  mRanks.put("7", SEVEN);
        mRanks.put("8", EIGHT); mRanks.put("9", NINE); mRanks.put("10", TEN);
        mRanks.put("t", TEN);   mRanks.put("j", JACK); mRanks.put("q", QUEEN);
        mRanks.put("k", KING);  mRanks.put("a", ACE);
    }
    private static final Hashtable<String, Integer> mSuits =
        new Hashtable<String, Integer>();
    static {
        mSuits.put("c", CLUB);  mSuits.put("d", DIAMOND);
        mSuits.put("h", HEART); mSuits.put("s", SPADE);
    }
    private static final Pattern mPat = Pattern.compile(
    "[^A-Za-z0-9]*((jk|jr|joker)|((2|3|4|5|6|7|8|9|10|t|j|q|k|a)\\s*(c|d|h|s)))",
    Pattern.CASE_INSENSITIVE);

    public static native void seed(int s);
    public static native int rand(int lim);
}
