/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 */

public class Test001 {
    static {
        System.loadLibrary("ojcard");
    }
    private static void constructors() {
        CardList cl1, cl2, cl3;

        cl1 = new CardList(10);
        cl2 = new CardList(20, "Ah Kd 10s 5c 9d");
        cl1.add("AhKdTs5c9d");
        cl3 = new CardList("3d 7s 5d");

        assert 5 == cl1.size();
        assert 3 == cl3.size();
        assert cl1.equals(cl2);
        System.out.printf("%s\n", cl1.toString());

        cl3.add(5);
        assert 3 == cl3.size();
        cl2.clear();
        assert 0 == cl2.size();
        for (int i = 0; i < 5; ++i) { cl2.add(Card.rand(52) + 1); }

        java.util.Iterator<Card> iter = cl2.iterator();
        while (iter.hasNext()) {
            Card c = iter.next();
            System.out.printf("%d %s ", c.intValue(), c.name());
        }
        System.out.printf("\n%s\n", cl2.toString());
    }

    private static void decktypes() {
        CardList cl1, cl2;

        cl1 = new CardList(60);
        cl1.fill(DeckType.STRIPPED40);
        System.out.printf("%d %s\n", DeckType.STANDARD.size(), cl1.toString());
    }

    public static void main(String[] args) {
        constructors();
        decktypes();
    }
}
